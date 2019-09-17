#pragma once
#include <stdint.h>

typedef enum adc_prescale_clk
{
	nill, div2, div4, div8, div16, div32, div64, div128
} adc_prescale_clk;

typedef enum adc_res
{
	bit8, bit10
} adc_res;

typedef enum adc_refv
{
	i1_1, ivref, aref
} adc_refv;

void adc_setup(adc_prescale_clk);
uint16_t adc_read(uint8_t, adc_refv, adc_res);
