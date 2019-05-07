/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_ebook_key.c
Creator: zj.zong					Date: 2009-05-02
Description: process key event
Others:
Version: V0.1
History:
	V0.1	2009-05-02		  zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_EBOOK_TIMER_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_EBook\ui_ebook_header.h"
#include ".\UI\UI_EBook\ui_ebook_var_ext.h"



extern void Ui_SelNext_Page_InEbook(void);

extern void Ap_Del_File(void);
extern void AP_InitPlay_Ebook(void);

extern void Api_Key_Mapping(void);
extern void Api_Check_Battery(void);
extern void Api_Check_BklightOff(void);
extern void Api_Check_Idle_PowerOff(void);
extern void Api_Check_Sleep_PowerOff(void);
extern void Api_Check_Battery(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ui_Display_NoFile(void);
extern void Ui_Check_Refresh_MenuFlag(void);

void Ui_Check_Del_EbookMark(void);
void UI_Check_ExitMenu(void);
void Ui_SetLcdFlag_0xfd(void);
void UI_ReturnTo_Browser(void);
void Ui_ReturnTo_MenuSD(void);
void UI_CheckAuto_PlayEbook(void);
void Ui_ReturnTo_MenuFlash(void);
void Ui_Delete_Return_FlahsSD(void);
void Ui_Init_TopIdx(void);
void Ui_Ebook_Return_FlahsSD(void);

/*******************************************************************************
Function: Ui_ProcessEvt_Timer()
Description: process timer 100ms/once
Calls:
Global:uc_timer_cnt
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Timer(void)
{
	if(uc_timer_cnt >= 5)							//5*20ms = 100ms
	{
		uc_timer_cnt = 0;

		Api_Check_Battery();

		Api_Check_Sleep_PowerOff();					//sleep mode
			
		UI_Check_ExitMenu();

		if(uc_window_no == WINDOW_EBOOK)
		{
			if(uc_ebook_play_mode != 0)
			{
				UI_CheckAuto_PlayEbook();			//auto play ebook
			}
		}
		else
		{
			Api_Check_BklightOff();
			Api_Check_Idle_PowerOff();
		}
	}
}



/*******************************************************************************
exit menu window
********************************************************************************/
void UI_Check_ExitMenu(void)						//exit from menu window
{
	if(uc_window_no == WINDOW_EBOOK_DELETE)			//delete menu
	{
		if((++uc_menu_exit_cnt) >= TIME_EXIT_MENU)
		{
			UI_ReturnTo_Browser();
			ui_sys_event |= EVT_DISPLAY;
		}
	}
	if((uc_window_no == WINDOW_EBOOK_MENU_SD)||(uc_window_no == WINDOW_EBOOK_MENU_FLASH))
	  {
     	Ui_Check_Refresh_MenuFlag();
	  }
}


/*******************************************************************************
prepare to get next ebook file in auto play mode
********************************************************************************/
void UI_CheckAuto_PlayEbook(void)
{
	uc_ebook_play_gap_cnt++;
	if(uc_ebook_play_gap_cnt >= (uc_ebook_play_gap * 10))	//gap = uc_ebook_play_gap * 10 * 100ms
	{
		uc_ebook_play_gap_cnt = 0;
		Ui_SelNext_Page_InEbook();
	}
}

/*******************************************************************************
return to file browser
*******************************************************************************
void UI_ReturnTo_Browser(void)
{
	uc_window_no = WINDOW_EBOOK_BROWSER;
	Ui_SetLcdFlag_0xfd();
}
	 */
/*******************************************************************************
Function: Ui_ReturnTo_Browser()
Description: return to photo browser
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ReturnTo_Browser(void)
{
	if(b_ebook_nofile_flg == 0)
	{
		uc_window_no = WINDOW_EBOOK_BROWSER;
		Ui_SetLcdFlag_0xfd();
	}
	else
	{
		if(b_sd_exist_flg == 0)			//if SD/MMC card not exist
		{
			Ui_Display_NoFile();
			Ap_SwitchTo_MainMenu();		//return to main menu		
		}
		else
		{
			Ui_Display_NoFile();
			Ui_ReturnTo_MenuSD();		//when have SD/MMC card, and return to sd stop menu
		}
	}
}
/*******************************************************************************
Function: Ui_Ebook_Return_FlahsSD()
Description: return to photo stop menu have SD/MMC card
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
 void Ui_Ebook_Return_FlahsSD(void)
{
	if(b_sd_exist_flg == 0)
	{
		uc_menu_cur_idx_bk = 1;
		Ui_ReturnTo_MenuFlash();
	}
	else
	{
		if(b_sd_flash_flg == 1)
		{	
			uc_menu_cur_idx_bk = 1;	
		}
		else
		{
			uc_menu_cur_idx_bk = 2;
		}
		Ui_ReturnTo_MenuSD();	
	}
}
/*******************************************************************************
Function: Ui_ReturnTo_MenuSD()
Description: return to photo stop menu have SD/MMC card
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_ReturnTo_MenuSD(void)
{
	uc_window_no = WINDOW_EBOOK_MENU_SD;
//	uc_menu_title_idx = TITLE_STR_EBOOK_MENU_IDX;
	uc_menu_total_num = 4;//Ebook_MenuSD_Num;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;	
	Ui_SetLcdFlag_0xfd();
	ui_sys_event |= EVT_DISPLAY;
}


/*******************************************************************************
Function: Ui_ReturnTo_MenuFlash()
Description: return to photo stop menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ui_ReturnTo_MenuFlash(void)
{
	uc_window_no = WINDOW_EBOOK_MENU_FLASH;
	//uc_menu_title_idx = TITLE_STR_EBOOK_MENU_IDX;
	uc_menu_total_num = 3;//Ebook_MenuFlash_Num;
	uc_menu_cur_idx = uc_menu_cur_idx_bk;
	Ui_SetLcdFlag_0xfd();
	ui_sys_event |= EVT_DISPLAY;	
}

void Ui_Delete_Return_FlahsSD(void)
{
	if(b_sd_exist_flg == 0)
	{
		uc_menu_cur_idx_bk = 2;
		Ui_ReturnTo_MenuFlash();
	}
	else
	{
		uc_menu_cur_idx_bk = 3;
		Ui_ReturnTo_MenuSD();	
	}
}
/*******************************************************************************
Function: Ui_Init_TopIdx()
Description: set clr lcd flag, and dis flag 0xff
*******************************************************************************/
void Ui_Init_TopIdx(void)
{
	if(uc_menu_cur_idx > (DisMenu_MaxNum + uc_menu_top_idx))	//"1 <= uc_menu_cur_idx <= 10"
	{
		uc_menu_top_idx = uc_menu_cur_idx - DisMenu_MaxNum;
	}
	else if(uc_menu_cur_idx <= uc_menu_top_idx)
	{
		uc_menu_top_idx = uc_menu_cur_idx - 1;
	}		
}

/*******************************************************************************
clear screen
********************************************************************************/
void Ui_SetLcdFlag_0xfd(void)
{
	b_lcd_clrall_flg = 1;
	uc_lcd_dis_flg= 0xfd;			  		//bit1  is hold picture	bit
}

void Ui_Check_Del_EbookMark(void)
{
	if((uc_ebook_mark1_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark1_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark1_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark1_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			uc_ebookmark_flag &= 0x36;		//clr EookMark1 flag bit0(for write) and bit3(for read and write) 
		}
	}
	else if((uc_ebook_mark1_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark1_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark1_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark1_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			uc_ebookmark_flag &= 0x2d;	  	//clr EookMark2 flag bit1(for write) and bit4(for read and write) 
		}
	}
	else if((uc_ebook_mark1_cluster1 == uc_fs_begin_cluster1) && (uc_ebook_mark1_cluster2 == uc_fs_begin_cluster2))
	{
		if((uc_ebook_mark1_length1 == (uc8)ul_fs_file_length) && (uc_ebook_mark1_length2 == (uc8)(ul_fs_file_length>>8)))
		{
			uc_ebookmark_flag &= 0x1b;	 	//clr EookMark3 flag bit2(for write) and bit5(for read and write) 
		}
	}
}

#endif