/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_Si4702.c
Creator: aj.zhao				Date: 2009-11-04
Description: fm driver function, fm module is Silicon LABs Si4702
Others:
Version: V0.1
History:
	V0.1	2009-11-04		aj.zhao
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_SI4702
#include "sh86278_sfr_reg.h"
#include "drv_fm.h"
#include "drv_fm_var_ext.h"

extern void _delay_1ms(uc8 timer_cnt);
extern void _fm_i2c_enable(void);
extern void _fm_i2c_disable(void);
extern void _i2c_set_sda_out(void);
extern void _i2c_clr_sda(void);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_read(uc8 rd_cnt);

void Api_RadioInit(void);
void Api_RadioClose(void);
void Api_RadioOpen(uc8 freq_mode);
void Api_RadioSetFreq(ui16 radio_freq);
void Api_RadioSetMute(uc8 mute_flag);
void Api_RadioSetStereo(uc8 stereo_flag);
bit	 fm_search_one_freq(void);
void fm_i2c_calculate_freq(void);
void fm_tuning(void);

code uc8 Si4702Reg[] ={
	0x40,
	0x01,
	0x00,
	0x00,
	0x90,
	0x04,
	0x0c,
	0x1f,
	0x00,
	0x48
};

/*******************************************************************************
Function: Api_RadioInit()
Description: initialize and enter standby mode;
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioInit(void)
{	
	Api_RadioOpen(0);
	Api_RadioClose();
}



/*******************************************************************************
Function: Api_RadioOpen()
Description: initialize fm module
Calls:
Global:radio_mode:0--FM; 1--AM
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioOpen(uc8 radio_mode)
{
	uc8 RegNum;

	uc_fm_valid_cnt = 0;

	/*** RESET WRITE REG ***/
	/*** write reg begin from R(02)~ R(06) 5 bytes ***/
	for(RegNum = 0; RegNum < 10; RegNum++)
	{
		uc_fm_write_bytes[RegNum] = 0;
	}
	uc_fm_write_bytes[10] = 0x81;

	_fm_i2c_enable();
	_i2c_set_sda_out();
	_i2c_clr_sda();
	_delay_1ms(10); 		// delay 10ms

	FM_RESET_TYPE |= FM_RESET_MASK;
	FM_RESET_PORT |= FM_RESET_MASK;
	_delay_1ms(50); 		// delay 50ms
	FM_RESET_PORT &= ~FM_RESET_MASK;
	_delay_1ms(200); 		// delay 200ms
	FM_RESET_PORT |= FM_RESET_MASK;

	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);
	_fm_i2c_write(11);		//include write_byte[10]
	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);
	_delay_1ms(200);
	_fm_i2c_write(11);		//include write_byte[10]

	/*** Init WriteReg ***/
	for(RegNum = 0; RegNum < 10; RegNum++)
	{
		uc_fm_write_bytes[RegNum] = Si4702Reg[RegNum];
	}

	switch(radio_mode)
	{
		/*** USA, Europe ***/
		case 0:		
			uc_fm_write_bytes[7] &= 0x3f;
			break;
		/*** Japan ***/
		case 1:
			uc_fm_write_bytes[7] &= 0x3f;
			uc_fm_write_bytes[7] |= 0x80;
			uc_fm_write_bytes[4] |= 0x08;	//DE for Japan
			break;
		/*** Japan wide band ***/
		case 2:
			uc_fm_write_bytes[7] &= 0x3f;
			uc_fm_write_bytes[7] |= 0x40;
			uc_fm_write_bytes[4] |= 0x08;	//DE for Japan
			break;
	}
	_fm_i2c_write(10);
	_delay_1ms(250);
}



