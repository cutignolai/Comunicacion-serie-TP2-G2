/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "data_manager.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/



/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

static board current_board;
static uint8_t current_group;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

O_EVENT GetPositionEvent(void);   


void GetPositionChange(void);
//updates the current board to the one indicated
void setBoard (void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
    
    dataManager_init();

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    updateBoard();

    //it's lacking functions getGroup, getRoll/Pitch/Yaw which essentialy let it know if a change is made and where it's made
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/
void updateBoard(void){

    switch(GetPositionEvent()){
        case ROLL_EVENT:
            setBoard();
            current_board.roll = getRoll();
            SendData(current_board, current_group, ROLL_EVENT);
            break;
        case PITCH_EVENT:
            setBoard();       
            current_board.pitch = getPitch();
            SendData(current_board, current_group, PITCH_EVENT);
            break;
        case YAW_EVENT:
            setBoard();        
            current_board.yaw = getYaw();
            SendData(current_board, current_group, YAW_EVENT);
            break;
        default:        
            //do nothing
    }
}

void setBoard (void){
    current_group = getGroup();
    board* p = getBoards();
    current_board.roll = *(p + current_group).roll;
    current_board.pitch = *(p + current_group).pitch;
    current_board.yaw = *(p + current_group).yaw;
}

/*******************************************************************************
 ******************************************************************************/
