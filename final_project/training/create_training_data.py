# TODO: Format data in receiver to desired format and parse here. Create app/ability to automatically label data just by setting label and doing gestures by pressing controller button. Likely will have a csv file with filenames corresponding to gesture label. The files will contain the IMU data.
import serial
port = '/dev/tty.usbmodem11301'
vr_handheld_controller_receiver = serial.Serial(port, 15200)

vr_handheld_controller_receiver.write(bytes('agm', 'utf-8'))

while True:
    msg = vr_handheld_controller_receiver.read_until()
    print("Message: {0}".format(msg))