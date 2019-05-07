/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_music_basal.c
Creator: zj.zong					Date: 2009-04-22
Description: subroutine with communicant of muc and mpeg
Others:
Version: V0.1
History:
	V0.1	2009-04-22		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_MUSIC_BASAL_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Music\ui_music_header.h"
#include "..\..\UI\UI_Music\ui_music_var_ext.h"

extern void Ui_ReturnTo_Music(void);
extern void Ui_SetLcdFlag_0xfd(void);

extern void Ap_ProcessEvt_FillDecBuf(void);
extern void Ap_SetDaci_SampleRate(void);
extern void Ap_SetDac_CancelMute(void);

extern void Api_Command_ContinueDec(void);
extern void Api_Command_ResetDec_MP3(void);
extern void Api_Command_ResetDec_MP3_AB(void);
extern void Api_Command_StartDec_mp3(void);
extern void Api_Command_PauseDec(void);
extern void Api_Command_StopDec(void);
extern void Api_Command_Ena_Fade(void);
extern void Api_Command_Set_EQ(uc8 eq_mode);
extern void Api_Command_Dec_MP3Header(void);
extern void Api_Cmd_Dbuf1_Filled_InvalidData(void);
extern void Ap_Exit_MusicMode(void);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_Delay_1ms(uc8 time_cnt);
extern void _media_readsector(void);
//extern void command_read_mpeg_reg(ui16 cmd_para);

extern bit FS_Fread(void);
extern bit FS_Fopen_Appointed(ui16 file_idx);
extern void Api_Set_Volume(uc8 volume_level);
extern void Api_Command_Set_EQ(uc8 eq_mode);
extern void Api_DynamicLoad_Music_Target(uc8 load_code_idx);

void Ap_Check_MpegState_Cnt(void);
void Ap_MpegState_Stop(void);
void Ap_Mpeg_ExitReset(void);
void Ap_Check_McuMpeg_State(void);
void Ap_Wait_MpegState(void);
uc8 Ap_Get_TimeLabel(void);
void Ap_Get_Lrc_LabelBuf(uc8 label_idx);
uc8 Ap_Get_TimeLabel(void);
bit Ap_Check_SDExist(void);

#ifdef SP_PLAY_RESERVEDMP3_DEF
void Ap_Rsv_Fill_Debuf_Firstly(void);
void Ap_Rsv_InitData(void);
#endif
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
	MR_MODEN2 |= 0x10;					//enable mpeg core	
		
 	MR_IE0 = 0x01;						//enable USB Detect irq
	MR_IE1 = 0x7F;						//enable mcu interrupt(mpeg cmd & special1 & flag irq, timer irq, ADC irq)
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}


