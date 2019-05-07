/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ap_rec_basal.c
Creator: zj.zong					Date: 2009-06-02
Description: subroutine with communicant of muc and mpeg
Others:
Version: V0.1
History:
	V0.1	2009-06S-02		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef AP_REC_BASAL_DEF

#include "sh86278_api_define.h"
#include "hwdriver_resident.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "sh86278_sfr_reg.h"
#include "..\..\UI\UI_Record\ui_rec_header.h"
#include "..\..\UI\UI_Record\ui_rec_var_ext.h"

extern void Ap_Generate_Nibble();
extern void Ui_Display_RecSaving(void);
extern void Ui_Display_Rec_NoSpace(void);
extern void Ui_Display_Rec_OverFlow(void);
extern void Api_DynamicLoad_Record_Target(uc8 load_code_idx);
extern void FS_Fcreate(void);
extern void Api_DynamicLoad_PowerOff(void);
extern void Api_DynamicLoad_MainMenu(void);
extern void Api_Delay_1ms(uc8 time_cnt); //allen
extern bit  FS_Get_VoiceDir_Idx(void);	  //allen
extern void Api_Read_SysPara_ToDbuf0(void);

void Ap_CalRec_RemainTime(void);
void Ap_Exit_RecMode(void);
void Ap_Enable_IrqBit(void);
void Ap_SetStartRec(void);
void Ap_SetPauseRec(void);
void Ap_SetContinueRec(void);	
void Ap_SetStopRec(void);
void Ap_Rec_Init(void);
void Ap_Wr_ADC_CompressData(ui16 tmp_cur_val);
void Ap_Init_VoiceDir(void);
bit  Ap_Check_SDExist(void);
void Ap_Restore_RecPara(void);

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
 	MR_IE0 = 0x01;						//enable USB Detect irq
	MR_IE1 = 0x07;						//enable mcu interrupt(timer irq, ADC irq,msi) allen
	MR_USBCON |= 0x04;					//enable USB Detect irq
	MR_IE0 |= 0x80;						//enable irq
}

/*******************************************************************************
Function: Ap_RecInit()
		  Ap_Init_VoiceDir()
Description: init rec or voice dir
Calls:
Global:b_rec_card_full_flg, b_rec_wav_full_flg
Input: Null
Output:Null
Others:
*******************************************************************************/
void Ap_Rec_Init(void)
{	
//  	MR_PCDATA |= MIC_POWER_MASK; 	// MIC and FM power	3.3V
  	Ap_Restore_RecPara();				//restore parameter from ef
if(Ap_Check_SDExist() == 1)
	{					
Label_SwitchCard:
    mSet_MCUClock_33M;
	uc_dynamicload_idx = 30;
	Api_DynamicLoad_Record_Target(CODE_RECORD_A_SWITCH_CARD_IDX);//
	Api_DynamicLoad_Record_Target(CODE_RECORD_1_INIT1_IDX);	//danamic load record init code
 	Api_DynamicLoad_Record_Target(CODE_RECORD_2_INIT2_IDX);	//danamic load record init code
	mSet_MCUClock_12M;
	}
else
	{
		b_sd_flash_flg = 0;				//select SD/MMC card
		b_sd_exist_flg = 0;				//if no file, and return to main menu	
		goto Label_SwitchCard;	
    }

}
/*******************************************************************************/
void Ap_Init_VoiceDir(void)
{
	if(FS_Get_VoiceDir_Idx() == 0)
	{
		b_rec_card_full_flg = 1;
	}
	else
	{
		b_rec_card_full_flg = 0;
	}

	b_rec_wav_full_flg = 0;
	ul_rec_cur_time = 0;
	ul_rec_remain_time = 0;
	Api_DynamicLoad_Record_Target(CODE_RECORD_2_INIT2_IDX);
}


