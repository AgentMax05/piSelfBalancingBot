#include <wiringPi.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <string>
#include "mpu6050.hpp"
#include "pid.hpp"
#include "motor.hpp"

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
            // output.push_back(input.substr(input.begin() + lastSplit, i - lastSplit));
            output.push_back(input.substr(lastSplit, i - lastSplit));
            lastSplit = i + 1;
        }
    }
    return output;
}

string readFile(string filename) {
    string line, data = "";
    ifstream file(filename);
    if (file.is_open()) {
        while (file) {
            getline(file, line);
            data += line;
        }
        file.close();
    }
    return data;
}

void ctrlc_callback(int sig) {
    cleanup(usedPins);
    exit(sig);
}

int main(int argc, char *argv[]) {

    // print out help if --help is entered
    if (hasFlag(argc, argv, "--help") || hasFlag(argc, argv, "-h")) {
        cout << "\nusage: sudo ./main <p> <i> <d> [-h | --help] [--target <target-angle>] [--delay <pid-delay-ms>] [--integralTime <integral-time>] [--startDelay <start-delay-s>] [-d | --debug <debug-duration-sec>] [--motorCalibrate <min-pwm-l,min-pwm-r>]\n";
        cout << "\n<p>: the pid proportional gain\n<i>: the pid integral gain\n<d>: the pid derivative gain\n";
        cout << "\n--target <target-angle>: set the target angle of the pid control (default 0)\n";
        cout << "--delay <pid-delay-ms>: set the delay between each pid iteration in ms (default 1)\n";
        cout << "--integralTime <integral-time>: set the delta time for the integral in seconds (0 is continuous, default 350)\n";
        cout << "--startDelay <start-delay-s>: set a delay in the beginning of the program in seconds (default 0)\n";
        cout << "-d | --debug <debug-duration-sec>: track all variables and write to robotData.txt for <debug-duration-sec> seconds (default 10)\n";
        cout << "--motorCalibrate <min-pwm-l,min-pwm-r>: set minimum pwm values for left and right motor, comma separated (default 670)\n\n";
        return 0;
    }

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
    
    // read offsets from offsets.txt (set by gyro_calibrate)
    string offsetsData = readFile("./offsets.txt");

    vector<string> offsetsDataVector = splitString(offsetsData, ',');
    vector<double> rawOffsets = {};
    for (int i = 0; i < offsetsDataVector.size(); i++) {
        rawOffsets.push_back(stod(offsetsDataVector[i]));
    }
    vector<double> offsets = rawOffsets.size() == 3 ? rawOffsets : vector<double>({-16.4125, 0.624438, -2.38807});

    mpu6050 gyroSensor(0x68, GYRO_RANGE_2000DEG, ACCEL_RANGE_2G, offsets);

    // execute start delay
    string startDelayVal = getFlagArg(argc, argv, "--startDelay");
    if (startDelayVal != "") {
        int startDelay = stoi(startDelayVal);
        cout << "\nstarting program in " << startDelay << "seconds\n";
        if (startDelay > 5) {
            this_thread::sleep_for(seconds(startDelay - 5));
            cout << "starting program in 5 seconds\n";
            this_thread::sleep_for(seconds(5));
        } else {
            this_thread::sleep_for(seconds(startDelay));
        }
    }

    double angle = 0;

    double kp, ki, kd;

    if (argc >= 4) {
        kp = stod(argv[1]);
        ki = stod(argv[2]);
        kd = stod(argv[3]);
    } else {
        kp = 1.0;
        ki = 1.0;
        kd = 1.0;
    }

    cout << "constants: " << kp << " " << ki << " " << kd << '\n';

    int integralTime;
    string integralTimeVal = getFlagArg(argc, argv, "--integralTime");
    if (integralTimeVal == "") {
        integralTime = 0;
    } else {
        integralTime = stoi(integralTimeVal);
    }

    PID drivePID(kp, ki, kd, 1024, -1024, integralTime);
    double target;
    if (hasFlag(argc, argv, "--target")) {
        target = stod(getFlagArg(argc, argv, "--target"));
    } else {
        target = 0;
    }
    drivePID.setTarget(target);
    cout << "target: " << target << '\n';

    thread gyroThread(angleTracker, ref(gyroSensor), ref(angle), 1);

    double pwmMinL = 670, pwmMinR = 670;
    vector<string> mins = {to_string(pwmMinL), to_string(pwmMinR)};
    if (hasFlag(argc, argv, "--motorCalibrate")) {
        mins = splitString(getFlagArg(argc, argv, "--motorCalibrate"), ',');
        pwmMinL = stoi(mins[0]);
        pwmMinR = stoi(mins[1]);
    } else {
        // attempt to read file set by motor_calibrate
        string motorData = readFile("./motors.txt");
        vector<string> motorDataVector = splitString(motorData, ',');
        if (motorDataVector.size() == 2) {
            pwmMinL = stod(motorDataVector[0]);
            pwmMinR = stod(motorDataVector[1]);
        }
    }
    cout << "motor mins: L " << stoi(mins[0]) << ", R " << stoi(mins[1]) << '\n';

    Motor leftMotor(FORWARD_LEFT, BACKWARD_LEFT, pwmMinL, 1024);
    Motor rightMotor(FORWARD_RIGHT, BACKWARD_RIGHT, pwmMinR, 1024);

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
            double pidCompute = mapPid(1024, 670, 1024, rawOutput);

            leftMotor.run(rawOutput);
            rightMotor.run(rawOutput);

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
