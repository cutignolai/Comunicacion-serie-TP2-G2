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

#define GROUPS 6

//message defines
#define SS  'S'
#define ES  'E'



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static board boards[GROUPS];
static uint8_t id;
static uart_cfg_t config;
static char message[MESSAGE_LENGHT];


/*******************************************************************************
 *         FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

void add_angle(uint8_t position, int16_t angle);


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

void sendData (board current_board, uint8_t group){

    //------------Message generation----------------
    // meesage: SS-Group number-Event-Sign-Value-ES
    


    //Start sentinel
    message[0] = SS;
    //Group number
    message[M_GROUP] = group + '0';
    //Roll, pitch or yaw
   
    message[M_ROLL] = 'R';
    add_angle(M_ROLL, current_board.roll);
    boards[group].roll = current_board.roll;

    message[M_PITCH] = 'P';
    add_angle(M_PITCH, current_board.pitch);
    boards[group].pitch = current_board.pitch;

    message[M_YAW] = 'Y';
    add_angle(M_YAW, current_board.yaw);
    boards[group].yaw = current_board.yaw;
    
    //End sentinel
    message[MESSAGE_LENGHT-1] = ES;


    	//S2R+030E
    //------------Post management----------------
    
    uint8_t bytes;
    bytes = uartWriteMsg(id, &message[0], MESSAGE_LENGHT);
    /*while (!uartIsTxMsgComplete(id)){
        bytes = uartWriteMsg(id, &message[0], MESSAGE_SIZE);
    }*/

    /*printf("message count: %d \n", message_count);
    message_count++;

    uint8_t i;
    printf("message:");
    for (i=0; i<8;i++){
    	printf("%c", message[i]);
    }
    printf("\n");*/



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

void add_angle(uint8_t position, int16_t angle){
    //In what direction does the change ocurr
    uint16_t data;

    if (angle>0){
        message[position + 1] = '+';
        data = angle;
    }
        
    else {
        message[position + 1] = '-';
        data = -angle;
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

    message[position + 2] = c;
    message[position + 3] = d;
    message[position + 4] = u;
}


/*******************************************************************************
 ******************************************************************************/
