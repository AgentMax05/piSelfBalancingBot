from mpu6050 import mpu6050
import time

mpu = mpu6050(0x68)    #instantiate a MPU6050 class object
ac = [0]*3               #store accelerometer data
gy = [0]*3                #store gyroscope data

sum_reading = 0
reading_num = 0

# def setup():
#     mpu.dmp_initialize()    #initialize MPU6050
    
def loop():
    global sum_reading, reading_num
    print("starting calibration in 5 secs.")
    time.sleep(5)
    print("starting...")
    while(True):
        ac = mpu.get_accel_data()      #get accelerometer data
        gy = mpu.get_gyro_data()           #get gyroscope data
        # print("\n\n\n\n")
        # print(ac)
        # print(gy["z"])
        sum_reading += gy["z"]
        reading_num += 1
        # print("Acceleration:\n")
        # print("X:%.2f g  Y:%.2f g  Z:%.2f g\n"%(ac[0]/16384.0,ac[1]/16384.0,ac[2]/16384.0))
        # print("Angular velocity:\n")
        # print("X:%.2f d/s Y:%.2f d/s Z:%.2f d/s\n"%(gy[0]/131.0,gy[1]/131.0,gy[2]/131.0))
        # time.sleep(0.001)
        
if __name__ == '__main__':     # Program start from here
    # setup()
    try:
        loop()
    except KeyboardInterrupt:  # When 'Ctrl+C' is pressed,the program will exit.
        print("average offset:")
        print(sum_reading / reading_num)
