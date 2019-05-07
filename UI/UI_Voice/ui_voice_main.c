/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_voice_main.c
Creator: zj.zong					Date: 2009-05-21
Description: voice main loop(wait for interrupt setting ui_sys_event, and process event)
Others:
Version: V0.1
History:
	V0.1	2009-05-21		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef UI_VOICE_MAIN_DEF

#include "sh86278_api_define.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include ".\UI\UI_Voice\ui_voice_header.h"
#include ".\UI\UI_Voice\ui_voice_var_ext.h"

extern void Ui_Display_NoFile(void);
extern void Ui_ReturnTo_MenuSD(void);
extern void Ui_ProcessEvt_Key(void);
extern void Ui_ProcessEvt_Timer(void);
extern void Ui_ProcessEvt_Display(void);
extern bit  Ui_Init_PlayWav(void);

extern void Ap_ProcessEvt_Bat(void);
extern void Ap_ProcessEvt_Usb(void);
extern void Ap_ProcessEvt_FileEnd(void);
extern void Ap_ProcessEvt_FileError(void);
extern void Ap_ProcessEvt_ABReplay(void);
extern void Ap_ProcessEvt_FillDecBuf(void);
extern void Ap_Enable_IrqBit(void);
extern void Ap_MpegState_Stop(void);


extern void Ap_SetWav_Play(void);
extern void Ap_Mpeg_ExitReset(void);
extern void Ap_SwitchTo_MainMenu(void);

extern void Api_Mpeg_Ena_VoiceAudioDec(void);
extern void Api_DynamicLoad_Voice_Target(uc8 load_code_idx);
extern void Api_Delay_1ms(uc8 uc_num_cnt);
extern void	Ap_ProcessEvt_Msi(void);
extern void	Ap_Check_Init_FlashSD(void);


/*******************************************************************************
Function: Ui_Voice_Main()
Description: voice main function
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ui_Voice_Main(void)
{

	mReset_PC_Stack;			//reset PC STACK
	mReset_WatchDog;			//reset watchdog
	mSet_MCUClock_12M;

	b_mpeg_init_ok = 0;
	Api_Mpeg_Ena_VoiceAudioDec();//load code into mpeg CM Space,data into cbuf,enable mpeg core

	Ap_Enable_IrqBit();   		//enable interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq,USB Detect irq)
				
//	Api_DynamicLoad_Voice_Target(CODE_VOICE_1_INIT1_IDX);	 //danamic load voice init code
//	Api_DynamicLoad_Voice_Target(CODE_VOICE_2_INIT2_IDX);	 //danamic load voice init code
	Ap_Check_Init_FlashSD();

	if(b_fb_nofile_flg == 0)
	{
		Ap_Mpeg_ExitReset();		  //make the mpeg core exit the reset state
		
		Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);	 //danamic load voice init code
	
		if(Ui_Init_PlayWav() == 1)		  //get wav file FS, BR, file name...
		{
			Ap_SetWav_Play();		  //if file is ok, and start play
		}
	}
	/*else									//no file
	{
		mMPEG_Disable;						//disable mpeg module
	}  */
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
	Ui_ProcessEvt_Display();				//display voice main window(first display)
	ui_sys_event &= ~EVT_DISPLAY;
	ui_sys_event &= ~EVT_KEY;				//clr key event, it is invalid before first display

	while(1)
	{
		mReset_WatchDog;					//clr watchdog

		if((ui_sys_event & EVT_USB) != 0)
		{
			ui_sys_event &= ~EVT_USB;
			Ap_ProcessEvt_Usb();			//plug usb , and enter usb mode
		}
	   	else if((ui_sys_event & EVT_MSI) != 0)
		{
			ui_sys_event &= ~EVT_MSI;
			Ap_ProcessEvt_Msi();			//insert sd or draw sd
		}

		else if((ui_sys_event & EVT_KEY) != 0)
		{
			ui_sys_event &= ~EVT_KEY;
			Ui_ProcessEvt_Key();			//process key event
		}

		else if((ui_sys_event & EVT_VOICE_FILE_ERR) != 0)
		{
			ui_sys_event &= ~EVT_VOICE_FILE_ERR;
			if(b_voice_file_err == 1)
			{
				Ap_ProcessEvt_FileError();	//decoder error
			}
		}

		else if((ui_sys_event & EVT_VOICE_FILE_END) != 0)
		{
			ui_sys_event &= ~EVT_VOICE_FILE_END;
			if(b_voice_file_end == 1)
			{
				Ap_ProcessEvt_FileEnd();	//file end
			}
		}
		
		else if((ui_sys_event & EVT_VOICE_FILL_DECBUF) != 0)
		{
			ui_sys_event &= ~EVT_VOICE_FILL_DECBUF;
			Ap_ProcessEvt_FillDecBuf();		//fill decoder buffer
		}

		else if((ui_sys_event & EVT_VOICE_AB_REPLAY) != 0)
		{
			ui_sys_event &= ~EVT_VOICE_AB_REPLAY;
			if(uc_voice_replay_ab_flg == 3)
			{
				Ap_ProcessEvt_ABReplay();	//A-B replay
			}
		}

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

		else if((ui_sys_event & EVT_DISPLAY) != 0)
		{
			ui_sys_event &= ~EVT_DISPLAY;
			Ui_ProcessEvt_Display();		//dislay function
		}

		if(uc_mpeg_state == MPEG_STATE_ERROR)
		{	
		  	Ap_MpegState_Stop();
			b_voice_file_end = 1;
			ui_sys_event |= EVT_VOICE_FILE_END;		//clr file end event and flag
		}
	}
}



#endif