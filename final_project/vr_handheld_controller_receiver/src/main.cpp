/*
Ryan Dalby- CS 6360 Virtual Reality Final Project 

.ino for vr_handheld_controller_receiver

This code is the code behind the bluetooth receiver of the handheld VR controller.
*/

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

#define ARDUINO_FLOAT_LENGTH_BYTES 4
#define BLE_LOCAL_NAME_IMU "VRHANDHELDCONTROLLER"
#define BLE_UUID_IMU_SERVICE "12307e98-a67e-45e2-b8f0-1445fa6e2cd8"
#define BLE_UUID_ACC "cf89ef50-4bdb-423a-a822-88aaf7f96c5a"
#define BLE_UUID_GYR "1552ab31-afbb-418c-bd23-1f4527751390"
#define BLE_UUID_MAG "55584dd1-b773-44aa-a38d-ed6d9c94a851"

// #define BLE_UUID_ACC_X "cf89ef50-4bdb-423a-a822-88aaf7f96c5a"
// #define BLE_UUID_ACC_Y "fe9c88f6-b2ef-475b-9827-59890d2c087a"
// #define BLE_UUID_ACC_Z "a7a21afb-8f7e-4d1a-a7d1-e7d8f6ae9d63"
// #define BLE_UUID_GYR_X "1552ab31-afbb-418c-bd23-1f4527751390"
// #define BLE_UUID_GYR_Y "7fa5b053-f0a0-4ec2-b663-d55c1f769f85"
// #define BLE_UUID_GYR_Z "ff84445b-fd7a-4274-bf20-0b4aedbb2b97"
// #define BLE_UUID_MAG_X "55584dd1-b773-44aa-a38d-ed6d9c94a851"
// #define BLE_UUID_MAG_Y "288a9b57-c1ea-4ebd-8ac4-ee04782e614e"
// #define BLE_UUID_MAG_Z "dfe58735-f321-42df-8122-4d523c0da19c"

float acc[3]; // x,y,z g = 9.80665 m/s^2
float gyr[3]; // x,y,z deg/s
float mag[3];  // x,y,z muT (micro-Teslas)

void setup() {
    delay(5000); // Delay so setup Serial output can be observed
    Serial.begin(9600);

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
    // Serial.println("***Discovering peripheral device***");

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
        Serial.println("Peripheral doesn't have Acc X characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.canSubscribe()){
        Serial.println("Peripheral does not have subscribeable Acc X characteristic!");
        peripheral.disconnect();
        return;
    } else if (!IMUCharacteristicAcc.subscribe()){
        Serial.println("Did not successfully subscripe to Acc X characteristic!");
        peripheral.disconnect();
        return;
    }

    while (peripheral.connected()) {
        if (IMUCharacteristicAcc.valueUpdated()){
            IMUCharacteristicAcc.readValue(&acc, ARDUINO_FLOAT_LENGTH_BYTES*3);
            Serial.println("Receiving:");
            Serial.print("Acc X: ");
            Serial.println(acc[0]);
            Serial.print("Acc Y: ");
            Serial.println(acc[1]);
            Serial.print("Acc Z: ");
            Serial.println(acc[2]);
        }
    }

    Serial.println("***Peripheral device disconnected***");
    Serial.println();
    Serial.println();

    BLE.scanForUuid(BLE_UUID_IMU_SERVICE);
    Serial.println("Started scanning for uuid");
    
}
