import _thread, time, socket
import numpy as np
import pandas as pd
from tkinter import *
import math



"""

Das wird eine schöne Beschreibung werden

"""
data_folder = "C:\\Users\\Enrico\\Desktop\\Enrico\\Uni\\Letztes Semester\\Bachelor\\data\\"

data = ''   # Declare an empty variable
hx_databank = []
hx_book_databank = []
imu_databank = []
int_imu = 0
int_hx = 0
command = ''
# Other IP
# https://stackoverflow.com/questions/18743962/python-send-udp-packet
UDP_IP = "192.168.178.58"
UDP_PORT = 2390
received = True
# UDP setup for listening
sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind(('', 12345))  # I'm using port 12345 to bind to

# last miiles of arduino data
milli = .0
# imu offsets
imu_offset_list = np.array([.0,.0,.0,.0,.0,.0])
# hx calibration
hx_offset_list = np.array([0,0,0,0,0])
hx_calib_list = np.array([1.0,1.0,1.0,1.0,1.0])
# wenn drauf drücken, verringert sich der wert: -1
hx_sign_list = np.array([-1,1,-1,1,1])


filename = ""

"""
Value which stores commands from the user:
0: No command
1: calibration of grid-sensors
2: calibration of tilt-sensor
3: offset recalculation
4: start
"""
command_active = 0
# hx
hx_seqnum = 0
hx_t_now = 0
hx_i = 0
hx_raw_value = .0
hx_scale_offset = .0
hx_value = 0.0
weight_seen = 0
debug = False

#imu output. bool_all : Show list
def imu(imu_entry, bool_all=False ):
    # umwandlung in Grad. Der ausgegebene wert kann zwischen -1 bis 0.97 sein.
    if not bool_all:
        output = "IMU: \n"
        if imu_entry[1] > 0:
            degreesX = (100*imu_entry[1])/97 * 90
            output = output + "x: " + str(degreesX) + " Degrees\n"
        if imu_entry[1] < 0:
           degreesX = (100*imu_entry[1])/100 * 90
           output = output + "x: " + str(degreesX) + " Degrees\n"

        if imu_entry[2] > 0:
            degreesY = (100*imu_entry[2])/97 * 90
            output = output + "y: " + str(degreesY) + " Degrees\n"
        if imu_entry[2] < 0:
            degreesY = (100*imu_entry[2])/100 * 90
            output = output + "y: " + str(degreesY) + " Degrees\n"

        if imu_entry[3] > 0:
            degreesZ = (100*imu_entry[3])/97 * 90
            output = output + "z: " + str(degreesZ) + " Degrees\n"
        if imu_entry[3] < 0:
            degreesZ = (100*imu_entry[3])/100 * 90
            output = output + "z: " + str(degreesZ)+ " Degrees\n"

        print(output)

    else:
        df = pd.DataFrame(imu_databank, columns=['ms', 'Axc_x', 'Axc_y', 'Axc_z', 'Gyro_x', 'Gyro_y', 'Gyro_z' ])
        print(df)



# Calibireren sollte in dieser Funktion passieren. man kann die %-Anteile des Gewichts der
# Sensoren messen. Daraus kann man dann die Gewichte errechnen, die auf den Sensoren liegen müssen
def calib_hx( recent_sensor_value_list, weight, tilt=False):
        grid_weight = recent_sensor_value_list[0][2] * hx_sign_list[0] + recent_sensor_value_list[1][2] * hx_sign_list[1] + recent_sensor_value_list[2][2] * hx_sign_list[2]+ recent_sensor_value_list[3][2] * hx_sign_list[3]

        if tilt:
            hx_calib_list[-1] =  weight / (recent_sensor_value_list[4][2] * hx_sign_list[4])
            return hx_calib_list

        # calculate percentagees with the raw values
        s0_percent =  recent_sensor_value_list[0][2] / grid_weight
        s1_percent =  recent_sensor_value_list[1][2] / grid_weight
        s2_percent =  recent_sensor_value_list[2][2] / grid_weight
        s3_percent =  recent_sensor_value_list[3][2] / grid_weight

        # calibrate with percentage weight and calculatet weight_value on the sensor
        hx_calib_list[0] = abs((s0_percent * weight) / recent_sensor_value_list[0][5])
        hx_calib_list[1] = abs((s1_percent * weight) / recent_sensor_value_list[1][5])
        hx_calib_list[2] = abs((s2_percent * weight) / recent_sensor_value_list[2][5])
        hx_calib_list[3] = abs((s3_percent * weight) / recent_sensor_value_list[3][5])

        return hx_calib_list


 # gets the real weight on the grid and on the tilt platform. Besides that it localizes
