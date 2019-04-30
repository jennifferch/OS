#ifndef UART_H_
#define UART_H_

#include "chip.h"
#include "string.h"

typedef enum _ciaaUarts_e
{
	CIAA_UART_485 = 0,
	CIAA_UART_USB = 1,
	CIAA_UART_232 = 2
}ciaaUART_e;

#define rs485Print(x) uartSend(0, (uint8_t *)(x), strlen(x))
#define dbgPrint(x)   uartSend(1, (uint8_t *)(x), strlen(x))
#define rs232Print(x) uartSend(2, (uint8_t *)(x), strlen(x))

#define UART_BUF_SIZE	512
#define UART_RX_FIFO_SIZE 16

typedef struct _uartData
{
	LPC_USART_T * uart;
	RINGBUFF_T * rrb;
	RINGBUFF_T * trb;
}uartData_t;

void ciaaUARTInit(void);
int uartSend(ciaaUART_e nUART, void * data, int datalen);
int uartRecv(ciaaUART_e nUART, void * data, int datalen);

#endif /* CIAAUART_H_ */
