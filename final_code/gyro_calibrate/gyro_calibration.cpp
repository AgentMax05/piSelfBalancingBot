#include "mpu6050.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>

using namespace std;
using namespace chrono;

int main(int argc, char *argv[]) {
    mpu6050 gyroSensor(0x68, GYRO_RANGE_2000DEG);
    
    vector<double> data = {0, 0, 0};
    int testCount = 0;

    int calibration_duration = argc > 1 ? stoi(argv[1]) : 30;

    auto start = high_resolution_clock::now();
    cout << "starting test\n";
    while (true) {
        vector<double> newData = gyroSensor.get_gyro_data();
        data[0] += newData[0];
        data[1] += newData[1];
        data[2] += newData[2];
        testCount++;

        if (duration_cast<seconds>(high_resolution_clock::now() - start).count() >= calibration_duration) {
            break;
        }
    }
    data[0] = data[0] / testCount;
    data[1] = data[1] / testCount;
    data[2] = data[2] / testCount;

    cout << "offsets: x: " << data[0] << ", y: " << data[1] << ", z: " << data[2] << '\n';
    cout << "writing offsets to file ../offsets.txt\n";
    
    ofstream offsetsFile("../offsets.txt");
    offsetsFile << data[0] << ',' << data[1] << ',' << data[2];
    offsetsFile.close();

    cout << "file written\n";
}