/*******************************************************************************
Function: Api_RadioClose()
Description: mute the tuner and set it standby
Calls:
Global:Null
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioClose(void)
{
	uc_fm_write_bytes[0] = 0x40;
	uc_fm_write_bytes[1] = 0x01;
	uc_fm_write_bytes[2] = 0x00;
	uc_fm_write_bytes[3] = 0x00;
	uc_fm_write_bytes[4] = 0x00;
	_fm_i2c_write(5);
	
	uc_fm_write_bytes[0] = 0x00;
	uc_fm_write_bytes[1] = 0x41;		
 	_fm_i2c_write(2);
	_delay_1ms(100); 
	
	FM_CLK_TYPE &= ~FM_CLK_MASK; 	//set the fm clk&data as input
	FM_DATA_TYPE &= ~FM_DATA_MASK;	
	MR_PBDATA &= 0xdf;					//PB5 is reset
	_delay_1ms(250);
	_delay_1ms(250);
}



/*******************************************************************************
Function: Api_RadioSetFreq()
Description: write CHAN[9:0](get from ui_fm_cur_freq), 
             and then set mute off
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioSetFreq(ui16 radio_freq)
{
	ui16 loop_counter;
	
	loop_counter = 0;
	ui_fm_cur_freq = radio_freq;
	fm_i2c_calculate_freq();

	/*** WAIT STC=1 ***/
	do{
		MR_WDT = 0xff;
		_fm_i2c_read(2);
		loop_counter++;
		_delay_1ms(10);
	}while(((uc_fm_read_bytes[0] & 0x40) == 0) && (loop_counter < 0x7f));

	/*** TUNE FAIL ***/
	if(loop_counter == 0x7f)
		return;

	/*** TUNE OK ***/
	loop_counter = 0;
	uc_fm_write_bytes[0] = 0x40;
	uc_fm_write_bytes[1] = 0x01;
	uc_fm_write_bytes[2] = 0x00;
	_fm_i2c_write(3);

	/*** WAIT STC=0 ***/
	do{
		MR_WDT = 0xff;
		_fm_i2c_read(2);
		loop_counter++;
		_delay_1ms(10);
	}while((uc_fm_read_bytes[0] & 0x40) && (loop_counter < 0x7f));
}



/*******************************************************************************
Function: Api_RadioGetFreq()
Description: calculate frequency from the READCHAN[9:0],store in ui_fm_cur_freq
Calls:
Global:uc_fm_read_bytes[]
Input: Null
Output:1word
Others:
********************************************************************************/
int Api_RadioGetFreq(void)
{
	ui16 TempReadChan, TempFreq, TempLimitFreq;

	_fm_i2c_read(4);		//READCHAN[9:0]	is in R(0B)
	TempReadChan = ((uc_fm_read_bytes[2]  << 8) | uc_fm_read_bytes[3]) & 0x03ff;

	if(uc_fm_band == 0)
	{
		TempLimitFreq = 8750;		
	}
	else
	{
		TempLimitFreq = 7600;	
	}

	TempFreq = TempReadChan * 10 + TempLimitFreq;
	
	return TempFreq;
}



/*******************************************************************************
Function: Api_RadioGetStereoFlag()
Description: get fm stereo or mono
Calls:
Global:uc_radio_status: bit6 set 1 for stereo, 0 for mono
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioGetStereoFlag(void)
{
	_fm_i2c_read(2);		//READCHAN[9:0]	is in R(0A)
	if(uc_fm_read_bytes[0] & 0x01)
	{
		uc_radio_status |= 0x40;		//stereo		
	}
	else
	{
		uc_radio_status &= 0xdf;		//mono	
	}
}



/*******************************************************************************
Function: Api_RadioSetMute()
Description: set fm mute or cancel fm mute
Calls:
Global:uc_radio_status: bit5=1 is mute, 0 is mute off
Input: mute_flag: 1--mute on, 0--cancel mute
Output:Null
Others:
********************************************************************************/
void Api_RadioSetMute(uc8 mute_flag)
{
	if(mute_flag)		//mute
	{
		uc_fm_write_bytes[0] &= ~FM_REG_MUTE;
		uc_radio_status &= ~0x20;
	}
	else				//cancel mute
	{		
		uc_fm_write_bytes[0] |= FM_REG_MUTE;
		uc_radio_status |= 0x20;
	}
	
	_fm_i2c_write(1);	
}



/*******************************************************************************
Function: Api_RadioSetStereo()
Description: set fm stereo or mono
Calls:
Global:
Input: stereo_flag: 1 is force strreo, 0 is force mono
Output:Null
Others:
********************************************************************************/
void Api_RadioSetStereo(uc8 stereo_flag)
{
	if(stereo_flag)	//force stereo
	{
		uc_fm_write_bytes[0] &= ~FM_REG_MS;
	}
	else			//force mono
	{
		uc_fm_write_bytes[0] |= FM_REG_MS;
	}
	
	_fm_i2c_write(1);
}



