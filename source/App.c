/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include "board.h"
#include "gpio.h"
#include "can.h"
#include "ComunicacionCAN.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define BASE_ID 0b00100000000
#define MY_BOARD_ID (BASE_ID + 2)


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/



/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
	canComunicationInit(0);
}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
	/*
    CAN_DataFrame frame;

    CAN_DataFrame *frame_pointer = &frame;

	frame.ID = MY_BOARD_ID;

	frame_pointer->dataByte0 = 0x00;
	frame_pointer->dataByte1 = 0x03;
	frame_pointer->dataByte2 = 0x03;
	frame_pointer->dataByte3 = 0x03;
	frame_pointer->dataByte4 = 0x03;
	frame_pointer->dataByte5 = 0x03;
	frame_pointer->dataByte6 = 0x03;
	frame_pointer->dataByte7 = 0x03;

	CAN_WriteTxMB(0, &frame);

	*/

	char message[] = "AAAAAAAA";

	sendCanMessage(message, 0);

	static volatile int i;

	i = 800000;

	while(i > 0)
	{
		--i;
	}

	char messageAns[] = "XXXXXXXX";

	int boardStatus = getBoardStatus(0);
	if (boardStatus == 1)
	{
		receiveCanMessage(messageAns, 0);
	}



	i = 800000;

	while(i > 0)
	{
		--i;
	}

    
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/




/*******************************************************************************
 ******************************************************************************/
