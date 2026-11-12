/*
** gyro.cpp
** 23 septembre 2026
** TRAN Olivier
*/

#define SDA_GYRO_PIN 10
#define SCL_GYRO_PIN 11

#include "gyro.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>


namespace
{
    Adafruit_MPU6050 mpu;
    TwoWire gyroWire(1);
}

namespace Gyro
{
    bool begin()
    {
        if (!gyroWire.begin(SDA_GYRO_PIN, SCL_GYRO_PIN, 100000)) {
            Serial.println("Gyro I2C init failed");
            Serial.println("Check SDA and SCL pins for the MPU6050.");
            return false;
        }

        if (!mpu.begin(0x68, &gyroWire)) {
            Serial.println("Gyro not found");
            return false;
        }

        Serial.println("Gyro found");

        mpu.setAccelerometerRange(MPU6050_RANGE_4_G);
        mpu.setGyroRange(MPU6050_RANGE_500_DEG);
        mpu.setFilterBandwidth(MPU6050_BAND_44_HZ);
        Serial.println("Gyro initialized");
        return true;
    }

    bool read(GyroData &data)
    {
        sensors_event_t accel;
        sensors_event_t gyro;
        sensors_event_t temp;

        if (!mpu.getEvent(&accel, &gyro, &temp)) {
            return false;
        }

        // Roll and Pitch (degrees)
        data.roll = atan2(accel.acceleration.y,
                        accel.acceleration.z) * 180.0 / PI;

        data.pitch = atan2(-accel.acceleration.x,
                        sqrt(accel.acceleration.y * accel.acceleration.y +
                                accel.acceleration.z * accel.acceleration.z))
                    * 180.0 / PI;

        // Yaw (degrees)
        static float yaw = 0.0;
        static unsigned long lastTime = 0;

        unsigned long currentTime = micros();

        if (lastTime != 0) {
            float dt = (currentTime - lastTime) / 1000000.0;

            yaw += gyro.gyro.z * dt * 180.0 / PI;
        }

        lastTime = currentTime;

        data.yaw = yaw;

        data.temperature = temp.temperature;

        return true;
    }
}