/*******************************************************************************
Function: Ap_SetStartRec()
Description: set start record
Calls:
Global:uc_rec_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetStartRec(void)
{
	Api_DynamicLoad_Record_Target(CODE_RECORD_4_START_IDX);
	if(b_rec_card_full_flg == 0)
	{
		mSet_MCUClock_33M;			//set MCU clock 33M
	}
	else
	{
		Ui_Display_Rec_NoSpace();	//flash no space
	}
	if(b_rec_wav_full_flg == 1)
	{
		Ui_Display_Rec_OverFlow();	//voice number overflow
	}
}


/*******************************************************************************
Function: Ap_SetPauseRec()
Description: set pause record 
Calls:
Global:uc_rec_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetPauseRec(void)
{
	uc_rec_status = STATE_PAUSE;
	b_rec_mode_flg = 0;			//clr recording state flag
   	mSet_MCUClock_12M;		   //set MCU clock 22M	
}


/*******************************************************************************
Function: Ap_SetContinueRec()
Description: set continue record
Calls:
Global:uc_rec_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetContinueRec(void)
{
	uc_rec_status = STATE_RECORD;
	b_rec_mode_flg = 1;			//set recording state flag
	b_flicker_time_flg = 0;
	mSet_MCUClock_33M;			//set MCU clock 33M
}


/*******************************************************************************
Function: Ap_SetStopRec()
Description: set stop record
Calls:
Global:uc_rec_status
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_SetStopRec(void)
{
	uc_rec_status = STATE_STOP;
	b_rec_mode_flg = 0;
   	mSet_MCUClock_12M;
	Ui_Display_RecSaving();		//display "Saving..."	

	Api_DynamicLoad_Record_Target(CODE_RECORD_5_STOP_IDX);	//write record file header......
}

/*******************************************************************************
Function: Ap_Encode_Block()
Description: encode, compress dbuf data into cbuf, then fwrite
Calls:
Global:si_dbuf0[], si_dbuf1[], b_rec_decbuf_flg, ui_cbuf
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_Encode_Block()
{
	ui16 tmp_enc_code, tmp_mic_sample, tmp_sample_inx;
	ui16 tmp_sample_buf[4];
	si16 tmp_raw_data;
	uc8  tmp_fill_samplebuf_idx;
	si16 tmp1, tmp2, tmp3, tmp4, tmp5;

	tmp_fill_samplebuf_idx = 0;
	ui_rec_dptr_var = 0x0000;			//combuf addr for compress adc data
	
	for( tmp_sample_inx = 0; tmp_sample_inx < 1010;  tmp_sample_inx++ )
	{
		if(b_rec_decbuf_flg == 1)		//b_rec_decbuf_flg = 1, meaning dbuf0 is full
		{
			tmp_raw_data = si_dbuf0[tmp_sample_inx];
		}
		else
		{
			tmp_raw_data = si_dbuf1[tmp_sample_inx];
		}

		if(b_rec_samplerate_flg == 1)
		{
			tmp1 = (tmp_raw_data >> 3) + (si_rec_adc_inbuf[0] >> 3);			//[x(n)+x(n-4)]*1/8
			tmp2 = ((si_rec_adc_inbuf[3] >> 3) + (si_rec_adc_inbuf[1] >> 3)) * 3;	//[x(n-1)+x(n-3)]*3/8
			tmp3 = si_rec_adc_inbuf[2] >> 1;								//x(n-2)*1/2
			tmp4 = si_rec_adc_outbuf[2] >> 1;								//y(n-2)*1/2
			tmp5 = si_rec_adc_outbuf[0] >> 6;								//y(n-4)*1/64
			
			si_rec_adc_inbuf[0] = si_rec_adc_inbuf[1];
			si_rec_adc_inbuf[1] = si_rec_adc_inbuf[2];
			si_rec_adc_inbuf[2] = si_rec_adc_inbuf[3];
			si_rec_adc_inbuf[3] = tmp_raw_data;
			
			tmp_raw_data = tmp1 - tmp4 + tmp2 - tmp5 + tmp3;
			
			si_rec_adc_outbuf[0] = si_rec_adc_outbuf[1];
			si_rec_adc_outbuf[1] = si_rec_adc_outbuf[2];
			si_rec_adc_outbuf[2] = si_rec_adc_outbuf[3];
			si_rec_adc_outbuf[3] = tmp_raw_data;
		}

		tmp_mic_sample = tmp_raw_data;
		
		if( tmp_sample_inx == 0 || tmp_sample_inx == 505)
		{
			ui_enc_sample = tmp_mic_sample;
			Ap_Wr_ADC_CompressData(tmp_mic_sample);
			Ap_Wr_ADC_CompressData(uc_enc_step_idx);
		}
		else
		{
			tmp_sample_buf[tmp_fill_samplebuf_idx] = tmp_mic_sample;
			tmp_fill_samplebuf_idx++;
			
			if(tmp_fill_samplebuf_idx == 4)
			{
				ui_enc_indata = tmp_sample_buf[0];
				Ap_Generate_Nibble();
				tmp_enc_code = uc_enc_thecode;

				ui_enc_indata = tmp_sample_buf[1];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 4);
	
				ui_enc_indata = tmp_sample_buf[2];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 8);
	
				ui_enc_indata = tmp_sample_buf[3];
				Ap_Generate_Nibble();
				tmp_enc_code |= (uc_enc_thecode << 12);

				Ap_Wr_ADC_CompressData(tmp_enc_code);
				tmp_fill_samplebuf_idx = 0;
			}
		}
	}
}

/*******************************************************************************
write copress data to cbuf
********************************************************************************/
void Ap_Wr_ADC_CompressData(ui16 tmp_cur_val)	//write compress 
{
	ui_cbuf[ui_rec_dptr_var + X_ADDR_REC_ENC_BUF] = tmp_cur_val;
	ui_rec_dptr_var++;							//addr + 2bytes
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
	Ap_Exit_RecMode();						//stop record and save record parameter
	Api_DynamicLoad_MainMenu();			 	//enter top menu
}

