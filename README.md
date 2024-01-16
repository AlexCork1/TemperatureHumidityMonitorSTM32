# STM32 One-Wire Communication with DHT22 (AM2302) Sensor
## Overview
This project, implemented in the C programming language using STM32CubeIDE and STM32F4 registers, focuses on establishing "one-wire" communication with the DHT22 (AM2302) temperature and humidity sensor. The collected data is then transmitted over UART to a receiver device, such as a PC or other compatible devices, formatted in JSON.

## Project Components
1. Platform and Language<br />
 - Platform: STM32CubeIDE<br />
 - Language: C<br />
2. Sensor<br />
 - Sensor Type: DHT22 (AM2302)<br />
 - Communication Protocol: One-Wire<br />
 - Pin: PE5<br />
3. Communication to next device<br />
 - Communication Interface: UART<br />
 - Data Format: JSON<br />

## Implementation Details
STM32F4 Registers:
The project utilizes STM32F4 registers for low-level hardware interaction, providing a platform-specific implementation for the STM32 architecture.

## Hardware
 - Project was tested on STM32F429-DISC1 discovery board. It shouldn't be to difficult to convert it to another STM32F4 device - make sure timers (TIM2, TIM6 and TIM7) are avilable and also pin PE5.
 - Adjust UART communication settings as needed, such as baud rate and data format, to match the requirements of the receiving device.

## Execution
 - Execute the project on the STM32 device, initiating one-wire communication with the DHT22 sensor and transmitting data over UART.
 - Monitor Data: Use a compatible terminal (like YAT (Yet Another Terminal) or application ([for windows](https://github.com/AlexCork1/TemperatureHumidityMonitorForm)) on the receiving device to monitor the incoming JSON-formatted temperature and humidity data.

## Additional information
Additional documentation and resources related to STM32F4 and DHT22 integration can be found in the respective datasheets and reference materials.

Feel free to explore and enhance the project to meet specific requirements or integrate additional features.
