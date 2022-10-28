#include "../headers/circularbuffer.h"


static double bit[14];
static uint16_t bit_pre_filter[14];


void deletion(int arr);
void compoutput2UART_callback (double* filterOutputPointer);
void bitstream_reconTX(char demod_bit);

static uint16_t cqueue_arr[MAX][2];
static int front_p[2] = {-1,-1};
static int rear_p[2] = {-1,-1};
static _Bool modulating_bit = 1;
void insert(uint16_t item, int arr)
{
	int* front = front_p + arr;
	int* rear = rear_p + arr;
    if ((*front == 0 && *rear == MAX - 1) || (*front == *rear + 1))
    {
//        printf("Queue Overflow n");
        return;
    }
    if (*front == -1)
    {
        *front = 0;
        *rear = 0;
    }
    else
    {
        if (*rear == MAX - 1)
            *rear = 0;
        else
            *rear = *rear + 1;
    }
    cqueue_arr[*rear][arr] = item;
}
uint16_t obtain(int arr)
{
    uint16_t temp = (cqueue_arr[*(front_p+arr)][arr]);
    deletion(arr);
    return temp;
}
void deletion(int arr)
{
	int* front = front_p + arr;
	int* rear = rear_p + arr;
    if (*front == -1)
    {
//        printf("Queue Underflown");
        return;
    }
   // printf("Element deleted from queue is : %dn", cqueue_arr[front]);
    if (*front == rear)
    {
        *front = -1;
        *rear = -1;
    }
    else
    {
        if (*front == MAX - 1)
            *front = 0;
        else
            *front = *front + 1;
    }
}


void compoutput2UART_callback (double* filterOutputPointer)
{
	float sample_mean = (*(filterOutputPointer+10)+*(filterOutputPointer+11)+*(filterOutputPointer+12))/3.0;
	if (sample_mean<0)
	{
		bitstream_reconTX(1);
	}
	else if (sample_mean>0)
	{
		bitstream_reconTX(0);
	}
}

void bitstream_reconTX(char demod_bit)
{
	static int i = 0;
	static uint8_t msg=0;
	static uint8_t oparity=0;
	char msg_sent;

	if (i==0)
	{
		if(demod_bit==1)
		{
			//ESTOY EN IDLE, nada para transmitirle a UART.
		}
		else if (demod_bit==0)
		{
			i++;	//Recibí un start
		}
	}
	else if ((i>=1)&(i<=8))
	{
		msg= (msg>>(1))|(demod_bit<<7);
		oparity = oparity ^ demod_bit;
	 	i++;
	}
	else if(i==9)
	{
		if(demod_bit==oparity)
		{
			msg_sent= msg;
			uartWriteMsg(1, &msg, 1);//LLAMO A UART1 TX MANDANDOLE msg
		}
		else if(demod_bit!=oparity)
		{
			// error
			i = 0;
		}
		i=0;
		msg=0;
		oparity=0;
	}
}

void change_bit_callback()
{
    static int i = 0;
    static uint8_t msg=0;
    static uint8_t* msg_p = &msg;
    static uint8_t oparity=0;
    if (i==0)
    {
        _Bool flag_UART=uartIsRxMsg(1);
        if (!flag_UART)
        {
        	modulating_bit=1;            //No llego nada,  por uart, modulo un 1
        }
        else
        {
            uartReadMsg(1, msg_p, 1);            //guardo lo que me llego por UART
            modulating_bit=0;                //seteo el valor a modular como START
            i++;
        }
    }
    else if ( (i>=1) && (i<=8))                    //
    {
    	modulating_bit=(*msg_p>>(i-1))&0b00000001;
        oparity = oparity ^ modulating_bit;
        i++;
    }
    else if(i==9)
    {
    	modulating_bit=oparity;
           i++;
    }
    else if (i==10)
    {
    	modulating_bit=1; //modulo el stop
        i=0;
        oparity=0;
        msg=0;
    }
}
