/*
 * CYANCORE LICENSE
 * Copyrights (C) 2024, Cyancore Team
 *
 * File Name		: eeprom_private.h
 * Description		: This file contains definitions used by eeprom HAL
 * Primary Author	: Shinika Balasundar [shinika.balasundar@yahoo.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once

/* EEPROM Register's Addresses */
#define EEARH (*(volatile uint8_t *)(0x42))
#define EEARL (*(volatile uint8_t *)(0x41))
#define EEDR (*(volatile uint8_t *)(0x40))
#define EECR (*(volatile uint8_t *)(0x3F))

/* EEPROM Control Register Bit Masks */
#define EERE 0x01
#define EEPE 0x02
#define EEMPE 0x04
#define EERIE 0x08
#define EEPMO 0x10
#define EEPM1 0x20

/* EEPROM Size */
#define EEPROM_SIZE 1024 /* 1 Kb = 1024 bytes */

/* Question(s)
1. How do I make the EEPROM mutable for all devices? Since if I define it here, the Atmega2560 uses different registers and has a different EEPROM size
2. In this header should there be a enum set for various standards of EEPROM's? Such as INTERNAL (which is the one being designed), EXTERNAL_I2C, EXTERNAL_SPI, where the .c would compare between the user choice and go within that function
*/
