import serial
import matplotlib.pyplot as plt
import time
import csv
import numpy as np
from vpython import *
import quaternion as q


ser = serial.Serial('COM7', 115200)

#plt.show()
#plt.ion()
#fig, ax = plt.subplots(figsize=(5, 6), subplot_kw={'projection': '3d'})
scene = canvas(title= "Current Orientation")
scene.userspin = False
scene.userzoom = False
scene.userpan  = False
scene.background = color.white
scene.center = vector(0,0,0)
scene.forward = vector(3,2,-1)
scene.up = vector(0,0,1)

#bg = sphere(
#    radius=1,
#    texture="https://imgur.com/gallery/twin-towers-wallpaper-5LyiUOP#/t/twin_towers",
#    emissive=True,
#    opacity=0.9
#)
#bg.flipx = True
cube = box(size=vector(1,1,0.1), color = color.blue)
front = arrow(length = 0.3, shaftwidth= 0.1, color = color.red)
upper = arrow(length=0.3, shaftwidth= 0.1, color= color.yellow)

# Open file to save data to
file = open('imu_data.csv', 'w', newline='')
writer = csv.writer(file)
writer.writerow(['time', 'roll', 'pitch', 'rough roll', 'rough pitch'])
start_time = time.time()

try:
    while True:

        line = ser.readline().decode('utf-8', errors='ignore').strip()
        print(line)

        parts = line.split(',')

        if len(parts) != 7:
            continue

        try:
            q0, q1, q2, q3, ax, ay, az = map(float, parts)
        except:
            continue

        att = q.quaternion(q0,q1,q2,q3)
        px = q.quaternion(0, 1, 0, 0)
        py = q.quaternion(0, 0, 1, 0)

        c_att = att.conjugate()

        px_rot = att*px*c_att
        py_rot = att*py*c_att
        

        # Plot points before/after
        # Convention: x is the roll axis, -y is the pitch axis, -z is the yaw axis
        cube.axis= vector(px_rot.x, px_rot.y, px_rot.z)
        cube.up = vector(py_rot.x, py_rot.y, py_rot.z)
        front.axis= cube.axis
        front.pos = cube.pos

        # Saving the data to graph later for report
        roll = np.degrees(np.arctan2(2*(q0*q1 + q2*q3), 1 - 2*(q1*q1 + q2*q2)))
        pitch = np.degrees(np.arcsin(2*(q0*q2 - q3*q1)))
        pitch_rough = np.degrees(np.arcsin(ax / np.sqrt(ax*ax + ay*ay + az*az)))
        roll_rough = np.degrees(np.arctan2(ay, az))
        t = time.time() - start_time
        writer.writerow([t, roll, pitch, roll_rough, pitch_rough])

except KeyboardInterrupt:
    print("Stopping and saving file...")
    file.close()
    ser.close()
