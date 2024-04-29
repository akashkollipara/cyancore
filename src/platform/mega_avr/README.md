# MegaAVR Family
This folder contains platform specific programs for MegaAVR family of controllers.

## Some differences between the two AVR microcontrollers:

| Controller | CPU | Memory (SRAM) | Flash | EEPROM | GPIO |
|---|---|---|---|---|---|
| AtMega328P | AVR5-8b | 2KB | 32KB | 1KB | 23 |
| AtMega2560 | AVR6-8b | 8KB | 256KB | 4KB | 86 |

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
