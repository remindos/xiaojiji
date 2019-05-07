/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_save_para.c
Creator: zj.zong					Date: 2009-05-21
Description: 1. save rec parameter into flash system area
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_REC_DYNAMIC_DISPLAY_DEF

#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_Dis_FileBrowser_Reverse(void);

void Ui_Display_MenuString(void);
void Ui_Display_SelectMenuFlag(void);

void Ui_Display_Menu_Item_Idx(uc8 num_idx,uc8 Total_num);

code ui16 MenuStr_Table_Record[]=
{
	0x0000,							//NULL music
	MENU_STR_REC_MENU_SD_IDX,	    //menu sd	
	MENU_STR_REC_MENU_FLASH_IDX,    //menu flash
	MENU_REC_QUALITY_BASE_IDX,		//WINDOW_REC_MENU_QUALITY
	0x0000,							//WINDOW_MUSIC_MENU_RPL_GAP
	0x0000,							//WINDOW_MUSIC_VOLUME
	0x0000,							//WINDOW_MUSIC_MENU_DEL
	0x0000,							//WINDOW_MUSIC_MENU_DELALL
	0x0000,							//WINDOW_MUSIC_MENU_BROWSER
	
};

/*******************************************************************************
Function: Ui_DisplayVoice_MenuCommon()
Description: the max number of menu item is 7 in voice menu, little than 8'menu item
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_DisplayRec_MenuCommon(void)
{
 	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_MenuString();
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		Ui_Display_Menu_Item_Idx(uc_menu_cur_idx,uc_menu_total_num);
	}
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)
	{
		Ui_Display_SelectMenuFlag();
	}
}

/*******************************************************************************
display voice menu String
********************************************************************************/
void Ui_Display_MenuString(void)
{
	uc8 i, j, tmp_max_num ,uc_lcd_page_num_tmp;
	
	if(uc_menu_total_num <= DisMenu_MaxNum)
	{
		tmp_max_num = uc_menu_total_num;
	}
	else
	{
		tmp_max_num = DisMenu_MaxNum;
	}
	
	uc_lcd_page_num = 2;
	uc_lcd_page_num_tmp = uc_lcd_page_num;

	for(i = uc_menu_top_idx, j = 0; j < tmp_max_num; i++, j++)
	{
		uc_lcd_column_num = 0;		   			    //display menu sel or not sel flg
	
		if(i == (uc_menu_cur_idx - 1))				//check displayed menu be selected?
		{
			uc_lcd_page_num_bk = uc_lcd_page_num;
			Api_Display_Picture(PIC_MENU_SEL_IDX);
			b_lcd_reverse_flg = 1;				
		}
		else
		{
			Api_Display_Picture(PIC_MENU_NOSEL_IDX);
			b_lcd_reverse_flg = 0;				
		}
		uc_lcd_page_num = uc_lcd_page_num_tmp;
		uc_lcd_column_num = 10;						//display menu string
		if(b_lcd_reverse_flg == 1)
		{
			Api_Dis_FileBrowser_Reverse();
		}
		uc_lcd_page_num = uc_lcd_page_num_tmp;
		Api_Display_Picture(i + MenuStr_Table_Record[uc_window_no]+ ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}

/*******************************************************************************
display voice menu idx
********************************************************************************/
void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num)										
{
	uc_lcd_page_num = 1;											
	uc_lcd_column_num = 37;	
									
	Api_Display_8x6_ZiKu(16);						//display "<<"	
//	Api_Display_8x6_ZiKu(numr_idx/100%10);						
	Api_Display_8x6_ZiKu(numr_idx%100/10);						
	Api_Display_8x6_ZiKu(numr_idx%100%10);					
	Api_Display_8x6_ZiKu(15);						//display "/"		
//	Api_Display_8x6_ZiKu(total_num/100%10);		
	Api_Display_8x6_ZiKu(total_num%100/10);		
	Api_Display_8x6_ZiKu(total_num%100%10);					
	Api_Display_8x6_ZiKu(17);						//display ">>"	
}

/*******************************************************************************
display voice menu flag
********************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}



#endif