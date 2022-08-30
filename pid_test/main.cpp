#include <iostream>
#include <chrono>

#include "pid.hpp"

using namespace std;

int main() {
    double current = 20;
    PID pid(0.55, 0.3, 0.1, 1.0);
    pid.setTarget(40);

    double p = 0.01, i = 0.01, d = 0.01, bp, bi, bd;
    
    for (int i = 0; i < 100; i++) {
        double output = pid.compute(current);
        current += output;
        cout << current << '\n';
    }

    // double output = pid.compute(current);
    // current += output;
    // cout << output << '\n';
    // output = pid.compute(current);
    // cout << output << '\n';
}