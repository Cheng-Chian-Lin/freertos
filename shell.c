#include "fio.h"
#include "shell.h"
#include "str_func.h"
#include "mmtest.h"


void shell_printf(char *buf, int count){
	char msg[] = "\n\r";

	fio_write(1, msg, sizeof(msg) );
	fio_write(1, buf, count);
}

void check_keyword(char *msg, char *msg_size)
{
	int i;
	
		if (!(strcmp(msg,"help"))) {
			fio_write(1, "\n\rsupported instructions are as follow:\n\r",41);
			fio_write(1, "\n\rhello --> greetings from computer!\n\r",38);
			fio_write(1, "\n\rmmtest --> perform malloc and free!\n\r",43);
			
		}
else if(!(strcmp(msg,"hello"))) {
fio_write(1, "\n\rHello! How are you doing?",28);

}	
else if(!(strcmp(msg,"mmtest"))) {
mmtest_command();
}
	else{
		fio_write(1, "\n\rcommand not found!\n\r",22);
	}
}

