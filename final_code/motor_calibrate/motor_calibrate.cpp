#include <wiringPi.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <fstream>

#include <thread>

using namespace std;

void cleanup(vector<int>& usedPins) {
    for (int i = 0; i < usedPins.size(); i++) {
        pinMode(usedPins[i], INPUT);
    }
}

void runTest(int pin, int &dutyCycle, bool &running) {
    pinMode(pin, PWM_OUTPUT);
    while (running) {
        pwmWrite(pin, dutyCycle);
        cout << dutyCycle << '\n';
        dutyCycle++;
        delay(250);
    }
    pinMode(pin, INPUT);
}

int main(int argc, char *argv[]) {
    vector<int> usedPins = {18, 19};
    
    int startDC = 500;
    if (argc > 1) {
        startDC = stoi(argv[1]);
    }

    wiringPiSetupGpio();
    cleanup(usedPins);
    cout << "Left Motor (press ENTER to stop):\n";
    
    bool testRunning = true;

    int leftDC = startDC, rightDC = startDC;

    thread leftTest(runTest, 18, ref(leftDC), ref(testRunning));

    cin.ignore();
    testRunning = false;
    leftTest.join();

    testRunning = true;
    cout << "\nRight Motor (press ENTER to stop):\n";
    thread rightTest(runTest, 19, ref(rightDC), ref(testRunning));
    
    cin.ignore();
    testRunning = false;
    rightTest.join();

    cleanup(usedPins);

    cout << "\nLeft min: " << leftDC << "\nRight min: " << rightDC << '\n';

    cout << "writing results to ../motors.txt\n";

    ofstream outputFile("../motors.txt");
    outputFile << leftDC << ',' << rightDC;
    outputFile.close();

    cout << "file written\n";
}