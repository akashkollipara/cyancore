/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: gpio.cpp
 * Description		: This file consists of gpio on board led driver in cpp
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

extern "C"
{
	#include <status.h>
	#include <hal/gpio.h>
}

#include "gpio.h"

/* Outside declaration of the member function of class onBoardLed: Utilized by the specific register allocation for the onboard LED */
onBoardLed::onBoardLed()
{
	gpio_pin_alloc(&led13, PORTB, 5);
}

/**
 * @brief the setting up of the onboard-LED
 *
 * This function configures the specific GPIO pin for the onboard-LED as an output
 * This function also sets the LED state to low (off)
 */

void onBoardLed::setup()
{
	gpio_pin_mode(&led13, out);
	gpio_pin_clear(&led13);
}

/**
 * @brief The toggling of the onboard LED
 *
 * This function toggles the state of the onboard LED on and off
 */

void onBoardLed::toggle()
{
	gpio_pin_toggle(&led13);
}
