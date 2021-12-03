/*
Ryan Dalby- CS 6360 Virtual Reality Final Project 

.ino for vr_handheld_controller_receiver

This code is the code behind the bluetooth receiver of the handheld VR controller demonstrating bluetooth functionality.
*/

// To prevent serial monitor crashes after a few minutes will have to explore:
//  Adding pairity bit to serial communication
//  Exploring if SRAM is being run out of on the Arduino (memory leak, likely check integrity of arrays(acc, gyr, mag) too by directly seeing integrity of the array right before crash)

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

bool printForViz = false;
bool print_a = false;
bool print_g = false;
bool print_m = false;

void printIMUValsForViz(float *acc, float *gyr, float *mag, bool a, bool g, bool m)
{
    // Will print values of Acc, Gyr, or Mag, depending on which is the first true bool is seen, if none seen prints button state
    if (a)
    {
        Serial.print(F("AccX:"));
        Serial.print(acc[0]);
        Serial.print(F(","));
        Serial.print(F("AccY:"));
        Serial.print(acc[1]);
        Serial.print(F(","));
        Serial.print(F("AccZ:"));
        Serial.println(acc[2]);
    }
    else if (g)
    {
        Serial.print(F("GyrX:"));
        Serial.print(gyr[0]);
        Serial.print(F(","));
        Serial.print(F("GyrY:"));
        Serial.print(gyr[1]);
        Serial.print(F(","));
        Serial.print(F("GyrZ:"));
        Serial.println(gyr[2]);
    }
    else if (m)
    {
        Serial.print(F("MagX:"));
        Serial.print(mag[0]);
        Serial.print(F(","));
        Serial.print(F("MagY:"));
        Serial.print(mag[1]);
        Serial.print(F(","));
        Serial.print(F("MagZ:"));
        Serial.println(mag[2]);
    }
    else
    {
        Serial.print(F("Button state:"));
        Serial.println(buttonPressed);
    }
}

void printIMUVals(float *acc, float *gyr, float *mag)
{
    // Prints button state, accelerometer data, gyroscope data, and magnetometer data sequentially
    Serial.print(buttonPressed);
    Serial.print(F(";"));

    Serial.print(acc[0]);
    Serial.print(F(";"));
    Serial.print(acc[1]);
    Serial.print(F(";"));
    Serial.print(acc[2]);
    Serial.print(F(";"));

    Serial.print(gyr[0]);
    Serial.print(F(";"));
    Serial.print(gyr[1]);
    Serial.print(F(";"));
    Serial.print(gyr[2]);
    Serial.print(F(";"));

    Serial.print(mag[0]);
    Serial.print(F(";"));
    Serial.print(mag[1]);
    Serial.print(F(";"));
    Serial.print(mag[2]);

    Serial.println(F(""));
}

void setup() {
    delay(5000); // Delay so setup Serial output can be observed
    Serial.begin(15200); 
    while (!Serial); // Wait if serial is not ready- may want to change this but for training this is useful so doesn't start until serial connection is made

    Serial.println(F("Press y in next 3 seconds for visualization print out"));
    delay(3000);
    if (Serial.available())
    {
        char data = Serial.read();
        if (data == 'y'){
            printForViz = true;
        }
    }

    // BLE 
    if (!BLE.begin()){
        Serial.println(F("Starting BLE module failed!"));
        while (1);
    }
    Serial.println(F("***Started BLE module***"));

    BLE.advertise();  
    Serial.println(F("Started BLE advertising"));

    Serial.println(F("***BLE initialization complete***"));
    Serial.println();
    Serial.println();

    BLE.scanForUuid(BLE_UUID_IMU_SERVICE);
    Serial.println(F("Started scanning for uuid"));
}


void loop() {
    BLEDevice peripheral = BLE.available();
    Serial.println(F("Discovering peripheral device"));
    // Exit loop() until peripheral is found (Note loop() will be called again, so like calling continue)
    if (!peripheral){
        return;
    }

    Serial.println(F("***Peripheral device discovered***"));
    Serial.print(F("MAC address: "));
    Serial.println(peripheral.address());
    Serial.print(F("Device name: "));
    Serial.println(peripheral.localName());
    Serial.print(F("Advertised service UUID: "));
    Serial.println(peripheral.advertisedServiceUuid());
    Serial.println();

    if (peripheral.localName() != BLE_LOCAL_NAME_IMU){
        Serial.println(F("Incorrect local name"));
        return;
    }

    BLE.stopScan();

    Serial.println(F("Connecting to peripheral device"));
    if (peripheral.connect()){
        Serial.println(F("Connected to peripheral device"));
    }
    else{
        Serial.println(F("Failed to connect to peripheral device"));
        return;
    }

    Serial.println(F("Discovering peripheral device attributes"));
    if (peripheral.discoverAttributes()){
        Serial.println(F("Peripheral device attributes discovered"));
    }
    else{
        Serial.println(F("Failed to discover peripheral device attributes"));
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicAcc = peripheral.characteristic(BLE_UUID_ACC);
    if (!IMUCharacteristicAcc){
        Serial.println(F("Peripheral doesn't have Acc characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.canSubscribe()){
        Serial.println(F("Peripheral does not have subscribeable Acc characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.subscribe()){
        Serial.println(F("Did not successfully subscribe to Acc characteristic!"));
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicGyr = peripheral.characteristic(BLE_UUID_GYR);
    if (!IMUCharacteristicGyr){
        Serial.println(F("Peripheral doesn't have Gyr characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicGyr.canSubscribe()){
        Serial.println(F("Peripheral does not have subscribeable Gyr characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicGyr.subscribe()){
        Serial.println(F("Did not successfully subscribe to Gyr characteristic!"));
        peripheral.disconnect();
        return;
    }

    BLECharacteristic IMUCharacteristicMag = peripheral.characteristic(BLE_UUID_MAG);
    if (!IMUCharacteristicMag){
        Serial.println(F("Peripheral doesn't have Mag characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicMag.canSubscribe()){
        Serial.println(F("Peripheral does not have subscribeable Mag characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicMag.subscribe()){
        Serial.println(F("Did not successfully subscribe to Mag characteristic!"));
        peripheral.disconnect();
        return;
    }

    BLECharacteristic ButtonPressedCharacteristic = peripheral.characteristic(BLE_UUID_BUTTON);
    if (!ButtonPressedCharacteristic){
        Serial.println(F("Peripheral doesn't have Button Pressed characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!ButtonPressedCharacteristic.canSubscribe()){
        Serial.println(F("Peripheral does not have subscribeable Button Pressed characteristic!"));
        peripheral.disconnect();
        return;
    } else if (!ButtonPressedCharacteristic.subscribe()){
        Serial.println(F("Did not successfully subscribe to Button Pressed characteristic!"));
        peripheral.disconnect();
        return;
    }

    while (peripheral.connected()) {
        delay(5); // Delay to not overwhelm serial buffer
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
        if (printForViz)
        {
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
            printIMUValsForViz(acc, gyr, mag, print_a, print_g, print_m);
        }
        else
        {
            printIMUVals(acc, gyr, mag);
        }
    }

    Serial.println(F("***Peripheral device disconnected***"));
    Serial.println();
    Serial.println();

    BLE.scanForUuid(BLE_UUID_IMU_SERVICE);
    Serial.println(F("Started scanning for uuid"));
}
