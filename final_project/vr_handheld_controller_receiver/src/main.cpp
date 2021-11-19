/*
Ryan Dalby- CS 6360 Virtual Reality Final Project 

.ino for vr_handheld_controller_receiver

This code is the code behind the bluetooth receiver of the handheld VR controller demonstrating bluetooth functionality.
*/

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

#define ARDUINO_FLOAT_LENGTH_BYTES 4
#define ARDUINO_BOOL_LENGTH_BYTES 1
#define ACC_ELEMENTS 3
#define GYR_ELEMENTS 3
#define MAG_ELEMENTS 3
#define BLE_LOCAL_NAME_IMU "VRHANDHELDCONTROLLER"
#define BLE_UUID_IMU_SERVICE "12307e98-a67e-45e2-b8f0-1445fa6e2cd8"
#define BLE_UUID_ACC "cf89ef50-4bdb-423a-a822-88aaf7f96c5a"
#define BLE_UUID_GYR "1552ab31-afbb-418c-bd23-1f4527751390"
#define BLE_UUID_MAG "55584dd1-b773-44aa-a38d-ed6d9c94a851"
#define BLE_UUID_BUTTON "017fa2e8-b256-47bf-953b-a494b75fb9bf"

float acc[ACC_ELEMENTS] = {0}; // x,y,z g = 9.80665 m/s^2
float gyr[GYR_ELEMENTS] = {0}; // x,y,z deg/s
float mag[MAG_ELEMENTS] = {0};  // x,y,z muT (micro-Teslas)
bool buttonPressed = false;

bool print_a = false;
bool print_g = false;
bool print_m = false;

void printIMUVals(bool a, bool g, bool m)
{
    // Will print values of Acc, Gyr, or Mag, depending on which is the first true bool is seen, if none seen prints button state
    if (a)
    {
        Serial.print("AccX:");
        Serial.print(acc[0]);
        Serial.print(",");
        Serial.print("AccY:");
        Serial.print(acc[1]);
        Serial.print(",");
        Serial.print("AccZ:");
        Serial.println(acc[2]);
    }
    else if (g)
    {
        Serial.print("GyrX:");
        Serial.print(gyr[0]);
        Serial.print(",");
        Serial.print("GyrY:");
        Serial.print(gyr[1]);
        Serial.print(",");
        Serial.print("GyrZ:");
        Serial.println(gyr[2]);
    }
    else if (m)
    {
        Serial.print("MagX:");
        Serial.print(mag[0]);
        Serial.print(",");
        Serial.print("MagY:");
        Serial.print(mag[1]);
        Serial.print(",");
        Serial.print("MagZ:");
        Serial.println(mag[2]);
    }
    else
    {
        Serial.print("Button state:");
        Serial.println(buttonPressed);
    }
}

void setup() {
    delay(5000); // Delay so setup Serial output can be observed
    Serial.begin(15200); // Will be receiving BLE data at fastest every 3 milliseconds (see delay(3)) thus up to 334 receives in a second. We receive 37 bytes so need to be able to output 12500 bytes per second (high estimate), a baud rate of 115200 gives approximately 14400 bytes per second.

    // BLE 
    if (!BLE.begin()){
        Serial.println("Starting BLE module failed!");
        while (1);
    }
    Serial.println("***Started BLE module***");

    BLE.advertise();  
    Serial.println("Started BLE advertising");

    Serial.println("***BLE initialization complete***");
    Serial.println();
    Serial.println();

    BLE.scanForUuid(BLE_UUID_IMU_SERVICE);
    Serial.println("Started scanning for uuid");
}


