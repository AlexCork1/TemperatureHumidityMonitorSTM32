#ifndef MESSAGEBUS_H_
#define MESSAGEBUS_H_

#include "stm32f429xx.h"
#include "SensorData.h"

void MessageBusInit(void);
void MessageBusSendString(char* arrayToSend);
void MessageBusSendSensorData(SensorData data);

#endif /* MESSAGEBUS_H_ */
