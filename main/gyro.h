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
    float roll;
    float pitch;
    float yaw;

    float temperature;
};

namespace Gyro
{
    bool begin();
    bool read(GyroData &data);
}

#endif
