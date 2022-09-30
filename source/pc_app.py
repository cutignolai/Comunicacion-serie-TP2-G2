from cgitb import text
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

roll = np.zeros(7)
pitch = np.zeros(7)
yaw = np.zeros(7)

showRoll = np.zeros(8)
showPitch = np.zeros(8)
showYaw = np.zeros(8)

targetShowRolido = np.zeros(8)
targetShowCabeceo = np.zeros(8)
targetShowOrientacion = np.zeros(8)

keys = {pygame.K_RETURN: 0}

texts = [
    "Board 0",
    "Board 1",
    "Board 2",
    "Board 3",
    "Board 4",
    "Board 5"
]

dataQueue = []

codes = {
    "0": 0,
    "1": 1,
    "2": 2,
    "3": 3,
    "4": 4,
    "5": 5
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

    gluPerspective(100, (display[0]/display[1]), 0.1, 50.0)    #estos dos desordenan todo
    glTranslatef(0.0,0.0, -5)

def drawText(position, textString, color):
    font = pygame.font.Font(None, 50)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)

def text_objects(text, font):
    textSurface = font.render(text, True, (255,255,255), (0,0,0,255))  

def NewDrawText(textString):     
    font = pygame.font.SysFont ("Courier New",25, True)
    textSurface = font.render(textString, True, (255,255,255), (0,0,0,255)) 
    textData = pygame.image.tostring(textSurface, "RGBA", True)    
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

                print("Available ports: " + str(available_ports))
                port = input("Select a port number: ")

                coms = serial.Serial(port, baudrate=9600, timeout=1)

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
                #incomming_info = coms.readline().decode('utf-8')
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
    
    drawText((-18, 12, -10), texts[0], (255, 255, 255))
    smallDrawText((-28, 12.5, -18), "Roll: %d " % roll[0], (255, 255, 255))
    smallDrawText((-28, 10, -18), "Pitch: %d " % pitch[0], (255, 255, 255))
    smallDrawText((-28, 7.5, -18), "Yaw: %d " % yaw[0], (255, 255, 255))

    drawText((-4, 12, -10), texts[1], (255, 255, 255))
    smallDrawText((-7, 12.5, -18), "Roll: %d " % roll[1], (255, 255, 255))
    smallDrawText((-7, 10, -18), "Pitch: %d " % pitch[1], (255, 255, 255))
    smallDrawText((-7, 7.5, -18), "Yaw: %d " % yaw[1], (255, 255, 255))

    drawText((12, 12, -10), texts[2], (255, 255, 255))
    smallDrawText((18, 12.5, -18), "Roll: %d " % roll[2], (255, 255, 255))
    smallDrawText((18, 10, -18), "Pitch: %d " % pitch[2], (255, 255, 255))
    smallDrawText((18, 7.5, -18), "Yaw: %d " % yaw[2], (255, 255, 255))

    drawText((-18, -2, -10), texts[3], (255, 255, 255))
    smallDrawText((-28, -10, -18), "Roll: %d " % roll[3], (255, 255, 255))
    smallDrawText((-28, -12.5, -18), "Pitch: %d " % pitch[3], (255, 255, 255))
    smallDrawText((-28, -15, -18), "Yaw: %d " % yaw[3], (255, 255, 255))

    drawText((-4, -2, -10), texts[4], (255, 255, 255))
    smallDrawText((-7, -10, -18), "Roll: %d " % roll[4], (255, 255, 255))
    smallDrawText((-7, -12.5, -18), "Pitch: %d " % pitch[4], (255, 255, 255))
    smallDrawText((-7, -15, -18), "Yaw: %d " % yaw[4], (255, 255, 255))

    drawText((12, -2, -10), texts[5], (255, 255, 255))
    smallDrawText((18, -10, -18), "Roll: %d " % roll[5], (255, 255, 255))
    smallDrawText((18, -12.5, -18), "Pitch: %d " % pitch[5], (255, 255, 255))
    smallDrawText((18, -15, -18), "Yaw: %d " % yaw[5], (255, 255, 255))

def axis():
    glColor3f(0, 0, 1)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(0, 0, 2)
    glEnd()

    glColor3f(0, 1, 0)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(0, 2, 0)
    glEnd()

    glColor3f(1, 0, 0)

    glBegin(GL_LINES)
    glVertex3f(0, 0, 0)
    glVertex3f(2, 0, 0)
    glEnd()

    glColor3f(1, 1, 1)

def DrawBoard():
    
    glBegin(GL_QUADS)
    x = 0
    
    for surface in surfaces:
        
        for vertex in surface:  
            glColor3fv((colors[x]))          
            glVertex3fv(vertices[vertex])
        x += 1
    glEnd()

