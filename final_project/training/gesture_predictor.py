import serial
import time
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
import os
import torch
from train_gesture_classification import GestureClassificationNet, read_and_interpolate_features
from create_training_data import next_vals, default_vals, save_csv

root_dir = './'
models_path = './models'
data_path = './data_files'
dtype = torch.float32
device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")

prediction_model_filename = 'gesture_model_12_12_2021_14_15_54.pt' 
prediction_model_filepath = os.path.join(models_path, prediction_model_filename)

def make_prediction(model, features):
    model.eval()
    with torch.no_grad():
        score = model(features)
    return torch.argmax(score, dim=1).cpu().item()

gesture_classification_model = GestureClassificationNet().to(device=device, dtype=dtype)
gesture_classification_model.load_state_dict(torch.load(prediction_model_filepath))

port = '/dev/tty.usbmodem11301'
baud_rate = 15200
vr_handheld_controller_receiver = serial.Serial(port=port, baudrate=baud_rate)
sample_size_min = 20
val_list = [''] * 9 # acc_x, acc_y, acc_z, gyr_x, gyr_y, gry_z, mag_x, mag_y, mag_z

temp_filepath = os.path.join(root_dir,'current_reading_tempfile.csv')

current_prediction = -1

while True:
    try:
        msg = vr_handheld_controller_receiver.read_until()
        msg = msg.decode('utf-8')
        if msg is None or msg == '':
            # print('msg is none or empty str')
            continue
        msg_list = msg.rstrip('\r\n').split(';')
        if len(msg_list) != 10:
            print(msg)
            continue
        if msg_list[0] == '1':
            val_list = next_vals(val_list, msg_list)
        else:
            if len(val_list[0])  > sample_size_min:
                save_csv(temp_filepath, val_list)
                features = torch.unsqueeze(read_and_interpolate_features(temp_filepath),dim=0)
                features = features.to(device=device, dtype=dtype)
                os.remove(temp_filepath)
                current_prediction = make_prediction(gesture_classification_model, features)
                print('Gesture detected, predicted gesture = {0}'.format(current_prediction))
            val_list = default_vals()
    except:
        print('Connection ended')
        vr_handheld_controller_receiver.close()
        time.sleep(5)
        vr_handheld_controller_receiver = serial.Serial(port=port, baudrate=baud_rate)
