#ifndef SENSORDATA_H_
#define SENSORDATA_H_

#include <stdint.h>
/*
 * this need to be changed if different sensor is used
 */
typedef struct {
	int32_t Temperature;
	int32_t Humidity;
	uint8_t Error; // 0 - no error, 1 - parity error
} SensorData;


#endif /* SENSORDATA_H_ */
