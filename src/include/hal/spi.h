/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: spi.h
 * Description		: This file consists of SPI-HAL prototypes
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once
#define _SPI_H_

#include <stdint.h>
#include <stdbool.h>
#include <status.h>
#include <resource.h>

typedef enum dataframe_format
{
	lsb_first = 0,
	msb_first = 1
} dataframe_format_t;

typedef enum clk_pol
{
	cpol_rising = 0,
	cpol_falling = 1
} clk_pol_t;

typedef enum clk_ph
{
	cpha_sample = 0,
	cpha_setup = 1
} clk_ph_t;

typedef struct spi_port
{
	uint8_t port_id;
	unsigned int clk_id;
	unsigned int fdiv;
	uintptr_t baddr;
	uintptr_t stride;
	uintptr_t com_port;
	uint8_t com_pins[4];	// MOIS, MISO, SCK, SS
	size_t trx_irq;
	void (*trx_handler)(void);
} spi_port_t;

status_t spi_master_setup(spi_port_t *, dataframe_format_t, clk_pol_t, clk_ph_t);
status_t spi_slave_setup(spi_port_t *, dataframe_format_t, clk_pol_t, clk_ph_t);
bool spi_trx_done(const spi_port_t *);
status_t spi_int_en(spi_port_t *);
status_t spi_int_dis(spi_port_t *);
status_t spi_tx(const spi_port_t *, char);
status_t spi_rx(const spi_port_t *, char *);
