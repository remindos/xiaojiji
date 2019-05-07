/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_fm_display.c
Creator: zj.zong				Date: 2009-05-25
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-05-25		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_FM_DISPLAY

#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_FM\ui_fm_header.h"
#include ".\UI\UI_FM\ui_fm_var_ext.h"
#include ".\DRV\DRV_FM\drv_fm.h"

extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_ClrAssigned_Area(uc8 uc_clr_width);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_Dis_FileBrowser_Reverse(void);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_LcdDriver_Clr_2345Page(void);
extern void Api_LcdDriver_ClrOne_Page(uc8 num_idx);

void Ui_Display_FM_Band(void);
void Ui_Display_FM(void);
void Ui_Display_FM_Freq(void);
void Ui_Display_FM_NO_Channel(void);
void Ui_Display_FM_CH_Idx(void);
void Ui_Display_FM_Menu(void);

void Ui_Display_Del_Menu(void);
void Ui_Display_FM_Menu_Del(void);
void Ui_Display_FM_Menu_Delall(void);
void Ui_Display_FM_Volume_Window(void);
void Ui_Display_FM_CH_Idx_Sub(void);
void Ui_Display_FM_SignalStrength(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MenuString(void);
void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num);



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
/*	if((uc_lcd_dis_flg & BIT1_MASK) != 0)
	{
		uc_hold_cnt = 10;
		Ui_Display_HoldPic();
	}

	if(uc_hold_cnt == 0)
	{  */
		if(b_lcd_clrall_flg == 1)		//clr LCD all page
		{
			Api_LcdDriver_Clr_AllPage();
		}
		
		switch(uc_window_no)
		{
			case WINDOW_FM:
				Ui_Display_FM();
				break;
	
			case WINDOW_FM_VOLUME:
				Ui_Display_FM_Volume_Window();
				break;
	
			case WINDOW_FM_MENU:
		   		Ui_Display_FM_Menu();
				break;
	
			case WINDOW_FM_MENU_DEL:
				Ui_Display_FM_Menu_Del();
				break;
	
			case WINDOW_FM_MENU_DELALL: 
				 Ui_Display_FM_Menu_Delall();
				break;
	
			case WINDOW_FM_SIGNAL: 
				 Ui_Display_FM_SignalStrength();
				break;
	
			default:
			  	break;
		}
		
		if(b_fm_search_flg == 1)			//for autosearch
		{
			if((uc_fm_mode != FM_MODE_PRESET) && (uc_fm_mode != FM_MODE_TUNING))
			{
				ui_sys_event |= EVT_FM_SEARCH;
			}
			else
			{
				b_fm_search_flg = 0;
			}
		}
//	}
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}

/*******************************************************************************
Function: 1.Ui_Display_FM()
Description: display FM main interface
Calls:
Global:uc_lcd_dis_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_FM(void)
{
	if((uc_lcd_dis_flg & BIT0_MASK) != 0)		//display battery is common func
	{
		Ui_Display_Battery();
	}
	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		Ui_Display_FM_Band();
	}
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)
	{
		Ui_Display_FM_Freq();
		
		Ui_Display_FM_CH_Idx();
	}
}


/*******************************************************************************
display fm band
*******************************************************************************/
void Ui_Display_FM_Band(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_FM_FLAG_IDX);
	
	uc_lcd_page_num = 6;
	uc_lcd_column_num = 0;
	if(uc_fm_band == FM_BAND_NORMAL)
	{
	   	Api_Display_Picture(PIC_FM_NOR_BAND_LOW_IDX);	 //"87MHz"  and  "108MHz"
	}
	else
	{
		Api_Display_Picture(PIC_FM_JP_BAND_LOW_IDX);     // "76MHz"  and  "90MHz"
	}
	
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 41;
	Api_Display_Picture(PIC_FM_IDX);					//FM
}


/*******************************************************************************
display fm fred
*******************************************************************************/
void Ui_Display_FM_Freq(void)
{
	uc8 tmp_freq_level, i;

	ui_fm_dis_freq = ui_fm_cur_freq;			//init displayed freq(ui_fm_cur_freq bit15 must be clear)

   	uc_lcd_page_num	= 3;
	uc_lcd_column_num = 60;
   	tmp_freq_level = ui_fm_dis_freq / 10000;
	if(tmp_freq_level != 0)
	{
		Api_Display_8x6_ZiKu(tmp_freq_level);	// >= 100.0MHz
	}
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 1000 % 10);
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 100 % 10);
	Api_Display_8x6_ZiKu(11);					//display "."
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 10 % 10);
	if(tmp_freq_level == 0)
	{
		Api_Display_8x6_ZiKu(13);
	}

	Api_LcdDriver_ClrOne_Page(5);		//clr point picture
	if(uc_fm_band==1)
	{
		i = 6;							//Japan band
	}
	else
	{
		i = 4;							//normal band
	}
	uc_lcd_page_num = 5;
	uc_lcd_column_num = ((ui_fm_dis_freq-ui_fm_freq_limit_low)/10*i/10) + 16;
	Api_Display_Picture(PIC_FM_POINT_IDX);
}

