#include "single_led.h"
#include "application.h"
#include "mcp_server.h"
#include <esp_log.h> 

#define TAG "SingleLed"

#define DEFAULT_BRIGHTNESS 4
#define HIGH_BRIGHTNESS 16
#define LOW_BRIGHTNESS 2

#define BLINK_INFINITE -1


SingleLed::SingleLed(gpio_num_t gpio) {
    if (gpio == GPIO_NUM_NC) {
        ESP_LOGW(TAG, "SingleLed initialized with GPIO_NUM_NC, LED will not function");
        return;
    }

    led_strip_config_t strip_config = {};
    strip_config.strip_gpio_num = gpio;
    strip_config.max_leds = 1;
    strip_config.color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB;
    strip_config.led_model = LED_MODEL_WS2812;

    led_strip_rmt_config_t rmt_config = {};
    rmt_config.resolution_hz = 10 * 1000 * 1000; // 10MHz

    ESP_ERROR_CHECK(led_strip_new_rmt_device(&strip_config, &rmt_config, &led_strip_));
    led_strip_clear(led_strip_);

    esp_timer_create_args_t blink_timer_args = {
        .callback = [](void *arg) {
            auto led = static_cast<SingleLed*>(arg);
            led->OnBlinkTimer();
        },
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "blink_timer",
        .skip_unhandled_events = false,
    };
    ESP_ERROR_CHECK(esp_timer_create(&blink_timer_args, &blink_timer_));

    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool("self.neopixel.set_color", 
        "Set the NeoPixel LED color. (设置 LED 颜色)", 
        PropertyList({
            Property("red", kPropertyTypeInteger, 0, 255),
            Property("green", kPropertyTypeInteger, 0, 255),
            Property("blue", kPropertyTypeInteger, 0, 255),
        }), [this](const PropertyList& properties) -> ReturnValue {
            uint8_t red = properties["red"].value<int>();
            uint8_t green = properties["green"].value<int>();
            uint8_t blue = properties["blue"].value<int>();
            ESP_LOGI(TAG, "Setting LED color to RGB(%d, %d, %d)", red, green, blue);
            SetManualMode(true);
            SetColor(red, green, blue);
            TurnOn();
            return true;
        });
        
    mcp_server.AddTool("self.neopixel.turn_off", 
        "Turn off the NeoPixel LED. (关闭 LED)", 
        PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            ESP_LOGI(TAG, "Turning off LED");
            SetManualMode(true);
            TurnOff();
            return true;
        });

    mcp_server.AddTool("self.neopixel.blink", 
        "Blink the NeoPixel LED. (闪烁 LED)", 
        PropertyList({
            Property("red", kPropertyTypeInteger, 0, 255),
            Property("green", kPropertyTypeInteger, 0, 255),
            Property("blue", kPropertyTypeInteger, 0, 255),
            Property("count", kPropertyTypeInteger, 1, 100),
            Property("interval", kPropertyTypeInteger, 10, 5000),
        }), [this](const PropertyList& properties) -> ReturnValue {
            uint8_t red = properties["red"].value<int>();
            uint8_t green = properties["green"].value<int>();
            uint8_t blue = properties["blue"].value<int>();
            int count = properties["count"].value<int>();
            int interval = properties["interval"].value<int>();
            ESP_LOGI(TAG, "Blinking LED RGB(%d, %d, %d) %d times every %d ms", red, green, blue, count, interval);
            SetManualMode(true);
            Blink(count, interval);
            SetColor(red, green, blue);
            return true;
        });

    mcp_server.AddTool("self.neopixel.reset_mode", 
        "Reset the NeoPixel LED to automatic status mode. (恢复默认状态指示)", 
        PropertyList(), [this](const PropertyList& properties) -> ReturnValue {
            ESP_LOGI(TAG, "Resetting LED to automatic mode");
            SetManualMode(false);
            return true;
        });
}

SingleLed::~SingleLed() {
    if (blink_timer_ != nullptr) {
        esp_timer_stop(blink_timer_);
    }
    if (led_strip_ != nullptr) {
        led_strip_del(led_strip_);
    }
}


void SingleLed::SetColor(uint8_t r, uint8_t g, uint8_t b) {
    r_ = r;
    g_ = g;
    b_ = b;
}

void SingleLed::SetManualMode(bool manual) {
    manual_mode_ = manual;
    if (!manual) {
        OnStateChanged();
    }
}

void SingleLed::TurnOn() {
    if (led_strip_ == nullptr) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    led_strip_set_pixel(led_strip_, 0, r_, g_, b_);
    led_strip_refresh(led_strip_);
}

void SingleLed::TurnOff() {
    if (led_strip_ == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    led_strip_clear(led_strip_);
}

void SingleLed::BlinkOnce() {
    Blink(1, 100);
}

void SingleLed::Blink(int times, int interval_ms) {
    StartBlinkTask(times, interval_ms);
}

void SingleLed::StartContinuousBlink(int interval_ms) {
    StartBlinkTask(BLINK_INFINITE, interval_ms);
}

void SingleLed::StartBlinkTask(int times, int interval_ms) {
    if (led_strip_ == nullptr) {
        return;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    esp_timer_stop(blink_timer_);
    
    blink_counter_ = times * 2;
    blink_interval_ms_ = interval_ms;
    esp_timer_start_periodic(blink_timer_, interval_ms * 1000);
}

void SingleLed::OnBlinkTimer() {
    std::lock_guard<std::mutex> lock(mutex_);
    blink_counter_--;
    if (blink_counter_ & 1) {
        led_strip_set_pixel(led_strip_, 0, r_, g_, b_);
        led_strip_refresh(led_strip_);
    } else {
        led_strip_clear(led_strip_);

        if (blink_counter_ == 0) {
            esp_timer_stop(blink_timer_);
        }
    }
}


void SingleLed::OnStateChanged() {
    if (manual_mode_) {
        return;
    }

    auto& app = Application::GetInstance();
    auto device_state = app.GetDeviceState();
    switch (device_state) {
        case kDeviceStateStarting:
            SetColor(0, 0, DEFAULT_BRIGHTNESS);
            StartContinuousBlink(100);
            break;
        case kDeviceStateWifiConfiguring:
            SetColor(0, 0, DEFAULT_BRIGHTNESS);
            StartContinuousBlink(500);
            break;
        case kDeviceStateIdle:
            TurnOff();
            break;
        case kDeviceStateConnecting:
            SetColor(0, 0, DEFAULT_BRIGHTNESS);
            TurnOn();
            break;
        case kDeviceStateListening:
        case kDeviceStateAudioTesting:
            if (app.IsVoiceDetected()) {
                SetColor(HIGH_BRIGHTNESS, 0, 0);
            } else {
                SetColor(LOW_BRIGHTNESS, 0, 0);
            }
            TurnOn();
            break;
        case kDeviceStateSpeaking:
            SetColor(0, DEFAULT_BRIGHTNESS, 0);
            TurnOn();
            break;
        case kDeviceStateUpgrading:
            SetColor(0, DEFAULT_BRIGHTNESS, 0);
            StartContinuousBlink(100);
            break;
        case kDeviceStateActivating:
            SetColor(0, DEFAULT_BRIGHTNESS, 0);
            StartContinuousBlink(500);
            break;
        default:
            ESP_LOGW(TAG, "Unknown led strip event: %d", device_state);
            return;
    }
}
