#include "pid.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

PID::PID(double new_kp, double new_ki, double new_kd, double max, double min) {
    setConstants(new_kp, new_ki, new_kd);
    computeMax = max;
    computeMin = min;
    pastError = vector<double>(350, 0);
}

void PID::setConstants(double new_kp, double new_ki, double new_kd) {
    kp = new_kp;
    ki = new_ki;
    kd = new_kd;
}

void PID::setTarget(double newTarget) {
    target = newTarget;
}

double PID::compute(double current, double deltaTime) {
    error = target - current;

    // integralTime = integralTime - pastDeltaTime[pastDeltaTime.size() - 1] + deltaTime;
    // integral = integral - pastError[pastError.size() - 1] + error;
    // pastDeltaTime.pop_back();
    // pastError.pop_pack();
    // pastDeltaTime.insert(pastDeltaTime.begin(), deltaTime);
    // pastError.insert(pastError.begin(), error);
    integral = integral - pastError[pastError.size() - 1] + error * deltaTime;
    pastError.pop_back();
    pastError.insert(pastError.begin(), error * deltaTime);

    proportional = error;
    // integral += error * deltaTime;

    // fix issue where pid runs faster than angle sensor
    // and angle values stay the same for 2 loops, 
    // causing derivative to jump to 0
    if (error == prevError) {
        derivative = prev_derivative;
    } else {
        derivative = (error - prevError) / deltaTime;
        prev_derivative = derivative;
    }

    prevError = error;
    // return max(min((proportional * kp) + (integral * ki) + (derivative * kd), computeMax), computeMin);
    // double output = (proportional * kp) + (integral * ki) + (derivative * kd);
    double output = (proportional * kp) + (integral * ki) + (derivative * kd);
    if (output > computeMax) output = computeMax;
    if (output < computeMin) output = computeMin;
    return output;
}

vector<double> PID::getRaw() {
    return {proportional * kp, integral * ki, derivative * kd};
}