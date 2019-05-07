/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_voice_display.c
Creator: zj.zong					Date: 2009-05-20
Description: display function
Others:
Version: V0.1
History:
	V0.1	2009-05-20		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_VOICE_PROCESS_DISPLAY_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include ".\UI\UI_Voice\ui_voice_header.h"
#include ".\UI\UI_Voice\ui_voice_var_ext.h"


extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);
extern void Ap_SwitchTo_MainMenu(void);

extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_ClrAssigned_Area(uc8 uc_clr_width);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_Dis_FileBrowser_Reverse(void);

extern void Api_LcdDriver_Clr_AllPage(void);
extern void	Hwdriver_ClrOne_Page(uc8 num_idx);

void Ui_Display_Voice(void);
void Ui_DisplayVoice_MenuCommon(void);
void Ui_DisplayVoice_MenuDel(void);
void Ui_DisplayVoice_MenuDelall(void);
void Ui_DisplayVoice_CurTime(void);
void Ui_DisplayVoice_TotalTime(void);
void Ui_DisplayVoice_Samplerate(void);
void Ui_DisplayVoice_Idx(void);
void Ui_DisplayVoice_State(void);
void Ui_DisplayVoice_Replay(void);
void Ui_DisplayVoice_Name(void);
void Ui_Display_Voice_VolumeWindow(void);
void Ui_DisplayVoice_Idx_Sub(void);
void Ui_Display_FileError(void);
void Ui_Display_Voice_Time_Sub(void);
void Ui_ProcessEvt_Display(void);
void Ui_Display_NoFile(void);
//void Ui_Display_InsertCard(void);
void Ui_Display_Voice_Flag(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MenuString(void);
void Ui_Display_Voice_Frequnce(void);

void Ui_Display_Menu_Item_Idx(uc8 numr_idx,uc8 total_num);		

code ui16 MenuStr_Table_Voice[]=
{
	0x0000,							//voice
	MENU_STR_VOICE_MENU_PLAY_IDX,	//play menu
	MENU_STR_VOICE_MENU_STOP_IDX,	//stop menu
	MENU_STR_VOICE_MENU_STOP_FLASH_IDX, //stop menu SD
	MENU_STR_VOICE_MENU_STOP_SD_IDX,	//stop menu FLASH
	MENU_STR_VOICE_MENU_REPEAT_IDX,	//repeat
  	0x0000,							//volume
	0x0000,							//delete
	0x0000,							//delete all
	

};



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
	{ */

		if(b_lcd_clrall_flg == 1)		//clr LCD all page(display background)
		{			
			Api_LcdDriver_Clr_AllPage();

		} 
		switch(uc_window_no)
		{
			case WINDOW_VOICE:
				Ui_Display_Voice();				//display voice main
				break;
				
			case WINDOW_VOICE_VOLUME:
				Ui_Display_Voice_VolumeWindow();   //display voice volume
				break;
				
			case WINDOW_VOICE_MENU_DEL:
				Ui_DisplayVoice_MenuDel();		  //display voice one file
				break;
				
			case WINDOW_VOICE_MENU_DELALL:
				Ui_DisplayVoice_MenuDelall();	  //display voice all file
				break;
				
			default:
				Ui_DisplayVoice_MenuCommon();	 //display voice menu
				break; 
		}	
//	 }
 	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;
}

/*******************************************************************************
Function: 1.Ui_Display_Voice()
Description: display voice play main interface
Calls:
Global:uc_lcd_dis_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_Voice(void)
{
/*	if(b_fb_nofile_flg == 0)			//display nofile
	{
		Ui_Display_NoFile();
		Ap_SwitchTo_MainMenu();
	}  */
	if((uc_lcd_dis_flg & BIT0_MASK) != 0)	//display battery is common func
	{
		Ui_Display_Battery();
	}
	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		Ui_DisplayVoice_CurTime();
	}
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
	   	Ui_Display_Voice_Flag();
		Ui_DisplayVoice_TotalTime();
		
		Ui_DisplayVoice_Samplerate();			//display mp3 bitrate
	}
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		Ui_DisplayVoice_State();				//display current state(play, pause, stop, backward, forward)
		
		Ui_DisplayVoice_Replay();				//display A-B replay
	}	
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)		//display id3 info or mp3 file name
	{
		Ui_DisplayVoice_Name();
	}

	if(b_voice_file_err == 1)					//display file error
	{
		Ui_Display_FileError();
		b_voice_file_err = 0;
	 	b_voice_file_end = 1;
		ui_sys_event |= EVT_MOVIE_FILE_END;		//clr file end event and flag
	}
	if((uc_lcd_dis_flg & BIT7_MASK) != 0)
	{
		if(b_voice_file_err == 1)
		{
			uc_refresh_pic_num = 0;
		}
		Ui_Display_Voice_Frequnce();
	}
}