/*******************************************************************************
Function: Ap_Check_EQ_Window()
Description: restore eq
Calls:
Global:uc_mp3_eq_mode
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_EQ_Window(void)
{	
	if(uc_window_no == WINDOW_MUSIC_MENU_EQ)	//restore eq
	{
		Api_Command_Set_EQ(uc_menu_cur_idx - 1);
	} 	
}

 	
/*******************************************************************************
Function: Ap_Restore_EQ()
Description: restore eq
Calls:
Global:uc_mp3_eq_mode
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Restore_EQ(void)
{	
	if(uc_window_no == WINDOW_MUSIC_MENU_EQ)	//restore eq
	{
		Api_Command_Set_EQ(uc_mp3_eq_mode);
	} 	
}


/*******************************************************************************
Function: Ap_Mpeg_ExitReset()
Description: start play
Calls:
Global:uc_mp3_status
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
	
	Api_Command_Set_EQ(uc_mp3_eq_mode);		//set eq
	Api_Command_Ena_Fade();					//enable volume fade-in and fade-out	
}


/*******************************************************************************
Function: Ap_SetMP3_Play()
Description: start play
Calls:
Global:uc_mp3_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetMP3_Play(void)
{
	b_dac_ws_flg = 0;

	Ap_Check_McuMpeg_State();
	uc_mp3_status = STATE_PLAY;				//set current play state
	mMPEG_Occupy_DbufCbuf;					//mpeg occupy DBUF & CBUF

	if(b_mp3_dec_reset_flg == 0)
	{
		b_mp3_dec_reset_flg = 1;			//for reset A-B and forward&backward

		Ap_SetDaci_SampleRate();			//set dac sample rate
	}
	else
	{
		Api_Command_ResetDec_MP3_AB();		//send command to mpeg, reset A-B or backward&forward
	}
	
	Api_Command_StartDec_mp3();				//send command to mpeg, start decode

	Api_Set_Volume(uc_mp3_volume_level);

	Ap_SetDac_CancelMute();

#ifdef AVOID_POP_NOISE
	b_mp3_startplay_flg = 1;
	uc_mp3_startplay_cnt = 0;
#else
	if((MR_DACCLK & 0x04) != 0)
	{
		MR_DACCLK &= 0xfb;				//Enable DAC reference voltage
	}
#endif
}



/*******************************************************************************
Function: Ap_SetMP3_Continue()
Description: continue play
Calls:
Global:uc_mp3_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetMP3_Continue(void)
{
	b_dac_ws_flg = 0;

	if(uc_mpeg_state == STATE_STOP)				//file end
	{
		b_mp3_file_end = 1;
		ui_sys_event |= EVT_MUSIC_FILE_END;		//clr file end event and flag
		return;
	}

	Ap_Check_McuMpeg_State();
	
	uc_mp3_status = STATE_PLAY;				//set current play state
	
	Api_Command_ContinueDec();					//send command to mpeg, continue decode
	
	Api_Set_Volume(uc_mp3_volume_level);

	Ap_SetDac_CancelMute();		
}


/*******************************************************************************
Function: Ap_SetMP3_Pause()
Description: pause play
Calls:
Global:uc_mp3_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetMP3_Pause(void)
{
	ul_mp3_byte_offset_bk = ul_fs_byte_offset;	//save file byte offset
	
	Ap_Check_McuMpeg_State();
	uc_mp3_status = STATE_PAUSE;				//set current play state

	Api_Command_PauseDec();						//send command to mpeg, pause decode
	uc_mpegstate_cnt = 0;
 	while((uc_mpeg_state != STATE_PAUSE) && (uc_mpeg_state != STATE_STOP) && (uc_mpeg_state != MPEG_STATE_ERROR))	//wait for mpeg enter pause or stop mode
	{
		Ap_Wait_MpegState();
	}
	
	Api_Set_Volume(0);							//set volume is 0	
}

/*******************************************************************************
Function: Ap_SetMP3_Stop()
Description: stop play
Calls:
Global:uc_mp3_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetMP3_Stop(void)
{
	ul_mp3_byte_offset_bk = ul_fs_byte_offset;	//save file byte offset
	
	Ap_Check_McuMpeg_State();
	uc_mp3_status = STATE_STOP;				//set current play state
	Api_Command_StopDec();						//send command to mpeg, stop decode, and volume fade out
	uc_mpegstate_cnt = 0;
	while((uc_mpeg_state != STATE_STOP) && (uc_mpeg_state != MPEG_STATE_ERROR))		//wait for mpeg enter stop mode
	{
		Ap_Wait_MpegState();
	}
	Api_Set_Volume(0);							//set volume is 0

	mMPEG_Release_Dbuf;							//release DBUF to MCU	
}

/*******************************************************************************
Function: Ap_Check_McuMpeg_State()
Description: check mpeg and mcu state
             1. mcu, not check STATE_FORWARD or STATE_BCKWARD
			 2. mcu STATE_PAUSE <--> mpeg STATE_PAUSE or STATE_STOP
			 3. mcu STATE_PLAY  <--> mpeg STATE_PLAY
			 4. mcu STATE_STOP  <--> mpeg STATE_STOP
Calls:
Global:uc_mp3_status, uc_mpeg_state
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Check_McuMpeg_State(void)
{
	uc_mpegstate_cnt = 0;
	if((uc_mp3_status != STATE_FORWARD) && (uc_mp3_status != STATE_BCKWARD))
	{
		if((uc_mp3_status != STATE_PAUSE) || (uc_mpeg_state == STATE_PLAY))
		{
			while((uc_mp3_status != uc_mpeg_state) && (uc_mpeg_state != MPEG_STATE_ERROR))
			{
				Ap_Wait_MpegState();
			}
		}
	}	
}


void Ap_Wait_MpegState(void)
{
	mReset_WatchDog;
	
	if(b_mp3_file_err == 1)
	{
		uc_mp3_status = STATE_STOP;				//set current play state
		b_mp3_file_err = 0;
		ui_sys_event &= ~EVT_MUSIC_FILE_ERR;	//clr file err event and flag
	}

	if((ui_sys_event & EVT_MUSIC_FILL_DECBUF) != 0)
	{
		uc_mpegstate_cnt = 0;
		ui_sys_event &= ~EVT_MUSIC_FILL_DECBUF;
		Ap_ProcessEvt_FillDecBuf();				//dbuf1 be filled with invalid data

		if(b_mp3_file_end == 1)
		{
			uc_mp3_status = STATE_STOP;				//set current play state
			b_mp3_file_end = 0;
			ui_sys_event &= ~EVT_MUSIC_FILE_END;//clr file end event and flag
		}
	}
	
	if((ui_sys_event & EVT_TIMER) != 0)
	{
		ui_sys_event &= ~EVT_TIMER;
		Ap_Check_MpegState_Cnt();		//20ms timer
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
	b_sd_exist_flg = 0;
	return 0;
/*	if(((MR_MSCON4 & 0x80)^((uc_user_option2 & 0x04)<<5)) == 0)
	{		
		return 0;
	}
	
	if(uc_msi_status != 0)
	{
		Api_Delay_1ms(100);
		uc_dynamicload_idx = 31;		//init SD/MMC
		Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);		

		if(uc_msi_status != 0)			//init SD/MMC card fail
		{
			b_sd_exist_flg = 0;
			return 0;
		}
		else							//init SD/MMC card ok
		{
			b_sd_exist_flg = 1;
			return 1;
		}
	}
	else
	{
		b_sd_exist_flg = 1;
		return 1;	
	}	*/
}

