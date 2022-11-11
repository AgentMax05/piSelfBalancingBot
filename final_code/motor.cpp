#include "motor.hpp"

#include <wiringPi.h>
#include <math.h>

using namespace std;

int Motor::signum(int num) {
    return (num > 0) - (num < 0);
}

Motor::Motor(int fPin, int bPin, double min, double max) {
    forwardPin = fPin;
    backPin = bPin;
    pwmMin = min;
    pwmMax = max;
}

double Motor::map(double rawOutput) {
    if (rawOutput == 0) return 0;
    int sign = signum(rawOutput);
    rawOutput = abs(rawOutput);
    return ((rawOutput / 1024) * (pwmMax-pwmMin) + pwmMin) * sign;
}

void Motor::run(double rawOutput) {
    double pidCompute = map(rawOutput);

    if (signum(pidCompute) > 0) {
        pinMode(backPin, INPUT);
        pinMode(forwardPin, PWM_OUTPUT);

        pwmWrite(forwardPin, abs(int(pidCompute)))
    } else if (signum(pidCompute) < 0) {
        pinMode(backPin, PWM_OUTPUT);
        pinMode(forwardPin, INPUT);

        pwmWrite(backPin, abs(int(pidCompute)));
    } else {
        pinMode(backPin, INPUT);
        pinMode(forwardPin, INPUT);
    }
}