#ifndef __USART_DEFINE_H
#define __USART_DEFINE_H
#include <stdint.h>
#include "stm32l4xx_hal.h"

/* Size of Transmission buffer */
#define TXBUFFERSIZE                      200
/* Size of Reception buffer */
#define RXBUFFERSIZE                      1024

extern uint8_t USART_1_RxBuffer [RXBUFFERSIZE];
extern uint8_t USART_1_TxBuffer	[TXBUFFERSIZE];

void USART_1_Initialization (void);
void USART_1_Send (uint8_t *out, uint32_t cnt);
int  USART_1_Receive (uint8_t *in, int timeout);
void USART_1_Thread (void *argument);

extern uint8_t USART_2_RxBuffer [RXBUFFERSIZE];
extern uint8_t USART_2_TxBuffer	[TXBUFFERSIZE];

void USART_2_Initialization (void);
void USART_2_Send (uint8_t *out, uint32_t cnt);
int  USART_2_Receive (uint8_t *in, int timeout);
void USART_2_Thread (void *argument);

#endif
