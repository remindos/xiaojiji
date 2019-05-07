/******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_bootblock_pm1_start.c
Creator: zj.zong					Date: 2009-04-10
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
		 1. build this module
*******************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_BOOTBLOCK_DEFINE
#include "sh86278_api_define.h"
#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "./UI/UI_BootBlock/ui_bootblock_var.h"
#include "hwdriver_resident.h"
extern void Ui_Display_BootCode_Picture(void);
extern void Ui_PowerON_Backlight(void);
extern void Ui_PowerOff_Backlight(void);
extern void Api_Delay_1ms(uc8 time_cut);

extern bit Ap_Detect_USBConnected(void);
extern bit Ap_Check_InitFlash(void);
extern bit Ap_Check_FlashPara(void);
extern void Ap_Get_DiskCapacity(void);
extern void Ap_Check_PowerOn_Mp3(void);
extern void Ap_Init_Timer(void);
extern void Ap_Backup_BootBlock_Mirror(void);
extern void Ap_Clear_XdataMemory(void);
extern void Ap_Enter_ModeUSB(void);
extern void Ap_Enter_ModePoweron(void);
extern void Ap_PowerOff_MP3(void);
extern void Ap_InitLcm_Var(void);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);

extern void Ap_Init_FM(void);
extern void Api_Init_Lcd(void);
extern code ui16 ui_bklight_time_table[];
extern void _media_readsector(void);
extern void FS_WrFlash_Interface(void);
					  
void Ui_Init_Gpio(void);
void Ui_Init_LcmVar(void);
void Ui_Display_Sandglass(void);
void Ui_Init_Sys_Para(void);
void UI_Get_Sys_Parameter(void);
void Ap_Get_ReadOnlyArea_SysPara(void);
extern bit b_poweroff_flg;
/*******************************************************************************
Function: Ui_BootCode_Main()
Description: load code to mpm
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_BootCode_Main(void)
{
 	mReset_PC_Stack;			//reset PC STACK
	mReset_WatchDog;						//clear watchdog
	mSet_MCUClock_12M;
	Ui_Init_Gpio();							//initial gpio for system functions
	Ap_Check_FlashPara();					//check ID 	

	Ap_Check_PowerOn_Mp3();					//check type of power on mp3
	//Ap_Init_FM();
	
	Ap_InitLcm_Var();
	Api_Init_Lcd();				  		 	//initial LCD, *only functions used for lcd in hardware driver like this can be modified by user*

/*	if(b_poweroff_flg == 1)
	{
		Ap_PowerOff_MP3();	
	} */
	Ap_Backup_BootBlock_Mirror();			//backup bootblock code for more safe

//	Ui_Display_Sandglass();					//display sandglass in power on

	if(Ap_Check_InitFlash() == 0)			//initial mapped table for physical to logic, 1--ok, 0--fail
	{
		Ap_PowerOff_MP3();	
	}

	UI_Get_Sys_Parameter();
	Ap_Get_DiskCapacity();					//get disk capacity after partitioned

//	Api_LcdDriver_Set_BklightLevel(uc_bklight_grade);

	Ap_Clear_XdataMemory();				    //clear variable area for user

	Ap_Init_Timer();						//initial timer

	if(Ap_Detect_USBConnected() == 1)		//if USB port is connected, enter USB mode, or else enter power on
	{
		Ap_Enter_ModeUSB();	
	}
	else
	{
		Ap_Enter_ModePoweron();
	}
}



