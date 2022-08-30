from mpu6050 import mpu6050
import time
import threading

mpu = mpu6050(0x68)    #instantiate a MPU6050 class object
ac = [0]*3               #store accelerometer data
gy = [0]*3                #store gyroscope data

# def setup():
#     mpu.dmp_initialize()    #initialize MPU6050
    
angle = 0
last_time = 0
# z_gyro_offset = 2.3398818954342495
# z_gyro_offset = 2.3728993756396934

offsets = {'x': 16.14545123364232, 'y': -0.7168607184760063, 'z': 2.3609238954836407}

def angle_loop():
    global angle, last_time
    last_time += time.time()
    while True:
        gy = mpu.get_gyro_data()
        angle = (angle + ((gy["z"] + offsets["z"]) * (time.time() - last_time)))
        if angle >= 360:
            angle -= 360
        elif angle < 0:
            angle += 360
        # angle = (angle + (gy["z"] * (time.time() - last_time))) % 360
        last_time = time.time()

def loop():
    global angle
    # last_time = time.time()
    while(True):
        # ac = mpu.get_accel_data()      #get accelerometer data
        # gy = mpu.get_gyro_data()           #get gyroscope data
        # print("\n\n\n\n")
        # print(ac)
        # print(gy["z"])
        # angle = (angle + (gy["z"] * (time.time() - last_time))) % 360
        # last_time = time.time()
        print(round(angle))
        # print("Acceleration:\n")
        # print("X:%.2f g  Y:%.2f g  Z:%.2f g\n"%(ac[0]/16384.0,ac[1]/16384.0,ac[2]/16384.0))
        # print("Angular velocity:\n")
        # print("X:%.2f d/s Y:%.2f d/s Z:%.2f d/s\n"%(gy[0]/131.0,gy[1]/131.0,gy[2]/131.0))
        time.sleep(0.01)
        
if __name__ == '__main__':     # Program start from here
    # setup()
    try:
        angle_thread = threading.Thread(target=angle_loop)
        angle_thread.start()
        loop()
    except KeyboardInterrupt:  # When 'Ctrl+C' is pressed,the program will exit.
        pass

