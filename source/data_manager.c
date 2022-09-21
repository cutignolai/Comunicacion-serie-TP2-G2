/***************************************************************************//**
  @file     data_manager.c
  @brief    provides functions to send coded data to the pc
  @author   Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "data_manager.h"
#include "uart.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define GROUPS 5

//message defines
#define SS  'S'
#define ES  'E'
#define MESSAGE_SIZE 8



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static board boards[GROUPS];
static uint8_t id;
static uart_cfg_t config;

/*******************************************************************************
 *         FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/




/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void dataManager_init(void){

    id = 0;   
    uartInit (id, config);

}

void sendData (board current_board, uint8_t group, O_EVENT event){

    //------------Message generation----------------
    // meesage: SS-Group number-Event-Sign-Value-ES
    char message[8];
    int16_t data;

    //Start sentinel
    message[0] = SS;
    //Group number
    message[1] = group + '0';
    //Roll, pitch or yaw
    switch (event)        
    {
    case ROLL_EVENT:
        message[2] = 'R';
        data = current_board.roll;
        boards[group].roll = current_board.roll;
        break;
    case PITCH_EVENT:
        message[2] = 'P';
        data = current_board.pitch;
        boards[group].pitch = current_board.pitch;
        break;
    case YAW_EVENT:
        message[2] = 'Y';
        data = current_board.yaw;
        boards[group].yaw = current_board.yaw;
        break;
    default:
        break;
    }
    //In what direction does the change ocurr
    if (data>0)
        message[3] = '+';
    else {
    message[3] = '-';
    data = -data;
    }

    //the corresponding variation, at a max number of 999
    uint8_t c = (data/100)+'0';
    uint8_t d = (data/10)+'0';
    if(d>=10);
        d = d - 10;
    uint8_t u = (data%10)+'0';

    //if the variation is not big enough
    if ((c == 0) && (d == 0) && (u <= 5)){   
        message[4] = 0;
        message[5] = 0;
        message[6] = 0;
    }
    else{
        message[4] = c;
        message[5] = d;
        message[6] = u;
    }
    //End sentinel
    message[7] = ES;

    //------------Post management----------------
    
    //send message to pc
    uartWriteMsg(id, &message, MESSAGE_SIZE);

    while (!uartIsTxMsgComplete(id)){
        uartWriteMsg(id, &message, MESSAGE_SIZE);
    }



}

board* getBoards(void){
    return &boards;
}

/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

uint8_t getGroup (void){    //for test purpose only will return our group number
    return 2;               // this should be data saved the moment an interuption is generated and new data is available
}

/*******************************************************************************
 ******************************************************************************/
