<h3 align=center>piSelfBalancingRobot</h3>

## About
This repository contains all the programs I created to power a self balancing robot. The robot is built using a Raspberry Pi Zero W, two brushless DC motors, 
a DC motor controller board, and an MPU 6050 sensor for keeping track of the lean angle.

The main part of the program is a constant loop that will get the current angle from the MPU 6050 sensor, then plug that angle into a 
[PID control function](https://en.wikipedia.org/wiki/PID_controller), map that output to a valid motor output value, and then set the motor speed 
to that value. This loop runs endlessly until the program is aborted.

## Setup
To build the robot you will need:
- A Raspberry Pi microcomputer with GPIO headers (not necessarily a Raspberry Pi Zero W)
- 2 DC motors that are powerful enough
- A DC motor controller board that can power 2 motors (or 2 of these boards)
- Wires to connect the components
- A battery to power the Raspberry Pi (to avoid having to connect it to an outlet)
- (optional) A second battery pack to power the motors (if your motors require more than 5V)

Then, after building your robot, clone this repository to the Raspberry Pi.

CD to the `final_code` directory and build the main program by entering `bash build.sh` or running `g++ *.cpp -lwiringPi -pthread -o main`. 

Before running the main program, build the code in the `final_code/gyro_calibrate` directory by running `g++ *.cpp -lwiringPi -pthread -o gyro_calibrate`, and run the
program with `./gyro_calibrate`. This will run a calibration program for approximately a minute to determine offsets for your angle sensor. Please do not move
the robot during the execution of this program, as that will ruin the results and cause your robot to have innacurate angle readings.

## Tuning the PID Controller
Next, you will have to tune your PID gains to work for your robot.
To do this, you will simply have to try many combinations until your robot can balance by itself and react to disruptions.

To run the program with your PID gains, in the `final_code` directory run `sudo ./main <p> <i> <d>` with each variable being the proportional, integral, and derivative
gain respectively.

Some tips for tuning:
- Start by only setting the proportional gain. Find a P gain where your robot can almost balancing by itself and is not shaking.
- Next, in combination with the P gain you found previously, keep increasing the derivative gain until the robot can almost balance and does not shake.
- Finally, keep increasing the integral gain until your robot can balance on its own. 
- The integral gain will most likely be the greatest, then the proportional gain, and then the derivative gain.
- Don't forget that the gains can be decimal numbers, so don't fear making them more accurate.

