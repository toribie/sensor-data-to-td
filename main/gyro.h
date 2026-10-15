/*
** gyro.h
** 28 septembre 2026
** TRAN Olivier
*/

#ifndef GYRO_H
#define GYRO_H

#include <Arduino.h>

struct GyroData
{
    float accelX;
    float accelY;
    float accelZ;

    float gyroX;
    float gyroY;
    float gyroZ;

    float temperature; //Temperature of the chip itself
};

namespace Gyro
{
    bool begin();
    bool read(GyroData &data);
}

#endif