/*******************************************************************************
Function: Api_RadioAutoSearch()
Description:
Calls:
Global:ui_fm_cur_freq
       uc_fm_mode :0x14 means halfsearch inc
                   0x1c means halfsearch dec
                   0x03 means autosearch
                   0x01 means autosearch finished
                   0x02 means halfsearch finished or reach band limit
       uc_fm_ch_total_num, uc_fm_cur_ch_idx: valid while autosearch
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioAutoSearch(uc8 search_flg, ui16 search_para)
{
	uc8 i;

	search_para = 0;
	if(search_flg == 0)						//continue search
	{
		switch(uc_fm_mode)
		{
			case FM_MODE_HALFSEARCH_INC:	//halfsearch inc freq
			case FM_MODE_HALFSEARCH_DEC:	//halfsearch dec freq
				fm_tuning();
				break;
			
			default:						//autosearch from low limit to high limit 
				if(uc_fm_mode != FM_MODE_AUTOSEARCH)//initial variants while first search start
				{
    				uc_fm_mode = FM_MODE_AUTOSEARCH; //auto search mode
					ui_fm_cur_freq = ui_fm_freq_limit_low; 
					Api_RadioSetFreq(ui_fm_cur_freq);

					ui_fm_cur_freq = ui_fm_freq_limit_low - FM_AUTO_STEP;
					uc_fm_valid_cnt = 0;
  	 				for(i = 0; i < FM_CH_NUM_MAX; i++)
  	 				{
      	 				ui_fm_preset_buf[i] = ui_fm_freq_limit_low;
  	 				}
  	 				uc_fm_cur_ch_idx = 0;
  	 				uc_fm_ch_total_num = 0;
				}
				fm_tuning();
				break;
		}
	}
	else		//end search
	{
		uc_fm_valid_cnt = 0;
		uc_fm_mode = FM_MODE_TUNING;
		Api_RadioSetFreq(ui_fm_cur_freq);
	}
}



void fm_tuning(void)
{
	ui16 xdata tmp_fm_freq;

	if((uc_fm_mode == FM_MODE_PRESET) || (uc_fm_mode == FM_MODE_TUNING))	//not auto&half search
	{
		return;
	}

	tmp_fm_freq = ui_fm_cur_freq + FM_AUTO_STEP;	//default is increase fm step
	if(uc_fm_mode == FM_MODE_AUTOSEARCH)			//auto search
	{
		if(tmp_fm_freq > ui_fm_freq_limit_high)
		{
			if(uc_fm_ch_total_num == 0)	//have not searched valid channel
			{
				uc_fm_mode = FM_MODE_TUNING;		//tuning mode
				uc_fm_cur_ch_idx = 0;	//0: means no station after auto search finished
				ui_fm_cur_freq = ui_fm_freq_limit_low;
			}
			else
			{
				uc_fm_mode = FM_MODE_PRESET;		//preset mode
				uc_fm_cur_ch_idx = 1;	//1: means the first channel number
				ui_fm_cur_freq = ui_fm_preset_buf[0]; //after search finished,set the first station as preset
			}
			
			uc_fm_valid_cnt = 0;		//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
			_delay_1ms(100);			//delay 100ms so that ui_fm_freq_limit_high can display for a while
			goto end_tuner_search;
		}
		else
		{
			goto auto_search_one_tune;
		}
	}
	else		//half search
	{
		if((uc_fm_mode & FM_MODE_HALFSEARCH) != 0)
		{
			if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)	//half search -0.1MHz
			{
				tmp_fm_freq = ui_fm_cur_freq - FM_AUTO_STEP;
				if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					tmp_fm_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else
				{
					goto auto_search_one_tune;
				}

				_delay_1ms(100);	//delay 100ms so that FM_LIMIT can display for a while
			}
			else	//half search +0.1MHz(bit3 = 0)
			{
				if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					tmp_fm_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				else
				{
					goto auto_search_one_tune;
				}

				_delay_1ms(100);			//delay 100ms so that FM_LIMIT can display for a while
			}
half_auto_one_time_finish:					//half searching end
			uc_fm_mode = FM_MODE_TUNING;	//frequency modulation mode
			goto end_tuner_search;
		}
	}
	return;

end_tuner_search:
	Api_RadioSetFreq(ui_fm_cur_freq);
	return;

auto_search_one_tune:
	MR_WDT = 0xff;

	ui_fm_cur_freq = tmp_fm_freq;
	if(fm_search_one_freq())
	{
		if(uc_fm_mode == FM_MODE_AUTOSEARCH)			//auto search
		{	
			uc_fm_valid_cnt=0;	
	       	if((uc_fm_cur_ch_idx + 1) <= FM_CH_NUM_MAX)
	       	{
	           	uc_fm_cur_ch_idx++;
		       	uc_fm_ch_total_num = uc_fm_cur_ch_idx;
			}
			ui_fm_preset_buf[uc_fm_cur_ch_idx - 1] = ui_fm_cur_freq;
		}
		else
		{
	  	 	goto half_auto_one_time_finish;
		}
	}
}


/*******************************************************************************
Function: fm_search_one_freq()
Description: set as normal mode first, write PLL(get from ui_fm_cur_freq), 
             and then set mute off
Calls:
Global:
Input: Null
Output:Null
Others:
********************************************************************************/
bit fm_search_one_freq(void)
{
	uc8 SeekFail, ValidChannel;
	ui16 loop_counter;
	
	loop_counter = 0;	   
	/*** SET SEEK_MODE SEEKUP SEEK ***/
	switch(uc_fm_mode)
	{
		case 0x03:
		case 0x14:		//FM_MODE_HALFSEARCH_INC, seek up
			uc_fm_write_bytes[0] = 0x47;
			break; 

		case 0x1c:		//FM_MODE_HALFSEARCH_DEC, seek down
			uc_fm_write_bytes[0] = 0x45;
			break;
	}
	_fm_i2c_write(1);
	_delay_1ms(20);

	/*** WAIT STC=1 ***/
	_fm_i2c_read(1);
	while(((uc_fm_read_bytes[0] & 0x40) == 0) && (loop_counter < 0xffff))
	{
		MR_WDT = 0xff;
		loop_counter++;
		ui_fm_cur_freq = Api_RadioGetFreq();
		_delay_1ms(20);
		return 0;			
	}

	if(loop_counter == 0xffff)
		return 0;

	ui_fm_cur_freq = Api_RadioGetFreq();	
	/*** CHECK SEEK FAIL/BAND LIMIT REACHED ***/
	loop_counter = 0;
	if(uc_fm_read_bytes[0] & 0x20)
	{
		SeekFail = 1;
	}
	else
	{
		SeekFail = 0;
	}

	/*** CHECK AFC RAIL ***/
	if(uc_fm_read_bytes[0] & 0x10)
	{
		ValidChannel = 0;
	}
	else
	{
		ValidChannel = 1;
	}

	/*** DISABLE SEEK ***/
	uc_fm_write_bytes[0] = 0x40;
	_fm_i2c_write(1);
	_delay_1ms(20);

	/*** WAIT STC=0 ***/
	do{
		loop_counter++;
		_fm_i2c_read(2);		
	}while((uc_fm_read_bytes[0] & 0x40) && (loop_counter < 0xff));

	if(loop_counter >= 0xff)
		return 0;

	if(ValidChannel)
	{
		if(SeekFail)	
		{
			if((ui_fm_freq_limit_high == ui_fm_cur_freq) || (ui_fm_freq_limit_low == ui_fm_cur_freq))
			{
				Api_RadioSetMute(0);		//cancel mute	
				return 1;
			}
			else
			{
				return 0;
			}
		}
		else 
		{
			return 1;	
		}
	}
	else
	{
		return 0;
	}
}



void fm_i2c_calculate_freq(void)
{
	ui16 TempChannel = 0;

	ui_fm_cur_freq &= 0x7fff;

	/*** CALC CHAN[9:0] ***/
	if(uc_fm_band == 0)
	{
		if(ui_fm_cur_freq > 8750)
		{
			TempChannel = (ui_fm_cur_freq - 8750) / 10;	
		}	
	}
	else
	{
		TempChannel = (ui_fm_cur_freq - 7600) / 10;	
	}
	
	/*** SET CHAN[9:0] ***/
	uc_fm_write_bytes[0] = 0x40;
	uc_fm_write_bytes[1] = 0x01;
	uc_fm_write_bytes[2] = 0x80;
	uc_fm_write_bytes[2] |= (TempChannel / 256);
	uc_fm_write_bytes[3] = TempChannel % 256;
	_fm_i2c_write(4);
}

#endif