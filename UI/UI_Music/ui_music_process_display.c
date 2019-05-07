/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_music_process_display.c
Creator: zj.zong					Date: 2009-04-24
Description: process music display event, as subroutine in music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MUSIC_PROCESS_DISPLAY_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Music\ui_music_header.h"
#include ".\UI\UI_Music\ui_music_var_ext.h"


extern void Ui_Display_Battery(void);
extern void Ui_Display_HoldPic(void);
extern void Ui_ReturnTo_MenuSD(void);

extern ui16	Ap_Get_BitRate(void);
extern void	Api_Display_8x6_ZiKu(uc8 ziku_idx);
extern void Api_Display_Picture(ui16 ui_pic_idx);
extern void Api_ClrAssigned_Area(uc8 uc_clr_width);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);
extern void Api_Delay_1ms(uc8 timer_cnt);
extern void Api_Dis_FileBrowser_Reverse(void);
extern void Api_Display_Unicode(ui16 unicode_idx);
extern void Api_LcdDriver_Clr_AllPage(void);
extern void Api_LcdDriver_Clr_2345Page(void);
extern void	Api_LcdDriver_ClrOne_Page(uc8 num_idx);

void Ui_Display_Music(void);
void Ui_Display_VolumeWindow(void);
void Ui_Display_Browser(void);
void Ui_Display_MenuCommon(void);
void Ui_Display_Music_Format(void);
void Ui_Display_BitRate(void);
void Ui_Display_BrowserItem(void);
void Ui_DisplayBrowser_ItemFlag(void);
void Ui_DisplayBrowser_ItemName(void);
void Ui_Display_DummyFolder(void);
void Ui_Display_Menu_Item_Idx(uc8 num_idx,uc8 Total_num);
void Ui_Display_Idx_Sub(void);
void Ui_Display_CurState(void);
void Ui_Display_Lrc(void);
void Ui_Display_DelAllMenu(void);
void Ui_Display_DelMenu(void);
void Ui_Display_Deleting(void);
void Ui_Display_CurTime(void);
void Ui_Display_TotalTime(void);
void Ui_Display_MusicFlag(void);
void Ui_Display_EQ(void);
void Ui_Display_Repeat(void);
void Ui_Display_Id3(void);
void Ui_Display_FileError(void);
void Ui_Display_Menu_RplGap_Time(void);
void Ui_Display_Number_Sub(ui16 ui_number);
void Ui_Display_Idx(void);
void Ui_Display_MusicVolumeFlag(void);
void Ui_Display_DrawSD(void);
void Ui_Display_MSWP(void);
void Ui_Display_MenuString(void);
void Ui_Display_SelectMenuFlag(void);
void Ui_Display_MusicFreq(void);
//void Ui_Display_InsertCard(void);
//void Ui_DisPlay_Page4Column0(ui16 index);
void Ui_Display_Sys_Contrast(void);
void Ui_Display_Rtc_Setup(void);
void Ui_Display_Powerff_Setup(void);
void Ui_Display_TopMenu(void);
void Ui_Display_Input(void);
void Ui_Display_Calendar(void);

void Ui_Display_WelcomePicture(void);
void Ui_Display_ByebyePicture(void);

void Ui_Display_Music_Num(void);

code ui16 MenuStr_Table_Music[]=
{
	0x0000,							//NULL music
	MENU_STR_MUSIC_MENU_IDX,		//menu play
    MENU_STR_MUSIC_SD_MENU_IDX,		//2
    MENU_STR_MUSIC_FLASH_MENU_IDX,	//3				//allen
	MENU_STR_MUSIC_MENU_EQ_IDX,		//eq
	MENU_STR_MUSIC_MENU_REPEAT_IDX,	//repeat
	MENU_STR_MUSIC_RTC_IDX,			//RTC SETUP
	0x0000,							//WINDOW_MUSIC_MENU_RPL_GAP
	0x0000,							//WINDOW_MUSIC_VOLUME
	0x0000,							//WINDOW_MUSIC_MENU_DEL
	0x0000,							//WINDOW_MUSIC_MENU_DELALL
	0x0000,							//WINDOW_MUSIC_MENU_BROWSER
	MENU_STR_MUSIC_SYS_MENU_IDX,	//SYS MENU
	MENU_STR_SYS_MENU_BKLIGHT_TIME_IDX,//bklight time
	0x0000,
	MENU_STR_SYS_MENU_POWEROFF_SLEEP_IDX,
	MENU_STR_SYS_MENU_LOCKKEY_IDX

};

