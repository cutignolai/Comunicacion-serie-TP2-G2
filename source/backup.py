from collections import deque

import pygame
from pygame.locals import *
from OpenGL.GL import *
from OpenGL.GLU import *
from figure import *
import random
import sys
import time
from serial.tools import list_ports
import serial
import numpy as np
import threading

# JOYSTICK
import pygame
from pygame import locals

# checks wether a device was connected to a certain port
connected = False
# buffer for communications via serial port
coms = None

roll = np.zeros(5)
pitch = np.zeros(5)
yaw = np.zeros(5)

showRoll = np.zeros(8)
showPitch = np.zeros(8)
showYaw = np.zeros(8)

targetShowRolido = np.zeros(8)
targetShowCabeceo = np.zeros(8)
targetShowOrientacion = np.zeros(8)

keys = {pygame.K_RETURN: 0}

texts = [
    "Board 1",
    "Board 2",
    "Board 3",
    "Board 4",
    "Board 5"
]

dataQueue = []

codes = {
    "1": 0,
    "2": 1,
    "3": 2,
    "4": 3,
    "5": 4
}

def InitPygame():
    global display
    pygame.init()
    display = (1000, 600)
    pygame.display.set_mode(display, DOUBLEBUF|OPENGL)
    pygame.display.set_caption('TP2 Labo de micros G2')

def InitGL():
    #glClearColor((1.0/255*46),(1.0/255*45),(1.0/255*64),1)     #quita el  color del fondo
    glEnable(GL_DEPTH_TEST)
    glDepthFunc(GL_LEQUAL)
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST)

    #gluPerspective(100, (display[0]/display[1]), 0.1, 50.0)    #estos dos desordenan todo
    #glTranslatef(0.0,0.0, -5)

def drawText(position, textString, color):
    font = pygame.font.Font(None, 50)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)


def smallDrawText(position, textString, color):
    font = pygame.font.Font(None, 30)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)


# decodes the information received according to a transmition code made
def manageInfo(info):
    try:
        # group number
        group = int(info[1])
        group -= 1
        # roll, pitch or yaw
        rpy = info[2]
        # +- the value variation
        sign = info[3]
        value = int(info[4]) * 100 + int(info[5]) * 10 + int(info[6])
        if sign == "-":
            value = -value

        return group, rpy, value

    except:
        print("¡A problem has appeard receiving information!")
        return None


# based on the processed information updates the current data
def ReadData():
    global connected
    global coms

    while True:

        pygame.time.wait(10)

        # testing
        # connected = True

        if not connected:
            try:
                ports = serial.tools.list_ports.comports()

                available_ports = []

                for p in ports:
                    available_ports.append(p.device)

                # print("Available ports: " + str(available_ports))
                # port = input("Select a port number: ")

                coms = serial.Serial('COM3', baudrate=9600, timeout=1)

                print("Connected!")
                connected = True
            except:
                coms = None
                print("Trying to connect ... ")
        else:
            try:
                # data that entries via serial port
                #incomming_info = ""
                # buffer for such data
                port_data = ""

                incomming_info = coms.read()
                print("USB received: ", incomming_info)
                incomming_info = incomming_info.decode("ascii")
                print("USB decoded: ", incomming_info)

                if incomming_info == 'S':  # start sentinel

                    while incomming_info != 'E':  # check for 'w' if it doesn't work, end sentinel
                        try:
                            # reads port data
                            incomming_info = coms.read()
                            incomming_info = incomming_info.decode("ascii")
                            # adds data to the buffer for post processing
                            port_data += incomming_info
                        except:
                            pass

                    port_data = port_data[-5:-1]

                    dataString = str(port_data)
                    print("Information received: ", dataString)

                    if dataString != "":
                        try:
                            # process data
                            group, rpy, value = manageInfo(dataString)

                            # updates the rpy value on their corresponding group
                            if rpy == "R":
                                roll[group] = value
                            if rpy == "P":
                                pitch[group] = value
                            if rpy == "Y":
                                yaw[group] = value

                            print("The information has been updated")
                        except:
                            print("Error, data corrupted")
            except:
                print("Decoding error")

def DrawData():
    drawText((-5, 3, -10), texts[0], (255, 255, 255))
    smallDrawText((-11.5, 4, -18), "Roll: %d " % roll[0], (255, 255, 255))
    smallDrawText((-11.5, 3, -18), "Pitch: %d " % pitch[0], (255, 255, 255))
    smallDrawText((-11.5, 2, -18), "Yaw: %d " % yaw[0], (255, 255, 255))

    drawText((-1, 3, -10), texts[1], (255, 255, 255))
    smallDrawText((-4.5, 3.5, -18), "Roll: %d " % roll[1], (255, 255, 255))
    smallDrawText((-4.5, 2.5, -18), "Pitch: %d " % pitch[1], (255, 255, 255))
    smallDrawText((-4.5, 1.5, -18), "Yaw: %d " % yaw[1], (255, 255, 255))

    drawText((3.5, 3, -10), texts[2], (255, 255, 255))
    smallDrawText((3, 3.5, -18), "Roll: %d " % roll[2], (255, 255, 255))
    smallDrawText((3, 2.5, -18), "Pitch: %d " % pitch[2], (255, 255, 255))
    smallDrawText((3, 1.5, -18), "Yaw: %d " % yaw[2], (255, 255, 255))

    drawText((-5, -1, -10), texts[3], (255, 255, 255))
    smallDrawText((-11.5, -4, -18), "Roll: %d " % roll[3], (255, 255, 255))
    smallDrawText((-11.5, -5, -18), "Pitch: %d " % pitch[3], (255, 255, 255))
    smallDrawText((-11.5, -6, -18), "Yaw: %d " % yaw[3], (255, 255, 255))

    drawText((-1, -1, -10), texts[4], (255, 255, 255))
    smallDrawText((-4.5, -4, -18), "Roll: %d " % roll[4], (255, 255, 255))
    smallDrawText((-4.5, -5, -18), "Pitch: %d " % pitch[4], (255, 255, 255))
    smallDrawText((-4.5, -6, -18), "Yaw: %d " % yaw[4], (255, 255, 255))

