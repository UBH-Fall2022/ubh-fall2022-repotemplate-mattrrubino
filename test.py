import cv2
import asyncio
import websockets
import numpy as np


async def main():
    async with websockets.connect("wss://changingtheworldwith.tech/user") as ws:
        while True:
            data = await ws.recv()

            npArr = np.frombuffer(data, np.uint8)
            img = cv2.imdecode(npArr, cv2.IMREAD_COLOR)
            cv2.imshow("image", img)
            key = cv2.waitKey(0)

            if key == ord('q'):
                break


asyncio.run(main())