/*******************************************************************************
Function: Ui_ProcessEvt_Display()
Description: music display function 
Calls:
Global:uc_lcd_dis_flg, uc_window_no
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_ProcessEvt_Display(void)
{
		if(b_lcd_clrall_flg == 1)				//clr LCD all page
		{
			Api_LcdDriver_Clr_AllPage();
		}
	  	switch(uc_window_no)
		{
			case WINDOW_MUSIC:
				Ui_Display_Music();					//display music main
				break;
	
			case WINDOW_MUSIC_VOLUME:	
				Ui_Display_VolumeWindow();			//display volume window
				break; 
	
			case WINDOW_MUSIC_MENU_BROWSER:
				Ui_Display_Browser();				//display mp3 folder and file browser
				break;
	
			case WINDOW_MUSIC_MENU_DEL:	
				Ui_Display_DelMenu(); 				//display delete window
				break;
	
			case WINDOW_MUSIC_MENU_DELALL:	
				Ui_Display_DelAllMenu(); 			//display delete window
				break;

			case WINDOW_SYS_MENU_CONTRAST:
				Ui_Display_Sys_Contrast();
				break;

			case WINDOW_SYS_RTCADJ:
				//Ui_Display_Rtc_Setup();
				break;


			case WINDOW_MUSIC_TOP_MENU:
				Ui_Display_TopMenu();
				break;
			case WINDOW_MUSIC_INPUT:
				Ui_Display_Input();
				break;
			case WINDOW_MUSIC_LOGO:
				Ui_Display_WelcomePicture();
				break;
			case WINDOW_MUSIC_BYEBYE:
				Ui_Display_ByebyePicture();
				break;
			case WINDOW_MUSIC_SHOW_LOCK:
				uc_lcd_page_num = 0;
				uc_lcd_column_num = 0;
				if( b_lock_flg!= 0)
    				Api_Display_Picture(0x4226);	//lock pic
				else Api_Display_Picture(0x4227);
				break;

			case WINDOW_MUSIC_NUM:
				Ui_Display_Music_Num();
				break;
			case WINDOW_MUSIC_BOOK:

				break;
				
			default:
				Ui_Display_MenuCommon();			//display menu
				break; 
		}
//	}
	
	b_lcd_clrall_flg = 0;
	uc_lcd_dis_flg = 0;

}

#define PIC_NUM1632_BASE 0x4814
void Ui_Display_Input(void)
{
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 32;
    Api_Display_Picture(PIC_NUM1632_BASE + ui_num_key/1000);
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 48;
    Api_Display_Picture(PIC_NUM1632_BASE + (ui_num_key%1000)/100);
	uc_lcd_page_num = 2;
   	uc_lcd_column_num = 64;
    Api_Display_Picture(PIC_NUM1632_BASE + (ui_num_key/10)%10);
	uc_lcd_page_num = 2;
   	uc_lcd_column_num = 80;
    Api_Display_Picture(PIC_NUM1632_BASE + ui_num_key%10);

}

void Ui_Display_Music_Num(void)
{
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 32;
    Api_Display_Picture(PIC_NUM1632_BASE + ui_mp3_idx_indir/1000);
	uc_lcd_page_num = 2;
	uc_lcd_column_num = 48;
    Api_Display_Picture(PIC_NUM1632_BASE + (ui_mp3_idx_indir%1000)/100);
	uc_lcd_page_num = 2;
   	uc_lcd_column_num = 64;
    Api_Display_Picture(PIC_NUM1632_BASE + (ui_mp3_idx_indir/10)%10);
	uc_lcd_page_num = 2;
   	uc_lcd_column_num = 80;
    Api_Display_Picture(PIC_NUM1632_BASE + ui_mp3_idx_indir%10);

}



void Ui_Display_TopMenu(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
    Api_Display_Picture(0x4214+uc_menu_cur_idx-1);
}


void Ui_Display_WelcomePicture(void)
{

   uc_lcd_column_num = 0;
	uc_lcd_page_num = 0;
	Api_Display_Picture(PIC_POWERON_BASE_IDX+uc_logo_idx);
} 

void Ui_Display_ByebyePicture(void)
{
	uc_lcd_column_num = 0;
	uc_lcd_page_num = 0;
	Api_Display_Picture(PIC_POWEROFF_BASE_IDX + uc_logo_idx);
}

/*******************************************************************************
Function: 1.Ui_Display_Music()
Description: display music play main interface
Calls:
Global:uc_lcd_dis_flg
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_Music(void)
{

	if((uc_lcd_dis_flg & BIT0_MASK) != 0)	//display battery is common func
	{
		Ui_Display_Battery();
		//Ui_Display_Calendar();
	}
	
	if((uc_lcd_dis_flg & BIT2_MASK) != 0)
	{
		Ui_Display_CurTime();				//display current time 
		//Ui_Display_Music_Format();
		Ui_Display_TotalTime();
	}
	
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{		
		//Ui_Display_EQ();					//display eq mode
		Ui_Display_Repeat();				//display repeat mode
	}
		
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		Ui_Display_MusicVolumeFlag();
		//Ui_Display_MusicFreq();
		Ui_Display_CurState();				//if have mp3 file, and display current state

	}
		
	if((uc_lcd_dis_flg & BIT5_MASK) != 0)
	{
		Ui_Display_Idx();					//display mp3 index
		
		//Ui_Display_MusicFlag();
	}
		
	if((uc_lcd_dis_flg & BIT6_MASK) != 0)	//display lrc or dynamic id3 or dis nofile or dis file error
	{
		if(b_lrc_file_exist == 1)
		{
		  	Ui_Display_Lrc();
		}
		else
		{
			Api_LcdDriver_Clr_2345Page();	
		}
		
	}
	if((uc_lcd_dis_flg & BIT7_MASK) != 0)
	{
		if(b_rsv_mp3_flg == 0)
		Ui_Display_Id3();
	}

	if(b_lcd_dis_file_err == 1)	//display file error
	{
		//Ui_Display_FileError();
		b_lcd_dis_file_err = 0;
	 	b_mp3_file_end = 1;
		ui_sys_event |= EVT_MUSIC_FILE_END;		//clr file end event and flag
	}

}



/*******************************************************************************
display music flag(lrc or have not lrc)
********************************************************************************/
/*void Ui_Display_MusicFlag(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	if(b_lrc_file_exist == 0)
	{
		Api_Display_Picture(PIC_MUSIC_FLAG_IDX);
	}
	else
	{
   		Api_Display_Picture(PIC_MUSICLRC_FLAG_IDX);
	}
}*/