void loop() {
    BLEDevice peripheral = BLE.available();
    Serial.println("Discovering peripheral device");
    // Exit loop() until peripheral is found (Note loop() will be called again, so like calling continue)
    if (!peripheral){
        return;
    }

    Serial.println("***Peripheral device discovered***");
    Serial.print("MAC address: ");
    Serial.println(peripheral.address());
    Serial.print("Device name: ");
    Serial.println(peripheral.localName());
    Serial.print("Advertised service UUID: ");
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != BLE_LOCAL_NAME_IMU){
        Serial.println("Incorrect local name");
        return;
    }

    BLE.stopScan();

    Serial.println("Connecting to peripheral device");
    if (peripheral.connect()){
        Serial.println("Connected to peripheral device");
    }
    else{
        Serial.println("Failed to connect to peripheral device");
        return;
    }

    Serial.println("Discovering peripheral device attributes");
    if (peripheral.discoverAttributes()){
        Serial.println("Peripheral device attributes discovered");
    }
    else{
        Serial.println("Failed to discover peripheral device attributes");
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicAcc = peripheral.characteristic(BLE_UUID_ACC);
    if (!IMUCharacteristicAcc){
        Serial.println("Peripheral doesn't have Acc characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.canSubscribe()){
        Serial.println("Peripheral does not have subscribeable Acc characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.subscribe()){
        Serial.println("Did not successfully subscribe to Acc characteristic!");
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicGyr = peripheral.characteristic(BLE_UUID_GYR);
    if (!IMUCharacteristicGyr){
        Serial.println("Peripheral doesn't have Gyr characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicGyr.canSubscribe()){
        Serial.println("Peripheral does not have subscribeable Gyr characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicGyr.subscribe()){
        Serial.println("Did not successfully subscribe to Gyr characteristic!");
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicMag = peripheral.characteristic(BLE_UUID_MAG);
    if (!IMUCharacteristicMag){
        Serial.println("Peripheral doesn't have Mag characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicMag.canSubscribe()){
        Serial.println("Peripheral does not have subscribeable Mag characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicMag.subscribe()){
        Serial.println("Did not successfully subscribe to Mag characteristic!");
        peripheral.disconnect();
        return;
    }

    BLECharacteristic ButtonPressedCharacteristic = peripheral.characteristic(BLE_UUID_BUTTON);
    if (!ButtonPressedCharacteristic){
        Serial.println("Peripheral doesn't have Button Pressed characteristic!");
        peripheral.disconnect();
        return;
    } else if (!ButtonPressedCharacteristic.canSubscribe()){
        Serial.println("Peripheral does not have subscribeable Button Pressed characteristic!");
        peripheral.disconnect();
        return;
    } else if (!ButtonPressedCharacteristic.subscribe()){
        Serial.println("Did not successfully subscribe to Button Pressed characteristic!");
        peripheral.disconnect();
        return;
    }

    while (peripheral.connected()) {
        delay(3); // Delay to not overwhelm serial buffer
        if (IMUCharacteristicAcc.valueUpdated()){
            IMUCharacteristicAcc.readValue(&acc, ARDUINO_FLOAT_LENGTH_BYTES*ACC_ELEMENTS);
        }
        if (IMUCharacteristicGyr.valueUpdated()){
            IMUCharacteristicGyr.readValue(&gyr, ARDUINO_FLOAT_LENGTH_BYTES*GYR_ELEMENTS);
        }
        if (IMUCharacteristicMag.valueUpdated()){
            IMUCharacteristicMag.readValue(&mag, ARDUINO_FLOAT_LENGTH_BYTES*MAG_ELEMENTS);
        }
        if (ButtonPressedCharacteristic.valueUpdated()){
            ButtonPressedCharacteristic.readValue(&buttonPressed, ARDUINO_BOOL_LENGTH_BYTES);
        }
        if (Serial.available())
        {
            char data = Serial.read();
            if (data == 'a'){
                print_a = !print_a;
            }
            if (data == 'g'){
                print_g = !print_g;
            }
            if (data == 'm'){
                print_m = !print_m;
            }
        }
        printIMUVals(print_a, print_g, print_m);
    }

    Serial.println("***Peripheral device disconnected***");
    Serial.println();
    Serial.println();

    BLE.scanForUuid(BLE_UUID_IMU_SERVICE);
    Serial.println("Started scanning for uuid");
}
