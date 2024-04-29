/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: adc.c
 * Description		: This file contains sources for mega-avr
 *			  HAL adc apis
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <status.h>
#include <mmio.h>
#include <platform.h>
#include <interrupt.h>
#include <hal/adc.h>
#include <arch.h>
#include <syslog.h>
#include "adc_private.h"

/**
 * _adc_enable - Enables ADC
 *
 * @brief Enables the ADC peripheral
 *
 * @param[in] port: Pointer to the ADC port structure
 */

static inline void _adc_enable(adc_port_t *port)
{
	MMIO8(port->baddr + ADCSRA_OFFSET) |= (1 << ADEN);
}

/**
 * _adc_disable - Disables ADC
 *
 * @brief Disables the ADC peripheral
 *
 * @param[in] port: Pointer to the ADC port structure
 */

static inline void _adc_disable(adc_port_t *port)
{
	MMIO8(port->baddr + ADCSRA_OFFSET) &= ~(1 << ADEN);
}

/**
 * _adc_set_prescaler - Sets ADC prescaler
 *
 * @brief Sets the ADC prescaler value based on the clock frequency specific in the port
 *
 * @param[in] port: Pointer to the ADC port structure
 *
 * @return status: Status of prescaler setup operation
 */

static inline status_t _adc_set_prescaler(adc_port_t *port)
{
	uint8_t pscale_value = 0;
	status_t ret = success;
	switch(port->fdiv)
	{
		case 2:
			pscale_value = fdiv2;
			break;
		case 4:
			pscale_value = fdiv4;
			break;
		case 8:
			pscale_value = fdiv8;
			break;
		case 16:
			pscale_value = fdiv16;
			break;
		case 32:
			pscale_value = fdiv32;
			break;
		case 64:
			pscale_value = fdiv64;
			break;
		case 128:
			pscale_value = fdiv128;
			break;
		default:
			ret = error_func_inval_arg;
	}
	MMIO8(port->baddr + ADCSRA_OFFSET) |= (pscale_value & 0x7);
	return ret;
}

/**
 * _adc_start_conv - Starts ADC conversion
 *
 * @brief Initiates ADC conversion process
 *
 * @param[in] port: Pointer to the ADC port structure
 */

static inline void _adc_start_conv(adc_port_t *port)
{
	MMIO8(port->baddr + ADCSRA_OFFSET) |= (1 << ADSC);
}

/**
 * _adc_config_trigger - Configures ADC trigger mode
 *
 * @brief Configures the ADC trigger mode for conversion
 *
 * @param[in] port: Pointer to the ADC port structure
 * @param[in] trigger: ADC trigger mode
 *
 * @return status: Status of trigger configuration operation
 */

static inline status_t _adc_config_trigger(adc_port_t *port, adc_trig_t trigger)
{
	status_t ret = success;
	uint8_t trig_value = (uint8_t) trigger;
	ret |= (trig_value > 7) ? error_func_inval_arg : success;
	MMIO8(port->baddr + ADCSRB_OFFSET) |= ((trig_value & 0x07) << ADTS);
	if(trigger > free_run)
		MMIO8(port->baddr + ADCSRA_OFFSET) |= (1 << ADATE);
	return ret;
}

/**
 * _adc_config_resolution - Configures ADC resolution
 *
 * @brief Configures the ADC resolution (8-bit | 10-bit)
 *
 * @param[in] port: Pointer to the ADC port structure
 * @param[in] resolution: ADC resolution wanted
 *
 * @return status: Status of resolution configuration operation
 */

static inline status_t _adc_config_resolution(adc_port_t *port, uint8_t resolution)
{
	status_t ret = success;
	if(resolution == 8)
		MMIO8(port->baddr + ADMUX_OFFSET) = (1 << ADLAR);
	else if(resolution != 10)
		ret = error_func_inval_arg;
	return ret;
}

/**
 * _adc_config_vref - Configures ADC voltage reference
 *
 * @brief Configures the ADC voltage reference source based on specific reference source
 *
 * @param[in] port: Pointer to the ADC port structure
 * @param[in] vref: ADC voltage reference source
 *
 * @return status: Status of voltage reference configuration operation
 */

static inline status_t _adc_config_vref(adc_port_t *port, adc_ref_t vref)
{
	status_t ret = success;
	uint8_t value = 0;
	switch(vref)
	{
		case ref_ext:
			value = 0;
			break;
		case ref_avcc:
			value = 1;
			break;
		case ref_1_1:
			value = 3;
			break;
		default:
			ret = error_func_inval_arg;
	}
	if(ret == success)
		MMIO8(port->baddr + ADMUX_OFFSET) |= (value << REFS);
	return ret;
}

/**
 * adc_setup - Setups ADC
 *
 * @brief Initializes and configures the ADC peripheral
 *
 * @param[out] port: Pointer to the ADC port structure
 *
 * @return status: Status of setup operation
 */