void Ui_Display_TimeSub(ui16 dis_time)
{
//	uc_lcd_column_num = 98;//85;
	Api_Display_8x6_ZiKu(dis_time/600%10);			//display timer minute
	Api_Display_8x6_ZiKu((dis_time/60)%10);
	Api_Display_8x6_ZiKu(10);						//display ":"
	uc_lcd_column_num--;
	Api_Display_8x6_ZiKu((dis_time%60)/10);			//display timer second
	Api_Display_8x6_ZiKu((dis_time%60)%10);	
}


/*******************************************************************************
display music current time
********************************************************************************/
void Ui_Display_CurTime(void)
{
	uc_lcd_page_num = 7;//0;
	uc_lcd_column_num = 98;//85;
	Ui_Display_TimeSub(ui_mp3_cur_time);
}


/*******************************************************************************
display music total time
********************************************************************************/
void Ui_Display_TotalTime(void)
{
	uc_lcd_page_num = 7;
	uc_lcd_column_num = 50;
	Ui_Display_TimeSub(ui_mp3_total_time);
}


/*******************************************************************************
display music format
********************************************************************************/
/*void Ui_Display_Music_Format(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 13;
   	Api_Display_Picture(PIC_MUSIC_FORMAT_IDX);	//Display music format "MP3"
	uc_lcd_page_num = 0;						//Display music bitrate "128kbps"
	uc_lcd_column_num = 14;
	Ui_Display_BitRate();
}*/

/*******************************************************************************
display music bitrate
********************************************************************************/
/*void Ui_Display_BitRate(void)
{
	ui_mp3_bitrate = Ap_Get_BitRate();
	Ui_Display_Number_Sub(ui_mp3_bitrate);
}*/

/*******************************************************************************
display music eq
********************************************************************************/
/*void Ui_Display_EQ(void)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 67;
	Api_Display_Picture(PIC_MUSIC_EQ_BASE_IDX + uc_mp3_eq_mode);
}*/

