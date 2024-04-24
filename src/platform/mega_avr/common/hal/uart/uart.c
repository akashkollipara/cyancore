/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: uart.c
 * Description		: This file contains sources for mega-avr
 *			  HAL uart apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <status.h>
#include <syslog.h>
#include <resource.h>
#include <mmio.h>
#include <lock/spinlock.h>
#include <platform.h>
#include <interrupt.h>
#include <hal/uart.h>
#include <visor_call.h>
#include <arch.h>
#include "uart_private.h"

/**
 * uart_setup - Configure UART port settings
 *
 * @brief Configures UART port settings including direction, parity, baud rate,
 *        interrupt handling, and frame configuration.
 *
 * @param[in] port: Pointer to the UART port structure
 * @param[in] d: Direction of UART communication (trx, rx, tx)
 * @param[in] p: Parity setting for UART communication
 *
 * @return status: Status of UART setup operation
 */

status_t uart_setup(uart_port_t *port, direction_t d, parity_t p)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + UCSRA_OFFSET) = 0x00;
	platform_clk_en(port->clk_id);
	vret_t vres;
	arch_visor_call(fetch_dp, clock, 0, 0, &vres);
	if(vres.status != success)
		return vres.status;
	unsigned long *clk = (unsigned long *)vres.p;


	// Enable module based on direction
	uint8_t en = 0;
	switch(d)
	{
		case trx:
			en |= (1 << TXEN);
			if(port->tx_irq)
			{
				sysdbg5("Linking module#%u Tx IRQ#%u\n", port->tx_irq->module, port->tx_irq->id);
				link_interrupt(port->tx_irq->module, (unsigned int) port->tx_irq->id,
						port->tx_handler);
				uart_tx_int_en(port);
			}
		case rx:
			en |= (1 << RXEN);
			if(port->rx_irq)
			{
				sysdbg5("Linking module#%u Rx IRQ#%u\n", port->rx_irq->module, port->rx_irq->id);
				link_interrupt(port->rx_irq->module, (unsigned int) port->rx_irq->id,
						port->rx_handler);
				uart_rx_int_en(port);
			}
			break;
		case tx:
			en |= (1 << TXEN);
			if(port->tx_irq)
			{
				sysdbg5("Linking module#%u Tx IRQ#%u\n", port->tx_irq->module, port->tx_irq->id);
				link_interrupt(port->tx_irq->module, (unsigned int )port->tx_irq->id,
						port->tx_handler);
				uart_tx_int_en(port);
			}
			break;
		default:
			en = 0;
			ret = error_func_inval_arg;
			goto uart_setup_exit;
	}
	MMIO8(port->baddr + UCSRB_OFFSET) |= en;

	// Set baud rate
	uint16_t b = BAUD_RATE_VALUE(*clk, port->baud);
	sysdbg5("Baud Prescalar = %u\n", b);
	MMIO8(port->baddr + UBRRH_OFFSET) = (b >> 8);
	MMIO8(port->baddr + UBRRL_OFFSET) = (b & 0xff);

	// Configure frame
	// Defaults to
	// Async UART, 1 stop bit, Rising edge clk
	MMIO8(port->baddr + UCSRC_OFFSET) = (uint8_t) (p << UPM0) | (3 << UCSZ0); // Set Parity & 8 bit frame
uart_setup_exit:
	return ret;
}

/**
 * uart_shutdown - Shutdown UART port
 *
 * @brief Disables UART port and associated interrupts
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return status: Status of UART shutdown operation
 */

status_t uart_shutdown(uart_port_t *port)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	if(port->tx_irq)
	{
		ret |= uart_tx_int_dis(port);
		ret |= unlink_interrupt(port->tx_irq->module, (unsigned int) port->tx_irq->id);
	}
	if(port->rx_irq)
	{
		ret |= uart_rx_int_dis(port);
		ret |= unlink_interrupt(port->rx_irq->module, (unsigned int) port->rx_irq->id);
	}
	ret |= platform_clk_dis(port->clk_id);
	return ret;
}