/*******************************************************************************
Function: Ui_Init_Gpio()
Description: initial gpio
Calls:	  PATYPE set 0 == input. PATYPE set 1 == Output.
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Init_Gpio(void)
{
	MR_PATYPE = 0x7A;
	MR_PADATA = 0x78;				//pa2 = 1 amp off
	MR_PAIEN = 0x00;
	MR_PAIRQ = 0x00;
	MR_PAPEN = 0x07;
	MR_PAPULL = 0x06; 					//pull-high is used in FM

	MR_IOCON &= 0xC7;				//set IO control
	MR_PBTYPE = 0x00;				//setting PB1~3 input for SD card after set REG'MR_IOCON'
	MR_PBDATA = 0x00; 				
	MR_PBPEN = 0x01;
	MR_PBPULL = 0x00;

	MR_PBTYPE &= 0x0F5;
	//ANL		MR_PBTYPE,#0FAH		;PB1 PB3 INPUT
	//MR_PBDATA |= BKLIGHT_MASK;
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);

	Ui_PowerOff_Backlight();
										
	MR_PCTYPE = 0x00;				//PC4 is MIC Power		
	MR_PCDATA &= 0xef;				//MIC power down 
	MR_PCDATA |= 0x00;				//LCD CS = 1
}


void Ui_Display_Sandglass(void)
{		
	Api_LcdDriver_Clr_AllPage();
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 57;

	Ui_Display_BootCode_Picture();	
	Ui_PowerON_Backlight();	
}

void UI_Get_Sys_Parameter(void)
{
	Api_Read_SysPara_ToDbuf0();
	sfc_reserved_area_flag = 0;					//read flash reserved area

	Api_Read_SysPara_ToDbuf0();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
	if((uc_dbuf0[0] != 'S')||(uc_dbuf0[1] !='I')||(uc_dbuf0[2] != 'N')||(uc_dbuf0[3] != 'O')
		||(uc_dbuf0[89] != '5')||(uc_dbuf0[243] != '5')||(uc_dbuf0[309] !='A')||(uc_dbuf0[439] != 'A'))
	{
		Ap_Get_ReadOnlyArea_SysPara();
	}
	 Ui_Init_Sys_Para();
}


/*******************************************************************************
Function: Ui_Init_Sys_Para()
Description: get system parameter from dbuf0 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Init_Sys_Para(void)
{
	if((uc_user_option1 & 0x08) != 0)
	{
		b_io_adc_scan_flg = 1;				//not adc
	}
	else
	{
		b_io_adc_scan_flg = 0;			   	//adc
   	}
	
	uc_user_option2 = uc_dbuf0[336];

  	uc_bklight_grade = uc_dbuf0[343];
	uc_lcd_contrast_base = uc_dbuf0[362];
  	uc_lcd_contrast_data = uc_dbuf0[363];

	key_vol_offset = uc_dbuf0[409];
	
	uc_bklight_time_level = uc_dbuf0[341];
	ui_bklight_time = ui_bklight_time_table[uc_bklight_time_level];
	if((uc_user_option2 & BIT2_MASK) != 0)
	{
	b_sd_wppin_flg=1;
	}
	else
	{
	b_sd_wppin_flg=0;
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
	 sfc_logblkaddr0 = uc_reserve_addr0 + 2;					 //read-write area system parameter  
	 sfc_logblkaddr1 = uc_reserve_addr1; 
	 sfc_logblkaddr2 = uc_reserve_addr2; 
	 sfc_logblkaddr3 = uc_reserve_addr3; 
	 sfc_reserved_area_flag = 1;				 //read flash reserved area
	 sfc_occ_buf_flag = SFC_APP_DBUF0;
	 buf_offset0 = 0;							 //buffer offset :0x0000
	 buf_offset1 = 0;
	 _media_readsector();						 //read one sector
	 
				 
	 sfc_logblkaddr0 += 1;						 //system parameter is 1K
	 sfc_occ_buf_flag = SFC_APP_DBUF0;			 //occupy dbuf0
	 buf_offset0 = 0x00;						 //buffer offset :0x0200
	 buf_offset1 = 0x01;
	 _media_readsector();						 //read one sector
	 
	/*** write backup data to read-write area ***/
	 sfc_logblkaddr0 = 0x02;					 //read-write area system parameter  
	 sfc_logblkaddr1 = 0x02; 
	 sfc_logblkaddr2 = 0x00; 
	 sfc_logblkaddr3 = 0x00; 
	 sfc_occ_buf_flag = SFC_APP_DBUF0;
	 buf_offset0 = 0;							 //buffer offset :0x0000
	 buf_offset1 = 0;
	 b_sfc_immediate_wr_flg = 1;
	 FS_WrFlash_Interface(); 						 //write one sector
	
	 sfc_logblkaddr0 = 0x03;
	 sfc_occ_buf_flag = SFC_APP_DBUF0;			 //occupy dbuf0
	 buf_offset0 = 0x00;						 //buffer offset :0x0200
	 buf_offset1 = 0x01;
	 b_sfc_immediate_wr_flg = 1;				 //do last write to flash
	 FS_WrFlash_Interface(); 						 //write one sector  
	 sfc_reserved_area_flag = 0;				 //clr reserved area flag

}

#endif