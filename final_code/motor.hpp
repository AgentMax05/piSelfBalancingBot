#pragma once

class Motor {
    public:
        int forwardPin, backPin;
        double pwmMin, pwmMax;

        Motor(int fPin, int bPin, double min, double max);
        void run(double rawOutput);

    private:
        double map(double rawOutput);
};