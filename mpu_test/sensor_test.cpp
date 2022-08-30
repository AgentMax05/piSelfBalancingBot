#include <iostream>
#include <chrono>
#include <thread>
#include "mpu6050.hpp"
// #include "pid.hpp"

using namespace std;
using namespace std::chrono;

template <typename T>
void print_output(vector<T> output) {
    cout << "x: " << output[0] << " y: " << output[1] << " z: " << output[2] << '\n';
}

void sensor_loop(mpu6050 &sensor, double &angle) {
    auto previous_time = high_resolution_clock::now();
    while (true) {
        double z_rot = sensor.get_gyro_data()[2];
        // float duration = duration_cast<microseconds>(high_resolution_clock::now() - previous_time).count() / 1000000.0;
        // cout << duration << '\n';
        // angle += ((z_rot + 2.37517) * ((duration_cast<microseconds>(high_resolution_clock::now() - previous_time)).count() / 1000000.0));
        angle += ((z_rot + 2.3706421) * ((duration_cast<microseconds>(high_resolution_clock::now() - previous_time)).count() / 1000000.0));
        previous_time = high_resolution_clock::now();
    }
}

int main() {
    mpu6050 sensor(0x68, GYRO_RANGE_250DEG);
    double angle = 0;
    // double reading_sum = 0;
    // int reading_count;
    thread sensor_thread(sensor_loop, ref(sensor), ref(angle));
    // sensor_thread.detach();
    // this_thread::sleep_for(seconds(5));
    // auto start = high_resolution_clock::now();
    while (true) {
        cout << angle << '\n';
        // cout << sensor.get_gyro_data()[2] << '\n';
        this_thread::sleep_for(milliseconds(10));
        // reading_sum += sensor.get_gyro_data()[2];
        // reading_count++;
        // float duration = duration_cast<seconds>(high_resolution_clock::now() - start).count();
        // if (duration >= 30) {
        //     break;
        // }
    }
    // cout << "offset: " << reading_sum / reading_count << '\n';
}