/*******************************************************************************
Function: Ap_Init_MP3_Info()
Description: init play, get ID3 info, Process LRC, Get file name...
			 return bit 1: init ok
           				0: init error
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/

bit Ap_Init_MP3_Info(void)
{
	mSet_MCUClock_33M;
	uc_dynamicload_idx = 53;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//init play mp3 var
#ifdef SP_PLAY_RESERVEDMP3_DEF
	if(b_rsv_mp3_flg == 1)
	{
		//uc_dynamicload_idx = 53;
		//Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//init play mp3 var

 		Ap_Rsv_Fill_Debuf_Firstly();
	}
	else
#endif
	{
	Api_DynamicLoad_Music_Target(CODE_MUSIC_2_GETID3_IDX);//get mp3 id3, get vbr total frame number								
	Api_DynamicLoad_Music_Target(CODE_MUSIC_3_PROID3_IDX);

	uc_dynamicload_idx = 3;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_6_GET_ITEM_NAME_IDX);//process music file name and check lrc file existed

	if(b_lrc_file_exist == 1)
	{
		mMPEG_Enter_Rest;		
		Api_DynamicLoad_Music_Target(CODE_MUSIC_4_LRC_IDX);
		Ap_Mpeg_ExitReset();				//make the mpeg core exit the reset state
	}
	
	uc_dynamicload_idx = 46;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);		//open current mp3 file

	//wang
/*	 
	mMPEG_Enter_Rest;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_A_SAVE_PARA_IDX);	
	Ap_Mpeg_ExitReset();
*/
	//wang

	uc_dynamicload_idx = 40;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);					
	}
	mMPEG_Occupy_DbufCbuf;						//mpeg occupy DBUF1 & DBUF0 & CBUF
	Api_Command_ResetDec_MP3();					//reset decode mp3
	Api_Command_Dec_MP3Header();				//inform mpeg to start parse header
	
	while(1)
	{
		mReset_WatchDog;						//decoding mp3 header 5s

		if(b_mp3_file_err == 1)
		{
			ui_sys_event |= EVT_MUSIC_FILE_ERR;
			mMPEG_Occupy_Dbuf1;					//mpeg occupy DBUF(0x5000)
			Api_Cmd_Dbuf1_Filled_InvalidData();
			break;								//file error, and get next mp3 file
		}
		
		if(b_mpeg_dec5s_ok == 1)				//mpeg decode header ok
		{
			break;
		}
		
		if((ui_sys_event & EVT_MUSIC_FILL_DECBUF) != 0)
		{
			ui_sys_event &= ~EVT_MUSIC_FILL_DECBUF;
			Ap_ProcessEvt_FillDecBuf();			//fill decoder buffer
		}
	}
	b_mp3_file_end = 0;
	ui_sys_event &= ~EVT_MUSIC_FILE_END;		//clr file end event and flag
#ifdef SP_PLAY_RESERVEDMP3_DEF
	if(b_rsv_mp3_flg==1)
	{
		Ap_Rsv_InitData();
	}
	else
