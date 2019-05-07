/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_common_func.c
Creator: zj.zong					Date: 2009-04-10
Description: common function for all mode
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef	UI_COMMON_FUNC_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"

extern void Api_Display_Picture(ui16 pic_idx);

uc8 Ui_Get_BatLevel(void);
void Ui_PowerOff_Backlight(void);
void Ui_PowerOn_Backlight(void);

#ifdef UI_PROCESS_EVT_BAT_SUB
/*******************************************************************************
Function: Ui_ProcessEvt_BatSub()
Description: process current battery, get battery level
Calls:
Global:uc_adc_bat_data--ADC output data
       uc_bat_level_bk--battery level backup
       uc_bat_level   --current battery level (for display battery)
       uc_bat_low_cnt --count detected low battery(if count>=3, and power off mp3)
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_BatSub(void)
{
	if(uc_bat_level_bk != Ui_Get_BatLevel())		//get battery level
	{
		b_bat_changed_flg = 1;
		uc_bat_level_bk = Ui_Get_BatLevel();
		uc_bat_detec_valid_cnt = 0;
	}
	else
	{
		uc_bat_detec_valid_cnt++;
	}
	
	if(uc_bat_detec_valid_cnt >= BATTERY_VALID_CNT)
	{
		uc_bat_detec_valid_cnt = 0;
		uc_bat_level = uc_bat_level_bk;

		if(uc_bat_level == 0)
		{
			uc_bat_low_cnt++;					//low battery
		}
		else
		{
			uc_bat_low_cnt = 0;
		}
	}
}


uc8 Ui_Get_BatLevel(void)
{	
	if(uc_adc_bat_data > BATTERY_LEVEL_0)
	{
	/*	if(uc_adc_bat_data >= BATTERY_CHARGE_FULL)
		{
			b_bat_full_flg = 1;
		}
		else
		{
			b_bat_full_flg = 0;
		} */

		if(uc_adc_bat_data <= BATTERY_LEVEL_1)
		{
		//	MR_PADATA &= 0xF7;	//pa3 = 0
			return 1;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_2)
		{
		//	MR_PADATA &= 0xEF;	//pa4 = 0
			return 2;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_3)
		{
		//	MR_PADATA &= 0xDF;	//pa5 = 0
			return 3;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_4)
		{
		//	MR_PADATA &= 0xBF;	//pa6 = 0
			return 4;
		}
		else
		{
		//	MR_PADATA &= 0xE7;	//pa4 = 0
			return 5;
		}
	}
	else
	{
	//	MR_PADATA &= 0x00;	//pa4 = 0
		return 0;
	}
}
#endif



#ifdef UI_POWER_ON_BKLIGHT
/*******************************************************************************
void Ui_PowerOn_Backlight(void)
This function is about restore bklight grade in key and process usb event
********************************************************************************/
code ui16 ui_bklight_time_table[] = {0x0000, 0x0032, 0x0064, 0x0096, 0x00c8, 0x00fa, 0x012c};
void Ui_PowerON_Backlight(void)
{
/*
#ifdef BKLIGHT_HIGH_ON
	MR_PBDATA |= BKLIGHT_MASK;						//1--bklight on; 0--bklight off
#else
	MR_PBDATA &= (~BKLIGHT_MASK);					//1--bklight off; 0--bklight on
#endif
*/
	ui_bklight_time = ui_bklight_time_table[uc_bklight_time_level];
}
#endif

/*************************************************************
power off bkligh
*************************************************************/
#ifdef UI_POWER_OFF_BKLIGHT
void Ui_PowerOff_Backlight(void)
{
/*
#ifdef BKLIGHT_HIGH_ON
	MR_PBDATA &= (~BKLIGHT_MASK);					//1--bklight on; 0--bklight off
#else
	MR_PBDATA |= BKLIGHT_MASK;						//1--bklight off; 0--bklight on
#endif
*/
}
#endif



/*************************************************************
display battery and HOLD pic
*************************************************************/
#ifdef UI_DIS_BATTERY_PIC

	#define DIS_BATTERY_X		114
	#define DIS_BATTERY_Y		0
	#define DIS_HOLD_X			0
	#define DIS_HOLD_Y			0

extern void Api_Display_Picture(ui16 pic_idx);

void Ui_Display_Battery(void)
{
	uc_lcd_column_num = DIS_BATTERY_X;
	uc_lcd_page_num = DIS_BATTERY_Y;

	if((MR_USBCON & 0x1) != 0)						//USB supply power
	{
		if(b_bat_full_flg == 0)
		{
			uc_bat_charge_idx++;
			if(uc_bat_charge_idx > 5)
			{
				uc_bat_charge_idx = uc_bat_level;
			}
		}
		else
		{
			uc_bat_charge_idx = 5;					//battrey be charged full
		}
	}
	else
	{
		uc_bat_charge_idx = uc_bat_level;
	}
	
	Api_Display_Picture(PIC_BATTERY_COMMON_BASE_IDX + uc_bat_charge_idx);
}

#endif

#ifdef UI_CHECK_REFRESH_MENU_FLAG

void Ui_Check_Refresh_MenuFlag(void)
{
	if((++uc_refresh_select_flg) >= TIME_REFRESH_FLG)
	{
		uc_refresh_select_flg =	0;
		if(++uc_refresh_menu_num >= 4)
		{
			uc_refresh_menu_num = 0;
		}
	    uc_lcd_dis_flg |= 0x40;
	    ui_sys_event |= EVT_DISPLAY;
	}

 }
#endif





#endif