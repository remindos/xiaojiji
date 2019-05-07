/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_poweron.c
Creator: zj.zong					Date: 2009-04-22
Description:
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_POWERON_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"

extern void FS_WrFlash_Interface(void);
extern void Api_Delay_1ms(uc8 time_cut);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_Sfc_InitInterface_para(void);
extern void _media_readsector(void);

uc8 get_battery_level(void);
void Ap_Init_Common_Variable(void);
void Ap_Get_Sys_Parameter(void);
void Ap_Init_Sys_ParameterVar(void);
void Ap_Get_ReadOnlyArea_SysPara(void);
void Ap_Detect_VoltageLevel(void);

code ui16 ui_sleep_time_table[]={0x0000,0x1770,0x2ee0,0x4650,0x5dc0,0x7530,0x8ca0,0xa410,0xbb80,0xd2f0,0xea60,0xffff,0xffff};
code ui16 ui_idle_time_table[] = {0x0000, 0x0064, 0x00c8, 0x012c, 0x0190, 0x01f4, 0x0258};
extern code ui16 ui_bklight_time_table[];
/*******************************************************************************
Function: Ap_Init_Common_Variable()
Description: initial common variable used in all mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Init_Common_Variable(void)
{
	ui_sys_event = 0;
	uc_key_cnt = 0;
	uc_key_release_cnt = 0;
	uc_timer_cnt = 0;
	b_record_key_flag = 0;
	b_bat_changed_flg = 0;

	b_bat_full_flg = 0;
	uc_bat_detec_valid_cnt = 0;
	uc_bat_level_bk = 5;
	uc_bat_low_cnt = 0;
	uc_bat_detect_time = 0;
	ui_idle_poweroff_cnt = 0;
	Api_Sfc_InitInterface_para();
	
	uc_msi_status = 0xff;
	b_sd_flash_flg = 0;	
}


/*******************************************************************************
Function: Ap_Get_Sys_Parameter()
Description: get system parameter (one sector)
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_Sys_Parameter(void)
{
	Api_Read_SysPara_ToDbuf0();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
	if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	{	
		Ap_Init_Sys_ParameterVar();	
	}
	else
	{
		Ap_Get_ReadOnlyArea_SysPara();
	}
}


/*******************************************************************************
Function: Ap_Init_Sys_ParameterVar()
Description: get system parameter from dbuf0 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Init_Sys_ParameterVar(void)
{
	uc8 i;

	uc_user_option1 = uc_dbuf0[335];
	uc_user_option2 = uc_dbuf0[336];
	uc_user_option3 = uc_dbuf0[337];
	uc_save_mode_idx = uc_dbuf0[339];			//0:mainmenu, 1:muisc, 2:rec; 3:voice, 4:fm, 5:ebook, 6:sys,7:telbook

	uc_idle_poweroff_level = uc_dbuf0[345];
	ui_idle_poweroff_time = ui_idle_time_table[uc_idle_poweroff_level];

	uc_sleep_poweroff_level = 0;//uc_dbuf0[346];
	ui_sleep_poweroff_time = 0;//ui_sleep_time_table[uc_sleep_poweroff_level];

	uc_language_idx = uc_dbuf0[347];

	uc_poweron_pic_num = uc_dbuf0[350];
	uc_poweroff_pic_num = uc_dbuf0[351];
	
	uc_lcd_column_offset = uc_dbuf0[356];
			
	uc_lcd_width = uc_dbuf0[359];
	uc_lcd_height = uc_dbuf0[361];

	for(i = 0; i < 6; i++)
	{
		uc_key_val_table[i] = uc_dbuf0[390 + i];
	}

	for(i = 0; i < 7; i++)
	{
		uc_key_voltage_table[i] = uc_dbuf0[400 + i];
	}


	if((uc_dbuf0[508]&0x01) != 0)
	{
		uc_disktype_flg = 0;				//普通盤
	}
	else if((uc_dbuf0[508]&0x02) != 0)
	{
		if((uc_dbuf0[509]&0xf0) == 0x30)
		{
			uc_disktype_flg = 0x00;			//加密盤
		}
		else
		{
			uc_disktype_flg = 0x00;			//可移動，本地盤，只讀盤
		}
	}
	else
	{
		uc_disktype_flg = 0;
	}	
}


/*******************************************************************************
Function: Ap_Get_ReadOnlyArea_SysPara()
Description: if get area of read-write unsuccessful, get system parameter from read only area 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Get_ReadOnlyArea_SysPara(void)
{
/*** read only area data ***/
	sfc_logblkaddr0 = uc_reserve_addr0 + 2;						//read-write area system parameter	
	sfc_logblkaddr1 = uc_reserve_addr1;	
	sfc_logblkaddr2 = uc_reserve_addr2;	
	sfc_logblkaddr3 = uc_reserve_addr3;	
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;
	buf_offset0 = 0;							//buffer offset :0x0000
	buf_offset1 = 0;
	_media_readsector();						//read one sector
	
	Ap_Init_Sys_ParameterVar();					//read all system parameter
				
	sfc_logblkaddr0 += 1;						//system parameter is 1K
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf0
	buf_offset0 = 0x00;							//buffer offset :0x0200
	buf_offset1 = 0x01;
	_media_readsector();						//read one sector
	
