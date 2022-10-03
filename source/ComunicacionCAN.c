/*******************************************************************************
  @file     ComunicacionCAN.c
  @author   Ignacio Cutignola
 ******************************************************************************/

#include "can.h"
#include "ComunicacionCAN.h"






static int mbStatus[] = {NOTHING_YET, NOTHING_YET, NOTHING_YET, NOTHING_YET, NOTHING_YET, NOTHING_YET};



void canComunicationInit(void)
{
    CAN_Init();
}


void canMBInit(uint8_t id)
{
    CAN_ConfigureRxMB(id, MY_BOARD_ID);
}

void sendCanMessage(char* message, int board)
{
    CAN_DataFrame frame;

    CAN_DataFrame *frame_pointer = &frame;

	frame.ID = MY_BOARD_ID;

	frame_pointer->dataByte0 = message[2];
	frame_pointer->dataByte1 = message[3];
	frame_pointer->dataByte2 = message[4];
	frame_pointer->dataByte3 = message[5];
	frame_pointer->dataByte4 = message[6];
	frame_pointer->dataByte5 = message[5];
	frame_pointer->dataByte6 = message[6];
	frame_pointer->dataByte7 = message[7];

	CAN_WriteTxMB(board, &frame);

    mbStatus[board] = MESSAGE_SENT;

    // int status = mbStatus[board];

}



bool receiveCanMessage(char* message, int board)
{
    CAN_DataFrame frameAnswer;

	CAN_DataFrame *frame_pointerAnsw = &frameAnswer;

	bool status;

    status = CAN_ReadRxMB(board, frame_pointerAnsw);

    message[0] = 'S';
    message[1] = 48+board;
    message[2] = frame_pointerAnsw->dataByte0;
    message[3] = frame_pointerAnsw->dataByte1;
    message[4] = frame_pointerAnsw->dataByte2;
    message[5] = frame_pointerAnsw->dataByte3;
    message[6] = frame_pointerAnsw->dataByte4;
    message[7] = 'E';

    mbStatus[board] = NOTHING_YET;

    return status;
}



bool getBoardStatus(int board)
{
    return mbStatus[board];
}
