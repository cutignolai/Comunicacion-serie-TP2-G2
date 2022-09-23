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

    config.baud_rate = 9600;
    config.non_blocking = 0;
    config.want_parity = 0;   
    config.data_9bits = 0;
    config.parity_type = 0;
    config.double_stop_bit = 0;
    config.use_fifo = 1;
    config.bit_rate = 0;        //check later
    uartInit (id, config);

}

void sendData (board current_board, uint8_t group, O_EVENT event){

    //------------Message generation----------------
    // meesage: SS-Group number-Event-Sign-Value-ES
    char message[MESSAGE_SIZE];
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
        boards[group-1].roll = current_board.roll;
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
    uint8_t c = (data/100);
    uint8_t d = (data/10);
    if(d>=10){
    	d = d - 10*c;
    }
    d = d + '0';
    c = c + '0';
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
    
    uint8_t bytes;
    bytes = uartWriteMsg(id, &message[0], MESSAGE_SIZE);
    /*while (!uartIsTxMsgComplete(id)){
        bytes = uartWriteMsg(id, &message[0], MESSAGE_SIZE);
    }*/



}

board getBoard(uint8_t group){
    return boards[group-1];
}

/*******************************************************************************
 ******************************************************************************/

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
