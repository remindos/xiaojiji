/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_ebook_event.c
Creator: zj.zong					Date: 2009-05-05
Description:1. process ebook event(except key & display event)
			2. common subroutine in ebook mode
Others:
Version: V0.1
History:
	V0.1	2009-05-05		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_EBOOK_EVENT_PROCESS

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_EBook\ui_ebook_header.h"
#include "..\..\UI\UI_EBook\ui_ebook_var_ext.h"

extern void Ui_ProcessEvt_BatSub(void);
extern void Ui_PowerOn_Backlight(void);
//extern void Ui_Display_InsertCard(void);
extern void Ui_Display_NoFile(void);//allen
extern void Ui_Display_DrawSD(void);//allen
extern void Ui_Display_NoFile(void);//allen
extern void Ui_ReturnTo_MenuFlash(void); //allen	
extern void Ui_ReturnTo_MenuSD(void); //allen
extern void Api_Delay_1ms(uc8 timer_cnt);//ALLEN

extern void Api_DynamicLoad_USB(void);
extern ui16 Api_Isn_ConvertTo_Unicode(ui16 isn_index);
extern ui16 Api_Merge_2Byte_Sub(uc8 lo,uc8 hi);
extern void Api_DynamicLoad_Ebook_Target(uc8 code_idx);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_PowerOff(void);	
extern void Ap_Exchange_HighLow_Byte(void);
extern void _media_readsector(void);
extern void FS_WrFlash_Interface(void);
extern void Ap_Get_OneByte(void);
extern bit FS_Fread(void);
extern bit FS_Fseek(ul32 offset);

void Ap_ProcessEvt_Usb(void);
void Ap_ProcessEvt_Msi(void);
void Ap_Exit_EbookMode(void);
void AP_InitPlay_Ebook(void);
void Ap_PowerOff_MP3(void);
void Ap_Enable_IrqBit(void);
void Ap_ProcessEvt_Bat(void);
void Ap_Exit_EbookMode(void);
void Ap_SwitchTo_MainMenu(void);
void Ap_FillDecbuf_WithUnicode(void);
void Ap_Check_Init_FlashSD(void);
bit Ap_Check_SDExist(void);

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
	MR_IE1 = 0x07;						//enable mcu interrupt(timer irq, ADC irq, MSI irq)//alllen
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}

