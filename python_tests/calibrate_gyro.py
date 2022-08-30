from mpu6050 import mpu6050
import time

mpu = mpu6050(0x68)    #instantiate a MPU6050 class object

sums = {"x":0, "y":0, "z":0}
reading_num = 0

def loop():
    global sums, reading_num
    print("starting calibration in 5 secs.")
    time.sleep(5)
    print("starting...")
    while(True):
        gy = mpu.get_gyro_data()   
        for key in gy.keys():
            sums[key] += gy[key]
        reading_num += 1
        
if __name__ == '__main__':     # Program start from here
    try:
        loop()
    except KeyboardInterrupt:  # When 'Ctrl+C' is pressed,the program will exit.
        print("average offset:")
        # print(sum_reading / reading_num)
        for key in sums.keys():
            sums[key] = sums[key] / reading_num
        print(sums)