/*******************************************************************************
display music repeat
********************************************************************************/
void Ui_Display_Repeat(void)
{
	

	uc_lcd_page_num = 7;//0;//7;//0;
	uc_lcd_column_num = 26;//34;
	//if(uc_mp3_replay_ab_flg == 0)
	{
		Api_Display_Picture(PIC_MUSIC_REPEAT_BASE_IDX + uc_mp3_repeat_mode);
	}
	/*else if(uc_mp3_replay_ab_flg == 3)
	{
		Api_Display_Picture(PIC_MUSIC_AB_BASE_IDX);		//display "A-B"
	}
	else
	{
		if(b_mp3_dis_ab_flicker == 1)
		{
			Api_Display_Picture(PIC_MUSIC_AB_BASE_IDX + uc_mp3_replay_ab_flg);//display "-B" or "-A"
		}
		else
		{
		  	Api_Display_Picture(PIC_MUSIC_AB_BASE_IDX);	//display "A-B"
		}
		b_mp3_dis_ab_flicker = ~b_mp3_dis_ab_flicker;
	}*/
}


/*******************************************************************************
display music current state
********************************************************************************/
void Ui_Display_CurState(void)
{
/*
	uc_lcd_page_num = 7;//6;
	uc_lcd_column_num = 86;//30;//2;
	if(b_status_flg != 0)
		Api_Display_Picture(PIC_STATE_BASE_IDX + uc_mp3_status - 1);
	else
		Api_Display_Picture(0x4255 ); //timer off	

	uc_lcd_page_num = 0;
	uc_lcd_column_num = 30;
	if(b_sd_flash_flg == 0)
		Api_Display_Picture(0x4250 ); //flash
	else
		Api_Display_Picture(0x4251 );	//tf card
		
	//lock status	
	uc_lcd_page_num = 7;
	uc_lcd_column_num = 0;
	if(b_lock_flg == 0)
		Api_Display_Picture(0x4252 ); //unlock
	else
		Api_Display_Picture(0x4253 );	//locked
		
	// timer status	
	uc_lcd_page_num = 7;
	uc_lcd_column_num = 14;
	if(uc_sleep_poweroff_level == 0)
		Api_Display_Picture(0x4255 ); //timer off
	else
		Api_Display_Picture(0x4254 );	//timer on
		*/
}


/*******************************************************************************
display music idx
********************************************************************************/
void Ui_Display_Idx(void)
{
 /*  	if((uc_mp3_repeat_mode == REPEAT_FOLDER) || (uc_mp3_repeat_mode == REPEAT_FOLDER_NOR))
	{
		ui_mp3_tmp_var = ui_mp3_idx_indir;
	}
	else
	{
		ui_mp3_tmp_var = ui_mp3_idx;
	}
	uc_lcd_page_num = 0;
	Ui_Display_Idx_Sub();
  	
	if((uc_mp3_repeat_mode == REPEAT_FOLDER) || (uc_mp3_repeat_mode == REPEAT_FOLDER_NOR))
	{
		ui_mp3_tmp_var = ui_fs_special_file_num_indir;
	}
	else
	{
		ui_mp3_tmp_var = ui_fb_total_num;
	}
	uc_lcd_page_num = 1;
	Ui_Display_Idx_Sub();
	*/
/*	uc_lcd_page_num = 0;
	uc_lcd_column_num = 52;//42;	
	//Api_Display_8x6_ZiKu(ui_fs_special_file_num_indir/1000);
	Api_Display_8x6_ZiKu((ui_fs_special_file_num_indir)/100);
	Api_Display_8x6_ZiKu((ui_fs_special_file_num_indir%100)/10);
	Api_Display_8x6_ZiKu((ui_fs_special_file_num_indir%10));

	uc_lcd_column_num = 76;//82;//72;
	Api_Display_8x6_ZiKu((ui_mp3_idx_indir)/100);
	Api_Display_8x6_ZiKu(((ui_mp3_idx_indir)/10)%10);
	Api_Display_8x6_ZiKu((ui_mp3_idx_indir)%10);
  */
}

/*******************************************************************************
display music rpl gap time
********************************************************************************/
/*void Ui_Display_Menu_RplGap_Time(void) 
{
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_REPLAY_GAP_TIME_BASE_IDX + uc_menu_cur_idx - 1);
}*/

/*******************************************************************************
display music idx sub
********************************************************************************/
void Ui_Display_Idx_Sub(void)
{
	uc_lcd_column_num = 48;
	Ui_Display_Number_Sub(ui_mp3_tmp_var);
}

