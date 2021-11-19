# import asyncio
# from bleak import BleakClient, BleakScanner, discover
# from bleak.exc import BleakDBusError, BleakError

# ARDUINO_FLOAT_LENGTH_BYTES =  4
# ACC_ELEMENTS = 3
# GYR_ELEMENTS = 3
# MAG_ELEMENTS = 3
# BLE_LOCAL_NAME_IMU = "VRHANDHELDCONTROLLER"
# BLE_UUID_IMU_SERVICE = "12307e98-a67e-45e2-b8f0-1445fa6e2cd8"
# BLE_UUID_ACC = "cf89ef50-4bdb-423a-a822-88aaf7f96c5a"
# BLE_UUID_GYR = "1552ab31-afbb-418c-bd23-1f4527751390"
# BLE_UUID_MAG = "55584dd1-b773-44aa-a38d-ed6d9c94a851"
# BLE_UUID_BUTTON = "017fa2e8-b256-47bf-953b-a494b75fb9bf"

# async def main(name):
#     device = await BleakScanner.find_device_by_filter(
#         lambda d: d.name and d.name.lower() == name.lower()
#     )
#     print(device)
#     # device = await BleakScanner.find_device_by_address(BLE_UUID_IMU_SERVICE, timeout=10.0)
#     # if not device:
#     #     raise BleakError("A device with address {0} could not be found.".format(ble_address))
#     # async with BleakClient(device) as client:
#     #     services = await client.get_services()
#     #     for s in services:
#     #         print(s)
# asyncio.run(main(BLE_LOCAL_NAME_IMU))
# import asyncio
# from bleak import BleakScanner

# async def main():
#     devices = await BleakScanner.discover()
#     for d in devices:
#         print(d)

# asyncio.run(main())
# bleak has error currently for mac that makes scanning devices unfeasible, will just use serial...

import serial
port = '/dev/tty.usbmodem11301'
vr_handheld_controller_reciever = serial.Serial(port, 9600)

vr_handheld_controller_reciever.write(bytes('agm', 'utf-8'))

while True:
    msg = vr_handheld_controller_reciever.read_until()
    print("Message: {0}".format(msg))