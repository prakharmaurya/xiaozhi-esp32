import asyncio
import websockets
import json
import sys

async def test(url):
    async with websockets.connect(url) as ws:
        msg = await ws.recv()
        print("Received:", msg)
        data = json.loads(msg)
        
        req_id = data.get("id")
        response = {
            "jsonrpc": "2.0",
            "id": req_id,
            "result": {
                "protocolVersion": "2024-11-05",
                "capabilities": {
                    "tools": {}
                },
                "serverInfo": {
                    "name": "laptop_volume_controller",
                    "version": "1.0"
                }
            }
        }
        
        print("Sending:", json.dumps(response))
        await ws.send(json.dumps(response))
        print("Sent!")
        
        try:
            while True:
                msg2 = await ws.recv()
                print("Received again:", msg2)
        except Exception as e:
            print("Exception while waiting:", e)

url = "wss://api.xiaozhi.me/mcp/?token=eyJhbGciOiJFUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOjEwMTUzODIsImFnZW50SWQiOjIxNzAwMjEsImVuZHBvaW50SWQiOiJhZ2VudF8yMTcwMDIxIiwicHVycG9zZSI6Im1jcC1lbmRwb2ludCIsImlhdCI6MTc4NDkyNDI0OCwiZXhwIjoxODE2NDgxODQ4fQ.xtpJC2lx6ReWvYqYRH58sqigrn5TR4jP_u_Px2IyfjmR46QKlrNBhirQeJAyK_e0GBBzkHRPGlASQmrBZhzC1Q"
asyncio.run(test(url))
