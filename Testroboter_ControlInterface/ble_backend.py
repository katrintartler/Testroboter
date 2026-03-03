import threading
import asyncio
from bleak import BleakClient

ESP_MAC = "a0:b7:65:69:9d:3e"
CHAR_UUID = "abcd1234-ab12-34cd-56ef-1234567890ab"

ble_loop = asyncio.new_event_loop()
client = None

async def ble_connect():
    global client 
    client = BleakClient(ESP_MAC)
    await client.connect()
    print("client connected")


def ble_start_thread():
    def run():
        asyncio.set_event_loop(ble_loop)
        ble_loop.run_until_complete(ble_connect())
        print("BLE loop running")
        ble_loop.run_forever()
    
    threading.Thread(target=run,daemon=True).start()


async def ble_send_cmds(data: bytes): 
    if not client.is_connected:
        print("not connected")
        await client.connect()
    
    await client.write_gatt_char(CHAR_UUID, data)
    print(f"sent {data} cmd")




def tw():
    print("in twitching")
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([1])), ble_loop)

def gm():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([2])), ble_loop)

def lm_l_left():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([3])), ble_loop)

def lm_l_right():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([4])), ble_loop)

def crying():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([5])), ble_loop)
def brabbeln():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([6])), ble_loop)
def coughing():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([7])), ble_loop)
def sneezing():
    asyncio.run_coroutine_threadsafe(ble_send_cmds(bytearray([8])), ble_loop)