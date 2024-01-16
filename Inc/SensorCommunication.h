#ifndef SENSORCOMMUNICATION_H_
#define SENSORCOMMUNICATION_H_

#include <stdint.h>
#include "SensorData.h"

void SensorCommunicationInit(void);
SensorData SensorCommunicationGetData(void);


#endif /* SENSORCOMMUNICATION_H_ */
