/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_telbook_enhance.c
Creator: zj.zong					Date: 2009-06-05
Description: 
Others:
Version: V0.1
History:
	V0.1	2009-06-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_TELBOOK_ENHANCE_DEF

#include "sh86278_sfr_reg.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_api_define.h"
#include "..\..\UI\UI_TelBook\ui_telbook_header.h"
#include "..\..\UI\UI_TelBook\ui_telbook_var_ext.h"

extern void Ui_SetLcdFlag_0xfd(void);
	 
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_Read_SysPara_ToDbuf0(void);
extern void FS_WrFlash_Interface(void);
extern void _media_readsector(void);

void Ap_TelBook_Init(void);
void Ap_Init_TelBookVar(void);
void Ap_Exit_TelBookMode(void);
void Ap_SaveTelBook_Para(void);	 
void Ap_Enable_IrqBit(void);
void Ap_PowerOff_MP3(void);
void Ap_SwitchTo_MainMenu(void);
void Ap_Init_EF_LogicAddr(void);
/*******************************************************************************
Function: Ap_TelBook_Init()
description: init telbook
             1. init telbook variable
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_TelBook_Init(void)
{
  	Ap_Init_TelBookVar();
	uc_window_no = WINDOW_TELBOOK_MAIN;		//set telbook window
	uc_save_mode_idx = MODE_7_TELBOOK;
 	 Ui_SetLcdFlag_0xfd();					//clr screen
}

/*******************************************************************************
init telbook var
********************************************************************************/
void Ap_Init_TelBookVar(void)
{
	uc_menu_cur_idx = 1;
	uc_menu_total_num = 4;
   	uc_menu_exit_cnt = 0;
	b_telbook_file_exist = 0;
	b_export_file_ok = 0;
	uc_tel_namelist_top_idx = 0;
	uc_tel_namelist_sel_idx = 0;
	uc_lcd_page_num_bk = 0;
	uc_menu_top_idx = 0;
	uc_refresh_menu_num = 0;			//must be cleared
	ui_fs_file_type = FS_FILE_TYPE_BIN;	//set bit8, file type is *.bin"
}


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
	MR_IE1 = 0x07;						//enable mcu interrupt(timer irq, ADC irq+MSI)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}

/*******************************************************************************
Function: Ap_Exit_TelBookMode()
Description: exit telbook mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exit_TelBookMode(void)
{
	Ap_SaveTelBook_Para(); 
}

/*******************************************************************************
Function: Ap_SaveTelBook_Para()
Description: save telbook parameter
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SaveTelBook_Para(void)
{
  	Api_Read_SysPara_ToDbuf0();
   	uc_dbuf0[339] = uc_save_mode_idx;				//save mode 
	b_sfc_immediate_wr_flg = 1;
	FS_WrFlash_Interface();
	sfc_reserved_area_flag = 0;					//clr reserved area flag
}

/*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: power off mp3
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_TelBookMode();			
	Api_DynamicLoad_PowerOff();		//enter power off mp3 mode
}												   


/*******************************************************************************
Function: Ap_SwitchTo_MainMenu()
Description: return to main menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_TelBookMode();	
	Api_DynamicLoad_MainMenu();		//enter main menu
}


/*******************************************************************************
Function: Ap_Exchange_HighLowByte()
Description: exchange high low byte
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exchange_HighLowByte(void)
{
	uc8 tmp_var;
	ui16 i;

	for(i = 0; i < 256; i++)
	{
		tmp_var = uc_dbuf1[i * 2];
		uc_dbuf1[i * 2] = uc_dbuf1[i * 2 + 1];
		uc_dbuf1[i * 2 + 1] = tmp_var;
	}
}


/*******************************************************************************
Function: Ap_Init_EF_LogicAddr()
Description: init ef logic address
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Init_EF_LogicAddr(void)
{
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
}


#endif