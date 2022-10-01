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
import vtkplotlib as vpl



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

keys = {pygame.K_RETURN: 0}

texts = [
    "Board 0",
    "Board 1",
    "Board 2",
    "Board 3",
    "Board 4",
    "Board 5"
]


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



def smallDrawText(position, textString, color):
    font = pygame.font.Font(None, 30)
    textSurface = font.render(textString, True, (color[0], color[1], color[2], 255), (0, 0, 0, 255))
    textData = pygame.image.tostring(textSurface, "RGBA", True)
    glRasterPos3d(*position)
    glDrawPixels(textSurface.get_width(), textSurface.get_height(), GL_RGBA, GL_UNSIGNED_BYTE, textData)


# decodes the information received according to a transmition code made
def manageInfo(info):
    try:
        # group number 2   R+060   P-100   Y+150   E
        group = int(info[0])
        # +- the value variation

        # roll
        sign = info[2]
        r = int(info[3]) * 100 + int(info[4]) * 10 + int(info[5])
        if sign == "-":
            r = -r

        # pitch
        sign = info[7]
        p = int(info[8]) * 100 + int(info[9]) * 10 + int(info[10])
        if sign == "-":
            p = -p

        # yaw
        sign = info[12]
        y = int(info[13]) * 100 + int(info[14]) * 10 + int(info[15])
        if sign == "-":
            y = -y

        return group, r, p, y

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

                coms = serial.Serial('COM5', baudrate=9600, timeout=1)

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

                #incomming_info = coms.read()
                #print("USB received: ", incomming_info)
                incomming_info = coms.read().decode("ascii")
                #incomming_info = coms.readline().decode('utf-8')
                #print("USB decoded: ", incomming_info)

                if incomming_info == 'S':  # start sentinel

                    while incomming_info != 'E':  # check for 'w' if it doesn't work, end sentinel
                        try:
                            # reads port data
                            incomming_info = coms.read()
                            incomming_info = incomming_info.decode("ascii")
                            print("USB decoded: ", incomming_info)
                            # adds data to the buffer for post processing
                            port_data += incomming_info
                        except:
                            pass
                    #print("Port data: ", port_data)
                    #port_data = port_data[-5:-1]

                    #dataString = str(port_data)
                    #print("Information received: ", dataString)

                    if port_data != "":
                        try:
                            # process data
                            group, r, p, y = manageInfo(port_data)

                            # updates the rpy value on their corresponding group

                            roll[group] = r
                            pitch[group] = p
                            yaw[group] = y

                            #print("Group: ", group)
                            #print("Roll: ", r)
                            #print("Pitch: ", p)
                            #print("Yaw: ", y)
                            print("The information has been updated")
                        except:
                            print("Error, data corrupted")
            except:
                print("Decoding error")




def DrawBoard():
    
    glBegin(GL_QUADS)
    x = 0
    
    for surface in surfaces:
        
        for vertex in surface:  
            glColor3fv((colors[x]))          
            glVertex3fv(vertices[vertex])
        x += 1
    glEnd()



def DrawGL0():          #DIBUJO UN BOARD


    glLoadIdentity() 
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[0], -1, 0, 0)
    glRotatef(roll[0], 0, 0, 1)
    glRotatef(yaw[0], 0, -1, 0)


    DrawBoard()


def DrawGL1():          #DIBUJO UN BOARD

    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0, 3, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[1], -1, 0, 0)
    glRotatef(roll[1], 0, 0, 1)
    glRotatef(yaw[1], 0, -1, 0)

    DrawBoard()


def DrawGL2():          #DIBUJO UN BOARD


    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(5.5, 3, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[2], -1, 0, 0)
    glRotatef(roll[2], 0, 0, 1)
    glRotatef(yaw[2], 0, -1, 0)

    DrawBoard()



def DrawGL3():          #DIBUJO UN BOARD


    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(-5.5, -2, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[3], -1, 0, 0)
    glRotatef(roll[3], 0, 0, 1)
    glRotatef(yaw[3], 0, -1, 0)

    DrawBoard()


def DrawGL4():          #DIBUJO UN BOARD


    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(0, -2, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[4], -1, 0, 0)
    glRotatef(roll[4], 0, 0, 1)
    glRotatef(yaw[4], 0, -1, 0)

    DrawBoard()



def DrawGL5():          #DIBUJO UN BOARD


    glLoadIdentity()
    gluPerspective(90, (display[0]/display[1]), 0.1, 50.0)
    glTranslatef(5.5, -2, -5)               #MATRIZ DE TRANSLACION

    glRotatef(pitch[5], -1, 0, 0)
    glRotatef(roll[5], 0, 0, 1)
    glRotatef(yaw[5], 0, -1, 0)

    DrawBoard()