def DrawDataBig():
    drawText((-18, 12, -10), texts[0], (255, 255, 255))
    smallDrawText((-36, 15, -18), "Roll: %d " % roll[0], (255, 255, 255))
    smallDrawText((-36, 12.5, -18), "Pitch: %d " % pitch[0], (255, 255, 255))
    smallDrawText((-36, 10, -18), "Yaw: %d " % yaw[0], (255, 255, 255))

    drawText((-4, 12, -10), texts[1], (255, 255, 255))
    smallDrawText((-15, 15, -18), "Roll: %d " % roll[1], (255, 255, 255))
    smallDrawText((-15, 12.5, -18), "Pitch: %d " % pitch[1], (255, 255, 255))
    smallDrawText((-15, 10, -18), "Yaw: %d " % yaw[1], (255, 255, 255))

    drawText((12, 12, -10), texts[2], (255, 255, 255))
    smallDrawText((10, 15, -18), "Roll: %d " % roll[2], (255, 255, 255))
    smallDrawText((10, 12.5, -18), "Pitch: %d " % pitch[2], (255, 255, 255))
    smallDrawText((10, 10, -18), "Yaw: %d " % yaw[2], (255, 255, 255))

    drawText((-18, -4, -10), texts[3], (255, 255, 255))
    smallDrawText((-36, -10, -18), "Roll: %d " % roll[3], (255, 255, 255))
    smallDrawText((-36, -12.5, -18), "Pitch: %d " % pitch[3], (255, 255, 255))
    smallDrawText((-36, -15, -18), "Yaw: %d " % yaw[3], (255, 255, 255))

    drawText((-4, -4, -10), texts[4], (255, 255, 255))
    smallDrawText((-15, -10, -18), "Roll: %d " % roll[4], (255, 255, 255))
    smallDrawText((-15, -12.5, -18), "Pitch: %d " % pitch[4], (255, 255, 255))
    smallDrawText((-15, -15, -18), "Yaw: %d " % yaw[4], (255, 255, 255))

def DrawBoard():
    
    glBegin(GL_QUADS)
    x = 0
    
    for surface in surfaces:
        
        for vertex in surface:  
            glColor3fv((colors[x]))          
            glVertex3fv(vertices[vertex])
        x += 1
    glEnd()

def DrawGL():

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity() 
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0.0,0.0, -5)   

    glRotatef(pitch[0], 0, 0, 1)
    glRotatef(roll[0], -1, 0, 0)

    DrawData()
    DrawBoard()
    #pygame.display.flip()



def main():
    global coms
    global showRoll, showPitch, showYaw, targetShowCabeceo, targetShowOrientacion, targetShowRolido
    selected_board = 0

    def on_closing():
        global run
        run = False

    InitPygame()
    InitGL()
   

    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)

    # glTranslatef(0.0, 0.0, -1)

    glRotatef(0, 0, 0, 0)

    coms = None

    t1 = threading.Thread(target=ReadData, args=())
    t1.start()

    while True:

        enterPressed = False

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            if event.type == pygame.KEYDOWN:
                if event.key == pygame.K_LEFT:
                    if selected_board > 0:
                        selected_board -= 1
                if event.key == pygame.K_LEFT:
                    if selected_board == 0:
                        selected_board = 4
                if event.key == pygame.K_RIGHT:
                    if selected_board < 5:
                        selected_board += 1
                if event.key == pygame.K_RIGHT:
                    if selected_board >= 5:
                        selected_board = 0

                if event.type == pygame.KEYDOWN:
                    keys[event.key] = 1
                    if event.key == pygame.K_RETURN:
                        enterPressed = True
                if event.type == pygame.KEYUP:
                    keys[event.key] = 0

        # print(status)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        DrawGL()

        if enterPressed:
            showRoll[selected_board] = 0
            showPitch[selected_board] = 0
            showYaw[selected_board] = 0

            targetShowRolido[selected_board] = roll[selected_board]
            targetShowCabeceo[selected_board] = pitch[selected_board]
            targetShowOrientacion[selected_board] = yaw[selected_board]

        pygame.display.flip()
        # pygame.time.wait(10)


if __name__ == '__main__':
    main()