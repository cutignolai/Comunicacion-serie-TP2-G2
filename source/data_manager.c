/***************************************************************************//**
  @file     data_manager.c
  @brief    provides functions to send coded data to the pc
  @author   Pedro DL
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "data_manager.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define GROUPS 5



/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

board boards[GROUPS];

/*******************************************************************************
 *         FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

uint8_t getGroup (void);       //indicates which group has sent data

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void sendData (board current_board, O_EVENT event){
      uint8_t group = getGroup();


      //------------Message generation----------------

      char message[7];
      int16_t data;

      //Group number
      message[0] = group + '0';
      //Roll, pitch or yaw
      switch (event)        
      {
      case ROLL_EVENT:
          message[1] = 'R';
          data = current_board.roll;
          boards[group].roll = current_board.roll;
          break;
      case PITCH_EVENT:
          message[1] = 'P';
          data = current_board.pitch;
          boards[group].pitch = current_board.pitch;
          break;
      case YAW_EVENT:
          message[1] = 'Y';
          data = current_board.yaw;
          boards[group].yaw = current_board.yaw;
          break;
      default:
          break;
      }
      //In what direction does the change ocurr
      if (data>0)
          message[2] = '+';
      else {
        message[2] = '-';
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
          message[3] = 0;
          message[4] = 0;
          message[5] = 0;
      }
      else{
          message[3] = c;
          message[4] = d;
          message[5] = u;
      }

      //------------Post management----------------

      //call a function that sends via UART to the pc or whatever


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
