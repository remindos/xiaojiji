/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_rec_display.c
Creator: zj.zong			Date: 2009-06-01
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-06-01		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_RECORD_DISPLAY_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Record\ui_rec_header.h"
#include ".\UI\UI_Record\ui_rec_var_ext.h"

extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);
extern void Ui_SetLcdFlag_0xfd(void);

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_ClrAssigned_Area(uc8 uc_clr_width);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_Dis_FileBrowser_Reverse(void);
extern void	Api_LcdDriver_ClrOne_Page(uc8 page_idx);
extern void Api_DynamicLoad_Record_Target(uc8 load_code_idx);

void Ui_Display_Rec(void);
void Ui_Display_Rec_Flag(void);
void Ui_Display_RecIdx(void);
void Ui_Display_RecName(void);
void Ui_Display_CurState(void);
void Ui_Display_RecSaving(void);
void Ui_Display_RecQuality(void);
void Ui_Display_Rec_CurTime(void);
void Ui_Display_Rec_NoSpace(void);
void Ui_Display_Rec_OverFlow(void);
//void Ui_Display_InsertCard(void);
void Ui_Display_Rec_RemainTime(void);
void Ui_Display_RecTimeSub(void);
void Ui_Display_RecIdxSub(void);
void Ui_Display_Refresh_Picture(void);

void Ui_DisplayRec_MenuCommon(void);
void Ui_Display_MenuString(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_Menu_Item_Idx(uc8 num_idx,uc8 Total_num);
void Ui_Display_MSWP(void);

/*******************************************************************************
Function: Ui_ProcessEvt_Display()
Description: display 
Calls:
Global:uc_lcd_dis_flg, uc_window_no
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Display(void)
{
/* 	if((uc_lcd_dis_flg & BIT1_MASK) != 0)
	{
		uc_hold_cnt = 10;
		Ui_Display_HoldPic();
	}
	if(uc_hold_cnt == 0)
	{	*/
		if(b_lcd_clrall_flg == 1)		//clr LCD all page
		{
			Api_LcdDriver_Clr_AllPage();	  
		}
		if(uc_window_no == WINDOW_REC) //display record main
		{
			Ui_Display_Rec();							
		}
		else
		{
		//	Ui_DisplayRec_MenuCommon();	   //display record menu		
			Api_DynamicLoad_Record_Target(CODE_RECORD_8_DYNAMIC_DISPLAY_IDX);
		}
//	}
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}

/*******************************************************************************
Function: 1.Ui_Display_Rec()
Description: display record main interface
Calls:
Global:uc_lcd_dis_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_Rec(void)
{
	if((uc_lcd_dis_flg & BIT0_MASK) != 0)	//display battery 
	{
		Ui_Display_Battery();
	}
  	if((uc_lcd_dis_flg & BIT2_MASK) != 0)	//display current time 
	{
		if(b_flicker_time_flg == 1)
		{
			uc_lcd_column_num = 62;		
			uc_lcd_page_num = 0;
			Api_ClrAssigned_Area(48);
		}
		else
		{
		 	Ui_Display_Rec_CurTime();						//display current time and total time
		}
		Ui_Display_Refresh_Picture();
	}
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)  //display remain time
	{
		Ui_Display_Rec_RemainTime();	
		Ui_Display_Rec_Flag();
	}
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)  //display recording, start, pause, stop
	{
		Ui_Display_CurState();				
	}
	if((uc_lcd_dis_flg & BIT5_MASK) != 0)  //display record quality
	{
		Ui_Display_RecQuality();			
	}
  	if((uc_lcd_dis_flg & BIT6_MASK) != 0)  //display record index and  file name
	{
		Ui_Display_RecIdx();				

		Ui_Display_RecName();				
	}
}

/*******************************************************************************
display record Refresh Picture
********************************************************************************/
void Ui_Display_Refresh_Picture(void)
{

 	uc_lcd_column_num = 16;		
	uc_lcd_page_num = 5;
	Api_Display_Picture(PIC_REC_REFRESH_IDX + uc_refresh_pic_num);
}


/*******************************************************************************
display record current time
********************************************************************************/
void Ui_Display_Rec_CurTime(void)
{
	uc_lcd_page_num = 0;		
  	uc_lcd_column_num = 62;					//display current timer
	ul_rec_temp_var = ul_rec_cur_time;		//display "xx:xx:xx"
	Ui_Display_RecTimeSub();
}

/*******************************************************************************
display record remain time
********************************************************************************/
void Ui_Display_Rec_RemainTime(void)
{
	uc_lcd_page_num = 1;
	uc_lcd_column_num = 62;			   		//display remain timer
	ul_rec_temp_var = ul_rec_remain_time;	//display "xx:xx:xx"
	Ui_Display_RecTimeSub();
}

/*******************************************************************************
display record remain time
********************************************************************************/
void Ui_Display_Rec_Flag(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_REC_FALG_IDX);	
} 
 
