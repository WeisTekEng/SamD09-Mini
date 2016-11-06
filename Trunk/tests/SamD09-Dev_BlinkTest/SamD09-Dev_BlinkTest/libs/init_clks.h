/*
 * init_clks.h
 *
 * Created: 6/26/2016 9:51:53 PM
 *  Author: ocybr
 */ 


#ifndef INIT_CLKS_H_
#define INIT_CLKS_H_

void clk_setup(uint8_t rtc,long EXTCLK_FREQ);
void clk_config(long EXTCLK_FREQ);
void RTC_init();




#endif /* INIT_CLKS_H_ */