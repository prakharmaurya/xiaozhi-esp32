# Laptop Volume MCP Server

This Python script acts as an MCP (Model Context Protocol) server that connects to the XiaoZhi backend via WebSocket and allows the XiaoZhi AI chatbot to control your Windows laptop's volume.

## Features

- **laptop.volume.set**: Set the volume to a specific percentage (0-100).
- **laptop.volume.mute**: Toggle the mute status.
- **laptop.volume.status**: Get the current volume level and mute status.
- **laptop.volume.mute**: Toggle laptop mute.
- **Auto-reconnect**: The server runs continuously in a `while True` loop and will automatically reconnect every 5 seconds if the WebSocket connection drops.

## Prerequisites

- Windows OS
- Python 3.7+

## Installation

Install the required Python package (`websockets`):

```bash
pip install -r requirements.txt
```

## Running the Server

Run the Python server script:

```bash
python server.py
```

The server will connect to the provided XiaoZhi MCP WebSocket endpoint. When you ask XiaoZhi to "turn my laptop volume up" or "turn the volume down", it will send the command to this Python script, which will emulate the volume key presses on your Windows machine.