/*******************************************************************************
display music number common sub
********************************************************************************/
void Ui_Display_Number_Sub(ui16 ui_number)
{
	Api_Display_8x6_ZiKu(ui_number / 100 );
	Api_Display_8x6_ZiKu(ui_number / 10 % 10);
	Api_Display_8x6_ZiKu(ui_number % 10);
}

/*******************************************************************************
display music id3
********************************************************************************/
void Ui_Display_Id3(void)
{
/*	uc8 i = 0, tmp_length;

	//if(uc_id3_length == 0)					//not ID3 info
	{
		tmp_length = uc_fs_item_name_length-4;
	}
   	if((uc_mp3_status != STATE_PLAY) )
	{
		uc_mp3_dis_offset = 0;
		uc_loop_cnt = 7;
	}

   	//Api_LcdDriver_Clr_2345Page();			//clr the id3 area
	Api_LcdDriver_ClrOne_Page(1);
	Api_LcdDriver_ClrOne_Page(2);
	uc_lcd_page_num = 1;//2;				//set the id3 address
	uc_lcd_column_num = 0;

	if(uc_loop_cnt < 8)
	{
		uc_lcd_column_num = 112-uc_loop_cnt*16;
	}
	else
	{
		uc_mp3_dis_offset++;
		if(uc_mp3_dis_offset>tmp_length)
		{
		 	uc_mp3_dis_offset = 0;
			uc_loop_cnt = 0;
			uc_lcd_column_num = 112-uc_loop_cnt*16;
		}
				
	}
	uc_loop_cnt++;

	i = uc_mp3_dis_offset;
	while(1)
	{
		if(uc_lcd_column_num <= 112)
		{
			if(i < tmp_length)
			{
				//if(uc_id3_length == 0)
				{
					Api_Display_Unicode(ui_fs_item_name[i]); //item name buffer
				}
				i++;
			}
			else
			{
				//uc_mp3_dis_offset++;		//addr + 1
				break;
			}
		}
		else
		{
			//uc_mp3_dis_offset++;			//addr + 1
			break;
		}
	} */
}

/*******************************************************************************
display music lrc
********************************************************************************/
void Ui_Display_Lrc(void)
{
/*	uc8 i;

	if(b_mp3_lrc_flg == 1)								//display  new lrc
	{
		b_mp3_lrc_flg = 0;								//clr the flag
		uc_mp3_lrc_offset = 0;
	}
	else
	{
		if(uc_mp3_lrc_offset == 0)
		{
			return;
		}
	}
//clr lrc area
	Api_LcdDriver_Clr_2345Page();						//clr the id3 area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	i = uc_mp3_lrc_offset;
	while(1)
	{
		if(uc_lcd_column_num <= 112)					 //check column overflow
		{
			if(ui_lrc_line_buf[i] == 0)					 //line lrc is over
			{
				return;
			}
			else
			{
				Api_Display_Unicode(ui_lrc_line_buf[i]); //music lrc infobuffer
				i++;
			}
		}
		else
		{
			uc_lcd_page_num += 2;
			uc_lcd_column_num = 0;	

			if(uc_lcd_page_num >= 6)
			{
				uc_mp3_lrc_offset++;
				break;
				
			}
		}
	}*/
}

/*******************************************************************************
if menu total number is great than DisMenu_MaxNum, and display menu bar,
otherwise, not display menu bar
********************************************************************************/
void Ui_Display_DelMenu(void)
{	
/*	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Ui_DisplayBrowser_ItemName();
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 4;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_MENU_YES_NO_IDX + uc_menu_cur_idx - 1);
	} */
}



void Ui_Display_DelAllMenu(void)
{
/*	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
		
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 16;
		if(b_fs_subdir_stk_pt == 1)
		{
			Api_Display_Picture(PIC_ROOT_NAME_IDX);
		}
		else
		{
			Ui_DisplayBrowser_ItemName();
		}
	}

	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 4;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_MENU_ALL_YES_NO_IDX + uc_menu_cur_idx - 1);
	} */
}

void Ui_Display_VolumeWindow(void)
{
	if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_VOLUME_BAST_IDX + uc_mp3_volume_level);
	}
}

