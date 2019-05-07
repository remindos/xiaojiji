/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: api_common_func.c
Creator: zj.zong					Date: 2009-04-10
Description: common function for all mode
Others:
Version: V0.1
History:
	V0.1	2009-04-10		zj.zong
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef API_COMMON_FUNC_C_DEF

#include "sh86278_sfr_reg.h"
#include "ui_resident.h"
#include "ui_resident_var_ext.h"
#include "hwdriver_resident.h"

extern void Ui_PowerOff_Backlight(void);
extern void Api_LcdDriver_Set_BklightLevel(uc8 bklight_grade);
extern void Api_DynamicLoad_CodeMpm1_8kExecute(void);
extern void Api_DynamicLoad_CodeCpm(void);

extern void _media_readsector(void);
extern void Ap_PowerOff_MP3(void);
extern void Api_DynamicLoad_CodeMpeg(void);
extern void Api_DynamicLoad_DataCbuf(void);
 //void Api_DynamicLoad_PowerOff(void);


#ifdef API_CHECK_BKLIGHT_OFF
/*******************************************************************************
void Api_Check_BklightOff(void)
This is function about timer in _process_evet_time()
********************************************************************************/
void Api_Check_BklightOff(void)
{
	if(uc_bklight_time_level != 0)
	{
		if(ui_bklight_time != 0)
		{
			if((--ui_bklight_time) == 0)
			{
				Ui_PowerOff_Backlight();			//for test usb
			}
		}
	}
}
#endif






/*******************************************************************************
void Api_Check_Battery(void)
This is function about timer in _process_evet_time()
********************************************************************************/
#ifdef API_CHECK_BAT_DETECT
void Api_Check_Battery(void)
{
	if((++uc_bat_detect_time) >= TIME_DETECT_VOLTAGE)
	{
		uc_bat_detect_time = 0;
		MR_MODEN1 |= 0x04;						//set bit2, enable ADC mode
		MR_ADCCON = 0x81;						//enable ADC interrupt, select Vbat-in
	}
}
#endif




/*******************************************************************************
void Api_Check_Idle_PowerOff(void)
This is function about timer in _process_evet_time()
********************************************************************************/
#ifdef API_CHECK_IDLE_POWEROFF
void Api_Check_Idle_PowerOff(void)
{
	if(uc_idle_poweroff_level != 0)
	{
		if((++ui_idle_poweroff_cnt) >= ui_idle_poweroff_time)
		{
			uc_poweroff_type = POWEROFF_NORMAL;
			Ap_PowerOff_MP3();
		}
	}
}
#endif



/*******************************************************************************
void Api_Check_Sleep_PowerOff(void)
This is function about timer in _process_evet_time()
********************************************************************************/
#ifdef API_CHECK_SLEEP_POWEROFF
void Api_Check_Sleep_PowerOff(void)
{
	if(uc_sleep_poweroff_level != 0)
	{
		if((--ui_sleep_poweroff_time) == 0)
		{
			uc_poweroff_type = POWEROFF_NORMAL;
			Ap_PowerOff_MP3();
		}
	}
}
#endif



/*******************************************************************************
Function: Api_Key_Mapping()
Description: key mapping
Calls:
Global:uc_key_value(bit7~4 is key state, bit3~0 is key value)
Input: Null
Output:Null
Others:
********************************************************************************/
#ifdef API_KEY_MAPPING
void Api_Key_Mapping()
{
	uc8 xdata tmp_key_value;

	tmp_key_value = uc_key_value & 0xf;
	if(tmp_key_value <= 7)								//not play & hold key
	{
		uc_key_value &= 0xf0;
		uc_key_value |= uc_key_val_table[tmp_key_value - 1];
	}
}
#endif

#ifdef API_READ_SYS_PARA_TO_DBUF
void Api_Read_SysPara_ToDbuf0(void)				//dbuf(0x4000)
{
	sfc_logblkaddr0 = 0x02;						//0x0000 0202
	sfc_logblkaddr1 = 0x02;
	sfc_logblkaddr2 = 0x00;
	sfc_logblkaddr3 = 0x00;
	sfc_reserved_area_flag = 1;					//read flash reserved area
	sfc_occ_buf_flag = SFC_APP_DBUF0;			//occupy dbuf 0x4000
	buf_offset0 = 0x00;							//buffer offset :0x0000
	buf_offset1 = 0x00;
	_media_readsector();						//read one sector
}
#endif