def DrawGLL(index):

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0.0, 0.0, -5)

    glRotatef(pitch[index], 0, 0, 1)
    glRotatef(roll[index], -1, 0, 0)
    glRotatef(yaw[index], 0, 1, 0)

    
    #pygame.display.flip()

def DrawGL(index):

    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
    glLoadIdentity() 
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0.0,0.0, -5)   

    glRotatef(pitch[index], 0, 0, 1)
    glRotatef(roll[index], -1, 0, 0)
    glRotatef(yaw[index], 0, 1, 0)


    #drawText((-4, 10, -10), texts[index], (255, 255, 255))
    #smallDrawText((-25, -15, -18), "Roll: %d " % roll[index], (255, 255, 255))
    #smallDrawText((-5, -15, -18), "Pitch: %d " % pitch[index], (255, 255, 255))
    #smallDrawText((15, -15, -18), "Yaw: %d " % yaw[index], (255, 255, 255))

    gnum = str(index)
    NewDrawText("       Board "+ gnum)
    #NewDrawText("Roll: {}° Pitch: {}° Yaw: {}°".format(round(roll[index],1),round(pitch[index],1), round(yaw[index],1)))
    #DrawText("Roll: {}°               Pitch: {}°".format(round(myimu.Roll,1),round(myimu.Pitch,1)))
    DrawBoard()
    #pygame.display.flip()

def EventManager(selected_board):
    if selected_board == 10:
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)
        glLoadIdentity() 
        gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
        glTranslatef(0.0,0.0, -5)   

        glRotatef(pitch[6], 0, 0, 1)
        glRotatef(roll[6], -1, 0, 0)
        glRotatef(yaw[6], 0, 1, 0)
        DrawData()
    else:
        DrawGL(selected_board)
    
    

def main():
    global coms
    global showRoll, showPitch, showYaw, targetShowCabeceo, targetShowOrientacion, targetShowRolido
    selected_board = 10
    def on_closing():
        global run
        run = False

    InitPygame()
    InitGL()
    DrawData()

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
                #navigating through 3D boards
                if event.key == pygame.K_LEFT:
                    if selected_board >= 0:
                        if selected_board < 10:
                            selected_board -= 1
                if event.key == pygame.K_LEFT:
                    if selected_board == -1:
                        selected_board = 5
                if event.key == pygame.K_RIGHT:
                    if selected_board < 6:
                        if selected_board >= 0:
                            selected_board += 1
                if event.key == pygame.K_RIGHT:
                    if selected_board == 6:
                        selected_board = 0
                #on main stage it doesn't change
                if event.key == pygame.K_LEFT:
                    if selected_board == 10:
                        selected_board = 10
                if event.key == pygame.K_RIGHT:
                    if selected_board == 10:
                        selected_board = 10
                #selecting a display, 0 for general view, otherwise select a board from 1 to 5
                if event.key == pygame.K_0:
                    selected_board = 0
                if event.key == pygame.K_1:
                    selected_board = 1
                if event.key == pygame.K_2:
                    selected_board = 2
                if event.key == pygame.K_3:
                    selected_board = 3
                if event.key == pygame.K_4:
                    selected_board = 4
                if event.key == pygame.K_5:
                    selected_board = 5
                if event.key == pygame.K_m:
                    selected_board = 10
                

                #testing
                if event.key == pygame.K_6:
                    roll[1] += 30
                    if roll[1] > 180:
                        roll[1] -= 360
                if event.key == pygame.K_7:
                    pitch[1] -= 25
                    if pitch[1] < -180:
                        pitch[1] += 360
                if event.key == pygame.K_8:
                    yaw[1] += 10
                    if yaw[1] > 180:
                        yaw[1] -= 360
                if event.key == pygame.K_9:
                    roll[0] += 50
                    if roll[0] > 180:
                        roll[0] -= 360

                if event.type == pygame.KEYDOWN:
                    keys[event.key] = 1
                    if event.key == pygame.K_RETURN:
                        enterPressed = True
                if event.type == pygame.KEYUP:
                    keys[event.key] = 0

        # print(status)

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        EventManager(selected_board)

        if enterPressed:
            showRoll[selected_board] = 0
            showPitch[selected_board] = 0
            showYaw[selected_board] = 0

            targetShowRolido[selected_board] = roll[selected_board]
            targetShowCabeceo[selected_board] = pitch[selected_board]
            targetShowOrientacion[selected_board] = yaw[selected_board]

        pygame.display.flip()
        #pygame.time.wait(10)


if __name__ == '__main__':
    main()