/*******************************************************************************
Function: 1.Ui_Display_Browser()
Description: bit3--display folder and file
                1. firstly, display one dummy folder(be used for return to previous layer dir)
                2. secondly, display all folders
                3. finally, display all files
                4. displayed folder and file max number is "DIS_BROWSER_ITEM_MAX_NUM"
             bit4--display selected lines and progress bar
Calls:
Global:ui_fb_top_item_idx, ui_fb_sel_item_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_Browser(void)
{
	if((uc_lcd_dis_flg & BIT3_MASK) != 0)
	{
		Ui_Display_BrowserItem();

		b_lcd_reverse_flg = 0;
	}
}


/*******************************************************************************
if mp3 file&folder number is great than DisMenu_MaxNum, and display browser bar,
otherwise, not display browser bar
********************************************************************************/
void Ui_Display_BrowserItem(void)
{
	uc_dynamicload_idx = 55;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);		//init display mp3 var

	for(uc_fb_dis_cnt = 0; uc_fb_dis_cnt < uc_fb_tmp_dir_max_item; uc_fb_dis_cnt++)
	{
		if((ui_fb_top_item_idx + uc_fb_dis_cnt) == 0)	//display dummy folder
		{
			if(uc_fs_subdir_stk_pt > 0)					//have not "dummy folder" in ROOT
			{
				if(ui_fb_sel_item_idx == 0)
				{
					b_lcd_reverse_flg = 1;
				}
				else
				{
					b_lcd_reverse_flg = 0;
				}
				
				Ui_Display_DummyFolder();
			}
			else
			{
				goto dis_other_item;
			}
		}
		else												//display file or folder
		{
dis_other_item:
			ui_fb_cur_item_idx = ui_fb_top_item_idx + uc_fb_dis_cnt;
			
			if(ui_fb_sel_item_idx == ui_fb_cur_item_idx)
			{
				b_lcd_reverse_flg = 1;						//check current item that is selected
			}
			else
			{
				b_lcd_reverse_flg = 0;
			}

			if(uc_fs_subdir_stk_pt > 0)						//if it's subdir, and ui_browser_cur_item_idx-1(dummy folder)
			{
				ui_fb_cur_item_idx--;
			}

			if(ui_fb_cur_item_idx < ui_fs_subdir_num)	//display folder
			{
				uc_dynamicload_idx = 2;	 //get browser item name
				Api_DynamicLoad_Music_Target(CODE_MUSIC_6_GET_ITEM_NAME_IDX);
			}
			else											//display file
			{
				uc_dynamicload_idx = 1;	//get browser item name
			  	Api_DynamicLoad_Music_Target(CODE_MUSIC_6_GET_ITEM_NAME_IDX);
			}
							
			if(b_lcd_reverse_flg == 1)
			{
				uc_fb_sel_item_type = uc_fs_item_type;	//backup selected item type
			}

		  	uc_lcd_column_num = 16;
	        uc_lcd_page_num_bk = uc_lcd_page_num;
			Ui_DisplayBrowser_ItemName();					//display file or folder name
			if(b_lcd_reverse_flg == 1)
			{
				Api_Dis_FileBrowser_Reverse();
				b_lcd_reverse_flg = 0;
			}
		   
		    uc_lcd_page_num = uc_lcd_page_num_bk;
		   	uc_lcd_column_num = 0;
			Ui_DisplayBrowser_ItemFlag();					//display file or folder flag pic
		}
		uc_lcd_page_num = uc_lcd_page_num_bk + 2;
	}
 	Ui_Display_Menu_Item_Idx((ui_fb_sel_item_idx + 1),ui_mp3_tmp_var);
}




void Ui_DisplayBrowser_ItemFlag(void)
{
	if(uc_fs_item_type == 1)						//1:mp3 file; 2:folder
	{
		Api_Display_Picture(PIC_MUSIC_FILEBROW_IDX);		//mp3 file flag
	}
	else
	{
		Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
	}
}



void Ui_Display_DummyFolder(void)
{
	uc_lcd_page_num_bk = uc_lcd_page_num;
 	uc_lcd_column_num = 16;				//display dummy folder ".."
//	Api_Display_Unicode('.');
//	Api_Display_Unicode('.');
	Api_Display_Picture(0x40AF);		//return previous level pic
	if(b_lcd_reverse_flg == 1)
	{
		Api_Dis_FileBrowser_Reverse();
		b_lcd_reverse_flg = 0;
	}
	uc_lcd_page_num = uc_lcd_page_num_bk;
	uc_lcd_column_num = 0;
	Api_Display_Picture(PIC_FILEBROW_FOLDER_IDX);
	uc_lcd_page_num = uc_lcd_page_num_bk;
}
 


