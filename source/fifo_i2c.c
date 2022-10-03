/***************************************************************************//**
  @file     fifo.c
  @brief    Software circular FIFO implementation
  @author   Olivia De Vincenti
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdio.h>
#include "fifo_i2c.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

typedef struct {
	I2C_fifo_value_t    queue[MAX_FIFO_SIZE];
	size_t          head;
	size_t          tail;
    bool            is_buffer_full;
} I2C_fifo_t;


/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void I2C_init_fifo(I2C_fifo_id_t id);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

static I2C_fifo_t FIFO_Array[I2C_FIFO_MAX_N];   // FIFO Array
static I2C_fifo_id_t fifo_n = 0;            // Amount of active FIFO queues

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

I2C_fifo_id_t I2C_FIFO_GetId(void)
{
#ifdef FIFO_DEVELOPMENT_MODE
    if (fifo_n >= I2C_FIFO_MAX_N)
    {
        return FIFO_INVALID_ID;
    }
    else
#endif // FIFO_DEVELOPMENT_MODE
    {
        I2C_init_fifo(fifo_n);
#ifdef FIFO_VERBOSE
        printf("fifo verbose - Fifo n° %u in use\n", fifo_n);
#endif
        return fifo_n++;
    }
}

bool I2C_FIFO_IsBufferFull(I2C_fifo_id_t id){

    return FIFO_Array[id].is_buffer_full;
}

bool I2C_FIFO_IsBufferEmpty(I2C_fifo_id_t id){

    return FIFO_Array[id].head == FIFO_Array[id].tail && !FIFO_Array[id].is_buffer_full;
}

size_t I2C_FIFO_WriteToBuffer(I2C_fifo_id_t id, I2C_fifo_value_t* data, size_t data_size){
    
    size_t i;

    for (i = 0; i < data_size && i < MAX_FIFO_SIZE; i++){

        if (I2C_FIFO_PushToBuffer(id, data + i) == FIFO_BUFFER_FULL){      // Push value to buffer, if buffer is full:
            break;              // Stop writing
        }
    }

#ifdef FIFO_VERBOSE
    printf("fifo verbose - Wrote %u values to FIFO %u: %s\n", i, id, FIFO_Array[id].queue + FIFO_Array[id].head - i);
#endif

    return i;
}

size_t I2C_FIFO_ReadFromBuffer(I2C_fifo_id_t id, I2C_fifo_value_t* data_ptr, size_t data_size){

    size_t i = 0;
    bool b = I2C_FIFO_IsBufferEmpty(id);        // Check if buffer is empty

    // Until the amount of values pulled is data_size or buffer is empty
    for (i = 0; b != FIFO_BUFFER_EMPTY && i < data_size && i < MAX_FIFO_SIZE; i++){      
        b = I2C_FIFO_PullFromBuffer(id, data_ptr + i) == FIFO_BUFFER_EMPTY;      // Pull value from buffer
    }

#ifdef FIFO_VERBOSE
    printf("fifo verbose - Read %u values from FIFO %u: %.*s\n", i, id, i, FIFO_Array[id].queue + FIFO_Array[id].tail - i);
#endif

    return i;           // Amount of values pulled
}

size_t I2C_FIFO_ReadAll(I2C_fifo_id_t id, I2C_fifo_value_t* data_ptr){

    size_t i = 0;
    bool b = I2C_FIFO_IsBufferEmpty(id);        // Check if buffer is empty

    for (i = 0; b != FIFO_BUFFER_EMPTY && i < MAX_FIFO_SIZE; i++){           // Until buffer is empty
        b = I2C_FIFO_PullFromBuffer(id, data_ptr + i) == FIFO_BUFFER_EMPTY;      // Pull value from buffer
    }

#ifdef FIFO_VERBOSE
    printf("fifo verbose - Read %u values from FIFO %u: %.*s\n", i, id, i, FIFO_Array[id].queue + FIFO_Array[id].tail - i);
#endif

    return i;           // Amount of values pulled
}

bool I2C_FIFO_PushToBuffer(I2C_fifo_id_t id, I2C_fifo_value_t* data){

    if (!FIFO_Array[id].is_buffer_full){                        // If buffer is not full

        *((I2C_fifo_value_t*)(&FIFO_Array[id].queue[0] + FIFO_Array[id].head)) = *data;   // Write data
#ifdef FIFO_VERBOSE
        printf("fifo verbose - Pushed to FIFO %u: %u\n", id, data);
#endif

        if (FIFO_Array[id].head + 1 < MAX_FIFO_SIZE){           // Overflow?
            FIFO_Array[id].head++;                              // Advance head
        } else {
            FIFO_Array[id].head = 0;                            // Reset head
        }

        if (FIFO_Array[id].head == FIFO_Array[id].tail ){       // If buffer is now full
            FIFO_Array[id].is_buffer_full = FIFO_BUFFER_FULL;   // Set flag
#ifdef FIFO_VERBOSE
            printf("fifo verbose - FIFO %u buffer full\n", id);
#endif
        }
    }

    return FIFO_Array[id].is_buffer_full;       // Return buffer state
}

bool I2C_FIFO_PullFromBuffer(I2C_fifo_id_t id, I2C_fifo_value_t* data_ptr){

    bool r = !FIFO_BUFFER_EMPTY;

    if (FIFO_Array[id].head == FIFO_Array[id].tail && !FIFO_Array[id].is_buffer_full){    // If buffer is empty
#ifdef FIFO_VERBOSE
        printf("fifo verbose - FIFO %u buffer empty\n", id);
#endif
        r = FIFO_BUFFER_EMPTY;       // Return buffer empty state and don't write in data_ptr*
    
    } else {                            // If buffer is not empty

        *(data_ptr) = *((I2C_fifo_value_t*) (&FIFO_Array[id].queue[0] + FIFO_Array[id].tail));      // Write tail value to data_ptr
#ifdef FIFO_VERBOSE
        printf("fifo verbose - Pulled from FIFO %u: %u\n", id, *(data_ptr));
#endif

        if (FIFO_Array[id].tail + 1 < MAX_FIFO_SIZE){           // Overflow?
            FIFO_Array[id].tail++;                              // Advance tail
        } else {
            FIFO_Array[id].tail = 0;                            // Reset tail
        }

        FIFO_Array[id].is_buffer_full = !FIFO_BUFFER_FULL;      // Buffer is now not full

        // if (FIFO_Array[id].tail == FIFO_Array[id].head){ 
        //     r = FIFO_BUFFER_EMPTY;                              // Return buffer now empty state
#ifdef FIFO_VERBOSE
            printf("fifo verbose - FIFO %u buffer empty\n", id);
#endif
        // }    

    }

    return r;
}

size_t I2C_FIFO_GetBufferLength(I2C_fifo_id_t id){
    size_t r;
    if (FIFO_Array[id].is_buffer_full){
        r = MAX_FIFO_SIZE;
    } else {
        r = FIFO_Array[id].head - FIFO_Array[id].tail;
    }
#ifdef FIFO_VERBOSE
    printf("fifo verbose - FIFO %u buffer length: %u\n", r);
#endif
    return r;
}

void I2C_FIFO_Reset(I2C_fifo_id_t id){
	I2C_init_fifo(id);
#ifdef FIFO_VERBOSE
    printf("fifo verbose - Reset FIFO %u\n", id);
#endif
}

void I2C_FIFO_ClearBuffer(I2C_fifo_id_t id){
    uint8_t i;
    for (i = 0; i < MAX_FIFO_SIZE - 1; i++){
        // FIFO_Array[id].queue[i] = fifo_empty_value;
    }
#ifdef FIFO_VERBOSE
    printf("fifo verbose - Clear FIFO %u\n", id);
#endif
    I2C_FIFO_Reset(id);
}

void I2C_FIFO_FreeId(I2C_fifo_id_t id){
    if (fifo_n > 0){
#ifdef FIFO_VERBOSE
    printf("fifo verbose - Free FIFO %u\n", id);
#endif
        fifo_n--;
    }
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

void I2C_init_fifo(I2C_fifo_id_t id){
    FIFO_Array[id].head = 0;
    FIFO_Array[id].tail = 0;
    FIFO_Array[id].is_buffer_full = false;
}

/******************************************************************************/