#endif
	{	
	if(b_mp3_file_err == 0)
	{
		uc_dynamicload_idx = 50;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);//get mp3 total played time
		
		uc_dynamicload_idx = 38;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);//check continue play?
	}

	uc_dynamicload_idx = 56;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_B_SUBFUNC_IDX);	//backup current mp3 file parameter for continue playing
	}

	mSet_MCUClock_12M;
	if(b_mp3_file_err == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*******************************************************************************
Function: Ap_SwitchTo_MainMenu()
Description: return to music browser
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
/*void Ap_SwitchTo_MainMenu(void)
{
	Ap_Exit_MusicMode();

	Api_DynamicLoad_MainMenu();
}*/

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
	Ap_Exit_MusicMode();
	
	Api_DynamicLoad_PowerOff();
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
	b_mp3_startplay_flg = 0;
#endif
	MR_DACCLK |= 0x04;								//set bit2, Vrefdac is grounded
	Api_Delay_1ms(100);								//delay 100ms
	MR_DACPCON = 0;									//DAC all power off
	Api_Delay_1ms(100);								//delay 100ms
	MR_DACI = 0x00;									//clr DAC "go" bit, and disable IRQ

	MR_MODEN1 &= 0xfe;								//Disable DAC Mode
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
	MR_DACSET &= 0x3f;
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

void Ap_InitLrc_FlashAddr(void)
{
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_occ_buf_flag = SFC_APP_FBUF;				//occupy ef common buffer(0x6600)
	buf_offset0 = (uc8)FCOMBUF_OFFSET_W;			//offset is 300 words(0x6600)
	buf_offset1 = (uc8)(FCOMBUF_OFFSET_W >> 8);
	sfc_reserved_area_flag = 1;						//read flash reserved area
	_media_readsector();							//read one sector to decbuf
	sfc_reserved_area_flag = 0;
}

/*******************************************************************************
Function: Ap_ProcessLrc()
Description: Process LRC
Calls:
Global:ui_mp3_cur_time
       b_mp3_lrc_flg--1:lrc have be updated
       ui_lrc_line_buf[70w]
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_ProcessLrc(void)
{
	uc8 i;
	if(b_lrc_file_exist == 0)
	{
		return;						//not existed lrc file or not dis lrc
	}

	i = Ap_Get_TimeLabel();		//get the cur time label index
	if(i != uc_lrc_curlabel_idx)
	{
		uc_lrc_curlabel_idx = i;
		Ap_Get_Lrc_LabelBuf(i);		//get the new lcr buffer
		b_mp3_lrc_flg = 1;
		uc_lcd_dis_flg |= 0x40;		//set bit6, display lrc
		ui_sys_event |= EVT_DISPLAY;
	}	
}

/*******************************************************************************
Function: Ap_Get_TimeLabel()
Description: search lrc time label according to the ui_mp3_cur_time
Calls:
Global:ui_mp3_cur_time, uc_lrc_label_num
Input: Null
Output:i--current lrc time label
Others:
********************************************************************************/
/*uc8 Ap_Get_TimeLabel(void)
{	
	uc8 i;
	
	for(i = 0; i < uc_lrc_label_num - 1; i++)
	{
		if(ui_mp3_cur_time < ui_lrc_time_buf[i])
		{
			break;
		}

		if((ui_mp3_cur_time >= ui_lrc_time_buf[i]) && (ui_mp3_cur_time < ui_lrc_time_buf[i+1])) 
		{		
			break;
		}
	}
	return i;
}*/
uc8 Ap_Get_TimeLabel(void)
{ 

	uc8 i, j=0, k, tmp_flg=0xFF;
	
	for(i = 0; i < uc_lrc_label_num - 1; i++)
	{
		if(uc_lrc_label_num > 100)
		{

			if((ui_sys_event & EVT_MUSIC_FILL_DECBUF) != 0)
			{
				ui_sys_event &= ~EVT_MUSIC_FILL_DECBUF;
				Ap_ProcessEvt_FillDecBuf(); //fill decoder buffer
			}

			if(i < 100)
			{
				if(tmp_flg == 0)
				{
					goto check_lrc_time;
				}
				j = 0;
			}
			else if(i < 200)
			{
				if(tmp_flg == 1)
				{
					goto check_lrc_time;
				}
				tmp_flg = 1;
				j = 100;
			}
			else
			{
				if(tmp_flg == 2)
				{
					goto check_lrc_time;
				}
				tmp_flg = 2;
				j = 200;
			}
			if(uc_lrc_label_num > 100)
			{
				sfc_logblkaddr0 = 0x64; //get lrc time buffer
				sfc_logblkaddr1 = 0x00;
				Ap_InitLrc_FlashAddr();
			}
			for(k=0; k<101; k++)
			{
				ui_lrc_time_buf[k] = ui_sfc_combuf[k+j];
			}
		}
check_lrc_time:
		if(ui_mp3_cur_time < ui_lrc_time_buf[i-j])
		{
			break;
		}
		if((ui_mp3_cur_time >= ui_lrc_time_buf[i-j]) && (ui_mp3_cur_time < ui_lrc_time_buf[i-j+1])) 
		{ 
			break;
		}
	}
	return i;
}


/*******************************************************************************
Function: Ap_Get_Lrc_LabelBuf()
Description: read lrc buffer to ui_lrc_line_buf[70]
		     0x0000 0001 ~ 0x0000 0064(addr of lrc buffer in flash)
Calls:
Global:ui_lrc_line_buf[70]
Input: label_idx(0~99)
Output:Null
Others:
********************************************************************************/
/*void Ap_Get_Lrc_LabelBuf(uc8 label_idx)
{
	uc8 i;
	
	sfc_logblkaddr0 = label_idx + LRC_FLASH_ADDRESS;
	sfc_logblkaddr1 = 0x00;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_FBUF;			//occupy ef common buffer(0x6600)
	buf_offset0 = (uc8)FCOMBUF_OFFSET_W;		//offset is 300 words(0x6600)
	buf_offset1 = (uc8)(FCOMBUF_OFFSET_W >> 8);
	_media_readsector();						//read one sector to decbuf
 	sfc_reserved_area_flag = 0;					//clr reserved area flag

	for(i = 0; i < LRC_ONE_LABEL_MAX; i++)
	{
		ui_lrc_line_buf[i] = 0;					//clr the buffer
	}

	for(i = 0; i < LRC_ONE_LABEL_MAX; i++)		//maximum is 70w
	{
		if(ui_sfc_combuf[i] == 0x00)
		{
			break;
		}
		else
		{
			ui_lrc_line_buf[i] = ui_sfc_combuf[i];//copy one word
		}
	}	
}*/
void Ap_Get_Lrc_LabelBuf(uc8 label_idx)
{
	uc8 i;
	sfc_logblkaddr0 = (uc8)((ui16)label_idx + LRC_FLASH_ADDRESS);
	sfc_logblkaddr1 = (uc8)(((ui16)label_idx + LRC_FLASH_ADDRESS)>>8);
	Ap_InitLrc_FlashAddr();

	for(i = 0; i < LRC_ONE_LABEL_MAX; i++)
	{
		ui_lrc_line_buf[i] = 0;					//clr the buffer
	}

	for(i = 0; i < LRC_ONE_LABEL_MAX; i++)		//maximum is 70w
	{
		if(ui_sfc_combuf[i] == 0x00)
		{
			break;
		}
		else
		{
			ui_lrc_line_buf[i] = ui_sfc_combuf[i];//copy one word
		}
	}	
}


/*******************************************************************************
Function:   Ap_SetStopWard()
Description: interface for initial module DAC in mode music, movie, voice
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_SetStopWard(void)
{
	uc_dynamicload_idx = 37;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_9_FSEEK_IDX);//check continue play?

	if(b_mp3_file_end == 0)				//ok
	{
		Ap_SetMP3_Play();
		uc_lcd_dis_flg |= 0x10;			//display state
	}
	else								//file end
	{
		ui_sys_event |= EVT_MUSIC_FILE_END;
	}
}

/*******************************************************************************
Function:   Ap_Play_SelFile_InFileBrowser()
Description: play this selected file in filebrowser
Calls:
Global:ui_mp3_idx_indir
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Play_SelFile_InFileBrowser(void)
{
	uc_dynamicload_idx = 47;
	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);	//open current mp3 file according to "mp3 index"
//	uc_dynamicload_idx = 44;
//	Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);	//get current file index in disk
		
	if(Ap_Init_MP3_Info() == 1)
	{
		Ap_SetMP3_Play();
	}
}

/*******************************************************************************
Function:   Ap_Save_Music_Parameter()
Description: save music parameter
Calls:
Global:b_mp3_save_flg
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Save_Music_Parameter(void)
{    mMPEG_Enter_Rest;       //allen
	ui_mp3_played_time = ui_mp3_cur_time;
	b_mp3_save_flg = 1;								//have saved music para
	Api_DynamicLoad_Music_Target(CODE_MUSIC_A_SAVE_PARA_IDX); //save music parameter
     Ap_Mpeg_ExitReset(); //allen
}

/*******************************************************************************
Function:   Ap_Check_Restore_LastFile()
Description: restore music parameter
Calls:
Global:b_mp3_restore_file_flg
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Check_Restore_LastFile(void)
{
	if(b_mp3_restore_file_flg == 1)
	{
		b_mp3_restore_file_flg = 0;
		uc_dynamicload_idx = 45;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_7_SELFILE_IDX);//get current file index in disk
		
		Ap_Init_MP3_Info();				//init play
	}
}
/*******************************************************************************
Description: 1. if it is root, and return to music main
             2. otherwise, exit from current directory
********************************************************************************/
void Ap_ExitFolder_InBrowser(void)						//exit the current folder
{
	if(uc_fs_subdir_stk_pt == 0)
	{
		b_mp3_save_flg = 0;
		Ui_ReturnTo_Music();						//return to music window
	}
	else
	{
		uc_dynamicload_idx = 11;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_5_GET_ITEM_NUM_IDX);  //get browser item number

		Ui_SetLcdFlag_0xfd();
	}
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
	uc_mp3_status = STATE_STOP;
	uc_mpeg_state = STATE_STOP;
}