/*******************************************************************************
display fm ch idx 
*******************************************************************************/
void Ui_Display_FM_CH_Idx(void)
{
	uc8 i;
	uc_lcd_page_num	= 1;
	uc_lcd_column_num = 50;
	Api_ClrAssigned_Area(36);			//clr channel 

	if (uc_fm_mode == FM_MODE_PRESET)		//preset mode
	{
		Ui_Display_FM_CH_Idx_Sub();
	}
	else if(uc_fm_mode == FM_MODE_TUNING)	//tuing mode, check current freq is existed in preset buffer
	{
		if(uc_fm_ch_total_num != 0)
		{
			for(i = 0; i < uc_fm_ch_total_num; i++)
			{
				if(ui_fm_dis_freq == (ui_fm_preset_buf[i]&0x7fff))
				{
					uc_fm_cur_ch_idx = i + 1;
					uc_fm_mode = FM_MODE_PRESET;
					
					Ui_Display_FM_CH_Idx_Sub();
					break;
				}
			}
		}
	}
}

/*******************************************************************************
display fm ch idx 
*******************************************************************************/
void Ui_Display_FM_CH_Idx_Sub(void)
{
	Api_Display_Picture(PIC_FM_CH_YES_IDX);			//display "CH" (X--DIS_FM_CH_X; Y--DIS_FM_CH_Y)
	
	uc_lcd_page_num = 1;
	uc_lcd_column_num = 70;
	Api_Display_8x6_ZiKu(uc_fm_cur_ch_idx / 10);
	Api_Display_8x6_ZiKu(uc_fm_cur_ch_idx % 10);
}


/******************************************************************************
Function: Ui_Display_FM_Volume_Window()
Description: display volume bar,process and uc_fm_volume_level
Calls:
Global: uc_fm_volume_level,uc_menu_cur_idx
Input:  Null
Output: Null
Others:
******************************************************************************/
void Ui_Display_FM_Volume_Window(void)
{
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_VOLUME_BAST_IDX + uc_fm_volume_level);
	}
}

/*******************************************************************************
display fm del menu 
*******************************************************************************/
void Ui_Display_FM_Menu_Del(void)
{
   	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
   	{
		Ui_Display_Del_Menu();			//display deleted channel index and freq
   	}
}

void Ui_Display_Del_Menu(void)
{
	uc8 tmp_freq_level;
	ui16 tmp_dis_freq_bk;

	tmp_dis_freq_bk = ui_fm_dis_freq;			//backup dis freq
	ui_fm_dis_freq = ui_fm_menu_dis_freq;
	
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_FM_DELCH_IDX);	//display string "delete channel"
 
   	uc_lcd_page_num = 5;
	uc_lcd_column_num = 15;
 	Api_Display_Picture(PIC_FM_CH_YES_IDX);//display "CHxx"
	
	uc_lcd_column_num  = 31;
	Api_Display_8x6_ZiKu(uc_fm_menu_ch_idx / 10);
	Api_Display_8x6_ZiKu(uc_fm_menu_ch_idx % 10);

	uc_lcd_column_num = 57;	//display "xxx.xMHz"
	tmp_freq_level = ui_fm_dis_freq / 10000;
	if(tmp_freq_level != 0)
	{
		Api_Display_8x6_ZiKu( tmp_freq_level);		//if freq >= 100.0MHz
	}
	else
	{
		Api_Display_8x6_ZiKu(13);					//if freq <= 99.9MHz, display "blank"
	}
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 1000 % 10);
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 100 % 10);
	Api_Display_8x6_ZiKu(18);						//display "."
	Api_Display_8x6_ZiKu(ui_fm_dis_freq / 10 % 10);
	Api_Display_Picture(PIC_FM_MHZ_IDX);			//display "MHz"
	
  	ui_fm_dis_freq = tmp_dis_freq_bk;				//restore dis freq
}

/*******************************************************************************
display fm del all 
*******************************************************************************/
void Ui_Display_FM_Menu_Delall(void)
{
   	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
   	{
  		uc_lcd_page_num = 5;	
		uc_lcd_column_num = 0;
 		Api_Display_Picture(PIC_FM_ALL_YES_NO_IDX + uc_menu_cur_idx - 1);	//display pic "Yes & No"
 	}
}

/*******************************************************************************
display fm no channel  
*******************************************************************************/

void Ui_Display_FM_NO_Channel(void)
{
	Api_LcdDriver_Clr_AllPage();
	
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_NO_CHANNEL_IDX + ALL_MENU_NUMBER * uc_language_idx);  	//display string "no channel"
	
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

/*******************************************************************************
display fm signal strength  
*******************************************************************************/
void Ui_Display_FM_SignalStrength(void)
{
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_SIGNAL_BAST_IDX + uc_menu_cur_idx - 1);
	}

}

/*******************************************************************************
Function: Ui_Display_FM_Menu()
Description: the max number of menu item is 7 in fm menu, little than 8'menu item
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_FM_Menu(void)
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
display fm menu string  
*******************************************************************************/
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
		Api_Display_Picture(i + MENU_STR_FM_MENU_IDX + ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}

/*******************************************************************************
display fm menu idx  
*******************************************************************************/
void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num)										
{
	uc_lcd_page_num = 1;											
	uc_lcd_column_num = 37;	
									
	Api_Display_8x6_ZiKu(16);						//display "<<"	
	Api_Display_8x6_ZiKu(numr_idx/100%10);						
	Api_Display_8x6_ZiKu(numr_idx%100/10);						
	Api_Display_8x6_ZiKu(numr_idx%100%10);					
	Api_Display_8x6_ZiKu(15);						//display "/"		
	Api_Display_8x6_ZiKu(total_num/100%10);		
	Api_Display_8x6_ZiKu(total_num%100/10);		
	Api_Display_8x6_ZiKu(total_num%100%10);					
	Api_Display_8x6_ZiKu(17);						//display ">>"	
}

/*******************************************************************************
display fm menu flag  
*******************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}



#endif