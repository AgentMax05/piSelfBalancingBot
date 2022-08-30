#include "pid.hpp"
#include <chrono>
#include <iostream>

using namespace std;
using namespace std::chrono;

PID::PID(double pKp, double pKi, double pKd, double pDeltaTime) {
	setConstants(pKp, pKi, pKd);
    deltaTime = pDeltaTime;
}

void PID::setTarget(double pTarget) {
    target = pTarget;
}

void PID::setConstants(double pKp, double pKi, double pKd) {
	kp = pKp;
	ki = pKi;
	kd = pKd;
}

double PID::compute(double current) {
	// deltaTime = (duration_cast<microseconds>(high_resolution_clock::now() - lastCompute).count()) / 1000000.0;
    // cout << deltaTime << '\n';
	double error = target - current;
	proportional = error;
	integral += error * deltaTime;
	derivative = (error - prevError) / deltaTime;
	prevError = error;
    // cout << target << " " << current << '\n';
    // cout << "e: " << error << " p: " << proportional << " i: " << integral << " d: " << derivative << '\n';
	// lastCompute = high_resolution_clock::now();
	return (proportional * kp) + (integral * ki) + (derivative * kd);
}

double PID::getTarget() {
	return target;
}
