#pragma once
#define _MEGA_AVR_PLATFORM_H_

#ifdef _MACHINE_CALL_H_
mret_t platform_fetch_dp(unsigned int);
mret_t platform_set_sleep_mode(const uint8_t);
#endif

void platform_dp_setup();
status_t platform_clk_reset();
status_t platform_clk_en(unsigned int);
status_t platform_clk_dis(unsigned int);