def BoardText0():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((-18, 0, -10), texts[0], (255, 255, 255))
    smallDrawText((-15, 3, -18), "Roll: %d " % roll[0], (255, 255, 255))
    smallDrawText((-15, 1, -18), "Pitch: %d " % pitch[0], (255, 255, 255))
    smallDrawText((-15, -1, -18), "Yaw: %d " % yaw[0], (255, 255, 255))

def BoardText1():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((0, 0, -10), texts[1], (255, 255, 255))
    smallDrawText((10, 3, -18), "Roll: %d " % roll[1], (255, 255, 255))
    smallDrawText((10, 1, -18), "Pitch: %d " % pitch[1], (255, 255, 255))
    smallDrawText((10, -1, -18), "Yaw: %d " % yaw[1], (255, 255, 255))

def BoardText2():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((17, 0, -10), texts[2], (255, 255, 255))
    smallDrawText((35, 3, -18), "Roll: %d " % roll[2], (255, 255, 255))
    smallDrawText((35, 1, -18), "Pitch: %d " % pitch[2], (255, 255, 255))
    smallDrawText((35, -1, -18), "Yaw: %d " % yaw[2], (255, 255, 255))

def BoardText3():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((-18, -15, -10), texts[3], (255, 255, 255))
    smallDrawText((-15, -20, -18), "Roll: %d " % roll[3], (255, 255, 255))
    smallDrawText((-15, -22, -18), "Pitch: %d " % pitch[3], (255, 255, 255))
    smallDrawText((-15, -24, -18), "Yaw: %d " % yaw[3], (255, 255, 255))

def BoardText4():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((0, -15, -10), texts[4], (255, 255, 255))
    smallDrawText((10, -20, -18), "Roll: %d " % roll[4], (255, 255, 255))
    smallDrawText((10, -22, -18), "Pitch: %d " % pitch[4], (255, 255, 255))
    smallDrawText((10, -24, -18), "Yaw: %d " % yaw[4], (255, 255, 255))


def BoardText5():           #PONGO TEXTO A UN TABLERO

    glLoadIdentity()
    gluPerspective(90, (display[0] / display[1]), 0.1, 50.0)
    glTranslatef(-5.5, 3, -5)  # MATRIZ DE TRANSLACION

    drawText((17, -15, -10), texts[5], (255, 255, 255))
    smallDrawText((35, -20, -18), "Roll: %d " % roll[5], (255, 255, 255))
    smallDrawText((35, -22, -18), "Pitch: %d " % pitch[5], (255, 255, 255))
    smallDrawText((35, -24, -18), "Yaw: %d " % yaw[5], (255, 255, 255))

def EventManager0():           #DIBUJA MENU O UN TABLERO
    DrawGL0()
    BoardText0()

def EventManager1():           #DIBUJA MENU O UN TABLERO
    DrawGL1()
    BoardText1()

def EventManager2():           #DIBUJA MENU O UN TABLERO
    DrawGL2()
    BoardText2()

def EventManager3():           #DIBUJA MENU O UN TABLERO
    DrawGL3()
    BoardText3()

def EventManager4():           #DIBUJA MENU O UN TABLERO
    DrawGL4()
    BoardText4()

def EventManager5():           #DIBUJA MENU O UN TABLERO
    DrawGL5()
    BoardText5()

def main():
    global coms
    #selected_board = 1


    InitPygame()
    InitGL()

    pitch[0] = -30
    yaw[0] = -45
    pitch[1] = -30
    pitch[2] = -30
    yaw[2] = 45
    pitch[3] = 22
    yaw[3] = -45
    pitch[4] = 22
    pitch[5] = 22
    yaw[5] = 45


    gluPerspective(45, (display[0] / display[1]), 0.1, 50.0)

    # glTranslatef(0.0, 0.0, -1)

    glRotatef(0, 0, 0, 0)

    coms = None

    t1 = threading.Thread(target=ReadData, args=())
    t1.start()

    while True:

     

        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                pygame.quit()
                quit()
            
            if event.type == pygame.KEYDOWN:
            #navigating through 3D boards
                if event.key == pygame.K_LEFT:
                    roll[1] -= 10
                
                if event.key == pygame.K_RIGHT:
                    roll[1] += 10

                if event.key == pygame.K_UP:
                    pitch[1] -= 10
                
                if event.key == pygame.K_DOWN:
                    pitch[1] += 10  
                
                if event.key == pygame.K_s:
                    yaw[1] -= 10
                
                if event.key == pygame.K_w:
                    yaw[1] += 10        

                
            if event.type == pygame.KEYUP:
                keys[event.key] = 0 

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

        EventManager0()
        EventManager1()
        EventManager2()
        EventManager3()
        EventManager4()
        EventManager5()

        
        pygame.display.flip()
        #pygame.time.wait(10)


if __name__ == '__main__':
    main()