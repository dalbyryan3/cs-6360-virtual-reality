# TODO: Format data in receiver to desired format and parse here. Create app/ability to automatically label data just by setting label and doing gestures by pressing controller button. Likely will have a csv file with filenames corresponding to gesture label. The files will contain the IMU data.
import serial
import time

label = 3
root_dir = './'

sample_size_min = 20

last_set_time = time.time()

val_list = [''] * 9 # acc_x, acc_y, acc_z, gyr_x, gyr_y, gry_z, mag_x, mag_y, mag_z

def default_vals(val_list):
    return [''] * 9 
def next_vals(val_list, msg_list):
    # for i in range(len(val_list)):
    #     print (val_list[i] + msg_list[i+1] + ',')
    return [(val_list[i] + msg_list[i+1] + ',') for i in range(len(val_list))]
def save_csv(filepath, val_list):
    with open(filepath, 'w+') as f:
        for val in val_list:
            f.write('{0}\n'.format(val))
def add_label_and_save_csv(root_dir, val_list, label, dt):
    data_filename = time.strftime("%Y%m%d_%H%M%S.csv")
    data_filepath = '{0}data_files/{1}'.format(root_dir, data_filename)
    save_csv(data_filepath, val_list)
    with open(root_dir+'vr_handheld_controller_gesture_data.csv', 'a+') as f:
        f.write('{0}, {1}, {2}\n'.format(data_filepath, label, dt))
    return data_filepath

port = '/dev/tty.usbmodem11301'
baud_rate = 15200
vr_handheld_controller_receiver = serial.Serial(port=port, baudrate=baud_rate)
while True:
    try:
        msg = vr_handheld_controller_receiver.read_until()
        msg = msg.decode('utf-8')
        print(msg)
        if msg is None or msg == '':
            # print('msg is none or empty str')
            continue
        msg_list = msg.rstrip('\r\n').split(';')
        if len(msg_list) != 10:
            continue
        if msg_list[0] == '1':
            val_list = next_vals(val_list, msg_list)
        else:
            if len(val_list[0])  > sample_size_min:
                dt = time.time()-last_set_time
                saved_filepath = add_label_and_save_csv(root_dir, val_list, label, dt)
                print('{0} second sample recorded and saved as {1}'.format(dt, saved_filepath))
            val_list = default_vals(val_list)
            last_set_time = time.time()
    except:
        print('Connection ended')
        vr_handheld_controller_receiver.close()
        time.sleep(5)
        vr_handheld_controller_receiver = serial.Serial(port=port, baudrate=baud_rate)