import asyncio
import websockets
import json
import os
import traceback
from pycaw.pycaw import AudioUtilities

def set_volume(percentage):
    try:
        percentage = max(0, min(100, int(percentage)))
        scalar = percentage / 100.0
        devices = AudioUtilities.GetSpeakers()
        volume = devices.EndpointVolume
        volume.SetMasterVolumeLevelScalar(scalar, None)
        print(f"Action: Set Volume to {percentage}%")
        return True
    except Exception as e:
        print(f"Error setting volume: {e}")
        return False

def toggle_mute():
    try:
        devices = AudioUtilities.GetSpeakers()
        volume = devices.EndpointVolume
        current_mute = volume.GetMute()
        volume.SetMute(not current_mute, None)
        print(f"Action: Toggle MUTE (Now: {not current_mute})")
        return True
    except Exception as e:
        print(f"Failed to toggle mute: {e}")
        return False

def get_volume_status():
    try:
        devices = AudioUtilities.GetSpeakers()
        volume = devices.EndpointVolume
        current_vol = int(volume.GetMasterVolumeLevelScalar() * 100)
        is_muted = volume.GetMute()
        return current_vol, is_muted
    except Exception as e:
        print(f"Failed to get status: {e}")
        return 0, False

async def connect_mcp_server(url):
    while True:
        print(f"Connecting to MCP server at {url}...")
        try:
            async with websockets.connect(url) as ws:
                print("Connected!")
                while True:
                    message = await ws.recv()
                    data = json.loads(message)
                    
                    # Handle optional wrapper
                    payload = data.get("payload", data)
                    method = payload.get("method")
                    
                    if method == "initialize":
                        req_id = payload.get("id")
                        response = {
                            "jsonrpc": "2.0",
                            "id": req_id,
                            "result": {
                                "protocolVersion": "2024-11-05",
                                "capabilities": {"tools": {}},
                                "serverInfo": {"name": "laptop_volume_controller", "version": "1.0"}
                            }
                        }
                        if "payload" in data:
                            wrapped = {"session_id": data.get("session_id", ""), "type": "mcp", "payload": response}
                            await ws.send(json.dumps(wrapped))
                        else:
                            await ws.send(json.dumps(response))
                        print("Sent initialize response")
                        
                    elif method == "notifications/initialized":
                        print("Received initialized notification")
                        
                    elif method == "tools/list":
                        req_id = payload.get("id")
                        response = {
                            "jsonrpc": "2.0",
                            "id": req_id,
                            "result": {
                                "tools": [
                                    {
                                        "name": "laptop.volume.set",
                                        "description": "Set the laptop volume to a specific percentage.",
                                        "inputSchema": {
                                            "type": "object",
                                            "properties": {
                                                "percentage": {
                                                    "type": "integer",
                                                    "description": "Target volume percentage (0-100)"
                                                }
                                            },
                                            "required": ["percentage"]
                                        }
                                    },
                                    {
                                        "name": "laptop.volume.mute",
                                        "description": "Toggle mute on the laptop.",
                                        "inputSchema": { "type": "object", "properties": {} }
                                    },
                                    {
                                        "name": "laptop.volume.status",
                                        "description": "Get the current volume level and mute status.",
                                        "inputSchema": { "type": "object", "properties": {} }
                                    }
                                ],
                                "nextCursor": ""
                            }
                        }
                        if "payload" in data:
                            wrapped = {"session_id": data.get("session_id", ""), "type": "mcp", "payload": response}
                            await ws.send(json.dumps(wrapped))
                        else:
                            await ws.send(json.dumps(response))
                        print("Sent tools/list response")
                        
                    elif method == "tools/call":
                        req_id = payload.get("id")
                        params = payload.get("params", {})
                        tool_name = params.get("name")
                        arguments = params.get("arguments", {})
                        
                        is_error = False
                        result_content = "unknown tool"
                        
                        if tool_name == "laptop.volume.set":
                            percentage = arguments.get("percentage", 50)
                            success = set_volume(percentage)
                            result_content = f"Volume set to {percentage}%" if success else "Failed to set volume"
                        elif tool_name == "laptop.volume.mute":
                            success = toggle_mute()
                            result_content = "Mute toggled successfully" if success else "Failed to toggle mute"
                        elif tool_name == "laptop.volume.status":
                            vol, is_muted = get_volume_status()
                            status_text = f"Volume is at {vol}%"
                            if is_muted:
                                status_text += " (Muted)"
                            result_content = status_text
                        else:
                            is_error = True
                        
                        response = {"jsonrpc": "2.0", "id": req_id}
                        if is_error:
                            response["error"] = {"code": -32601, "message": f"Unknown tool: {tool_name}"}
                        else:
                            response["result"] = {"content": [{"type": "text", "text": result_content}], "isError": False}
                            
                        if "payload" in data:
                            wrapped = {"session_id": data.get("session_id", ""), "type": "mcp", "payload": response}
                            await ws.send(json.dumps(wrapped))
                        else:
                            await ws.send(json.dumps(response))
                        print(f"Sent tools/call response for {tool_name}")
                        
        except Exception as e:
            print(f"Connection error: {e}")
            await asyncio.sleep(5)

if __name__ == "__main__":
    import sys
    url = "wss://api.xiaozhi.me/mcp/?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjEwMTUzODIsImFnZW50SWQiOjIxNzAwMjEsImVuZHBvaW50SWQiOiJhZ2VudF8yMTcwMDIxIiwicHVycG9zZSI6Im1jcC1lbmRwb2ludCIsImlhdCI6MTc4NDkyNjU4OSwiZXhwIjoxODE2NDg0MTg5fQ.50Lnd8mUCLehqyOOq2iRNxYEPYVwHo886IoDk7rhQzdL-OJ69jTgITXTW6QiCpEuapqm66E1nyK9gqb2EOOP6A"
    if len(sys.argv) > 1:
        url = sys.argv[1]
    
    asyncio.run(connect_mcp_server(url))
