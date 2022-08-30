#pragma once

#include <chrono>

using namespace std::chrono;

class PID {
	public:
		PID(double kp, double ki, double kd, double deltaTime);
		double compute(double current);
		void setTarget(double target);
        void setConstants(double kp, double ki, double kd);
		double getTarget();
	private:
		double proportional, integral = 0, derivative;
		double kp, ki, kd;
		double target;
		double prevError = 0;
		// time_point<system_clock> lastCompute = high_resolution_clock::now();
        double deltaTime;
};