/*******************************************************************************
display record time sub
********************************************************************************/
void Ui_Display_RecTimeSub()
{
	uc8 tmp_h, tmp_m, tmp_s;	

	tmp_h = ul_rec_temp_var / 3600;
	tmp_m = ul_rec_temp_var % 3600 / 60;
	tmp_s = ul_rec_temp_var % 3600 % 60;
	Api_Display_8x6_ZiKu( tmp_h / 10 % 10); //display timer "hour"
	Api_Display_8x6_ZiKu( tmp_h % 10);
	Api_Display_8x6_ZiKu(10);				//display ":"
	Api_Display_8x6_ZiKu(tmp_m / 10);		//display timer "minute"
	Api_Display_8x6_ZiKu(tmp_m % 10);
	Api_Display_8x6_ZiKu(10);				//display ":"
	Api_Display_8x6_ZiKu(tmp_s / 10);		//display timer "second"
	Api_Display_8x6_ZiKu(tmp_s % 10);	
}

/*******************************************************************************
display record current state
********************************************************************************/
void Ui_Display_CurState(void)
{
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;

	if(uc_rec_status != STATE_RECORD)
	{
		Api_Display_Picture(PIC_REC_STATE_BASE_IDX + uc_rec_status - 1);
	}
	else
	{
		Api_Display_Picture(PIC_REC_STATE_BASE_IDX );
	}
}

/*******************************************************************************
display record quality
********************************************************************************/
void Ui_Display_RecQuality(void)
{
 	uc_lcd_page_num = 0;
 	uc_lcd_column_num = 17;
	if(b_rec_samplerate_flg == 1)
	{						
		Api_Display_Picture(PIC_VOICE_16K_IDX);
	}
	else
	{
	  	Api_Display_Picture(PIC_VOICE_8K_IDX);
	}
}

/*******************************************************************************
display record index
********************************************************************************/
void Ui_Display_RecIdx(void)
{
  	uc_lcd_page_num = 0;
	uc_lcd_column_num = 40;		
   	ui_rec_temp_var = uc_fs_wav_idx;
	Ui_Display_RecIdxSub();		  //display "xxx"

	uc_lcd_page_num = 1;
	uc_lcd_column_num = 40;		
   	ui_rec_temp_var = ui_rec_wav_total_num;
	Ui_Display_RecIdxSub();		  //display "xxx"
}

/*******************************************************************************
display record index sub
********************************************************************************/
void Ui_Display_RecIdxSub(void)
{
	Api_Display_8x6_ZiKu(ui_rec_temp_var / 100 % 10);
	Api_Display_8x6_ZiKu(ui_rec_temp_var / 10 % 10);
	Api_Display_8x6_ZiKu(ui_rec_temp_var % 10);
}

/*******************************************************************************
display record name
********************************************************************************/
void Ui_Display_RecName(void)
{
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 14;		
	Api_Display_Unicode('M');
	Api_Display_Unicode('I');
	Api_Display_Unicode('C');
	Api_Display_Unicode('R');
	Api_Display_Unicode('0');
	Api_Display_Unicode('0' + uc_fs_wav_idx / 100);
	Api_Display_Unicode('0' + uc_fs_wav_idx / 10 % 10);
	Api_Display_Unicode('0' + uc_fs_wav_idx % 10);
	Api_Display_Unicode('.');
	Api_Display_Unicode('W');
	Api_Display_Unicode('A');
	Api_Display_Unicode('V');
}


/*******************************************************************************
Function: Ui_DisplayVoice_MenuCommon()
Description: the max number of menu item is 7 in voice menu, little than 8'menu item
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
*******************************************************************************
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
*/
/*******************************************************************************
display voice menu String
*******************************************************************************
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
		Api_Display_Picture(i + MENU_REC_QUALITY_BASE_IDX+ ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}
*/
/*******************************************************************************
display voice menu idx
*******************************************************************************
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
*/
/*******************************************************************************
display voice menu flag
*******************************************************************************
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}

*/
/*******************************************************************************
delay 500ms
********************************************************************************/
void Ui_Delay_500ms(void)
{
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);	
	Ui_SetLcdFlag_0xfd();
}

/*******************************************************************************
display record saving
********************************************************************************/
void Ui_Display_RecSaving(void)
{
	Api_LcdDriver_ClrOne_Page(2);
	Api_LcdDriver_ClrOne_Page(3);
	Api_LcdDriver_ClrOne_Page(4);
	
	uc_lcd_page_num = 2;	
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_SAVING_IDX+ALL_MENU_NUMBER*uc_language_idx);	
	Ui_Delay_500ms();
}

/*******************************************************************************
display record no space
********************************************************************************/
void Ui_Display_Rec_NoSpace(void)
{ 
	Api_LcdDriver_ClrOne_Page(2);
	Api_LcdDriver_ClrOne_Page(3);
	Api_LcdDriver_ClrOne_Page(4);

 	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_NOSPACE_IDX+ALL_MENU_NUMBER*uc_language_idx);	
	Ui_Delay_500ms();
}

/*******************************************************************************
display record over flow
********************************************************************************/
void Ui_Display_Rec_OverFlow(void)
{
	Api_LcdDriver_ClrOne_Page(2);
	Api_LcdDriver_ClrOne_Page(3);
	Api_LcdDriver_ClrOne_Page(4);

	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_RECOVERFLOW_IDX+ALL_MENU_NUMBER*uc_language_idx);
	Ui_Delay_500ms();
}

/*void Ui_Display_InsertCard(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_INSERT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}*/ 
/*******************************************************************************
display draw SDcard
*******************************************************************************/
void Ui_Display_DrawSD(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DRAW_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}

 /*******************************************************************************
Function: Ui_Display_MSWP()
Description: display "SD/MMC card has been WP" 
*******************************************************************************/
void Ui_Display_MSWP(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_PROTECT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();	
}
#endif