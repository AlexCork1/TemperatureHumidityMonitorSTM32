#include <MessageBus.h>
#include <stdint.h>
#include <SensorCommunication.h>
#include "stm32f429xx.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void PeriodicTimerInit();
void ProcessSensorData();

volatile int8_t DHTSensorPeridoicFlag;

//we will read sensor data every 30s
void TIM7_IRQHandler(void){
	DHTSensorPeridoicFlag = 1;
	CLEAR_BIT(TIM7->SR, TIM_SR_UIF);
}

int main(void)
{
	MessageBusInit();
	SensorCommunicationInit();
	PeriodicTimerInit();

	for(;;)
	{
		if (DHTSensorPeridoicFlag != 0)
		{
			ProcessSensorData();
			DHTSensorPeridoicFlag = 0;//clear our flag
		}
	}
}

void PeriodicTimerInit(){
	//default process clock is 16MHz for timers
	SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM7EN);
	SET_BIT(TIM7->CR1, TIM_CR1_ARPE);
	//enable interrupt
	SET_BIT(TIM7->DIER, TIM_DIER_UIE);
	NVIC_EnableIRQ(TIM7_IRQn);

	//slow timer down
	WRITE_REG(TIM7->PSC, 15999); //slow it down to 1000Hz
	WRITE_REG(TIM7->ARR, 29999); //triggers on every 30s

	//start timer
	SET_BIT(TIM7->CR1, TIM_CR1_CEN);
}
void ProcessSensorData(){
	SensorData data = SensorCommunicationGetData();
	MessageBusSendSensorData(data);

	//ERROR processing is done on receving side
//	if (data.Error == 0)
//		MessageBusSendSensorData(data);
//	else
//		MessageBusSendString("Parity error detected");
}
