#ifndef __SERIAL_H

#define __SERIAL_H

#define BUFFER_SIZE  4096
#define REI_SIZE 	 30
#define MAX_COM_NUM  5

int set_com_config(int fd,int baud_rate, int data_bits,char parity,int stop_bits);
int serial_init_us(void);

#endif