void Ui_DisplayBrowser_ItemName(void)
{
	uc8 i = 0;
	while(1)
	{
		if(uc_lcd_column_num <= 112)
		{
			if(i < uc_fs_item_name_length)
			{
				Api_Display_Unicode(ui_fs_item_name[i]);
				i++;
			}
			else
			{
				break;
			}
		}
		else
		{
			break;
		}
	}
}


void Ui_Delay500ms(void)
{
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
}


/*******************************************************************************
display  file error
*******************************************************************************/
void Ui_Display_FileError(void)
{
  Api_LcdDriver_Clr_2345Page();
 	uc_lcd_page_num = 2;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_FILEERR_IDX + ALL_MENU_NUMBER * uc_language_idx);	//display file error
	Ui_Delay500ms();
}


 /*******************************************************************************
display draw SDcard
*******************************************************************************/
void Ui_Display_DrawSD(void)
{
	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DRAW_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay500ms();
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
	Ui_Delay500ms();
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
	Ui_Delay500ms();	
}	
/*******************************************************************************
display delete file
*******************************************************************************/
void Ui_Display_Deleting(void)
{
   	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_DELETING_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay500ms();
}
/*void Ui_Display_InsertCard(void)
{

	Api_LcdDriver_Clr_AllPage();		//clr dis lrc area
	uc_lcd_page_num = 3;
	uc_lcd_column_num = 0;
	Api_Display_Picture(NORM_STR_INSERT_CARD_IDX + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay500ms();
} */

/*******************************************************************************
Function: Ui_Display_MenuCommon()
Description: bit2--display menu title	
		      1. displayed menu max number is DisMenu_MaxNum
Calls:
Global:uc_menu_total_num, uc_menu_cur_idx
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Display_MenuCommon(void)
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



void Ui_Display_SelectMenuFlag(void)
{
  	uc_lcd_column_num = 112;
	uc_lcd_page_num = uc_lcd_page_num_bk;
	Api_Display_Picture(PIC_SELECT_FLAG_IDX + uc_refresh_menu_num);
}

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
		Api_Display_Picture(i + MenuStr_Table_Music[uc_window_no] + ALL_MENU_NUMBER * uc_language_idx);
	
		b_lcd_reverse_flg = 0;				
		uc_lcd_page_num = uc_lcd_page_num + 1;
		uc_lcd_page_num_tmp = uc_lcd_page_num;
	}
}



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

void Ui_Display_MusicVolumeFlag(void)
{
 	uc_lcd_page_num = 0;//6;
	uc_lcd_column_num = 0;//15;
   	Api_Display_Picture(PIC_VOLUME_FLAG_IDX);
	uc_lcd_page_num = 0;//6;
	uc_lcd_column_num = 14;//34;
	Api_Display_8x6_ZiKu(uc_mp3_volume_level/10);
	Api_Display_8x6_ZiKu(uc_mp3_volume_level%10);
    //Api_Display_Picture(PIC_NUMBER_BASE_IDX + uc_mp3_volume_level/10);
	//uc_lcd_page_num = 6;
   	//uc_lcd_column_num = 43;
    //Api_Display_Picture(PIC_NUMBER_BASE_IDX + uc_mp3_volume_level%10);

}
/*void Ui_DisPlay_Page4Column0(ui16 index)
{
	uc_lcd_page_num = 4;
	uc_lcd_column_num = 0;
	Api_Display_Picture(index + ALL_MENU_NUMBER * uc_language_idx);
	Ui_Delay500ms();
}	*/

void Ui_Display_MusicFreq(void)
{
	uc_lcd_column_num = 56;
	uc_lcd_page_num = 6;
	Api_Display_Picture(PIC_MUSIC_FREQ_BASE_IDX + uc_refresh_pic_num);
}

/*******************************************************************************
display system contrast
********************************************************************************/
void Ui_Display_Sys_Contrast(void)
{
	//if((uc_lcd_dis_flg & BIT4_MASK) != 0)
	{
		uc_lcd_page_num = 2;
		uc_lcd_column_num = 0;
		Api_Display_Picture(PIC_SYS_BKGRADE_BASE_IDX+uc_menu_cur_idx-1);
	}
}

void Ui_Display_BookName(uc8 idx)
{
	uc_lcd_page_num = 0;
	uc_lcd_column_num = 0;
	Api_Display_Picture(0x4240+idx-1);
}

#endif