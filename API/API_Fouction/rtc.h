/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: rtc.h
Creator: andyliu					Date: 2009-03-30
Description: define Macro in rtc driver
Others:
Version: V0.1
History:
	V0.1	2009-03-30		andyliu
			1. build this module
********************************************************************************/
#ifndef RTC_HEADER
#define RTC_HEADER


#define	RTC_READ 		0xA3
#define	RTC_WRITE 		0xA2

#define RTC_REG_CONTROL		0x00
#define RTC_REG_STATUS		0x01
#define RTC_REG_SECOND		0x02
#define RTC_REG_MINUTE		0x03
#define RTC_REG_HOUR		0x04
#define RTC_REG_DATE		0x05
#define RTC_REG_WEEK		0x06
#define RTC_REG_MONTH		0x07
#define RTC_REG_YEAR		0x08
#define RTC_REG_A1M_MIN		0x09
#define RTC_REG_A1M_HOUR	0x0A
#define RTC_REG_A1M_DATE	0x0B

#define RTC_REG_A1M_SEC		0x07
#define RTC_REG_A2M_MIN		0x0B
#define RTC_REG_A2M_HOUR	0x0C
#define RTC_REG_A2M_DATE	0x0D

#define RTC_REG_CLOCK		0x0d
#define RTC_REG_TIMER		0x0e

//define register
#define RTC_CTRL0	0x00
#define RTC_CTRL1	0x01
#define RTC_SECOND	0x02
#define RTC_MINUTE	0x03
#define RTC_HOUR	0x04
#define RTC_DAY		0x05
#define RTC_WEEK	0x06
#define RTC_MONTH	0x07
#define RTC_YEAR	0x08
#define RTC_ALM_M	0x09
#define RTC_ALM_H	0x0a
#define RTC_ALM_D	0x0b
#define RTC_ALM_W	0x0c
#define RTC_CLOCK	0x0d
#define RTC_TIMER	0x0e

#define DEFAULT_CTRL0	0x00
#define DEFAULT_CTRL1	0x00
#define DEFAULT_SECOND	0x00
#define DEFAULT_MINUTE	0x00
#define DEFAULT_HOUR	0x00
#define DEFAULT_DAY		0x01
#define DEFAULT_WEEK	0x06	//no use
#define DEFAULT_MONTH	0x01	
#define DEFAULT_YEAR	0x12	//start 2012/1/1 0:0



#define	RTC_DATA_MASK		(1<<2)
#define	RTC_CLK_MASK		(1<<3)



#endif