/*******************************************************************************
display voice Frequnce 
********************************************************************************/
void Ui_Display_Voice_Frequnce(void)
{
	uc_lcd_page_num = 5;
	uc_lcd_column_num = 50;
	Api_Display_Picture(PIC_VOICE_FREQ_IDX + uc_refresh_pic_num);
}

/*******************************************************************************
display voice current time
********************************************************************************/
void Ui_DisplayVoice_CurTime(void)
{
	uc_lcd_page_num = 0;		 //display current time "xx:xx:xx"
	ui_voice_temp_var = ui_voice_cur_time;
	Ui_Display_Voice_Time_Sub();
}

/*******************************************************************************
display voice total time
********************************************************************************/
void Ui_DisplayVoice_TotalTime(void)
{
	uc_lcd_page_num = 1;		 //display total time "xx:xx:xx"
	ui_voice_temp_var = ui_voice_total_time;
	Ui_Display_Voice_Time_Sub();
}

/*******************************************************************************
display 16*8 Unicode"0~9"
********************************************************************************/
void Ui_Display_Voice_Time_Sub(void)
{
	uc8 tmp_h, tmp_m, tmp_s;

	uc_lcd_column_num = 62;			
	tmp_h = ui_voice_temp_var / 3600;
	tmp_m = ui_voice_temp_var % 3600 / 60;
	tmp_s = ui_voice_temp_var % 3600 % 60;

	Api_Display_8x6_ZiKu( tmp_h / 10 % 10);	//display timer "hour"
	Api_Display_8x6_ZiKu( tmp_h % 10);
	Api_Display_8x6_ZiKu(10);					//display ":"
	Api_Display_8x6_ZiKu(tmp_m / 10);			//display timer "minute"
	Api_Display_8x6_ZiKu(tmp_m % 10);
	Api_Display_8x6_ZiKu(10);					//display ":"
	Api_Display_8x6_ZiKu(tmp_s / 10);			//display timer "second"
	Api_Display_8x6_ZiKu(tmp_s % 10);
}

/*******************************************************************************
display voice time 
********************************************************************************/							
void Ui_DisplayVoice_Idx(void)
{
	uc_lcd_page_num = 0;
	ui_voice_temp_var = ui_voice_wav_idx;
	Ui_DisplayVoice_Idx_Sub();				//display "xxx"

	uc_lcd_page_num = 1;
	ui_voice_temp_var = ui_fb_total_num;
	Ui_DisplayVoice_Idx_Sub();				//display "xxx"
}

/*******************************************************************************
display 16*8 Unicode"0~9"
********************************************************************************/
void Ui_DisplayVoice_Idx_Sub(void)
{
	uc_lcd_column_num = 40;
	Api_Display_8x6_ZiKu(ui_voice_temp_var / 100 % 10);
	Api_Display_8x6_ZiKu(ui_voice_temp_var / 10 % 10);
	Api_Display_8x6_ZiKu(ui_voice_temp_var % 10);
}

/*******************************************************************************
display voice sample rate
********************************************************************************/
void Ui_DisplayVoice_Samplerate(void)
{
	uc_lcd_page_num = 0;
 	uc_lcd_column_num = 17;

	if(ul_voice_samplerate == 8000)
	 {
	  	Api_Display_Picture(PIC_VOICE_8K_IDX) ;
	 }
	 else
	 {
	  	Api_Display_Picture(PIC_VOICE_16K_IDX) ;
	 }
}

 
/*******************************************************************************
display voice current state
********************************************************************************/
void Ui_DisplayVoice_State(void)
{
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 2;
  	Api_Display_Picture(PIC_STATE_BASE_IDX + uc_voice_status - 1);
}

