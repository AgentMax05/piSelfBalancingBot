#include <wiringPi.h>
#include <iostream>
#include <chrono>
#include <thread>
#include "mpu6050.hpp"
#include "pid.hpp"

#include <string.h> // for flag checking
#include <fstream> // DEBUG (write) and reading offsets (read)
#include <signal.h> // for catching ctrl+c

using namespace std;
using namespace std::chrono;

vector<int> usedPins = {18, 12, 13, 19};

void cleanup(vector<int>& usedPins) {
    cout << "cleaning gpio\n";
    for (int i = 0; i < usedPins.size(); i++) {
        pinMode(usedPins[i], INPUT);
    }
}

int signum(double value) {
    return (value > 0) - (value < 0);
}

double mapPid(double pidMax, double resultMin, double resultMax, double result) {
    if (result == 0) return 0;
    int sign = signum(result);
    result = abs(result);
    return ((result/pidMax) * (resultMax-resultMin) + resultMin) * sign;
}

// tracks angle
// axis: 0 = x, 1 = y, 2 = z
void angleTracker(mpu6050 &gyroSensor, double &angle, int axis) {
    auto previousTime = high_resolution_clock::now();
    while (true) {
        double rotation = gyroSensor.get_gyro_data()[axis];
        angle += rotation * (duration_cast<microseconds>(high_resolution_clock::now() - previousTime).count() / 1000000.0);
        previousTime = high_resolution_clock::now();
    }
}

bool hasFlag(int argc, char *argv[], string flag) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], flag.c_str()) == 0) {
            return true;
        }
    }
    return false;
}

string getFlagArg(int argc, char *argv[], string flag) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], flag.c_str()) == 0 && i + 1 < argc) {
            return argv[i + 1];
        }
    }
    return "";
}

vector<string> splitString(string input, char delimiter) {
    vector<string> output = {};
    int lastSplit = 0;
    input += delimiter; // add delimiter to the end to make sure end of input is added
    for (int i = 0; i < input.size(); i++) {
        if (input[i] == delimiter) {
            if (i == 0) {
                lastSplit = 1;
                continue;
            }
            output.push_back(input.substr(input.begin() + lastSplit, i - lastSplit));
            lastSplit = i + 1;
        }
    }
    return output;
}

void ctrlc_callback(int sig) {
    cleanup(usedPins);
    exit(sig);
}

