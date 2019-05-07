/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_music_main.c
Creator: zj.zong					Date: 2009-04-23
Description:
Others:
Version: V0.1
History:
	V0.1	2009-04-23		zj.zongs
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_MUSIC_MAIN_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Music\ui_music_header.h"
#include ".\UI\UI_Music\ui_music_var_ext.h"
extern void Ui_ProcessEvt_Key(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ui_ProcessEvt_Display(void);
extern void Ui_Display_NoFile(void);

extern void Ap_ProcessEvt_Bat(void);
extern void Ap_ProcessEvt_Usb(void);
extern void Ap_ProcessEvt_Msi(void);
extern void Ap_ProcessEvt_FileEnd(void);
extern void Ap_ProcessEvt_FileError(void);
extern void Ap_ProcessEvt_AB_Replay(void);
extern void Ap_ProcessEvt_FillDecBuf(void);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);

extern void Ap_Check_Init_FlashSD(void);
extern void Ap_Enable_IrqBit(void);
extern void Api_Mpeg_Ena_AudioDec(void);
extern void Ap_SwitchTo_MainMenu(void);
extern void Ui_ReturnTo_MenuSD(void);

extern bit  Ap_Init_MP3_Info(void);
extern void Api_Mpeg_Ena_AudioDec(void);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);
extern void Ap_Enable_IrqBit(void);
extern void Ap_MpegState_Stop(void);
extern void Ap_SetMP3_Play(void);
extern void Ap_SetMP3_Pause(void);
extern void Ap_Mpeg_ExitReset();
void rtc_init(uc8 init_flg);
extern void rtc_read_all(void);
extern void check_led(void);
/*******************************************************************************
Function: Ui_Music_Main()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Music_Main(void)
{
	mReset_PC_Stack;			//reset PC STACK
	mReset_WatchDog;			//reset watchdog

	b_bat_low_flag = 0;
	uc_sys_leddis_cnt = 0;
	uc_sys_led_cunter = 0;
	uc_Setled_flg = 1;
	uc_Flashled_flg = 1;
	ui_Flashled_Count = 0;
	b_mpeg_init_ok = 0;
	Api_Mpeg_Ena_AudioDec();

	Ap_Enable_IrqBit();   		//enable interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq,USB Detect irq)
	
	uc_dynamicload_idx = 40;		//init variable, restore parameter and init dac	
	Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);		
//	uc_disktype_flg = 0x81;
			
	Ap_Check_Init_FlashSD();			//check if init sd ok
	Ap_Mpeg_ExitReset();		  	//make the mpeg core exit the reset state

	if((MR_PBDATA & MODE1KEY_MASK) != 0)   // Mode1 有效……
	{
	//	if(b_fb_nofile_flg == 0)
	//	{
			
			if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
			{
				Ap_SetMP3_Play();		  //if file is ok, and start play
				//Ap_SetMP3_Pause();
			}
	 /*   }
		else
		{
			//Ui_Display_NoFile();
			//if(b_fb_nofile_flg == 1)
			{
				uc_disktype_flg = 0x00; 
				uc_dynamicload_idx = 30;
				Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);
	
				uc_dynamicload_idx = 41;
				Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);
				if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
				{
					Ap_SetMP3_Play();		  //if file is ok, and start play
				}
	     	}
		}*/
	}
	while(1)
	{
		mReset_WatchDog;					//clr watchdog
				
		check_led();	   // Jason delete... Function Date 2012-12-12
		if((ui_sys_event & EVT_USB) != 0)
		{
			ui_sys_event &= ~EVT_USB;
			Ap_ProcessEvt_Usb();			//plug usb , and enter usb mode
		}

		
		else if((ui_sys_event & EVT_MSI) != 0)
		{
			ui_sys_event &= ~EVT_MSI;
			Ap_ProcessEvt_Msi();						//insert sd or draw sd
		}

		else if((ui_sys_event & EVT_KEY) != 0)
		{
			ui_sys_event &= ~EVT_KEY;
		   	if((MR_PBDATA & MODE1KEY_MASK) != 0)   // Mode1 有效……
			{
				Ui_ProcessEvt_Key();			//process key event
			}
		}

		else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;  // process display
		/*	mSet_MCUClock_33M;
			Ui_ProcessEvt_Display();
			mSet_MCUClock_12M;	*/
		}

		else if((ui_sys_event & EVT_MUSIC_FILE_ERR) != 0)
		{
			ui_sys_event &= ~EVT_MUSIC_FILE_ERR;
			if(b_mp3_file_err == 1)
			{
				Ap_ProcessEvt_FileError();	//decoder error
			}
		}

		else if((ui_sys_event & EVT_MUSIC_FILE_END) != 0)
		{
			ui_sys_event &= ~EVT_MUSIC_FILE_END;
			if(b_mp3_file_end == 1)
			{
				Ap_ProcessEvt_FileEnd();	//file end
			}
		}
		
		else if((ui_sys_event & EVT_MUSIC_FILL_DECBUF) != 0)
		{
			ui_sys_event &= ~EVT_MUSIC_FILL_DECBUF;
			Ap_ProcessEvt_FillDecBuf();		//fill decoder buffer
		}

		/*else if((ui_sys_event & EVT_MUSIC_AB_REPLAY) != 0)
		{
			ui_sys_event &= ~EVT_MUSIC_AB_REPLAY;
			Ap_ProcessEvt_AB_Replay();		//A-B replay
		}*/

		else if((ui_sys_event & EVT_TIMER) != 0)
		{
			ui_sys_event &= ~EVT_TIMER;
			Ui_ProcessEvt_Timer();			//20ms timer
		}

		else if((ui_sys_event & EVT_BATTERY) != 0)
		{
			ui_sys_event &= ~EVT_BATTERY;
			Ap_ProcessEvt_Bat();			//process adc data (battery in)
		}

		

		if(uc_mpeg_state == MPEG_STATE_ERROR)
		{	
		  	Ap_MpegState_Stop();
			b_mp3_file_end = 1;
			ui_sys_event |= EVT_MUSIC_FILE_END;		//clr file end event and flag
		}
	}
}

#endif