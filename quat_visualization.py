import serial
import matplotlib.pyplot as plt
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

while True:

    input = ser.readline().decode('utf-8').strip()
    print(input)
    q0,q1,q2,q3 = map(float, input.split(','))
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

