
#define SDA_GYRO_PIN 10
#define SCL_GYRO_PIN 11
#define GYRO_DEADZONE 0.015f

#include "gyro.h"
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

namespace
{
    Adafruit_MPU6050 mpu;
    TwoWire gyroWire(1);

    float gyroZOffset = 0.0f;
}

float angle_normalise(float angle) //without it the angle values will go to 179 to -180 and vice-versa.
{
    while (angle > 180.0f)
        angle -= 360.0f;
    while (angle < -180.0f)
        angle += 360.0f;
}

namespace Gyro
{
    bool begin()
    {
        if (!gyroWire.begin(SDA_GYRO_PIN, SCL_GYRO_PIN, 100000)) {
            Serial.println("Gyro I2C init failed");
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

        // Calibrate gyroscope Z-axis
        Serial.println("Calibrating gyro...");

        sensors_event_t accel, rotation, temp;

        const int samples = 2000;
        float total = 0.0f;

        for (int i = 0; i < samples; i++) {
            mpu.getEvent(&accel, &rotation, &temp);
            total += rotation.gyro.z;
            delay(2);
        }

        gyroZOffset = total / samples;

        Serial.print("Yaw offset: ");
        Serial.println(gyroZOffset, 6);

        Serial.println("Gyro initialized");

        return true;
    }

    bool read(GyroData &data)
    {
        sensors_event_t accel;
        sensors_event_t rotation;
        sensors_event_t temp;

        if (!mpu.getEvent(&accel, &rotation, &temp)) {
            return false;
        }

        // Roll and Pitch (degrees)
        // Roll has also a slight drift, to fix
        data.roll = atan2(accel.acceleration.y, accel.acceleration.z) * 180.0 / PI;

        data.pitch = atan2(
            -accel.acceleration.x,
            sqrt(
                accel.acceleration.y * accel.acceleration.y +
                accel.acceleration.z * accel.acceleration.z
            )
        ) * 180.0 / PI;

        // Yaw (degrees)
        static float yaw = 0.0f;
        static unsigned long lastTime = 0;

        unsigned long currentTime = micros();

        if (lastTime != 0) {
            float dt = (currentTime - lastTime) / 1000000.0f;

            // To counterbalance the drift, though it still happens to a lesser extend.
            float gyroZ = rotation.gyro.z - gyroZOffset;

            // Integrate angular velocity
            yaw += gyroZ * dt * 180.0f / PI;
        }

        lastTime = currentTime;
        data.yaw = yaw;
        data.temperature = temp.temperature;
        return true;
    }
}
