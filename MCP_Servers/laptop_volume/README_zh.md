# 笔记本电脑音量控制 MCP 服务端 (Laptop Volume MCP Server)

此 Python 脚本是一个 MCP (模型上下文协议) 服务端，它通过 WebSocket 连接到 XiaoZhi (小智) 后端。它允许 XiaoZhi AI 语音助手通过语音指令直接控制您的 Windows 笔记本电脑的系统音量。

## 功能特性

该服务端向 XiaoZhi 暴露了以下 MCP 工具：

- **laptop.volume.set**: 将主音量设置为指定的百分比 (0-100)。
- **laptop.volume.mute**: 切换系统的静音状态 (开启/关闭)。
- **laptop.volume.status**: 获取当前的音量级别和静音状态。
- **自动重连**: 服务端在后台循环中持续运行，如果 WebSocket 连接断开，它将自动每隔 5 秒尝试重新连接。

## 前置要求

- **操作系统**: Windows (使用了依赖于 Windows Core Audio API 的 `pycaw` 库)
- **Python 环境**: Python 3.7 或更高版本

## 安装步骤

1. 在终端中进入到此目录。
2. 安装所需的 Python 依赖包 (`websockets` 和 `pycaw`)：

```bash
pip install -r requirements.txt
```

*(注意：如果缺少 `requirements.txt`，您可以手动安装依赖项：`pip install websockets pycaw`)*

## 运行服务端

运行 Python 服务端脚本：

```bash
python server.py [WSS_URL]
```

- 默认情况下，它将连接到脚本中硬编码的 XiaoZhi API URL。
- 您也可以选择将您自己的 WebSocket URL 作为命令行参数传入。

运行后，服务端将连接到 XiaoZhi 的 MCP WebSocket 节点。当您对 XiaoZhi 设备说“把我的笔记本音量调大”、“静音电脑”或“我的笔记本当前音量是多少”时，AI 就会将相应的指令发送到这个 Python 脚本，从而实时调节您 Windows 电脑的音量。
