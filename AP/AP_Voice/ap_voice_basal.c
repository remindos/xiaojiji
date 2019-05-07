/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_voice_basal.c
Creator: zj.zong					Date: 2009-05-22
Description: subroutine with communicant of muc and mpeg
Others:
Version: V0.1
History:
	V0.1	2009-05-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_VOCIE_BASAL_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Voice\ui_voice_header.h"
#include "..\..\UI\UI_Voice\ui_voice_var_ext.h"

extern bit  Ui_Init_PlayWav(void);


extern void Ap_ProcessEvt_FillDecBuf(void);
extern void Ap_SetDaci_SampleRate(void);
extern void Ap_SetDac_CancelMute(void);

extern void Api_Command_ResetDec_Wav(void);
extern void Api_Command_ResetDec_Wav_AB(void);
extern void Api_Command_StartDec_Wav(void);
extern void Api_Command_PauseDec(void);
extern void Api_Command_StopDec(void);
extern void Api_Set_Volume(uc8 volume_level);
extern void Api_Command_ContinueDec(void);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Ui_Display_NoFile(void);
extern void Api_Delay_1ms(uc8 time_cnt);
extern void Api_DynamicLoad_Voice_Target(uc8 load_code_idx);


void Ap_Enable_IrqBit(void);
void Ap_Mpeg_ExitReset(void);
void Ap_Check_McuMpeg_State(void);
void Ap_Wait_MpegState(void);	 
void Ap_SetWav_Play(void);
void Ap_SetWav_Stop(void);
void Ap_Exit_VoiceMode(void);
void Ap_Disable_DAC(void);
void Ap_Check_MpegState_Cnt(void);
void Ap_MpegState_Stop(void);
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
 	b_mpeg_init_ok = 0;
	MR_MODEN2 |= 0x10;						//enable mpeg core	

 	MR_IE0 = 0x01;						//enable USB Detect irq
	MR_IE1 = 0x3f;						//enable mcu interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq, MSI)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}