#ifdef API_DYNAMICLOAD_USB
/*******************************************************************************
Function:   Api_DynamicLoad_USB()
Description: interface for dynamic loading USB mode
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Api_DynamicLoad_USB(void)
{
   	uc_load_code_idx = CODE_USB_MODE_CPM_IDX;
	Api_DynamicLoad_CodeCpm();
	uc_load_code_idx = CODE_USB_MODE_IDX;
	Api_DynamicLoad_CodeMpm1_8kExecute();
}

#endif


#ifdef API_DYNAMICLOAD_POWER_OFF
/*******************************************************************************
Function:   1.Api_DynamicLoad_PowerOff()
Description: interface for dynamic loading PowerOff mode or main menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Api_DynamicLoad_PowerOff(void)
{
	uc_load_code_idx = CODE_POWEROFF_MODE_IDX;	//enter power off mp3 mode
	Api_DynamicLoad_CodeMpm1_8kExecute();	
}

#endif

#ifdef API_SYS_ENABLE_VOICE_AUDIO_DEC_DEF
/*******************************************************************************
Function:   Api_Mpeg_Ena_VoiceAudioDec()
Description: interface for mpeg enable voice audio decode
*******************************************************************************/
void Api_Mpeg_Ena_VoiceAudioDec(void)
{
	if(ef_type & 0x01)
	{
		uc_load_code_idx = CODE_MPEG_VOICE_AUDIO_DCM_IDX;
	}
	else
	{
		uc_load_code_idx = CODE_MPEG_VOICE_AUDIO_DCM_SB_IDX;	
	}	
	Api_DynamicLoad_CodeMpeg();			//load code into mpeg CM Space
	uc_load_code_idx = CODE_MPEG_VOICE_AUDIO_CBUF_IDX;
	Api_DynamicLoad_DataCbuf();			//load data into cbuf	
}
#endif

#ifdef API_DYNAMICLOAD_MAIN_MENU
/*******************************************************************************
Function:   1.Api_DynamicLoad_MainMenu()
Description: interface for dynamic loading PowerOff mode or main menu
Calls:
Global:Null
Input: Null
Output:Null
Others:
*******************************************************************************/
void Api_DynamicLoad_MainMenu(void)
{
	uc_load_code_idx = CODE_MAINMENU_IDX;		//enter main menu
	Api_DynamicLoad_CodeMpm1_8kExecute();
}

#endif

#ifdef API_SYS_ENABLE_AUDIO_DEC_DEF
/*******************************************************************************
Function:   Api_Mpeg_Ena_AudioDec()
Description: interface for mpeg enable audio decode
*******************************************************************************/
void Api_Mpeg_Ena_AudioDec(void)
{
	if((ef_type&0x01) != 0)  
	{
		uc_load_code_idx = CODE_MPEG_AUDIO_DCM_IDX;	   //large block
	}
	else
	{
	 	uc_load_code_idx = CODE_MPEG_AUDIO_DCM_SB_IDX;	   //small block
	}
	Api_DynamicLoad_CodeMpeg();			//load code into mpeg CM Space
	uc_load_code_idx = CODE_MPEG_AUDIO_CBUF_IDX;
	Api_DynamicLoad_DataCbuf();			//load data into cbuf	
}
#endif


#ifdef API_DETECT_HEADPHONE_PLUG_DEF
#ifdef PCB_WITH_SPEAKER
void Api_Detect_Headphone_Plug(void)
{
//	if(b_headphone_plug_flg == 0)					//for sample
	MR_PATYPE &= 0x7F;
	if((MR_PADATA & HEADPHONE_DETECT_MASK) != 0)
	{
		uc_headphone_plug_cnt = 0;
		uc_headphone_unplug_cnt++;
		if(uc_headphone_unplug_cnt >= TIME_DETECT_HEADPHONE)
		{
			MR_PADATA |= SPEAKER_CONTROL_MASK;		//PA1 = 1
		}
	}
	else
	{
		uc_headphone_unplug_cnt = 0;
		uc_headphone_plug_cnt++;
		if(uc_headphone_plug_cnt >= TIME_DETECT_HEADPHONE)
		{
			MR_PADATA &= (~SPEAKER_CONTROL_MASK);	//PA1 = 0
		}
	}
}
#endif
#endif

#endif