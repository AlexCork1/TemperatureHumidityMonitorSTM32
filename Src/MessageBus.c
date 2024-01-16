#include <MessageBus.h>
#include "stm32f429xx.h"
#include <stdio.h>
#include <string.h>

/*
 * This implementation uses UART to send data to PC,
 * but this methods could be implemented differently (to send data to cloud or somewhere else...)
 */
#define JSON_BUFFER_SIZE 50
const char* JSON_FORMAT = "{\"Temperature\":%d,\"Humidity\":%d,\"Error\":%d}\0";
char jsonBuffer[JSON_BUFFER_SIZE];

void MessageBusInit(void)
{
	//enable clocks for GPIOA and for USART1
	SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
	SET_BIT(RCC->APB2ENR, RCC_APB2ENR_USART1EN);

	//configure pins to enable PA9
	SET_BIT(GPIOA->MODER, GPIO_MODER_MODER9_1);
	SET_BIT(GPIOA->AFR[1], 7 << 4);

	//configure baud rate - 115200 (Table 136: Error calculation for programmed baud rates at in reference manual RM0090
	WRITE_REG(USART1->BRR, (8 << 4) | 11);

	//enable USART with transmitter
	SET_BIT(USART1->CR1, USART_CR1_TE | USART_CR1_UE);
}
void MessageBusSendString(char* arrayToSend)
{
	char* p = arrayToSend;
	while(*p != '\0')
	{
		WRITE_REG(USART1->DR, *p++);
		while(READ_BIT(USART1->SR, USART_SR_TXE) == 0);
	}
}
void MessageBusSendSensorData(SensorData data)
{
	memset(jsonBuffer, '\0', JSON_BUFFER_SIZE);
    snprintf(jsonBuffer, JSON_BUFFER_SIZE, JSON_FORMAT, data.Temperature, data.Humidity, data.Error);
    MessageBusSendString(jsonBuffer);
}
