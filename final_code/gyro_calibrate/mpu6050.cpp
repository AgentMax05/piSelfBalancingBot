#include <wiringPi.h>
#include <wiringPiI2C.h>
#include <vector>
#include <iostream>
#include "mpu6050.hpp"

using namespace std;

mpu6050::mpu6050(int address, int gyro_range, int accel_range, vector<double> new_gyro_offsets) : address{address} {
    file = wiringPiI2CSetup(0x68);
    wiringPiI2CWriteReg8(file, PWR_MGMT_1, 0x00);

    set_gyro_range(gyro_range);
    set_accel_range(accel_range);

    gyro_offsets = new_gyro_offsets;
}

int mpu6050::read_register(int input_register) {
    // int high = wiringPiI2CReadReg8(file, input_register);
    short high = wiringPiI2CReadReg8(file, input_register) << 8;
    // int low = wiringPiI2CReadReg8(file, input_register + 1);
    short low = wiringPiI2CReadReg8(file, input_register + 1);
    
    // cout << "high: " << high << " low: " << low << '\n';

    // int value = (high << 8) + low;
    int value = high + low;
    return value;
}

void mpu6050::set_gyro_range(int gyro_range) {
    wiringPiI2CWriteReg8(file, GYRO_CONFIG, 0x00);
    wiringPiI2CWriteReg8(file, GYRO_CONFIG, gyro_range);
}

void mpu6050::set_accel_range(int accel_range) {
    wiringPiI2CWriteReg8(file, ACCEL_CONFIG, 0x00);
    wiringPiI2CWriteReg8(file, ACCEL_CONFIG, accel_range);
}

int mpu6050::read_gyro_range() {
    return wiringPiI2CReadReg8(file, GYRO_CONFIG);
}

int mpu6050::read_accel_range() {
    return wiringPiI2CReadReg8(file, ACCEL_CONFIG);
}

vector<double> mpu6050::get_gyro_data() {
    cout << "running get_gyro_data()\n";
    double x = read_register(GYRO_XOUT0);
    double y = read_register(GYRO_YOUT0);
    double z = read_register(GYRO_ZOUT0);

    cout << x << ' ' << y << ' ' << z << '\n';

    int gyro_range = read_gyro_range();
    cout << "59\n";
    float gyro_scale_modifier = gyro_range_to_scale.at(gyro_range);
    cout << "61\n";
    x = (x / gyro_scale_modifier) - gyro_offsets[0];
    y = (y / gyro_scale_modifier) - gyro_offsets[1];
    z = (z / gyro_scale_modifier) - gyro_offsets[2];
    cout << "65\n";
    return {x, y, z};
}

vector<double> mpu6050::get_accel_data() {
    double x = read_register(ACCEL_XOUT0);
    double y = read_register(ACCEL_YOUT0);
    double z = read_register(ACCEL_ZOUT0);

    int accel_range = read_accel_range();
    float accel_scale_modifier = accel_range_to_scale.at(accel_range);

    x = x / accel_scale_modifier;
    y = y / accel_scale_modifier;
    z = z / accel_scale_modifier;

    return {x, y, z};
}


    