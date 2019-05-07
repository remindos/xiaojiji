/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_common_func.c
Creator: zj.zong					Date: 2009-08-18
Description: music common func
Others:
Version: V0.1
History:
	V0.1	2009-04-27		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_COMMON_FUNC_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"   
#include "sh86278_sfr_reg.h"
#include "ui_resident_var_ext.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"
#include "sh86278_sfr_reg.h"

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Set_Volume(uc8 volume_level);


#ifdef AP_ENABLE_DAC_DEF
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
	
	MR_MODEN1 |= 0x01;						//Enable DAC Mode
	MR_ADCGAIN = 0x40;						//MIC PGA is power-on
	MR_DACSRST = 0x01;						//set bit0, soft reset DAC

	Api_Delay_1ms(1);						//The reset must be finished in 2 MCU system clocks.

	MR_DACCLK |= 0x7;						//bit1,0:set DAC Clock;
	Api_Delay_1ms(10);

	MR_DACI |= 0x10;						//start DAC command
	MR_DACPCON = 0xc6;						//set bit7=1, bit6=1, bit2=1, bit1=1, power control
	MR_DACSET |= 0xc0;						//mute left & right
	
	Api_Set_Volume(0);					//set left & right master volume is "0"
	
	MR_DACI |= 0x80;						//reset DACI
	Api_Delay_1ms(200);					//delay 250ms for wating DAC V-refernce
	Api_Delay_1ms(200);					//delay 250ms for wating DAC V-refernce	
}
#endif

#ifdef MUSIC_GET_SAMPLERATE_DEF
/*******************************************************************************
Function: Ap_Get_SampleRate()
Description: get mp3 sample rate according to the sample rate index
Calls:
Global:uc_daci_sr
Input: Null
Output:sample rate
Others:
********************************************************************************/
code ui16 ui_sr_table[]={44100,44100,44100,44100,48000,44100,32000,44100,24000,22050,16000,44100,12000,11025,8000,44100};
ui16 Ap_Get_SampleRate(void)
{
	return ui_sr_table[uc_daci_sr];
}

#endif

#ifdef MUSIC_GET_BITRATE_DEF
/*******************************************************************************
Function: Ap_Get_BitRate()
Description: get mp3 bitrate according to the bitrate index
Calls:
Global:uc_mp3_bitrate_idx
Input: Null
Output:bitrate value
Others:
********************************************************************************/
code ui16 ui_br_table[]={8,16,24,32,40,48,56,64,80,96,112,128,144,160,192,224,256,320};
ui16 Ap_Get_BitRate(void)
{
	return ui_br_table[uc_mp3_bitrate_idx];
}

#endif

#endif