# the force-middle-point on the grid.
def get_real_hx_values(recent_sensor_value_list ):
    grid_weight = recent_sensor_value_list[0][5]  + recent_sensor_value_list[1][5] + recent_sensor_value_list[2][5] + recent_sensor_value_list[3][5]
    tilt_weight = recent_sensor_value_list[4][5]
    if (grid_weight > 100) | (grid_weight < 100):
        value_lu_rd = recent_sensor_value_list[2][5] + recent_sensor_value_list[0][5]
        sensor_0_percent = recent_sensor_value_list[0][5] / value_lu_rd
        s0_s2_coordinate =  math.sqrt(192 ** 2 + 152**2) * sensor_0_percent

        value_ld_ru = recent_sensor_value_list[3][5] + recent_sensor_value_list[1][5]
        sensor_1_percent = recent_sensor_value_list[1][5] / value_ld_ru
        s1_s3_coordinate =  math.sqrt(192 ** 2 + 152**2) * sensor_1_percent

        return [recent_sensor_value_list[0][0], grid_weight, tilt_weight, s0_s2_coordinate, s1_s3_coordinate]

    return [recent_sensor_value_list[0][1], grid_weight, tilt_weight, 0.0, 0.0]


    # Define a function for the thread


def save_data(filename_para):
    df_hx = pd.DataFrame(hx_book_databank, columns=['ms', 'grid weight', 'tilt weight', 'posX*', 'posy*'])
    df_imu = pd.DataFrame(imu_databank, columns=['ms', 'x_acce', 'y_acce', 'z_acce', 'x_gyro', 'y_gyro', 'z_gyro'])

    try:
        open(data_folder + filename_para + "-hx.csv", "a")
        df_hx.to_csv(data_folder + filename_para + "-hx.csv", sep='\t', encoding='utf-8', mode='w')
        df_imu.to_csv(data_folder + filename_para  + "-imu.csv", sep='\t', encoding='utf-8', mode='w')

    except NameError as e:
        print(e)


def listening_thread():
    global data     # data needs to be defined as global inside the thread
    global UDP_IP
    global UDP_PORT
    global received
    global debug
    global imu_offset_list
    global int_imu, int_hx
    global hx_seqnum, hx_t_now, hx_databank, hx_i, hx_raw_value, hx_scale_offset, hx_value
    global hx_calib_list, hx_offset_list
    global calib, offset, calib_tilt, weight_seen, filename, command_active
    while True:
        data_raw, addr = sock.recvfrom(1024)
        #UDP_IP, UDP_PORT = addr

        data = data_raw.decode()    # test message is encoded
        # Msg reading
        if data.startswith("acknowledged"):
            received = True
            print(data)
            continue
        if data.startswith("imu"):
            data_array = data.split(',')

            if command_active == 100:
                # Offsets neu bestimmen
                imu_offset_list[0] = float(data_array[2])   # Acce x
                imu_offset_list[1] = float(data_array[3])   # Acce y
                imu_offset_list[2] = float(data_array[4])   # Acce z
                imu_offset_list[3] = float(data_array[4])
                imu_offset_list[4] = float(data_array[5])
                imu_offset_list[5] = float(data_array[6])
            #print(data)
            x  = (float(data_array[2]) - imu_offset_list[0])
            y  = (float(data_array[3]) - imu_offset_list[1])
            z  = (float(data_array[4]) - imu_offset_list[2])

        # tnow x, x, z, gyroX, gyroY, gyroZ
            imu_databank.append([ float(data_array[1]), x, y,
                                  z, (float(data_array[5])),
                                  (float(data_array[6])),  (float(data_array[7]))])

            int_imu +=1
            if int_imu % 100 == 0:
                print("IMU Values cleared: " + str(int_imu))
            if int_imu > 50000:
                imu_databank.clear()
            continue

        if data.startswith("W "):
                #print(data)
                data_array = data.split(' ')
                hx_i = int(data_array[4])
                hx_seqnum= float(data_array[1])
                hx_t_now= float(data_array[2])
                hx_raw_value = float(data_array[3])
                hx_scale_offset = hx_offset_list[hx_i]
                    # noch sicherstellen, dass das erste hier ist.
                # noch -/+ der Arduino Werte anpasen; sodass alle - sind, wenn kraft von unten kommt
                if command_active == 3:
                    hx_scale_offset = hx_offset_list[hx_i] = hx_raw_value
                    if hx_i == 4:
                        command_active = 0

                hx_value = ((hx_raw_value - hx_scale_offset) * hx_calib_list[hx_i]) *  hx_sign_list[hx_i]

                hx_databank.append([hx_seqnum, hx_t_now, hx_raw_value, hx_scale_offset, hx_i, hx_value])
                #print("A: "+ str(hx_databank[-1]))
                int_hx +=1
                if hx_i == 4:
                    hx_book_databank.append(get_real_hx_values(hx_databank[-5:]))
                    print(hx_book_databank[-1])
                    #print(hx_calib_list[0])
                    if command_active == 1:
                        hx_calib_list= calib_hx(hx_databank[-5:], weight_seen)
                        command_active = 0
                        #print(hx_calib_list[0])
                    if command_active == 2:
                        hx_calib_list= calib_hx(hx_databank[-5:], weight_seen, tilt=True)
                        command_active = 0
                    if command_active == 5:
                        save_data(filename)
                        command_active = 0
                if int_hx % 500 == 0:
                    print("HX Values cleared: " + str(int_hx))
                if int_hx > 50000:
                    hx_databank.clear()
                continue

        print ("Received message inside thread:", data)