/*******************************************************************************
Function: Ap_Mpeg_ExitReset()
Description: start play
Calls:
Global:uc_voice_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Mpeg_ExitReset(void)
{
	mMPEG_Exit_Rest;						//make the mpeg core exit the reset state
	while(1)
	{
		mReset_WatchDog;					//wait for mpeg ready
		if(b_mpeg_init_ok == 1)
		{
			b_mpeg_init_ok = 0;
			break;
		}
	}
}

/*******************************************************************************
Function: Ap_SetWav_Play()
Description: start play
Calls:
Global:uc_wav_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetWav_Play(void)
{
	Ap_Check_McuMpeg_State();
	uc_voice_status = STATE_PLAY;				//set current play state

	mMPEG_Occupy_DbufCbuf;					//mpeg apply dbuf(0x5000) & cbuf
	if(b_voice_dec_reset_flg == 0)
	{
		b_voice_dec_reset_flg = 1;			//for reset A-B and forward&backward

		Ap_SetDaci_SampleRate();	//set dac sample rate
		
		Api_Command_ResetDec_Wav();			//reset decode
	}
	else
	{
		Api_Command_ResetDec_Wav_AB();		//send command to mpeg, reset A-B or backward&forward
	}

	Api_Command_StartDec_Wav();				//send command to mpeg, start decode

	Api_Set_Volume(uc_voice_volume_level);	//set volume

	Ap_SetDac_CancelMute();			//cancel mute dac


#ifdef AVOID_POP_NOISE
	b_voice_startplay_flg = 1;
	uc_voice_startplay_cnt = 0;
#else
	
	if((MR_DACCLK & 0x04) != 0)
	{
		MR_DACCLK &= 0xfb;				//Enable DAC reference voltage
	}
#endif
}


/*******************************************************************************
Function: Ap_SetWav_Continue()
Description: continue play
Calls:
Global:uc_wav_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetWav_Continue(void)
{
	if(uc_mpeg_state == STATE_STOP)			//file end
	{	
		b_voice_file_end = 1;
		ui_sys_event |= EVT_VOICE_FILE_END;	//clr file end event and flag
		return;
	}

	Ap_Check_McuMpeg_State();

	uc_voice_status = STATE_PLAY;				//set current play state
	mMPEG_Occupy_DbufCbuf;
	
	Api_Command_ContinueDec();				//send command to mpeg, continue decode

	Api_Set_Volume(uc_voice_volume_level);	//set volume

	Ap_SetDac_CancelMute();			//cancel mute dac
}


/*******************************************************************************
Function: Ap_SetWav_Pause()
Description: pause play
Calls:
Global:uc_wav_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetWav_Pause(void)
{
	Ap_Check_McuMpeg_State();

	uc_voice_status = STATE_PAUSE;			//set current play state

	Api_Command_PauseDec();					//send command to mpeg, pause decode

	uc_mpegstate_cnt = 0;
	while((uc_mpeg_state != STATE_PAUSE) && (uc_mpeg_state != STATE_STOP) && (uc_mpeg_state != MPEG_STATE_ERROR))	//wait for mpeg enter pause or stop mode
	{
		Ap_Wait_MpegState();
	}

	Api_Set_Volume(0);						//set volume is 0
	mMPEG_Release_Dbuf;	
}

/*******************************************************************************
Function: Ap_SetWav_Stop()
Description: stop play
Calls:
Global:uc_wav_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetWav_Stop(void)
{ 	
	Ap_Check_McuMpeg_State();
	
	uc_voice_status = STATE_STOP;			//set current play state
	Api_Command_StopDec();					//send command to mpeg, stop decode

	uc_mpegstate_cnt = 0;
	while((uc_mpeg_state != STATE_STOP) && (uc_mpeg_state != MPEG_STATE_ERROR))		//wait for mpeg enter stop mode
	{
		Ap_Wait_MpegState();
	}

	Api_Set_Volume(0);						//set volume is 0

	mMPEG_Release_Dbuf;	
}


/*******************************************************************************
Function: Ap_Check_McuMpeg_State()
Description: check mpeg and mcu state
             1. mcu, not check STATE_FORWARD or STATE_BCKWARD
			 2. mcu STATE_PAUSE <--> mpeg STATE_PAUSE or STATE_STOP
			 3. mcu STATE_PLAY  <--> mpeg STATE_PLAY
			 4. mcu STATE_STOP  <--> mpeg STATE_STOP
Calls:
Global:uc_wav_status, uc_mpeg_state
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_McuMpeg_State(void)
{
	uc_mpegstate_cnt = 0;
	if((uc_voice_status != STATE_FORWARD) && (uc_voice_status != STATE_BCKWARD))
	{
		if((uc_voice_status != STATE_PAUSE) || (uc_voice_status == STATE_PLAY))
		{
			while((uc_voice_status != uc_mpeg_state) && (uc_mpeg_state != MPEG_STATE_ERROR))
			{
				Ap_Wait_MpegState();
			}
		}
	}	
}


void Ap_Wait_MpegState(void)
{
	mReset_WatchDog;

	if(b_voice_file_err == 1)
	{
		uc_voice_status = STATE_STOP;			//set current play state
		b_voice_file_err = 0;
		ui_sys_event &= ~EVT_VOICE_FILE_ERR;//clr file err event and flag
	}
	
	if((ui_sys_event & EVT_VOICE_FILL_DECBUF) != 0)
	{
		uc_mpegstate_cnt = 0;
		ui_sys_event &= ~EVT_VOICE_FILL_DECBUF;
		Ap_ProcessEvt_FillDecBuf();			//dbuf(0x5000) be filled with invalid data 

		if(b_voice_file_end == 1)
		{
			uc_voice_status = STATE_STOP;			//set current play state
			b_voice_file_end = 0;		
			ui_sys_event &= ~EVT_VOICE_FILE_END;//clr file end event and flag
		}
	}
	
	if((ui_sys_event & EVT_TIMER) != 0)
	{
		ui_sys_event &= ~EVT_TIMER;
		Ap_Check_MpegState_Cnt();		//20ms timer
	}			
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
	Ap_Exit_VoiceMode();
	
	Api_DynamicLoad_MainMenu();	  //enter main menu
}

/********************************************************************************/
 void Ap_NoFileTo_MainMenu(void)
{
	Ui_Display_NoFile();
	Ap_SwitchTo_MainMenu();				//return to main menu
}


 /*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: enter power off mode
Calls:
Global:uc_poweroff_type
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_VoiceMode();
	Api_DynamicLoad_PowerOff();
}

/*******************************************************************************
Function: Ap_Exit_VoiceMode()
Description: save voice parameter into flash
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Exit_VoiceMode(void)
{
	Ap_SetWav_Stop();			//stop play
	mMPEG_Disable;					//disable mpeg module

 #ifdef PCB_WITH_SPEAKER
//	MR_PADATA &= 0xfd;								//pa1 = 0
	Api_Delay_1ms(250);
	Api_Delay_1ms(250);
#endif
	Ap_Disable_DAC();		//power off DAC

	Api_DynamicLoad_Voice_Target(CODE_VOICE_7_SAVE_IDX);  //save voice parameter function
}

/*******************************************************************************
Function: Ap_Disable_DAC()
Description: power off DAC
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Disable_DAC(void)
{
#ifdef AVOID_POP_NOISE
	b_voice_startplay_flg = 0;
#endif
	MR_DACCLK |= 0x04;								//set bit2, Vrefdac is grounded
	Api_Delay_1ms(100);								//delay 100ms
	MR_DACPCON = 0;									//DAC all power off
	Api_Delay_1ms(100);								//delay 100ms
	MR_DACI = 0x00;									//clr DAC "go" bit, and disable IRQ

	MR_MODEN1 &= 0xfe;								//Disable DAC Mode
}

/*******************************************************************************
Function: Ap_Play_Next_file)
Description: paly next file
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Play_Next_file(void)
{
	uc_dynamicload_idx = 37;		  		// bit1,bit2
	Api_DynamicLoad_Voice_Target(CODE_VOICE_4_SEL_FILE_IDX); 	//sel next file
   	Api_DynamicLoad_Voice_Target(CODE_VOICE_3_INIT_PLAY_IDX);	 //danamic load voice init code
	if(Ui_Init_PlayWav() == 1)
	{
		if(b_voice_get_next_file == 1)
		{
			Ap_SetWav_Play();					//start play
		}
	}
}

/*******************************************************************************
Function: Ap_SetDac_CancelMute()
Description: enable the digital mute for the (left, right) master
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetDac_CancelMute(void)
{
	b_dac_mute_flg = 0;
	MR_DACSET &= 0x3f;
}

/*******************************************************************************
Function: Ap_Check_MpegState_Cnt()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_MpegState_Cnt(void)
{
	uc_mpegstate_cnt++;
	if(uc_mpegstate_cnt >= 150)		//20ms*150=3s
	{
		uc_mpegstate_cnt = 0;
		Ap_MpegState_Stop();
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
		Api_DynamicLoad_Voice_Target(CODE_VOICE_9_SWITCH_CARD_IDX);				

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
/*******************************************************************************
Function: Ap_MpegState_Stop()
Description:
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_MpegState_Stop(void)
{
	mMPEG_Enter_Rest;
	Api_Delay_1ms(1);
	Ap_Mpeg_ExitReset();

	uc_voice_status = STATE_STOP;
	uc_mpeg_state = STATE_STOP;
}


/*******************************************************************************
Function: Ap_SetDaci_SampleRate()
Description: set dac sample rate for bit0~3
Calls:
Global: uc_daci_sr
Input:  Null
Output: Null
Others:
********************************************************************************/
void Ap_SetDaci_SampleRate(void)
{
	MR_DACI &= 0xf0;
	MR_DACI |= uc_daci_sr;							//set DACI sample rate
	
	if(uc_daci_sr <= 6)
	{
		MR_DACSET |= 0x10;							//bit4=1, set 5rd-order noise shaper
	}
	else
	{
		MR_DACSET &= 0xef;							//bit4=0, set 3rd-order noise shaper
	}	
}

/*******************************************************************************
Function: Ap_Forward_Backward_Over()
Description: 
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

void Ap_Forward_Backward_Over(void)
{
	if((uc_voice_status == STATE_FORWARD) || (uc_voice_status == STATE_BCKWARD))
	{
		uc_dynamicload_idx = 30;		  		// bit2
		Api_DynamicLoad_Voice_Target(CODE_VOICE_5_FSEEK_IDX);	//fseek according to byte offset

		if(b_voice_file_end == 0)			//ok
		{
			Ap_SetWav_Play();
			uc_lcd_dis_flg |= 0x10;					//display state
		}
		else										//file end
		{
			ui_sys_event |= EVT_VOICE_FILE_END;
		}
	}
}


#endif