/*******************************************************************************
exit record mode
********************************************************************************/
void Ap_Exit_RecMode(void)
{
	if(uc_rec_status != STATE_STOP)
	{
		Ap_SetStopRec();			  //stop record
	}
  	MR_DACCLK |= 0x04;				  //set VREFDAC is grounded
	Api_DynamicLoad_Record_Target(CODE_RECORD_6_SAVE_IDX);
}

/*******************************************************************************
Function: Ap_PowerOff_MP3()
Description: power off mp3
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Ap_PowerOff_MP3(void)
{
	Ap_Exit_RecMode();				//stop record and save record parameter			  		
	Api_DynamicLoad_PowerOff();		//enter power off mp3 mode
}

/*******************************************************************************
calcuate record remain time according to disk space	
********************************************************************************/
void Ap_CalRec_RemainTime(void)
{
	if(b_rec_samplerate_flg == 1)				//16KHz sample rate(collect 16*1024 byte per second)
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 1600;
	}
	else										//8KHz sample rate(collect 8*1024 byte per second)
	{
		ul_rec_remain_time = ul_fs_spare_space * 101 / 800;
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
		Api_DynamicLoad_Record_Target(CODE_RECORD_A_SWITCH_CARD_IDX);				

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
 Function: Ap_Restore_RecPara()
 Description: read some parameter for rec mode	
 *******************************************************************************/
 void Ap_Restore_RecPara(void)
 {
	 Api_Read_SysPara_ToDbuf0();
 
	 if((uc_dbuf0[0] == 'S') && (uc_dbuf0[1] == 'I') && (uc_dbuf0[2] == 'N') && (uc_dbuf0[3] == 'O'))
	 {
		 b_sd_flash_flg = uc_dbuf0[78];
		 if(uc_dbuf0[81] == 0)				 //record quality, 1--high; 0--normal
		 {
			 b_rec_samplerate_flg = 0;
		 }
		 else
		 {
			 b_rec_samplerate_flg = 1;
		 }
	 }
	 else
	 {
		 b_rec_samplerate_flg = 1;
	 }
 }
 

#endif