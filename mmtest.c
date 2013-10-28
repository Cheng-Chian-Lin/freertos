#define ENABLE_DEBUG

#include "FreeRTOS.h"
#include "fio.h"




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

unsigned int write_pointer, read_pointer;

static struct slot slots[CIRCBUFSIZE];

static unsigned int lfsr = 0xACE1;



static unsigned int circbuf_size(void)

{

    return (write_pointer + CIRCBUFSIZE - read_pointer) % CIRCBUFSIZE;

}



static void write_cb(struct slot foo)

{

    if (circbuf_size() == CIRCBUFSIZE - 1) {

        //fprintf(stderr, "circular buffer overflow\n");

        //exit(1);
	fio_write(1, "\r\ncircular buffer overflow\r\n",21);

    }

    slots[write_pointer++] = foo;

    write_pointer %= CIRCBUFSIZE;

}



static struct slot read_cb(void)

{

    struct slot foo;

    if (write_pointer == read_pointer) {

        // circular buffer is empty

        return (struct slot){ .pointer=NULL, .size=0, .lfsr=0 };

    }

    foo = slots[read_pointer++];

    read_pointer %= CIRCBUFSIZE;

    return foo;

}





// Get a pseudorandom number generator from Wikipedia

static int prng(void)

{

    static unsigned int bit;

    /* taps: 16 14 13 11; characteristic polynomial: x^16 + x^14 + x^13 + x^11 + 1 */

    bit  = ((lfsr >> 0) ^ (lfsr >> 2) ^ (lfsr >> 3) ^ (lfsr >> 5) ) & 1;

    lfsr =  (lfsr >> 1) | (bit << 15);

    return lfsr & 0xffff;

}



int mmtest_command()

{

    int i, size;

    char *p;



    while (1) {

        size = prng() & 0x7FF;

        //fio_write(1,"\r\ntry to allocate ", size);
	//fio_write(1,size,sizeof(size));
	//fio_write(1," bytes\r\n",10);
fio_printf(1, "try to allocate %d bytes\r\n", size);

        p = (char *) pvPortMalloc(size);

        //fio_write(1,"\r\nmalloc returned ",20);
	//fio_write(1,p,sizeof(p));
	//fio_write(1,"\r\n",4);
 fio_printf(1, "malloc returned 0x%x\r\n", p);

        if (p == NULL) {

            // can't do new allocations until we free some older ones

            while (circbuf_size() > 0) {

                // confirm that data didn't get trampled before freeing

                struct slot foo = read_cb();

                p = foo.pointer;

                lfsr = foo.lfsr;  // reset the PRNG to its earlier state

                size = foo.size;

                //fio_write(1,"free a block, size ",19);
 		//fio_write(1,size,sizeof(size));
		//fio_write(1,"\r\n",4);
fio_printf(1, "free a block, size %d\r\n", size);

                for (i = 0; i < size; i++) {

                    unsigned char u = p[i];

                    unsigned char v = (unsigned char) prng();

                    if (u != v) {

                       /* fio_write(1,"\r\nOUCH: u=",12);
			fio_write(1,u,sizeof(u));
			fio_write(1,"v=",2);
			fio_write(1,v,sizeof(v));
			fio_write(1,"\r\n",4);*/
 fio_printf(1, "OUCH: u=%x, v=%x\r\n", u, v);

                        return 1;

                    }

                }

                vPortFree(p);

                if ((prng() & 1) == 0) break;

            }

        } else {

            //fio_write(1,"\r\nallocate a block, size ",23);
	    //fio_write(1,size,sizeof(size));
	   // fio_write(1,"\r\n",4);
fio_printf(1, "allocate a block, size %d\r\n", size);

            write_cb((struct slot){.pointer=p, .size=size, .lfsr=lfsr});

            for (i = 0; i < size; i++) {

                p[i] = (unsigned char) prng();

            }

        }

    }



    return 0;

}