try:
    _thread.start_new_thread(listening_thread, ())
except:
    print ("Error: unable to start thread")
    quit()

print("initialization complete!")
while 0:
    # print ('Now I can do something useful while waiting in the main body.')
    if input().__len__() >= 1:
        if input().startswith("a"):
            imu(True)
            continue
        if input().startswith("i"):
            imu()
            continue
        print(UDP_IP)
        print(UDP_PORT)
        print('COMMAND WILL BE SENT: ' + command)
        while received:
            sock.sendto(bytes(command, "utf-8"), (UDP_IP, UDP_PORT))
            time.sleep(1)
    if data:
        #print ('THE PACKET RECEIVED BY THE MAIN BODY IS: ' + data)
        data = ''   # Empty the variable ready for the next one
    #time.sleep(2)

def callback():
    print (e.get()) # This is the text you may want to use later
    command = e.get()
    global calib, offset, calib_tilt, weight_seen, command_active, filename
    if e.get().__len__() >= 1:
        #imu
        if e.get().startswith("all_imu"):
            imu(True)
            return
        if e.get().startswith("imu"):
            imu()
            return
        if e.get().startswith("save_imu "):
            df = pd.DataFrame(imu_databank, columns=['ms', 'x_acce', 'y_acce', 'z_acce', 'x_gyro', 'y_gyro', 'z_gyro'])
            # safe and append
            df.to_csv(data_folder + "imu_data", sep='\t', encoding='utf-8', mode='a', header=False)
            return

        #hx711
        if e.get().startswith("offset_hx"):

            if command_active != 0:
                print("Wait for Command to be executed")
                return
            if command_active == 3:
                print("Already working on it!")
                return
            command_active = 3
            print("Offset-calculation is starting!")
            return
        if e.get().startswith("calib_book"):
            if command_active == 1:
                print("Already calibrating!")
                return
            string_array = e.get().split(' ')
            command_active = 1
            weight_seen = int(string_array[-1])
            print("Calibration is starting!")
            return
        if e.get().startswith("calib_tilt"):
            if command_active == 2:
                print("Already calibrating!")
                return
            string_array = e.get().split(' ')
            command_active = 2
            weight_seen = int(string_array[-1])
            print("Calibration is starting!")
            return
        if e.get().startswith("start"):
            if command_active == 4:
                print("Already recording!")
                return
            command_active = 4
            hx_databank.clear()
            hx_book_databank.clear()
            imu_databank.clear()
            print("Starting to record!")
            return
        if e.get().startswith("stop"):
            if command_active == 5:
                print("Already on the way to stop the Record!")
                return
            command_active = 5
            string_array = e.get().split(' ')
            filename = string_array[-1]
            print("Stopping to record!")
            return
        if e.get().startswith("offset_imu"):
            if command_active == 100:
                print("Already Resetting Imu-Offsets!")
                return
            command_active = 100
            print("Resetting imu-Offsets!")
            return

        print(UDP_IP)
        print(UDP_PORT)
        print('COMMAND WILL BE SENT: ' + command)
        if received:
            sock.sendto(command.encode(), (UDP_IP, UDP_PORT))


master = Tk()
e = Entry(master)
e.pack()

e.focus_set()



b = Button(master, text = "OK", width = 20, command = callback)
b.pack()
mainloop()