/**
 * uart_buffer_available - Check if UART buffer is free for transmission
 *
 * @brief Checks if UART buffer is available for data transmission.
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return bool: True if UART buffer is available, false otherwise
 */

bool uart_buffer_available(const uart_port_t *port)
{
	assert(port);
	return (bool)((MMIO8(port->baddr + UCSRA_OFFSET) >> UDRE) & 0x01);
}

/**
 * uart_tx_wait_till_done - Wait until UART transmission is done
 *
 * @brief Waits until UART transmission is complete
 *
 * @param[in] port: Pointer to the UART port structure
 */

void uart_tx_wait_till_done(const uart_port_t *port)
{
	assert(port);
	while(!(MMIO8(port->baddr + UCSRA_OFFSET) & (1 << TXC)))
		arch_nop();
	MMIO8(port->baddr + UCSRA_OFFSET) |= (1 << TXC);
}

/**
 * uart_rx_done - Check if UART reception is complete
 *
 * @brief Checks if UART reception is complete.
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return bool: True if UART reception is complete, false otherwise
 */

bool uart_rx_done(const uart_port_t *port)
{
	assert(port);
	return (bool)((MMIO8(port->baddr + UCSRA_OFFSET) >> RXC) & 0x01);
}

/**
 * uart_frame_error - Check if UART frame error occurred
 *
 * @brief Checks if UART frame error occurred
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return bool: True if UART frame error occurred, false otherwise
 */

bool uart_frame_error(const uart_port_t *port)
{
	bool ret = false;
	assert(port);
	if(MMIO8(port->baddr + UCSRC_OFFSET) & (1 << UPM0))
		ret = (MMIO8(port->baddr + UCSRA_OFFSET) & (1 << FE)) ? true : false;
	return ret;
}

/**
 * uart_tx - Transmit data over UART
 *
 * @brief Function transmits the data over on UART
 *
 * @param[in] port: Pointer to the UART port structure
 * @param[in] data: Data to be transmitted
 *
 * @return status: Status of UART transmission operation
 */

status_t uart_tx(const uart_port_t *port, const char data)
{
	STATUS_CHECK_POINTER(port);
	while(!uart_buffer_available(port))
		arch_nop();
	MMIO8(port->baddr + UDR_OFFSET) = data;
	return success;
}

/**
 * uart_rx - Receive data over UART
 *
 * @brief Receives data over UART.
 *
 * @param[in] port: Pointer to the UART port structure
 * @param[out] data: Pointer to store received data
 *
 * @return status: Status of UART reception operation
 */

status_t uart_rx(const uart_port_t *port, char *data)
{
	STATUS_CHECK_POINTER(port);
	if(uart_frame_error(port))
		return error_driver_data;
	*data = MMIO8(port->baddr + UDR_OFFSET);
	return success;
}

/**
 * uart_tx_int_en - Enable UART transmission interrupt
 *
 * @brief Enables UART transmission interrupt
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return status: Status of UART transmission interrupt enable operation
 */

status_t uart_tx_int_en(const uart_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + UCSRB_OFFSET) |= (1 << TXCIE);
	return success;
}

/**
 * uart_tx_int_dis - Disable UART transmission interrupt
 *
 * @brief Disables UART transmission interrupt
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return status: Status of UART transmission interrupt disable operation
 */

status_t uart_tx_int_dis(const uart_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + UCSRB_OFFSET) &= ~(1 << TXCIE);
	return success;
}

/**
 * uart_rx_int_en - Enable UART reception interrupt
 *
 * @brief Enables UART reception interrupt
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return status: Status of UART reception interrupt enable operation
 */

status_t uart_rx_int_en(const uart_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + UCSRB_OFFSET) |= (1 << RXCIE);
	return success;
}

/**
 * uart_rx_int_dis - Disable UART reception interrupt
 *
 * @brief Disables UART reception interrupt
 *
 * @param[in] port: Pointer to the UART port structure
 *
 * @return status: Status of UART reception interrupt disable operation
 */

status_t uart_rx_int_dis(const uart_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + UCSRB_OFFSET) &= ~(1 << RXCIE);
	return success;
}
