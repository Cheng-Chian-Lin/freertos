#include <stdio.h>

#include <stdlib.h>



#define ENABLE_DEBUG

#include "FreeRTOS.h"



/* heap_ww.c */

void *pvPortMalloc(size_t xWantedSize);

void vPortFree(void *pv);

size_t xPortGetFreeHeapSize(void);



struct slot {

    void *pointer;

    unsigned int size;

    unsigned int lfsr;

};



#define CIRCBUFSIZE 5000

//unsigned int write_pointer, read_pointer;

static struct slot slots[CIRCBUFSIZE];

//static unsigned int lfsr = 0xACE1;



static unsigned int circbuf_size(void);





static void write_cb(struct slot foo);



static struct slot read_cb(void);



// Get a pseudorandom number generator from Wikipedia

static int prng(void);



int mmtest_command();