/*** write backup data to read-write area ***/
	sfc_logblkaddr0 = 0x02;						//read-write area system parameter	
	sfc_logblkaddr1 = 0x02;	
	sfc_logblkaddr2 = 0x00;	
	sfc_logblkaddr3 = 0x00;	
	sfc_occ_buf_flag = SFC_APP_DBUF0;
	buf_offset0 = 0;							//buffer offset :0x0000
	buf_offset1 = 0;
	FS_WrFlash_Interface();							//write one sector

	sfc_logblkaddr0 = 0x03;
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf0
	buf_offset0 = 0x00;							//buffer offset :0x0200
	buf_offset1 = 0x01;
	b_sfc_immediate_wr_flg = 1;					//do last write to flash
	FS_WrFlash_Interface();							//write one sector	
	sfc_reserved_area_flag = 0;					//clr reserved area flag

	
}


/********************************************************************************
10 times voltage detection
********************************************************************************/
void Ap_Detect_VoltageLevel(void)
{
	uc8 i,uc_bat_cnt,uc_bat_level_temp;

	uc_bat_cnt = 0;

	for(i = 0; i < 50; i++)						//maxium is 50 times
	{
		MR_MODEN1 |= 0x04;						//set bit2, enable ADC
		MR_ADCCON = 0x81;						//enable ADC interrupt,select Vbat-in
		MR_IE1 |= 0x02;							//enable ADC irq
		MR_IE0 |= 0x80;							

		while((ui_sys_event & EVT_BATTERY) == 0)//wating for the interrupt generated
		{
		}
		ui_sys_event &= ~EVT_BATTERY;

		if(uc_bat_level_temp != get_battery_level())
		{
			uc_bat_level_temp = get_battery_level();
			uc_bat_cnt = 0;
		}
		else
		{
			uc_bat_cnt++;
		}

		if(uc_bat_cnt >= BATTERY_VALID_CNT)
		{
			break;
		}

		MR_MODEN1 &= 0xFB;						//clr bit2, disable ADC
		MR_ADCCON = 0x00;						//disable ADC interrupt
		MR_IE1 &= 0xfd;							//disable ADC irq

		Api_Delay_1ms(20);
	}

	MR_MODEN1 |= 0x04;							//set bit2, enable ADC
	MR_ADCCON = 0x80;							//enable ADC interrupt
	MR_IE1 &= 0xfd;								//disable ADC irq

	uc_bat_level = uc_bat_level_temp;
}


uc8 get_battery_level(void)
{	
	if(uc_adc_bat_data > BATTERY_LEVEL_0)
	{
		if(uc_adc_bat_data <= BATTERY_LEVEL_1)
		{
			return 1;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_2)
		{
			return 2;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_3)
		{
			return 3;
		}
		else if(uc_adc_bat_data <= BATTERY_LEVEL_4)
		{
			return 4;
		}
		else
		{
			return 5;
		}
	}
	else
	{
		return 0;
	}
}


#endif