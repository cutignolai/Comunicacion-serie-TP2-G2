/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "data_manager.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

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

//testing
static uint8_t rand_event;
static bool position_event;
static board periferic;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

O_EVENT GetPositionEvent(void);   


void GetPositionChange(void);
//updates the current board to the one indicated
void setBoard (void);

void updateBoard(void);

//testing

void genData(void);

O_EVENT GetPositionEvent(void);

uint16_t getRoll(void);

uint16_t getPitch(void);

uint16_t getYaw(void);

uint8_t getGroup (void);      
/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */
void App_Init (void)
{
      
    dataManager_init();

    //testing
    rand_event = 0;
    position_event = false; 
    periferic.roll = 0;
    periferic.pitch = 0;
    periferic.yaw = 0;

}

/* Función que se llama constantemente en un ciclo infinito */
void App_Run (void)
{
    updateBoard();

    //test
    genData();
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
            sendData(current_board, current_group, ROLL_EVENT);
            break;
        case PITCH_EVENT:
            setBoard();       
            current_board.pitch = getPitch();
            sendData(current_board, current_group, PITCH_EVENT);
            break;
        case YAW_EVENT:
            setBoard();        
            current_board.yaw = getYaw();
            sendData(current_board, current_group, YAW_EVENT);
            break;
        default:
        	//do nothing
        	break;
    }
    //testing -> turns off the event so 
    position_event = false;
}

void setBoard (void){
    current_group = getGroup();
    board* p = getBoards();
    current_board.roll = (p+current_group)->roll;
    current_board.pitch = (p+current_group)->pitch;
    current_board.yaw = (p+current_group)->yaw;
}

//testing functions

void genData(void)
{
    uint32_t veces = 4000000UL;
    while (veces--);
    if (rand_event == 0){
        periferic.roll += 30;
        rand_event++;
        position_event = true;
    }
    else if (rand_event == 1){
        periferic.pitch -= 25;
        rand_event++;
        position_event = true;
    }
    else if (rand_event == 2){
        periferic.yaw += 10;
        rand_event++;
        position_event = true;
    }
    else{
        rand_event = 0;
        position_event = false;
    }
    
    
}

O_EVENT GetPositionEvent(void){
    O_EVENT my_event;
    if (position_event){
        switch (rand_event)
        {
            case 0:
                my_event = ROLL_EVENT;
                break;
            
            case 1:
                my_event = PITCH_EVENT;
                break;
            case 2:
                my_event = YAW_EVENT;
                break;
            default:
                my_event = NO_EVENT;
                break;
        }
        return my_event;
    }
    else
        return NO_EVENT;
    
}

uint16_t getRoll(void){
    return periferic.roll;
}

uint16_t getPitch(void){
    return periferic.pitch;
}

uint16_t getYaw(void){
    return periferic.yaw;
}

uint8_t getGroup (void){    //for test purpose only will return our group number
	uint8_t group = 2;
    return (group-1);           // this should be data saved the moment an interuption is generated and new data is available
}
/*******************************************************************************
 ******************************************************************************/
