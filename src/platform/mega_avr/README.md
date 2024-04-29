# Folder Structure
This folder contains platform specific programs for megaAVR family of controllers.

Some differences between the two AVR microcontrollers:

Atmega 328p: It has 32KB of Flash memory, 1KB of SRAM, and 1KB of EEPROM. It provides 23 general-purpose I/O pins.
Atmega 2560: It has 256KB of Flash memory, 8KB of SRAM, and 4KB of EEPROM. It provides 86 general-purpose I/O pins. Has advanced PWM channels.

## Directories Overview
| Folder                | Description                                                   |
| --------------------- | ------------------------------------------------------------- |
| [common](common)      | Holds common code shared between platforms.|
| [atmega328p](atmega328p) | Houses platform-specific programs for Atmega328p. Contains: Architecture Specifics,  Platform Includes, Platform Clock, and Resources|
| [atmega2560](atmega2560) | Houses platform-specific programs for Atmega2560. Contains: Architecture Specifics,  Platform Includes, Platform Clock, and Resources|

### [common](common)

Contains code shared between Atmega328p and Atmega2560.

| Subfolder       | Description                                             |
| --------------- | ------------------------------------------------------- |
| [arch](common/arch)    | CPU architecture related src|
| [hal](common/hal)      | Hardware abstraction layer src|
| [include](common/include) | Includes for platform header|
| [platform](common/platform) | Platform specific src|