/*******************************************************************************
display voice replay
********************************************************************************/
void Ui_DisplayVoice_Replay(void)
{

	uc_lcd_page_num = 0;
	uc_lcd_column_num = 40;

	if(uc_voice_replay_ab_flg == 0)
	{
		Ui_DisplayVoice_Idx();
	}
	else if(uc_voice_replay_ab_flg == 3)
	{
		 Api_ClrAssigned_Area(18);
		 uc_lcd_page_num = 1;
		 Api_ClrAssigned_Area(18);
		 uc_lcd_page_num = 0;
		Api_Display_Picture(PIC_VOICE_AB_BASE_IDX);	//display "A-B"
	}
	else
	{
		 Api_ClrAssigned_Area(18);
		 uc_lcd_page_num = 1;
		 Api_ClrAssigned_Area(18);
		 uc_lcd_page_num = 0;
		if(b_voice_dis_ab_flicker == 1)
		{
			Api_Display_Picture(PIC_VOICE_AB_BASE_IDX + uc_voice_replay_ab_flg);//display "-B" or "-A"
		}
		else
		{
			Api_Display_Picture(PIC_VOICE_AB_BASE_IDX);	//display "A-B"
		}
		b_voice_dis_ab_flicker = ~b_voice_dis_ab_flicker;
	}
}

/*******************************************************************************
display voice name
********************************************************************************/
void Ui_DisplayVoice_Name(void)
{
	uc8 i;
	uc_lcd_page_num = 2;	
	uc_lcd_column_num =20;

	for(i = 0; (i < uc_fs_item_name_length) && (uc_lcd_column_num < DIS_NAME_X_END); i++)
	{
		Api_Display_Unicode(ui_fs_item_name[i]);
	}
}


/******************************************************************************
Function: Ui_Display_Voice_VolumeWindow()
Description: display volume bar,process and uc_voice_volume_level
Calls:
Global: uc_voice_volume_level,uc_menu_cur_idx
Input:  Null
Output: Null
Others:
******************************************************************************/
void Ui_Display_Voice_VolumeWindow(void)
{
 	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_VOLUME_BAST_IDX + uc_voice_volume_level);
	}
}


/*******************************************************************************
Function: Ui_DisplayVoice_MenuCommon()
Description: the max number of menu item is 7 in voice menu, little than 8'menu item
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_DisplayVoice_MenuCommon(void)
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
display menu del all
********************************************************************************/
void Ui_DisplayVoice_MenuDelall(void)
{
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
  	Api_Display_Picture(PIC_VOICE_DRI_IDX);

	uc_lcd_page_num = 5;
	uc_lcd_column_num = 0;		//display pic "Yes & No"
	Api_Display_Picture(PIC_VOICE_MENU_DEL_ALL_IDX + uc_menu_cur_idx - 1);
}

/*******************************************************************************
display menu del
********************************************************************************/
void Ui_DisplayVoice_MenuDel(void)
{
	Ui_DisplayVoice_Name();
	uc_lcd_page_num = 5;
	uc_lcd_column_num = 0;		//display pic "Yes & No"
	Api_Display_Picture(PIC_VOICE_MENU_DEL_IDX + uc_menu_cur_idx - 1);
}

/*******************************************************************************
delay 500ms
********************************************************************************/
void Ui_Delay_500ms(void)
{
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

/*******************************************************************************
display string deleting
********************************************************************************/
void Ui_Display_StringDeleting(void)
{
	uc_lcd_page_num = 5;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DELETING_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}

/*******************************************************************************
display string file error
********************************************************************************/
void Ui_Display_FileError(void)
{
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_FILEERR_IDX + ALL_MENU_NUMBER * uc_language_idx);	
	Ui_Delay_500ms();
}

/*******************************************************************************
display no file
*******************************************************************************/
void Ui_Display_NoFile(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	if(b_sd_flash_flg == 1)		//SD/MMC care exsit, and select flash
	{
	Api_Display_Picture(NORM_STR_SD_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	uc_menu_cur_idx_bk = 1;
	}
	else
	{ 	
	Api_Display_Picture(NORM_STR_FLASH_NOFILE_IDX + ALL_MENU_NUMBER * uc_language_idx);
	uc_menu_cur_idx_bk = 2;
	}
	Ui_Delay_500ms();
}

/*******************************************************************************
display string InsertCard
********************************************************************************/
/*void Ui_Display_InsertCard(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_INSERT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay_500ms();
}	  */
/*******************************************************************************
display string draw sd card
********************************************************************************/
void Ui_Display_DrawSD(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DRAW_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}

/*******************************************************************************
display voice flag
********************************************************************************/
void Ui_Display_Voice_Flag(void)
{ 
   	uc_lcd_page_num = 0;		
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_VOICE_FLAG_IDX);
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
		Api_Display_Picture(i + MenuStr_Table_Voice[uc_window_no] + ALL_MENU_NUMBER * uc_language_idx);
	
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
display voice menu flag
********************************************************************************/
void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
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
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}
#endif