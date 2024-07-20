/*
 * CYANCORE LICENSE
 * Copyrights (C) 2019, Cyancore Team
 *
 * File Name		: adc.h
 * Description		: This file consists of ADC-HAL prototypes
 * Primary Author	: Akash Kollipara [akashkollipara@gmail.com]
 * Organisation		: Cyancore Core-Team
 */

#pragma once
#define _ADC_H_

#include <stdint.h>
#include <stdbool.h>
#include <status.h>
#include <resource.h>

typedef enum adc_trigger
{
	free_run = 0,
	analog_comp = 1,
	ext_int0 = 2,
	t0_cmp_a = 3,
	t0_ov = 4,
	t1_cmp_b = 5,
	t1_ov = 6,
	t1_cp_eve = 7,
} adc_trig_t;

typedef enum adc_ref
{
	ref_ext = 0,
	ref_avcc = 1,
	ref_1_1 = 2,
} adc_ref_t;

typedef struct adc_port
{
	hw_devid_t port_id;
	unsigned int clk_id;
	unsigned int fdiv;
	uintptr_t baddr;
	uintptr_t stride;
	size_t adc_irq;
	void (*adc_handler)(void);
} adc_port_t;

status_t adc_setup(adc_port_t *);
status_t adc_shutdown(const adc_port_t *);
bool adc_busy(const adc_port_t *);
status_t adc_int_en(const adc_port_t *);
status_t adc_int_dis(const adc_port_t *);
status_t adc_config_pin(const adc_port_t *, uint8_t, adc_trig_t, uint8_t, adc_ref_t);
status_t adc_read(const adc_port_t *, uint16_t *);
status_t adc_temperature_convert(uint16_t, float *);
