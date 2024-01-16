#include <SensorCommunication.h>
#include "stm32f429xx.h"

//sensor is connected to pin PE5
#define PORT GPIOE
#define PIN_NUMBER 5

/*
 * ####################################################################
 * declarations of "private" functions
 * ####################################################################
 */
static inline void EnablePortClock(uint32_t port);
static inline void SetSensorPinToOutput();
static inline void SetSensorPinToInput();
static inline void SetSensorPinToHigh();
static inline void SetSensorPinToLow();

static inline void TimerDelay1MS();
static inline void TimerCounterInit();
static inline void StartCounting();
static inline uint32_t StopCounting();


/*
 * ####################################################################
 * implementation of "public" functions
 * ####################################################################
 */
void SensorCommunicationInit(void)
{
	EnablePortClock((uint32_t)PORT);
	TimerCounterInit();
	SetSensorPinToOutput();
	SetSensorPinToHigh();

}
SensorData SensorCommunicationGetData(void)
{
	/* this implementation can cause non responsive behaviour if there is sensor is not connected */

	uint64_t rawData = 0;

	//start signal - handshake
	SetSensorPinToOutput();
	SetSensorPinToLow();

	TimerDelay1MS(); //we have line for 1.5 ms low and then we pull it back up

	SetSensorPinToHigh();
	SetSensorPinToInput();

	//this is a blocking hazard!!!
	while(READ_BIT(PORT->IDR, 1 << PIN_NUMBER) != 0);//response to high time
	while(READ_BIT(PORT->IDR, 1 << PIN_NUMBER) == 0);//response to low time
	while(READ_BIT(PORT->IDR, 1 << PIN_NUMBER) != 0);//response to high time
	//TimerDelayMicroSeconds(200);

	//here the sensor starts sending actual data
	for(int32_t i = 39; i >= 0; i--)
	{
		while(READ_BIT(PORT->IDR,1 << PIN_NUMBER) == 0);//response to high time
		StartCounting();
		while(READ_BIT(PORT->IDR,1 << PIN_NUMBER) != 0);//response to high time

		uint32_t elapsedTime = StopCounting();
		if (elapsedTime >= 50)
			rawData |= 1;

		rawData <<= 1;

	}
	rawData >>= 1; //we make one shift to far in loop

	while(READ_BIT(PORT->IDR, 1 << PIN_NUMBER) == 0);//response to high time
	SetSensorPinToOutput();
	SetSensorPinToHigh();

	/*############################################################################*/
	SensorData sensordata;

	uint16_t humTop = (rawData >> 32) & 0xFF;
	uint16_t humBot = (rawData >> 24) & 0xFF;
	uint16_t humidity = (humTop << 8) | humBot;

	uint16_t temTop = (rawData >> 16) & 0xFF;
	uint16_t temBot = (rawData >> 8) & 0xFF;
	uint16_t temperature = (temTop << 8) | temBot;

	uint16_t parity = rawData & 0xFF;

	uint8_t sum = (humTop+humBot+temBot+temTop) & 255;

	if(sum == parity){
		sensordata.Temperature = temperature;
		sensordata.Humidity = humidity;
		sensordata.Error = 0;
	}
	else
		sensordata.Error = 1;


	return sensordata;
}


/*
 * ####################################################################
 * implementation of "private" functions
 * ####################################################################
 */

static void EnablePortClock(uint32_t port){
	switch(port){
		case GPIOA_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); break;
		case GPIOB_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN); break;
		case GPIOC_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); break;
		case GPIOD_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); break;
		case GPIOE_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOEEN); break;
		case GPIOF_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOFEN); break;
		case GPIOG_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOGEN); break;
		case GPIOH_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOHEN); break;
		case GPIOI_BASE: SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOIEN); break;
		case TIM2_BASE:  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN);  break;
		case TIM6_BASE:  SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM6EN);  break;
	}
}
static inline void SetSensorPinToOutput(){
	//set pin to output
	CLEAR_BIT(PORT->MODER, ~(3 << (PIN_NUMBER << 1)));
	SET_BIT(PORT->MODER, 1 << (PIN_NUMBER << 1));
}
static inline void SetSensorPinToInput()
{
	//set pin to input
	CLEAR_BIT(PORT->MODER, (3 << (PIN_NUMBER << 1)));
}
static inline void SetSensorPinToHigh()
{
	//set pin to high
	SET_BIT(PORT->BSRRL, 1 << (PIN_NUMBER));
}
static inline void SetSensorPinToLow()
{
	//set pin to low
	SET_BIT(PORT->BSRRH, 1 << (PIN_NUMBER));
}

static inline void TimerDelay1MS()
{
	EnablePortClock(TIM2_BASE);
	WRITE_REG(TIM2->PSC, 16 - 1); 				//set prescaler to 16 (F_CPU = 16 000 000 / 16 = 1MHz => 1 = 1us)

	CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN); 			//stop timer

	SET_BIT(TIM2->CR1, TIM_CR1_ARPE);
	WRITE_REG(TIM2->CNT, 0);					//reset timer
	WRITE_REG(TIM2->ARR, 0xFFFFFFFF);			//set upper limit for timer

	SET_BIT(TIM2->EGR, TIM_EGR_UG);				//update values in register
	CLEAR_BIT(TIM2->SR, TIM_SR_UIF);			//clear update flag

	SET_BIT(TIM2->CR1, TIM_CR1_CEN);			//start timer

	while(TIM2->CNT < 1000);
	CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN); 			//stop timer

}
static inline void TimerCounterInit()
{
	EnablePortClock(TIM6_BASE);
	CLEAR_BIT(TIM6->CR1, TIM_CR1_CEN);			//stop timer
	SET_BIT(TIM6->CR1, TIM_CR1_ARPE);
	WRITE_REG(TIM6->PSC, 16 - 1); 				//set prescaler to 16 (F_CPU = 16 000 000)
	WRITE_REG(TIM6->ARR, 65535);
	SET_BIT(TIM6->EGR, TIM_EGR_UG);
	CLEAR_BIT(TIM6->SR, TIM_SR_UIF);;
}
static inline void StartCounting()
{
	SET_BIT(TIM6->CR1, TIM_CR1_CEN); 			//start timer
}
static inline uint32_t StopCounting()
{
	uint32_t vr1 = READ_REG(TIM6->CNT); 		//get current value
	CLEAR_BIT(TIM6->CR1, TIM_CR1_CEN);			//stop timer
	WRITE_REG(TIM6->CNT, 0);					//reset timer
	return vr1;
}
