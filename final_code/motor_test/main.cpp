#include <wiringPi.h>
#include <iostream>
#include <vector>

using namespace std;

void cleanup(vector<int>& usedPins) {
    for (int i = 0; i < usedPins.size(); i++) {
        pinMode(usedPins[i], INPUT);
    }
}

int main(int argc, char *argv[]) {
    
    vector<int> usedPins = {18, 12, 13, 19};

    wiringPiSetupGpio();
    cleanup(usedPins);
    int FORWARD_LEFT = 18;
    // int BACKWARD_LEFT = 12;
    int FORWARD_RIGHT = 19;
    // int BACKWARD_RIGHT = 13;
    
    pinMode(FORWARD_LEFT, PWM_OUTPUT);
    // pinMode(FORWARD_LEFT, OUTPUT);
    // pinMode(BACKWARD_LEFT, PWM_OUTPUT);
    pinMode(FORWARD_RIGHT, PWM_OUTPUT);
    // pinMode(FORWARD_RIGHT, OUTPUT);
    // pinMode(BACKWARD_RIGHT, PWM_OUTPUT);

    int start_i = argc > 1 ? stoi(argv[1]) : 1024;

    for (int i = start_i; i >= 0; i--) {
        pwmWrite(FORWARD_LEFT, i);
        pwmWrite(FORWARD_RIGHT, i);
        // digitalWrite(18, HIGH);
        // digitalWrite(19, HIGH);
        cout << i << '\n';
        delay(500);
    }

    cleanup(usedPins);
}