/*******************************************************************************
Function: process_evt_usb()
Description: process usb event, enter usb mode
Calls:
Global:uc_load_code_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Usb(void)
{
	Ap_Exit_EbookMode();					
	Ui_PowerOn_Backlight();						
	Api_DynamicLoad_USB();
}

void AP_InitPlay_Ebook(void)
{
	uc_dynamicload_idx = 30;
	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_2_INIT_PLAY_IDX);
	
	b_ebook_file_end = 0;
	b_ebook_first_unicode_flg = 0;
	b_ebbok_unicode_8_16_flg = 0;
	uc_ebook_unicode_flg = 0;				//1--unicode txt; 0--ascii txt
	ui_ebook_byte_idx = 0;
	ui_ebook_page_num = 0;
	ui_ebook_sfc_addr = EBOOK_SFC_START_ADDR;	//the address(0x0000 009A) is used to store start bytes of one page
	
	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_3_GET_PAGE_NUM_IDX);//get ui_ebook_page_num and save bytes into flash reserved area
			
	b_ebook_file_end = 0;
	ui_ebook_page_idx = 1;
	uc_dynamicload_idx = 31;
	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_2_INIT_PLAY_IDX);
}

/*******************************************************************************
Function: Ap_ProcessEvt_Bat()
Description: process battery
Calls:
Global:uc_bat_low_cnt, uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessEvt_Bat(void)
{
	Ui_ProcessEvt_BatSub();				

	uc_lcd_dis_flg |= 0x01;			//display battery
	ui_sys_event |= EVT_DISPLAY;	

	if((MR_USBCON & 0x1) == 0)			//if USB connected, and not power off
	{
		if(uc_bat_low_cnt >= BATTERY_LOW_CNT)
		{
			uc_poweroff_type = POWEROFF_LOW_VOL;
			Ap_PowerOff_MP3();
		}
	}
}


/*******************************************************************************
Function: Ap_ProcessEvt_Msi()
Description: check SD/MMC card plug in or out, and process event corresponding  
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_ProcessEvt_Msi(void)
{
	if((ui_bklight_time == 0) && (uc_bklight_time_level != 0))
	{
		Ui_PowerOn_Backlight();
	}
	
	if(b_msi_plug_flg == 0)				//only process unplug SD/MMC card event	
	{
		b_sd_exist_flg = 0;				//have not sd card
		uc_msi_status = 0xff;
		if(b_sd_flash_flg == 0)			//when in flash	
		{
			if(b_ebook_nofile_flg == 1)	
			{
				Ui_Display_NoFile();
				Ap_SwitchTo_MainMenu();	
			}
			else
			{
				switch(uc_window_no)
				{	
					case WINDOW_EBOOK_MENU_SD:
						Ui_Display_DrawSD();
						uc_menu_cur_idx_bk = 1;
 						Ui_ReturnTo_MenuFlash();
						break;

					default:
						break;
				}
			}
		}
		else
		{
			Ui_Display_DrawSD();
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_4_SAVE_IDX);

			b_sd_flash_flg = 0;			//switch to flash
			uc_dynamicload_idx = 30;
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);
			uc_dynamicload_idx = 41;
			Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX);

			if(b_ebook_nofile_flg == 1)
			{
				Ui_Display_NoFile();
				Ap_SwitchTo_MainMenu();	
			}
			else
			{
				uc_menu_cur_idx_bk = 1;
				Ui_ReturnTo_MenuFlash();
			}
		}
	}
	else
	{													//refresh menu interface  
		if(Ap_Check_SDExist())
		{
			if(uc_window_no == WINDOW_EBOOK_MENU_FLASH)	
			{
				uc_menu_cur_idx_bk = 2;	
				Ui_ReturnTo_MenuSD();
			}	
		}		
	}
}

/*******************************************************************************
1. get the start bytes of current page
2. fseek, get one sectore data and
3. convert isn into unicode, and fill dbuf1 with unicode, 
   the max unicode number is 512, be used to display one page at least
********************************************************************************/
void Ap_FillDecbuf_WithUnicode(void)
{
	uc8 tmp_page_pt;
	ui16 i, tmp_unicode, tmp_ebook_val;
	ul32 tmp_ebook_byte_offset;

/*1. get the start bytes of current page*/
	tmp_page_pt = (ui_ebook_page_idx - 1) & 0x007f;	//0~127
	ui_ebook_sfc_addr = (((ui_ebook_page_idx - 1) & 0xff80) >> 7) + EBOOK_SFC_START_ADDR;
	
	sfc_logblkaddr0 = (uc8)ui_ebook_sfc_addr;
	sfc_logblkaddr1 = (uc8)(ui_ebook_sfc_addr >> 8);
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_DBUF1;			 //occupy dbuf
	sfc_reserved_area_flag = 1;						//reserved area
	buf_offset0 = 0x00;								//buffer offset: 0x0000
	buf_offset1 = 0x00;								//occupy dbuf(0x5000)
	_media_readsector();							//read one page
	sfc_reserved_area_flag = 0;					//clr reserved area flag
	
	tmp_ebook_byte_offset = ul_dbuf1[tmp_page_pt];
	ui_ebook_byte_idx = (ui16)(tmp_ebook_byte_offset & 0x01ff); //0~511
	
/*2. fseek, get one sectore data */
	FS_Fseek(tmp_ebook_byte_offset & 0xfffffe00);	//go to current sector
	uc_fs_assign_buf_flg = SFC_APP_DBUF1;	//assign dbuf(0x5000) to fs
	FS_Fread();
	if(uc_ebook_unicode_flg != 1)					//if it's unicode little endian, and do not exchange high with low
	{
		Ap_Exchange_HighLow_Byte();
	}

/*3. convert isn into unicode, and fill dbuf0 with unicode, 
     the max unicode number is 512, be used to display one page at least*/
	for(i = 0; i < 512; i++)
	{
		ui_dbuf0[i] = 0;				//clr dbuf0
	}
	
	for(i = 0; i < 512; i++)
	{
		Ap_Get_OneByte();					//read one byte "ISN"
		if(b_ebook_file_end == 1)
		{
			break;
		}
		
		if(uc_ebook_unicode_flg == 0)
		{
			tmp_ebook_val = ui_ebook_val;
			tmp_unicode = Api_Isn_ConvertTo_Unicode(tmp_ebook_val);
			if(tmp_unicode == 0)
			{
				Ap_Get_OneByte();				//read one byte "ISN"
			
				tmp_unicode = Api_Isn_ConvertTo_Unicode(Api_Merge_2Byte_Sub((uc8)ui_ebook_val, (uc8)tmp_ebook_val));
			}
			
			ui_dbuf0[i] = tmp_unicode;
		}
		else
		{
			ui_dbuf0[i] = ui_ebook_val;
		}
	}
}


