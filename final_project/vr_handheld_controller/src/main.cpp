/*
Ryan Dalby- CS 6360 Virtual Reality Final Project 

.ino for vr_handheld_controller

This code is the code behind the handheld VR controller as well as communication with the receiver.
*/

#include <Arduino.h>
#include <ArduinoBLE.h>
#include <Arduino_LSM9DS1.h>

#define BLE_LOCAL_NAME_IMU "VRHANDHELDCONTROLLER"
#define BLE_UUID_IMU_SERVICE "12307e98-a67e-45e2-b8f0-1445fa6e2cd8"
#define BLE_UUID_ACC_X "cf89ef50-4bdb-423a-a822-88aaf7f96c5a"
#define BLE_UUID_ACC_Y "fe9c88f6-b2ef-475b-9827-59890d2c087a"
#define BLE_UUID_ACC_Z "a7a21afb-8f7e-4d1a-a7d1-e7d8f6ae9d63"
#define BLE_UUID_GYR_X "1552ab31-afbb-418c-bd23-1f4527751390"
#define BLE_UUID_GYR_Y "7fa5b053-f0a0-4ec2-b663-d55c1f769f85"
#define BLE_UUID_GYR_Z "ff84445b-fd7a-4274-bf20-0b4aedbb2b97"
#define BLE_UUID_MAG_X "55584dd1-b773-44aa-a38d-ed6d9c94a851"
#define BLE_UUID_MAG_Y "288a9b57-c1ea-4ebd-8ac4-ee04782e614e"
#define BLE_UUID_MAG_Z "dfe58735-f321-42df-8122-4d523c0da19c"

BLEService IMUService(BLE_UUID_IMU_SERVICE); 
BLEFloatCharacteristic IMUCharacteristicAccX(BLE_UUID_ACC_X, BLENotify); // May need to change from notify...
// BLEFloatCharacteristic IMUCharacteristicAccX(BLE_UUID_ACC_X, BLERead | BLEWrite); // May need to change from notify...

float accX, accY, accZ; // g = 9.80665 m/s^2
float gyrX, gyrY, gyrZ; // deg/s
float magX, magY, magZ;  // muT (micro-Teslas)

void setup() {
    delay(5000); // Delay so setup Serial output can be observed
    Serial.begin(9600);

    // BLE
    if (!BLE.begin()){
        Serial.println("Starting BLE module failed!");
        while (1);
    }
    Serial.println("***Started BLE module***");

    BLE.setLocalName(BLE_LOCAL_NAME_IMU);
    BLE.setAdvertisedService(IMUService);
    Serial.println("Set advertised local name and service UUID");

    IMUService.addCharacteristic(IMUCharacteristicAccX);
    Serial.println("Added characteristics to service");

    BLE.addService(IMUService);
    Serial.println("Added service");

    IMUCharacteristicAccX.writeValue(accX);
    Serial.println("Wrote initial characteristic values");

    BLE.advertise();  
    Serial.println("Started BLE advertising");
    Serial.println("***BLE initialization complete***");
    Serial.println();
    Serial.println();


    // IMU
    if (!IMU.begin()){
        Serial.println("Starting IMU module failed!");
        while (1);
    }
    Serial.println("***IMU module started***");
    Serial.print("Accelerometer sample rate = ");
    Serial.print(IMU.accelerationSampleRate());
    Serial.println("Hz");

    Serial.print("Gyroscope sample rate = ");
    Serial.print(IMU.gyroscopeSampleRate());
    Serial.println("Hz");

    Serial.print("Magnetometer sample rate = ");
    Serial.print(IMU.magneticFieldSampleRate());
    Serial.println("Hz");
    Serial.println("***IMU initialization complete***");
    Serial.println();
    Serial.println();
}

void loop() {
    BLEDevice central = BLE.central();
    Serial.println("Discovering central device");

    if(central){
        Serial.println("Connected to central device");
        Serial.print("MAC address: ");
        Serial.println(central.address());

        while (central.connected()) {
            if (IMU.accelerationAvailable()) {
                // Acc range [-4,+4]g -/+0.122 mg
                IMU.readAcceleration(accX, accY, accZ); // g = 9.80665 m/s^2
                Serial.print("Writing: ");
                Serial.println(accX);
                IMUCharacteristicAccX.writeValue(accX);
            }
            if (IMU.gyroscopeAvailable()) {
                // Gyr range [-2000, +2000] dps +/-70 mdps
                IMU.readGyroscope(gyrX, gyrY, gyrZ); // deg/s
            }
            if (IMU.magneticFieldAvailable()) {
                // Mag range  [-400, +400] uT +/-0.014 uT
                IMU.readMagneticField(magX, magY, magZ); // muT (micro-Teslas)
            }
        }

        Serial.println("Disconnected to central device");

    }

}