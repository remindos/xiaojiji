/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_fm_enhance.c
Creator: zj.zong					Date: 2009-05-25
Description: subroutine with no decode for mcu
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_FM_BASAL_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Api_Set_FM_Volume(uc8 volume_level);
extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_RadioSetFreq(ui16 cur_freq);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_DynamicLoad_FM_Target(uc8 load_code_idx);
extern void Api_DynamicLoad_FM_CodeMpm1_3k(uc8 load_code_idx);
extern void Ap_FM_Turner_Close(void);

void Ap_Init_FM_Driver(void);
void Ap_FM_Disable_DAC(void);
void Ap_Exit_FM_Mode(void);
/*******************************************************************************
Function: Ap_Enable_IrqBit()
Description: enable irq
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Enable_IrqBit(void)
{
 	MR_IE0 = 0x01;						//enable USB Detect irq
	MR_IE1 = 0x3f;						//enable mcu interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}



/*******************************************************************************
Function: Ap_Init_FM_Driver()
Description: select lcd driver
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Init_FM_Driver(void)
{
	switch((uc_user_option1&0xe0))
	{
		case 0x00:					//bit5,6,7=0x00:tea5767 
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_3_DRVIER_TEA5767_IDX);
			break;

		case 0x20:					//bit5,6,7=0x20:5800C 
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_4_DRVIER_5800C_IDX);
			break;

		case 0x40:				   	//bit5,6,7=0x40:AR1010 
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_5_DRVIER_AR1010_IDX);
			break;
	
		case 0x60:				   	//bit5,6,7=0x60:CL6017S 
	  		Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_6_DRVIER_CL6017S_IDX);
			break;	  

		case 0x80:				   	//bit5,6,7=0x80:SI4702 
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_7_DRVIER_SI4702_IDX);
			break;

		case 0xa0:				   	//bit5,6,7=0xa0:Rda5807SP 
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_8_DRVIER_Rda5807SP_IDX);
			break;

		case 0xc0:				   	//bit5,6,7=0xC0:BK1080
			Api_DynamicLoad_FM_CodeMpm1_3k(CODE_FM_9_DRVIER_BK1080_IDX);
			break;	
	}
}
/*******************************************************************************
Function: Ap_Change_CurBand()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Change_CurBand(void)
{
	Api_DynamicLoad_FM_Target(CODE_FM_2_SAVE_PARA_IDX);	
	Api_DynamicLoad_FM_Target(CODE_FM_1_RESTORE_PARA_IDX);	
	Api_RadioSetFreq(ui_fm_cur_freq);		//set freq
}


/*******************************************************************************
Function: Ap_SwitchTo_MainMenu()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_FM_Mode();
	Api_DynamicLoad_MainMenu();	   //enter main menu
}


/*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: enter power off mode
Calls:
Global:uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_FM_Mode();
	
	Api_DynamicLoad_PowerOff();
}

/*******************************************************************************
Function: Ap_Exit_FM_Mode()
Description: 
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exit_FM_Mode(void)
{
	if(uc_fm_volume_level != 0)
	{
		Api_Set_FM_Volume(1);
	}
//	Ap_FM_Disable_DAC();					//power off DAC
//	Api_Delay_1ms(250);
	
	Ap_FM_Turner_Close();				//set fm module standby

#ifdef PCB_WITH_SPEAKER
//	MR_PADATA &= 0xfd;								//pa1 = 0
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
#endif
	Ap_FM_Disable_DAC();					//power off DAC
	uc_fm_band_bk = uc_fm_band;
	Api_DynamicLoad_FM_Target(CODE_FM_2_SAVE_PARA_IDX);	
}

/*******************************************************************************
Function: Ap_FM_Disable_DAC()
Description: power off DAC
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_FM_Disable_DAC(void)
{
	MR_DACPCON &= 0xfb;								//close headphone driver power
	Api_Delay_1ms(100);								//delay 100ms

 	MR_DACCLK |= 0x04;								//set bit2, Vrefdac is grounded

	MR_DACPCON = 0;									//DAC all power off
	Api_Delay_1ms(100);								//delay 100ms
	MR_DACI = 0x00;									//clr DAC "go" bit, and disable IRQ

	MR_MODEN1 &= 0xfe;								//Disable DAC Mode
}


/*******************************************************************************
init DAC
********************************************************************************/
void Ap_FM_Enable_DAC(void)
{
	mReset_WatchDog;	
	MR_MODEN1 |= 0x01;					//Enable DAC Mode

	MR_ADCGAIN = 0x40;					//MIC PGA is power-on
	MR_DACSRST = 0x01;					//set bit0, soft reset DAC
	Api_Delay_1ms(1);					//The reset must be finished in 2 MCU system clocks.

	MR_DACCLK |= 0x7;					//bit1,0:set DAC Clock; bit2:set VREFDAC is grounded(clr bit2 after start play)
	Api_Delay_1ms(10);
	MR_DACI = 0x10;						//set DACI "go bit", sample rate 48KHz
	MR_DACPCON = 0xc5;					//set bit7=1, bit6=1, bit2=1, bit0=1, power control
	MR_DACSET |= 0xC0;					//mute left & right

	Api_Set_FM_Volume(0);					//set left & right master volume is "0"

	MR_DACI |= 0x80;					//reset DACI
	Api_Delay_1ms(200);					//delay 250ms for wating DAC V-refernce
	Api_Delay_1ms(200);					//delay 250ms for wating DAC V-refernce

	MR_DACSET = 0x00;					//cancel mute DAC left & right
	MR_DACI = 0x1e;						//set DAC sample rate 8KHz, set "go"
}


#endif