/*******************************************************************************
1. save voice parameter
2. enter power off mode
Global:uc_poweroff_type
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_EbookMode();
	Api_DynamicLoad_PowerOff();
}

/*******************************************************************************
save begin cluster and file length of current ebook file
********************************************************************************/
void Ap_Exit_EbookMode(void)
{
	if((uc_ebookmark_flag & 0x08)!=0)				//current save EbookMark1
	{	 											   
		ui_ebook_mark1 = ui_ebook_page_idx;	  		//save EbookMark1 page idx
  	}
	else if((uc_ebookmark_flag & 0x10)!=0)			//current save EbookMark2
	{
	  	ui_ebook_mark2 = ui_ebook_page_idx;	  		//save EbookMark2 page idx
  	}
	else if((uc_ebookmark_flag & 0x20)!=0)			//current save EbookMark3
	{
	 	ui_ebook_mark3 = ui_ebook_page_idx;	  		//save EbookMark3 page idx
	}

	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_4_SAVE_IDX);
}

/*******************************************************************************
return to main menu
********************************************************************************/
void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_EbookMode();
	Api_DynamicLoad_MainMenu();	//enter top menu
}

/*******************************************************************************
Function: Ap_Check_Init_FlashSD()
Description: check scan which disk, and if no file return to which interface  
*******************************************************************************/
void Ap_Check_Init_FlashSD(void)
{	 
	uc_dynamicload_idx = 40;			//init variable, restore parameter and init dac	
	Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX);	

	if(Ap_Check_SDExist() == 1)			//SD card exist, and init ok
	{			
Label_SwitchCard:
		uc_dynamicload_idx = 30;		//switch flash or SD/MMC card		
		Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);
		uc_dynamicload_idx = 41;		//scan disk and get Ebook total number	
		Api_DynamicLoad_Ebook_Target(CODE_EBOOK_1_INIT_IDX);
	}
	else
	{
		b_sd_flash_flg = 0;				//select SD/MMC card
		b_sd_exist_flg = 0;				//if no file, and return to main menu	
		goto Label_SwitchCard;
	}		
}


/*******************************************************************************
Function: Ap_Check_SDExist()
description: check if has SD card or if the card init ok when the card is already exist
             1. if has SD card
             2. if the card init ok while there has a SD card
Calls:
Global:Null
Input: Null
Output:return 1 bit		1-init SD card ok;	0-fail
Others:
*******************************************************************************/
bit Ap_Check_SDExist(void)
{
	if(((MR_MSCON4 & 0x80)^((uc_user_option2 & 0x04)<<5)) == 0)
	{		
		return 0;
	}
	
	if(uc_msi_status != 0)
	{
		Api_Delay_1ms(100);		
		uc_dynamicload_idx = 31;						//init SD/MMC
		Api_DynamicLoad_Ebook_Target(CODE_EBOOK_8_SWITCHCARD_IDX);		

		if(uc_msi_status != 0)							//init SD/MMC card fail
		{
			b_sd_exist_flg = 0;
			return 0;
		}
		else											//init SD/MMC card ok
		{
			b_sd_exist_flg = 1;
			return 1;
		}
	}
	else
	{
		b_sd_exist_flg = 1;
		return 1;	
	}
}


#endif