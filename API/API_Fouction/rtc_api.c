/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: rtc_api.c
Creator: andyliu					Date: 2009-03-30
Description:
Others:
Version: V0.1
History:
	V0.1	2009-03-30		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef RTC_I2C_API_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "hwdriver_resident.h"
#include "rtc.h"

extern uc8 xdata	uc_rtc_second;
extern uc8 xdata	uc_rtc_minute;
extern uc8 xdata	uc_rtc_hour;
extern uc8 xdata	uc_rtc_day;
extern uc8 xdata	uc_rtc_week;
extern uc8 xdata	uc_rtc_month;
extern uc8 xdata	uc_rtc_year;

extern bit _rtc_i2c_init(void);
extern bit _rtc_i2c_first_init(void);
extern uc8 rtc_i2c_read(uc8 reg);
extern void	_rtc_i2c_write(uc8 addr, uc8 bcd_data);
extern void _delay_1ms(uc8 timer_cnt);
extern uc8 read_time(uc8 reg);

//uc8 hex_to_bcd(uc8 hex_data);
uc8 HexToBcd(uc8 hex_data);
void set_rtc_second(uc8 rtc_second);
void set_rtc_minute(uc8 rtc_minute);
void set_rtc_hour(uc8 rtc_hour);
void set_rtc_date(uc8 rtc_date);
void set_rtc_month(uc8 rtc_month);
void set_rtc_year(ui16 rtc_year);

#ifdef RTC_I2C_INIT_DEF


/*******************************************************************************
Function: rtc_init()
Description: init rtc hardware
Calls:
Global:Null
Input: Null
Output:
Others:
********************************************************************************/
void rtc_init(uc8 init_flg)
{
	_rtc_i2c_write(RTC_CTRL0,RTC_CTRL0);	//CONTROL REGISTER 0
	
	_rtc_i2c_write(RTC_CTRL1,RTC_CTRL1);
	
	_rtc_i2c_write(RTC_ALM_M,0x80);	//diable alarm
	
	_rtc_i2c_write(RTC_ALM_H,0x80);
	
	_rtc_i2c_write(RTC_ALM_D,0x80);
	
	_rtc_i2c_write(RTC_ALM_W,0x80);
	
	_rtc_i2c_write(RTC_CLOCK,0x80);	//clock output disable
	
	_rtc_i2c_write(RTC_TIMER,0x80);	//timer disable
	
	//set default valume
	if(init_flg != 0)
	{
	_rtc_i2c_write(RTC_MINUTE,DEFAULT_MINUTE);
	
	_rtc_i2c_write(RTC_SECOND,DEFAULT_SECOND);
	
	_rtc_i2c_write(RTC_HOUR,DEFAULT_HOUR);

	_rtc_i2c_write(RTC_DAY,DEFAULT_DAY);
	
	_rtc_i2c_write(RTC_WEEK,DEFAULT_WEEK);
	
	_rtc_i2c_write(RTC_MONTH,DEFAULT_MONTH);
	
	_rtc_i2c_write(RTC_YEAR,DEFAULT_YEAR);
	}
}
#endif

#ifdef RTC_I2C_API_READ_DEF
void rtc_read(void)
{
//	uc_rtc_second =rtc_i2c_read(RTC_SECOND);
//	uc_rtc_second &= 0x7f;

	uc_rtc_minute = rtc_i2c_read(RTC_MINUTE);
	uc_rtc_minute &= 0x7f;
	
	uc_rtc_hour = rtc_i2c_read(RTC_HOUR);
	uc_rtc_hour &= 0x3f;	
}

void rtc_read_all(void)
{
//	uc_rtc_second =rtc_i2c_read(RTC_SECOND);
//	uc_rtc_second &= 0x7f;

	uc_rtc_minute = rtc_i2c_read(RTC_MINUTE);
	uc_rtc_minute &= 0x7f;
	
	uc_rtc_hour = rtc_i2c_read(RTC_HOUR);
	uc_rtc_hour &= 0x3f;	

	uc_rtc_day = rtc_i2c_read(RTC_DAY);
	uc_rtc_day &= 0x3f;

	uc_rtc_month = rtc_i2c_read(RTC_MONTH);
	uc_rtc_month &= 0x1f;

	uc_rtc_year = rtc_i2c_read(RTC_YEAR);
	//uc_rtc_year &=0x7f;
}

void rtc_read_time(void)
{
	read_time(3);	//start from minute
	uc_rtc_minute &= 0x7f;
	uc_rtc_hour &= 0x3f;

}
#endif



#ifdef RTC_I2C_API_SET_DEF
#define RTC_I2C_HEX_TO_BCD_DEF
void set_rtc_day_time(void)
{
	uc8 tmp;
	
	tmp=HexToBcd(uc_rtc_year);
	_rtc_i2c_write(RTC_YEAR,tmp);
	
	tmp=HexToBcd(uc_rtc_month);
	_rtc_i2c_write(RTC_MONTH,tmp);
	
	tmp=HexToBcd(uc_rtc_day);
	_rtc_i2c_write(RTC_DAY,tmp);
	
	tmp=HexToBcd(uc_rtc_hour);
	_rtc_i2c_write(RTC_HOUR,tmp);
	
	tmp=HexToBcd(uc_rtc_minute);
	_rtc_i2c_write(RTC_MINUTE,tmp);	
}



#endif


#ifdef RTC_I2C_HEX_TO_BCD_DEF
uc8 HexToBcd(uc8 hex_data)
{
	uc8 bcd_data;

	bcd_data = (hex_data / 10) << 4;
	bcd_data |= hex_data % 10;
	return bcd_data;
}
#endif
#endif