# Laptop Volume MCP Server

This Python script acts as an MCP (Model Context Protocol) server that connects to the XiaoZhi backend via WebSocket. It allows the XiaoZhi AI chatbot to interact with and control your Windows laptop's system volume directly through voice commands.

## Features

This server exposes the following MCP tools to XiaoZhi:

- **laptop.volume.set**: Set the master volume to a specific percentage (0-100).
- **laptop.volume.mute**: Toggle the system mute status on/off.
- **laptop.volume.status**: Get the current volume level and mute status.
- **Auto-reconnect**: The server runs continuously in a background loop and will automatically attempt to reconnect every 5 seconds if the WebSocket connection drops.

## Prerequisites

- **OS**: Windows (Uses `pycaw` which relies on Windows Core Audio APIs)
- **Python**: Python 3.7 or newer

## Installation

1. Navigate to this directory in your terminal.
2. Install the required Python packages (`websockets` and `pycaw`):

```bash
pip install -r requirements.txt
```

*(Note: If `requirements.txt` is missing, you can install the dependencies manually: `pip install websockets pycaw`)*

## Running the Server

Run the Python server script:

```bash
python server.py [WSS_URL]
```

- By default, it will connect to the XiaoZhi API URL hardcoded in the script.
- You can optionally provide your own WebSocket URL as a command-line argument.

Once running, the server will connect to the XiaoZhi MCP WebSocket endpoint. When you speak to your XiaoZhi device and ask it to "turn my laptop volume up", "mute the computer", or "what is the current volume on my laptop?", the AI will send the corresponding command to this Python script, which will execute the volume adjustments on your Windows machine in real-time.