/*******************************************************************************
1.folder
  1.1 if the selected folder is dummy, and exit from the current directory
  1.2 otherwise, enter the current directory(max is 8'layer dir)
2.file
  2.1 enter deleting file window
********************************************************************************/
void Ap_EnterFolder_InBrowser(void)
{
	if(ui_fb_sel_item_idx == 0)
	{
		if(uc_fs_subdir_stk_pt == 0)			//have not "dummy folder" in ROOT
		{
			goto enter_folder;
		}
		else
		{
			Ap_ExitFolder_InBrowser();			//the first folder is dummy
		}
	}
	else
	{
enter_folder:
		if(uc_fb_sel_item_type == 1)
		{
 			Ap_Play_SelFile_InFileBrowser();
			Ui_ReturnTo_Music();				//return to music window
		}
		else if(uc_fb_sel_item_type == 2)		//uc_fs_item_type: 1-file, 2-folder
		{
			if(uc_fs_subdir_stk_pt < FS_MAX_SUBDIR_DEPTH)
			{
				uc_dynamicload_idx = 10;
				Api_DynamicLoad_Music_Target(CODE_MUSIC_5_GET_ITEM_NUM_IDX);

				Ui_SetLcdFlag_0xfd();
			}
		}
	}
}
/*******************************************************************************
Function: Ap_Init_ItemIdx()
Description: init item index
*******************************************************************************/
void Ap_Init_ItemIdx(void)
{
	ui_fb_sel_item_idx = ui_mp3_idx_indir + ui_fs_subdir_num;
	if(uc_fs_subdir_stk_pt == 0)
	{
		ui_fb_sel_item_idx--;			//have not "dummy folder" in ROOT
	}
			
	if(ui_fb_sel_item_idx < Dis_BrowserItem_MaxNum)
	{
		ui_fb_top_item_idx = 0;
	}
	else
	{
		ui_fb_top_item_idx = ui_fb_sel_item_idx - Dis_BrowserItem_MaxNum + 1;
	}	
}
extern void Ap_Check_Init_FlashSD(void);
void Ap_Switch_Disk_Process(uc8 disk_idx)
{	

	uc_disktype_flg = 0;
	b_sd_flash_flg = 0;
	if(disk_idx == 3)
	{
		b_sd_flash_flg = 1;			//select SD/MMC card
		b_sd_exist_flg = 1;	
		Ap_Check_Init_FlashSD();
	}
	else
	{
		if(disk_idx == 2) PartitionEntryNo = 1;
		//PartitionEntryNo = 1;
		if(disk_idx == 1) uc_disktype_flg = 0x00; 
		uc_dynamicload_idx = 30;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);

		uc_dynamicload_idx = 41;
		Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);
		if(b_fb_nofile_flg == 1)
		{
			uc_disktype_flg = 0x00; 
			uc_dynamicload_idx = 30;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_C_SWITCH_CARD_IDX);

			uc_dynamicload_idx = 41;
			Api_DynamicLoad_Music_Target(CODE_MUSIC_1_INIT_IDX);
			/*if(Ap_Init_MP3_Info() == 1) 	//init info of current mp3(get id3&vbr info, process lrc file, get mp3 filename...)
			{
				Ap_SetMP3_Play();		  //if file is ok, and start play
			}*/
     	}
	}
			
	//Ui_SetLcdFlag_0xfd();
}  

#endif