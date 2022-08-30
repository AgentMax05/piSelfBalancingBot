#pragma once

#include <vector>

using namespace std;

class PID {
  public:
    PID(double kp, double ki, double kd, double max, double min);
    void setConstants(double kp, double ki, double kd);
    void setTarget(double target);
    vector<double> getRaw();
    double compute(double current, double deltaTime);
  private:
    double target;
    double error, prevError = 0;
    double kp, ki, kd;
    double computeMax, computeMin;
    double proportional, integral = 0, derivative;
    double prev_derivative = 0;
    vector<double> pastError;
};