int main(int argc, char *argv[]) {
    // mpu6050 sensor(0x68, GYRO_RANGE_2000DEG);
    // double angle = 0;

    signal(SIGINT, ctrlc_callback);
    
    // vector<int> usedPins = {18, 12, 13, 19};
    wiringPiSetupGpio();
    cleanup(usedPins);

    int FORWARD_LEFT = 18;
    int BACKWARD_LEFT = 12;
    int FORWARD_RIGHT = 19;
    int BACKWARD_RIGHT = 13;
    
    // pinMode(FORWARD_LEFT, PWM_OUTPUT);
    // pinMode(BACKWARD_LEFT, PWM_OUTPUT);
    // pinMode(FORWARD_RIGHT, PWM_OUTPUT);
    // pinMode(BACKWARD_RIGHT, PWM_OUTPUT);

    // pwmWrite(18, 1024);
    // pwmWrite(FORWARD_RIGHT, 1024);
    // delay(5000);
    // this_thread::sleep_for(seconds(5));
    // int start_i = argc > 1 ? stoi(argv[1]) : 1024;

    // for (int i = start_i; i >= 0; i--) {
    //     pwmWrite(18, i);
    //     pwmWrite(19, i);
    //     cout << i << '\n';
    //     delay(500);
    // }
    // mpu6050 gyroSensor(0x68, GYRO_RANGE_2000DEG, ACCEL_RANGE_2G, {-16.5033, 0.679927, -2.37347});

    // read offsets from offsets.txt (set by gyro_calibrate)
    string line, data = "";
    ifstream offsetsFile("./offsets.txt");
    if (offsetsFile.is_open()) {
        while (offsetsFile) {
            getline(offsetsFile, line);
            data += line;
        }
        offsetsFile.close();
    }

    vector<string> dataVector = splitString(data, ',');
    vector<double> rawOffsets = {};
    for (int i = 0; i < dataVector.size(); i++) {
        rawOffsets.push_back(stod(dataVector[i]));
    }
    vector<double> offsets = rawOffsets.size() == 3 ? rawOffsets : vector<double>({-16.4125, 0.624438, -2.38807});

    mpu6050 gyroSensor(0x68, GYRO_RANGE_2000DEG, ACCEL_RANGE_2G, offsets);
    double angle = 0;

    double kp, ki, kd;

    if (argc >= 4) {
        // kp = stoi(argv[1]);
        // ki = stoi(argv[2]);
        // kd = stoi(argv[3]);
        kp = stod(argv[1]);
        ki = stod(argv[2]);
        kd = stod(argv[3]);
    } else {
        kp = 1.0;
        ki = 1.0;
        kd = 1.0;
    }

    cout << "constants: " << kp << " " << ki << " " << kd << '\n';

    PID drivePID(kp, ki, kd, 1024, -1024);
    // drivePID.setTarget(0);
    // drivePID.setTarget(10);
    double target;
    if (hasFlag(argc, argv, "--target")) {
        target = stod(getFlagArg(argc, argv, "--target"));
    } else {
        target = 0;
    }
    drivePID.setTarget(target);
    cout << "target: " << target << '\n';

    thread gyroThread(angleTracker, ref(gyroSensor), ref(angle), 1);

    // double pwmMin = 760;
    // double pwmMin = 630; // original pwmMin
    // double pwmMin = 700;
    double pwmMin = 670;
    double pwmMax = 1024;

    // DEBUG
    bool DEBUG = hasFlag(argc, argv, "-d") || hasFlag(argc, argv, "--debug");
    // bool DEBUG = true;

    vector<double> angleData = {};
    vector<double> motorOutputData = {};
    vector<double> pData = {};
    vector<double> iData = {};
    vector<double> dData = {};
    vector<double> output = {};
    vector<double> time = {};
    double totalTime = 0;

    auto startTime = high_resolution_clock::now();
    // END DEBUG

    // for (int i = 0; i < argc; i++) {
    //     cout << argv[i] << '\n';
    // }
    int pidDelay; // pid delay in ms
    // cout << hasFlag(argc, argv, "--delay") << '\n';
    if (hasFlag(argc, argv, "--delay")) {
        pidDelay = stoi(getFlagArg(argc, argv, "--delay"));
    } else {
        pidDelay = 1;
        // pidDelay = 10;
    }
    cout << "delay: " << pidDelay << '\n';

    auto previousTime = high_resolution_clock::now();

    try {
        while (true) {
            auto duration = duration_cast<microseconds>(high_resolution_clock::now() - previousTime);
            previousTime = high_resolution_clock::now();
            double rawOutput = drivePID.compute(angle, duration.count() / 1000000.0);
            double pidCompute = mapPid(1024, pwmMin, pwmMax, rawOutput);
            // cout << abs(int(pidCompute)) << " " << signum(pidCompute) << '\n';
            if (signum(pidCompute) > 0) {
                // pwmWrite(BACKWARD_LEFT, 0);
                // pwmWrite(BACKWARD_RIGHT, 0);
                pinMode(BACKWARD_LEFT, INPUT);
                pinMode(BACKWARD_RIGHT, INPUT);
                pinMode(FORWARD_LEFT, PWM_OUTPUT);
                pinMode(FORWARD_RIGHT, PWM_OUTPUT);

                pwmWrite(FORWARD_LEFT, abs(int(pidCompute)));
                pwmWrite(FORWARD_RIGHT, abs(int(pidCompute)));
            } else if (signum(pidCompute) < 0) {
                // pwmWrite(FORWARD_LEFT, 0);
                // pwmWrite(FORWARD_RIGHT, 0);
                pinMode(BACKWARD_LEFT, PWM_OUTPUT);
                pinMode(BACKWARD_RIGHT, PWM_OUTPUT);
                pinMode(FORWARD_LEFT, INPUT);
                pinMode(FORWARD_RIGHT, INPUT);

                pwmWrite(BACKWARD_LEFT, abs(int(pidCompute)));
                pwmWrite(BACKWARD_RIGHT, abs(int(pidCompute)));
            } else {
                // pwmWrite(FORWARD_LEFT, 0);
                // pwmWrite(FORWARD_RIGHT, 0);
                // pwmWrite(BACKWARD_LEFT, 0);
                // pwmWrite(BACKWARD_RIGHT, 0);
                pinMode(BACKWARD_LEFT, INPUT);
                pinMode(BACKWARD_RIGHT, INPUT);
                pinMode(FORWARD_LEFT, INPUT);
                pinMode(FORWARD_RIGHT, INPUT);
            }

            if (DEBUG) {
                vector<double> pidRawData = drivePID.getRaw();
                angleData.push_back(angle);
                motorOutputData.push_back((int)pidCompute);
                pData.push_back(pidRawData[0]);
                iData.push_back(pidRawData[1]);
                dData.push_back(pidRawData[2]);
                output.push_back(rawOutput);
                totalTime += duration.count() / 1000000.0;
                time.push_back(totalTime);
            }

            // cout << "angle: " << angle << " output: " << pidCompute << " raw: " << rawOutput << '\n';
            
            // this_thread::sleep_for(milliseconds(100));

            delay(pidDelay);

            if (DEBUG && duration_cast<seconds>(high_resolution_clock::now() - startTime).count() >= 10) {
                break;
            }
        }

        cleanup(usedPins);

        if (DEBUG) {
            ofstream dataFile("./robotData.txt");
            vector<vector<double>> dataVectors = {angleData, motorOutputData, pData, iData, dData, output, time};
            vector<string> headers = {"angle", "motor", "p", "i", "d", "output", "time"};
            for (int i = 0; i < dataVectors.size(); i++) {
                dataFile << headers[i];
                for (int j = 0; j < dataVectors[i].size(); j++) {
                    dataFile << "," << dataVectors[i][j];
                }
                dataFile << '\n';
            }
            // dataFile << headers[headers.size() - 1];
            // for (int i = 0; i < dataVectors[0].size(); i++) {
            //     dataFile << ',' << i;
            // }
            dataFile << '\n';
            dataFile.close();
        }
    } catch (...) {
        cout << "ran into error\n";
        cleanup(usedPins);
    }
}
