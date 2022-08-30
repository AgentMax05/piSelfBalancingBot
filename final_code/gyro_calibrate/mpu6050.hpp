#pragma once

#include <vector>
#include <map>

using namespace std;

// struct mpu6050_options {
//     const int GYRO_RANGE_250DEG = 0x00;
//     const int GYRO_RANGE_500DEG = 0x08;
//     const int GYRO_RANGE_1000DEG = 0x10;
//     const int GYRO_RANGE_2000DEG = 0x18;

//     const int ACCEL_RANGE_2G = 0x00;
//     const int ACCEL_RANGE_4G = 0x08;
//     const int ACCEL_RANGE_8G = 0x10;
//     const int ACCEL_RANGE_16G = 0x18;
// };

const int GYRO_RANGE_250DEG = 0x00;
const int GYRO_RANGE_500DEG = 0x08;
const int GYRO_RANGE_1000DEG = 0x10;
const int GYRO_RANGE_2000DEG = 0x18;

const int ACCEL_RANGE_2G = 0x00;
const int ACCEL_RANGE_4G = 0x08;
const int ACCEL_RANGE_8G = 0x10;
const int ACCEL_RANGE_16G = 0x18;

class mpu6050 {
    public:
        mpu6050(int address, int gyro_range = GYRO_RANGE_250DEG, int accel_range = ACCEL_RANGE_2G, vector<double> gyro_offsets = {0, 0, 0});
        void set_gyro_range(int gyro_range);
        void set_accel_range(int accel_range);

        int read_gyro_range();
        int read_accel_range();

        int read_register(int register);

        vector<double> get_gyro_data();
        vector<double> get_accel_data();

    private:
        // registers:
        const int PWR_MGMT_1 = 0x6B;
        const int PWR_MGMT_2 = 0x6C;
        const int ACCEL_XOUT0 = 0x3B;
        const int ACCEL_YOUT0 = 0x3D;
        const int ACCEL_ZOUT0 = 0x3F;
        const int GYRO_XOUT0 = 0x43;
        const int GYRO_YOUT0 = 0x45;
        const int GYRO_ZOUT0 = 0x47;
        const int ACCEL_CONFIG = 0x1C;
        const int GYRO_CONFIG = 0x1B;

        // scale modifiers for ranges:
        const double ACCEL_SCALE_MODIFIER_2G = 16384.0;
        const double ACCEL_SCALE_MODIFIER_4G = 8192.0;
        const double ACCEL_SCALE_MODIFIER_8G = 4096.0;
        const double ACCEL_SCALE_MODIFIER_16G = 2048.0;

        const double GYRO_SCALE_MODIFIER_250DEG = 131.0;
        const double GYRO_SCALE_MODIFIER_500DEG = 65.5;
        const double GYRO_SCALE_MODIFIER_1000DEG = 32.8;
        const double GYRO_SCALE_MODIFIER_2000DEG = 16.4;

        const map<int, double> gyro_range_to_scale {
            {GYRO_RANGE_250DEG, GYRO_SCALE_MODIFIER_250DEG},
            {GYRO_RANGE_500DEG, GYRO_SCALE_MODIFIER_500DEG},
            {GYRO_RANGE_1000DEG, GYRO_SCALE_MODIFIER_1000DEG},
            {GYRO_RANGE_2000DEG, GYRO_SCALE_MODIFIER_2000DEG}
        };

        const map<int, double> accel_range_to_scale {
            {ACCEL_RANGE_2G, ACCEL_SCALE_MODIFIER_2G},
            {ACCEL_RANGE_4G, ACCEL_SCALE_MODIFIER_4G},
            {ACCEL_RANGE_8G, ACCEL_SCALE_MODIFIER_8G},
            {ACCEL_RANGE_16G, ACCEL_SCALE_MODIFIER_16G}
        };

        int address, file;
        vector<double> gyro_offsets;
};