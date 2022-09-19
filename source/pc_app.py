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

#checks wether a device was connected to a certain port
connected = False
#buffer for communications via serial port
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
    "Placa 1",
    "Placa 2",
    "Placa 3",
    "Placa 4",
    "Placa 5",
    "Placa 6",
    "Placa 7",
    "Placa 8"
]

dataQueue = []


codes = {
    "1": 0,
    "2": 1,
    "3": 2,
    "4": 3,
    "5": 4,
    "6": 5,
    "7": 6,
    "8": 7
}

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

def Cube():
    GL.glBegin(GL.GL_LINES)
    for edge in edges:
        for vertex in edge:
            GL.glVertex3fv(vertices[vertex])
    GL.glEnd()


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

#decodes the information received according to a transmition code made
def manageInfo(info):
    try:
        #group number
        group = int(info[0])
        group -= 1
        #roll, pitch or yaw
        rpy = info[1]
        #+- the value variation
        sign = info [2]
        value = int(info[3])*100 + int(info[4])*10 + int(info[5])
        if sign == "-":
            value = -value
        
        return group, rpy, value
        
    except:
        print("¡A problem has appeard receiving information!")
        return None

#based on the processed information updates the current data        
def ReadData():
    global connected
    global coms
   
    while True:

        pygame.time.wait(10)

        #testing
        connected = True

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
                #data that entries via serial port  
                incomming_info = ""
                #buffer for such data
                port_data = ""
                while incomming_info != 'E':        #check for 'w' if it doesn't work
                    try:
                        #reads port data
                        incomming_info = coms.read().decode("ascii")
                        #adds data to the buffer for post processing
                        port_data += incomming_info
                    except:
                        pass

                port_data = port_data[-4:-1]       


                dataString = str(port_data)
                print("Information received: ", dataString)
                
                #testing
                dataString = "2R125"

                if dataString != "":
                    try:
                        #process data
                        group, rpy, value = manageInfo(dataString)
                      
                        #updates the rpy value on their corresponding group
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

def main():
    global coms
    global showRoll, showPitch, showYaw, targetShowCabeceo, targetShowOrientacion, targetShowRolido
    selected_board = 0

    pygame.init()
    display = (800, 600)
    pygame.display.set_mode(display, DOUBLEBUF | OPENGL)

    gluPerspective(45, (display[0]/display[1]), 0.1, 50.0)

    #glTranslatef(0.0, 0.0, -1)

    glRotatef(0, 0, 0, 0)

    camPosition = 0

    coms= None

    t1 = threading.Thread(target = ReadData, args = ())
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
                if event.key == pygame.K_RIGHT:
                    if selected_board < 5:
                        selected_board += 1

                if event.type == pygame.KEYDOWN:
                    keys[event.key] = 1
                    if event.key == pygame.K_RETURN:
                        enterPressed = True
                if event.type == pygame.KEYUP:
                    keys[event.key] = 0



        #print(status)

        targetPostion = -positions[selected_board]

        camPosition += (targetPostion - camPosition)/10

        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT)

        glTranslatef(camPosition, 0.0, -10)

        for i in [selected_board-1, selected_board, selected_board + 1]:
            if i < 0:
                continue
            if i >= 5:
                continue

            drawText((positions[i]-2, 6, -10), texts[i], (255, 255, 255))
            smallDrawText((positions[i]-2.5, 5, -10), "(Press enter)",(255,255,255))

            drawText((positions[i]-4, -10, -18), "Roll: %d " % roll[i], (255, 0, 0))
            drawText((positions[i]-4, -8, -18), "Pitch: %d " % pitch[i], (0, 0, 255))
            drawText((positions[i]-4, -6, -18), "Yaw: %d " % yaw[i], (0, 255, 0))
            

            glTranslate(
                cubePositions[i][0],
                cubePositions[i][1],
                cubePositions[i][2]
            )

            # roll - rolido
            glRotatef(showRoll[i], 0, 0, 1)

            # pitch - cabeceo
            glRotatef(showPitch[i], 1, 0, 0)

            # yaw - orientacion
            glRotatef(-showYaw[i], 0, 1, 0)

            axis()
            Cube()

            
            # roll - rolido
            glRotatef(-showRoll[i], 0, 0, 1)

            # pitch - cabeceo
            glRotatef(-showPitch[i], 1, 0, 0)

            # yaw - orientacion
            glRotatef(showYaw[i], 0, 1, 0)

            glTranslate(
                -cubePositions[i][0],
                -cubePositions[i][1],
                -cubePositions[i][2]
            )

            showYaw[i] += (targetShowOrientacion[i] - showYaw[i]) / 30
            if abs(showYaw[i] - targetShowOrientacion[i]) < 0.5:
                showYaw[i] = targetShowOrientacion[i]
                showRoll[i] += (targetShowRolido[i] - showRoll[i]) / 30
                if abs(showRoll[i] - targetShowRolido[i]) < 0.5:
                    showRoll[i] = targetShowRolido[i]
                    showPitch[i] += (targetShowCabeceo[i] - showPitch[i]) / 30

            if enterPressed:
                showRoll[i] = 0
                showPitch[i] = 0
                showYaw[i] = 0

                targetShowRolido[i] = roll[i]
                targetShowCabeceo[i] = pitch[i]
                targetShowOrientacion[i] = yaw[i]                

        #glTranslatef(-camPosition, 0.0, -15)

        glTranslatef(-camPosition, 0.0, 10)

        pygame.display.flip()
        #pygame.time.wait(10)






if __name__ == '__main__':

    main()