/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_common_func.c
Creator: zj.zong					Date: 2009-08-18
Description: voice func
Others:
Version: V0.1
History:
	V0.1	2009-08-19		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOICE_COMMON_FUNC_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
		
extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_Set_Volume(uc8 vol_level);

void Ap_Enable_DAC(void);

/*******************************************************************************
Function:   Ap_Enable_DAC()
Description: interface for initial module DAC in mode music, movie, voice
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Enable_DAC(void)
{
	mReset_WatchDog;			//reset watchdog
	
	MR_MODEN1 |= 0x01;			//Enable DAC Mode
	MR_ADCGAIN = 0x40;			//MIC PGA is power-on
	MR_DACSRST = 0x01;			//set bit0, soft reset DAC
								
	Api_Delay_1ms(1);			//The reset must be finished in 2 MCU system clocks.

	MR_DACCLK |= 0x7;			//bit1,0:set DAC Clock;
	Api_Delay_1ms(10);

	MR_DACI |= 0x10;			//start DAC command
	MR_DACPCON = 0xc6;			//set bit7=1, bit6=1, bit2=1, bit1=1, power control
	MR_DACSET |= 0xc0;			//mute left & right
	b_dac_mute_flg = 1;			//set mute flag
	
	Api_Set_Volume(0);			//set left & right master volume is "0"
	
	MR_DACI |= 0x80;			//reset DACI
	Api_Delay_1ms(200);			//delay 250ms for wating DAC V-refernce
	Api_Delay_1ms(200);			//delay 250ms for wating DAC V-refernce	
}

#endif