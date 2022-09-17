#include "mpu6050.hpp"
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <thread>

using namespace std;
using namespace chrono;

int main(int argc, char *argv[]) {
    mpu6050 gyroSensor(0x68, GYRO_RANGE_2000DEG);
    
    int testCount = 0;

    int calibration_duration = argc > 1 ? stoi(argv[1]) : 30;
    int calibration_start_delay = argc > 2 ? stoi(argv[2]) : 30;

    if (calibration_start_delay > 0) {
        cout << "\nstarting test in " << calibration_start_delay << " seconds\n";
        if (calibration_start_delay > 5) {
            this_thread::sleep_for(seconds(calibration_start_delay - 5));
            cout << "starting program in 5 seconds\n";
            this_thread::sleep_for(seconds(5));
        } else {
            this_thread::sleep_for(seconds(calibration_start_delay));
        }
    }

    vector<double> data = {0, 0, 0};
    cout << "starting test\n" << "data[0]: " << data[0] << '\n';
    auto start = high_resolution_clock::now();
    while (true) {
        vector<double> newData = gyroSensor.get_gyro_data();
        data[0] += newData[0];
        data[1] += newData[1];
        data[2] += newData[2];
        testCount++;
        cout << "loop finished\n";
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