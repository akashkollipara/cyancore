/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: spi.c
 * Description		: This file contains sources for mega-avr
 *			  HAL spi apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <status.h>
#include <stdbool.h>
#include <stdint.h>
#include <assert.h>
#include <mmio.h>
#include <platform.h>
#include <interrupt.h>
#include <hal/gpio.h>
#include <hal/spi.h>
#include "spi_private.h"

/**
 * spi_master_setup - Setups SPI master mode
 *
 * @brief Initializes SPI in master mode with specified settings
 *
 * @param[out] port: Pointer to the SPI port structure
 * @param[in] df_format: Data frame format
 * @param[in] cpol: Clock polarity
 * @param[in] cpha: Clock phase
 *
 * @return status: Status of master setup operation
 */

status_t spi_master_setup(spi_port_t *port, dataframe_format_t df_format, clk_pol_t cpol, clk_ph_t cpha)
{
	status_t ret = success;
	uint8_t spcr_value = 0;
	gpio_port_t spi_mosi, spi_miso, spi_sck, spi_ss;
	STATUS_CHECK_POINTER(port);
	platform_clk_en(port->clk_id);
	spcr_value |= (1 << SPE) | (1 << MSTR);
	switch(port->fdiv)
	{
		case 4:
			spcr_value |= (fdiv4 << SPR0);
			break;
		case 16:
			spcr_value |= (fdiv16 << SPR0);
			break;
		case 64:
			spcr_value |= (fdiv64 << SPR0);
			break;
		case 128:
			spcr_value |= (fdiv128 << SPR0);
			break;
		default:
			ret = error_func_inval_arg;
			break;
	}
	spcr_value |= df_format ? (1 << DODR) : 0;
	spcr_value |= cpol ? (1 << CPOL) : 0;
	spcr_value |= cpha ? (1 << CPHA) : 0;

	ret |= gpio_pin_alloc(&spi_mosi, port->com_port, port->com_pins[0]);
	ret |= gpio_pin_alloc(&spi_miso, port->com_port, port->com_pins[1]);
	ret |= gpio_pin_alloc(&spi_sck, port->com_port, port->com_pins[2]);
	ret |= gpio_pin_alloc(&spi_ss, port->com_port, port->com_pins[3]);

	ret |= gpio_pin_mode(&spi_mosi, out);
	ret |= gpio_pin_mode(&spi_miso, in);
	ret |= gpio_pin_mode(&spi_sck, out);
	ret |= gpio_pin_mode(&spi_ss, out);
	MMIO8(port->baddr | SPCR_OFFSET) = spcr_value;
	return ret;
}

/**
 * spi_slave_setup - Setups SPI slave mode
 *
 * @brief Initializes SPI in slave mode with specified settings
 *
 * @param[out] port: Pointer to the SPI port structure
 * @param[in] df_format: Data frame format
 * @param[in] cpol: Clock polarity
 * @param[in] cpha: Clock phase
 *
 * @return status: Status of slave setup operation
 */

status_t spi_slave_setup(spi_port_t *port, dataframe_format_t df_format, clk_pol_t cpol, clk_ph_t cpha)
{
	status_t ret = success;
	uint8_t spcr_value = 0;
	gpio_port_t spi_mosi, spi_miso, spi_sck, spi_ss;
	STATUS_CHECK_POINTER(port);
	platform_clk_en(port->clk_id);
	spcr_value |= (1 << SPE);
	switch(port->fdiv)
	{
		case 4:
			spcr_value |= (fdiv4 << SPR0);
			break;
		case 16:
			spcr_value |= (fdiv16 << SPR0);
			break;
		case 64:
			spcr_value |= (fdiv64 << SPR0);
			break;
		case 128:
			spcr_value |= (fdiv128 << SPR0);
			break;
		default:
			ret = error_func_inval_arg;
			break;
	}
	spcr_value |= df_format ? (1 << DODR) : 0;
	spcr_value |= cpol ? (1 << CPOL) : 0;
	spcr_value |= cpha ? (1 << CPHA) : 0;

	ret |= gpio_pin_alloc(&spi_mosi, port->com_port, port->com_pins[0]);
	ret |= gpio_pin_alloc(&spi_miso, port->com_port, port->com_pins[1]);
	ret |= gpio_pin_alloc(&spi_sck, port->com_port, port->com_pins[2]);
	ret |= gpio_pin_alloc(&spi_ss, port->com_port, port->com_pins[3]);

	ret |= gpio_pin_mode(&spi_mosi, in);
	ret |= gpio_pin_mode(&spi_miso, out);
	ret |= gpio_pin_mode(&spi_sck, in);
	ret |= gpio_pin_mode(&spi_ss, in);
	MMIO8(port->baddr | SPCR_OFFSET) = spcr_value;
	return ret;
}

/**
 * spi_wcol_error - Checks SPI write collision error
 *
 * @brief Checks if a write collision error occurred during SPI communication
 *
 * @param[in] port: Pointer to the SPI port structure
 *
 * @return state: State of the write collision error (true = error, false = no error)
 */

bool spi_wcol_error(spi_port_t *port)
{
	assert(port);
	return (MMIO8(port->baddr + SPSR_OFFSET) & (1 << WCOL)) ? true : false;
}

/**
 * spi_trx_done - Checks SPI transaction completion
 *
 * @brief Checks if an SPI transaction is completed
 *
 * @param[in] port: Pointer to the SPI port structure
 *
 * @return state: State of the transaction completion (true = completed, false = not completed)
 */

bool spi_trx_done(spi_port_t *port)
{
	assert(port);
	return (MMIO8(port->baddr + SPSR_OFFSET) & (1 << SPIF)) ? true : false;
}

/**
 * spi_int_en - Enables SPI interrupts
 *
 * @brief Enables SPI interrupts for the given SPI port
 *
 * @param[in] port: Pointer to the SPI port structure
 *
 * @return status: Status of interrupt enable operation
 */

status_t spi_int_en(spi_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + SPCR_OFFSET) |= (1 << SPIE);
	return success;
}

/**
 * spi_int_dis - Disables SPI interrupts
 *
 * @brief Disables SPI interrupts for the given SPI port
 *
 * @param[in] port: Pointer to the SPI port structure
 *
 * @return status: Status of interrupt disable operation
 */

status_t spi_int_dis(spi_port_t * port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + SPCR_OFFSET) &= ~(1 << SPIE);
	return success;
}

/**
 * spi_tx - Transmits data via SPI
 *
 * @brief Transmits data via SPI using the given SPI port
 *
 * @param[in] port: Pointer to the SPI port structure
 * @param[in] data: Data to be transmitted
 *
 * @return status: Status of data transmission operation
 */

status_t spi_tx(spi_port_t *port, char data)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + SPDR_OFFSET) = data;
	return success;
}

/**
 * spi_rx - Receives data via SPI
 *
 * @brief Receives data via SPI using the given SPI port
 *
 * @param[in] port: Pointer to the SPI port structure
 * @param[out] data: Pointer to store the received data
 *
 * @return status: Status of data reception operation
 */

status_t spi_rx(spi_port_t *port, char *data)
{
	STATUS_CHECK_POINTER(port);
	STATUS_CHECK_POINTER(data);
	*data = MMIO8(port->baddr + SPDR_OFFSET);
	return success;
}
