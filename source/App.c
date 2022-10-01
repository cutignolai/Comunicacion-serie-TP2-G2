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
static char* can_message_ptr;

//testing
static uint8_t rand_event;
static bool position_event;
static bool data_event;
static board periferic;

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


void GetPositionChange(void);
//updates the current board to the one indicated
void setBoard (void);

void updateBoard(void);

bool checkAngle (int16_t angle);

//testing

void genData(void);

bool GetDataEvent(void);

uint16_t getRoll(void);

uint16_t getPitch(void);

uint16_t getYaw(void);

uint8_t getGroup (void);

char* getChange(void);
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
    data_event = false;
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

    if(GetDataEvent()){
        data_event = false;                     //turn off flag

        //codigo de testeo

        if (rand_event == 0){
        	char message1 []= "S2R+030P-050Y+100E";
        	can_message_ptr = &message1[0];
        	rand_event++;
        }
        else if ((rand_event == 1) || (rand_event == 3)){
			char message2 []= "S2R+045P-075Y+125E";
			can_message_ptr = &message2[0];
			rand_event++;
		}
        else if (rand_event == 2){
			char message3 []= "S2R+060P-100Y+150E";
			can_message_ptr = &message3[0];
			rand_event++;
		}
        if (rand_event == 4)
        	rand_event = 0;



        //can_message_ptr = &message[0];          //get the pointer to the new message
        setBoard();                             //set the board ready to update         
        sendData(current_board, current_group); //send board to pc  

    }
}

void setBoard (void){
	//get the current board being used
	current_group = *(can_message_ptr + M_GROUP) - '0';

    int16_t roll;
    int16_t pitch;
    int16_t yaw;

    int16_t c = *(can_message_ptr + M_ROLL + 2) - '0';
    int16_t d = *(can_message_ptr + M_ROLL + 3) - '0';
    int16_t u = *(can_message_ptr + M_ROLL + 4) - '0';

    roll = c*100 + d*10 + u;

    if ((*(can_message_ptr + M_ROLL + 1)) == '-')
        roll = -roll;

    

    c = *(can_message_ptr + M_PITCH + 2) - '0';
    d = *(can_message_ptr + M_PITCH + 3) - '0';
    u = *(can_message_ptr + M_PITCH + 4) - '0';

    pitch = c*100 + d*10 + u;

    if ((*(can_message_ptr + M_PITCH + 1)) == '-')
        pitch = -pitch;
   

    c = *(can_message_ptr + M_YAW + 2) - '0';
    d = *(can_message_ptr + M_YAW + 3) - '0';
    u = *(can_message_ptr + M_YAW + 4) - '0';

    yaw = c*100 + d*10 + u;

    if ((*(can_message_ptr + M_YAW + 1)) == '-')
        yaw = -yaw;
    

    if(checkAngle(roll)){
        current_board.roll = roll;
    }
    if(checkAngle(yaw)){
        current_board.yaw = yaw;
    }
    if(checkAngle(pitch)){
        current_board.pitch = pitch;
    }
}

int16_t getMessageNum (void){
    
}

bool checkAngle (int16_t angle){
    if (angle > 180)
        return false;
    if (angle < -180)
        return false;
    else
        return true;
}

//testing functions

char* getChange(void){
    char message []= "S2R+030P-050Y+100E";
    return &message[0];
}

void genData(void)
{
    uint32_t veces = 14000000UL;
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

bool GetDataEvent(void){
    return true;
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
    return group;           // this should be data saved the moment an interuption is generated and new data is available
}
/*******************************************************************************
 ******************************************************************************/