status_t adc_setup(adc_port_t *port)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	ret |= platform_clk_en(port->clk_id);
	_adc_enable(port);
	ret |= _adc_set_prescaler(port);
	if(port->adc_handler)
	{
		ret |= link_interrupt(int_arch, port->adc_irq, port->adc_handler);
		ret |= adc_int_en(port);
	}
	return ret;
}

/**
 * adc_shutdown - Shutdown ADC
 *
 * @brief Disables and shuts down the ADC peripheral
 *
 * @param[out] port: Pointer to the ADC port structure
 *
 * @return status: Status of shutdown operation
 */

status_t adc_shutdown(adc_port_t *port)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	ret |= adc_int_dis(port);
	_adc_disable(port);
	ret |= platform_clk_dis(port->clk_id);
	return ret;
}

/**
 * adc_busy - Checks ADC busy state
 *
 * @brief Checks if the ADC is currently busy
 *
 * @param[in] port: Pointer to the ADC port structure
 *
 * @return state: State of ADC busy status
 */

bool adc_busy(adc_port_t *port)
{
	bool ret;
	assert(port);
	ret = (MMIO8(port->baddr + ADCSRA_OFFSET) & (1 << ADSC)) ? true : false;
	return ret;
}

/**
 * adc_int_en - Enables ADC interrupts
 *
 * @brief Enables ADC interrupts for the specified ADC port
 *
 * @param[in] port: Pointer to the ADC port structure
 *
 * @return status: Status of interrupt enable operation
 */

status_t adc_int_en(adc_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + ADCSRA_OFFSET) |= (1 << ADIE);
	return success;
}

/**
 * adc_int_dis - Disables ADC interrupts
 *
 * @brief Disables ADC interrupts for the specified ADC port
 *
 * @param[in] port: Pointer to the ADC port structure
 *
 * @return status: Status of interrupt disable operation
 */

status_t adc_int_dis(adc_port_t *port)
{
	STATUS_CHECK_POINTER(port);
	MMIO8(port->baddr + ADCSRA_OFFSET) &= ~(1 << ADIE);
	return success;
}

/**
 * adc_config_pin - Configures ADC pin parameters
 *
 * @brief Configures ADC pin settings for conversion
 *
 * @param[in] port: Pointer to the ADC port structure
 * @param[in] pin: ADC pin to configure
 * @param[in] trigger: ADC trigger mode
 * @param[in] resolution: ADC resolution (8 or 10 bits)
 * @param[in] vref: ADC voltage reference source
 *
 * @return status: Status of pin configuration operation
 */

status_t adc_config_pin(adc_port_t *port, uint8_t pin, adc_trig_t trigger, uint8_t resolution, adc_ref_t vref)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	if(pin > N_ADC_PINS)
		return error_func_inval_arg;
	ret |= _adc_config_vref(port, vref);
	ret |= _adc_config_resolution(port, resolution);
	ret |= _adc_config_trigger(port, trigger);
	MMIO8(port->baddr + ADMUX_OFFSET) |= (pin << MUX);
	_adc_start_conv(port);
	return ret;
}

/**
 * adc_read - Reads ADC conversion result
 *
 * @brief Reads the ADC conversion result from the specified ADC port
 *
 * @param[in] port: Pointer to the ADC port structure
 * @param[out] adc_val: Pointer to store the ADC conversion value
 *
 * @return status: Status of read operation
 */

status_t adc_read(adc_port_t *port, uint16_t *adc_val)
{
	status_t ret = success;
	STATUS_CHECK_POINTER(port);
	STATUS_CHECK_POINTER(adc_val);
	if(MMIO8(port->baddr + ADMUX_OFFSET) & (1 << ADLAR))
	{
		*adc_val = MMIO8(port->baddr + ADCH_OFFSET);
	}
	else
	{
		*adc_val = MMIO8(port->baddr + ADCL_OFFSET);
		uint16_t temp = MMIO8(port->baddr + ADCH_OFFSET);
		temp <<= 8;
		*adc_val |= temp;
	}
	MMIO8(port->baddr + ADMUX_OFFSET) &= ~((3 << REFS) | (1 << ADLAR));
	return ret;
}

/**
 * adc_temperature_convert - Converts ADC raw value to temperature
 *
 * @brief Converts the raw ADC value to temperature in Celsius
 *
 * @param[in] raw_adc: Raw ADC value
 * @param[out] temperature: Pointer to store the temperature value
 *
 * @return status: Status of temperature conversion operation
 */

status_t adc_temperature_convert(uint16_t raw_adc, float *temperature)
{
	status_t ret = success;
	unsigned long scaled_val = raw_adc * 1000;
	*temperature = ((float)(scaled_val - CTO) / CTK);
	return ret;
}
