/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_BK1080.c
Creator: 	zj.zong			Date: 2009-08-10
Description: fm driver function, fm module is BK1080
Others:
Version: V0.1
History:
	V0.1	2009-08-10		zj.zong	
		1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#ifdef DRV_FM_BK1080
#include "sh86278_sfr_reg.h"
#include "drv_fm.h"
#include "drv_fm_var_ext.h"

extern void _delay_1ms(uc8 cnt);
extern void _fm_i2c_write(uc8 wr_cnt);
extern void _fm_i2c_read(uc8 rd_cnt);

void Api_RadioOpen(uc8 freq_mode);
uc8 fm_search_one_freq(uc8 searchmode);
void Api_RadioClose(void);
void fm_tuning(void);
void reset_fm_preset_buffer(void);
//code ui16 Api_FreqdL_table[]={130,120,100,98,96,84,92};
code ui16 Api_FreqdL_table[]={130,120,110,100,98,96,94};
code ui16 Api_FreqdH_table[]={0xffff,0xffff,0xffff,0xffff,0xffff,0xffff,0xffff};

code ui16 BK1080reg[34]={ //using  BK1080  function
	0x0008,	//REG0
	0x1080,	//REG1
	0x1201,	//REG2
	0x0000,	//REG3
	0x40C0,	//REG4
	0x0A1F,	//REG5
	0x002E,	//REG6
	0x02FF,	//REG7
	0x5B11,	//REG8
	0x0000,	//REG9
	0x411E,	//REG10
	0x0000,	//REG11
	0xCE00,	//REG12
	0x0000,	//REG13
	0x0000,	//REG14
	0x1000,	//REG15
	0x0010,	//REG16
	0x0000,	//REG17
	0x13FF,	//REG18
	0x9852,	//REG19
	0x0000,	//REG20
	0x0000,	//REG21
	0x0008,	//REG22
	0x0000,	//REG23
	0x51E0,	//REG24
	0x28DC,	//REG25
	0x2645,	//REG26
	0x00E4,	//REG27
	0x1CD8,	//REG28
	0x3A50,	//REG29
	0xEAF0,	//REG30
	0x3000,	//REG31
	0x0000,	//REG32
	0x0000,	//REG33
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
void Api_RadioOpen(uc8 freq_mode)
{
	uc8 xdata Cnt1;
	freq_mode = 0;			//87.5M~107.5M
	uc_fm_read_bytes[0] = 0;
	uc_fm_read_bytes[1] = 0;
	uc_fm_write_bytes[0] = 0;
	uc_fm_write_bytes[1] = 0;

	for(Cnt1=0;Cnt1<34;Cnt1++)
  	{
  		uc_fm_write_bytes[0] = (uc8)(BK1080reg[Cnt1]>>8);
		uc_fm_write_bytes[1] = (uc8)(BK1080reg[Cnt1]);
  		_fm_i2c_write(Cnt1);
	}
	_delay_1ms(250);
	_delay_1ms(250);
	_delay_1ms(250);
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
void Api_RadioClose()
{
	uc_fm_write_bytes[0] = (uc8)(BK1080reg[2]>>8);
	uc_fm_write_bytes[1] = (uc8)(BK1080reg[2]);
	
	uc_fm_write_bytes[0] |= 0x40;  // mute on
	uc_fm_write_bytes[1] |= 0x41; //  power down
	_fm_i2c_write(2);
	_delay_1ms(100); 		// delay 100ms
	
	FM_CLK_TYPE &= ~FM_CLK_MASK; 	//set the fm clk&data as input
	FM_DATA_TYPE &= ~FM_DATA_MASK;
}

/*******************************************************************************
Function: Api_RadioSetFreq()
Description: set as normal mode first, write PLL(get from ui_fm_cur_freq), 
             and then set mute off
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioSetFreq(ui16 ui_fm_cur_freq)
{
	ui16 xdata tmp_pll;
	ui_fm_cur_freq = ui_fm_cur_freq & 0x7fff;

	if(uc_fm_band == 0)
	{
		if(ui_fm_cur_freq >= 8750) 
		{
			tmp_pll = (ui16)((ui_fm_cur_freq - 8750) /10);
		}
		else
		{
			tmp_pll = 0;
		}
	}
	else
	{
		tmp_pll = (ui16)((ui_fm_cur_freq - 7600) /10);

	}
	
	uc_fm_write_bytes[0] = (uc8) ((tmp_pll >>8)|FM_REG_TUNE);
	uc_fm_write_bytes[1] = (uc8) (tmp_pll);
	_fm_i2c_write(0x03);
	
	uc_fm_write_bytes[0] &= 0x7f;				//set disable ture
	_fm_i2c_write(0x03);	
	_delay_1ms(100); 		// delay 100ms
}
/*******************************************************************************
Function: Api_RadioGetFreq()
Description: calculate frequency from the read PLL,store in ui_fm_cur_freq
Calls:
Global:ui_fm_cur_freq
Input: Null
Output:Null
Others:
********************************************************************************/
ui16 Api_RadioGetFreq()
{
	uc8 xdata temp_chan_l,temp_chan_h;
	ui16 xdata temp_chan;

	_fm_i2c_read(0x0b);

	temp_chan_l = uc_fm_read_bytes[1];
	temp_chan_h = uc_fm_read_bytes[0];
	temp_chan_h &= 0x03;
	temp_chan = temp_chan_h * 256 + temp_chan_l;

	if(uc_fm_band == 0)
	{
		ui_fm_cur_freq = temp_chan * 10 + 8750;
	}
	else
	{
		ui_fm_cur_freq = temp_chan * 10 + 7600;
	}
	return ui_fm_cur_freq;
}

/*******************************************************************************
Function: Api_RadioGetStereoFlag()
Description: get fm stereo or mono
Calls:
Global:uc_radio_status: bit6=1 is stereo, 0 is mono
Input: Null
Output:Null
Others:
********************************************************************************/
void Api_RadioGetStereoFlag(void)
{
	_fm_i2c_read(0x0a);

	if(uc_fm_read_bytes[0] &0x01)
	{
		uc_radio_status |= 0x40;		//bit6=1 is stereo
	}
	else
	{
		uc_radio_status &= ~0x40;	   	//bit6=0 is mono
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
		uc_fm_write_bytes[0] = (uc8)((BK1080reg[2]|0x4000)>>8);	 		//mute on	bit 14
	}
	else
	{
		uc_fm_write_bytes[0] = (uc8)((BK1080reg[2]&0xbfff)>>8);		 	// mute off	bit	14
	}
	uc_fm_write_bytes[1] = (uc8)(BK1080reg[2]);
	_fm_i2c_write(2);
}
/*******************************************************************************
Function: Api_RadioSetStereo()
Description: set fm stereo or mono
Calls:
Global:
Input: stereo_flag: 1 is force stereo, 0 is force mono
Output:Null
Others:
********************************************************************************/
void Api_RadioSetStereo(uc8 stereo_flag)
{
	if(stereo_flag)	//force stereo
	{
		uc_fm_write_bytes[0] = (uc8)((BK1080reg[2]&0xdfff)>>8);	 		//stereo  bit13 0
	}
	else			//force mono
	{
		uc_fm_write_bytes[0] = (uc8)((BK1080reg[2]|0x2000)>>8);	 		//mono	bit13 1
	}
	uc_fm_write_bytes[1] = (uc8)(BK1080reg[2]);
	_fm_i2c_write(2);
}



/*******************************************************************************
Function: Api_RadioAutoSearch()
Description: check the high/low side injection, and set uc_fm_write_bytes[2] bit4 "HLSI"
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
	ui_seek_value =	Api_FreqdL_table[(uc8)(search_para>>8) - 1];
	ui_envelop_value =	Api_FreqdH_table[(uc8)(search_para>>8) - 1];

	if(search_flg == 0)
	{	switch(uc_fm_mode)
		{
			case FM_MODE_HALFSEARCH_INC:	//halfsearch inc freq
			case FM_MODE_HALFSEARCH_DEC:	//halfsearch dec freq
				fm_tuning();
				break;
					 
			default:
		  		if(uc_fm_mode != FM_MODE_AUTOSEARCH)//initial variants while first search start
				{
    				uc_fm_mode = FM_MODE_AUTOSEARCH; //auto search mode
			
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					ui_fm_cur_freq_bk = ui_fm_freq_limit_low;

					Api_RadioSetFreq(ui_fm_cur_freq);
					_delay_1ms(100); 		// delay 100ms
					ui_fm_cur_freq = ui_fm_freq_limit_low - FM_AUTO_STEP;
				
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
			goto end_tuner_search;
		}
		else
		{
			goto auto_search_one_tune;
		}
	}
	else		//half search
	{
		if((uc_fm_mode & FM_MODE_HALFSEARCH) == 0)
		{
			return;	//half search finished
		}
		else
		{
			if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)	//half search -0.1MHz
			{
				tmp_fm_freq = ui_fm_cur_freq - FM_AUTO_STEP;
				if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#if 0
				else if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#endif
				else
				{
					goto auto_search_one_tune;
				}

			}
			else	//half search +0.1MHz(bit3 = 0)
			{
				if(tmp_fm_freq > ui_fm_freq_limit_high)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_low;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#if 0
				else if(tmp_fm_freq < ui_fm_freq_limit_low)
				{
					ui_fm_cur_freq = ui_fm_freq_limit_high;
					uc_fm_valid_cnt = 0;	//avoid border skip(while reach border but uc_fm_valid_cnt != 0)
				}
				#endif
				else
				{
					goto auto_search_one_tune;
				}
			}

half_auto_one_time_finish:					//half searching end
			uc_fm_valid_cnt=0;	
			uc_fm_valid_station_flg=0;
			uc_fm_mode = FM_MODE_TUNING;	//frequency modulation mode
  			goto end_tuner_search;
		}
	}
	
	return;

end_tuner_search:
	Api_RadioSetFreq(ui_fm_cur_freq);
	return;

auto_search_one_tune:
	 ui_fm_cur_freq = tmp_fm_freq;
	 MR_WDT = 0xff;					//clr watchdog

	 if(uc_fm_mode == FM_MODE_HALFSEARCH_DEC)
	 {
		uc_fm_valid_station_flg = fm_search_one_freq(0);
	 }
	 else
	 {		
		uc_fm_valid_station_flg = fm_search_one_freq(1);
	 }
		   
	if(uc_fm_valid_station_flg == 1)
	{	
		if(uc_fm_mode == FM_MODE_AUTOSEARCH)			//auto search
		{	
			uc_fm_valid_cnt=0;	
			uc_fm_valid_station_flg=0;
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


uc8 fm_search_one_freq(uc8 searchmode)
{ 
 	uc8 snr,afc,rssi,seekover;
	ui16 ch,freqd;

	searchmode = 0;
	seekover = 0;

	ch = 0;
	ui_fm_cur_freq = ui_fm_cur_freq & 0x7fff;

	if(uc_fm_band == 0)
	{
		if(ui_fm_cur_freq >= 8750)
		{
    		ch = (ui_fm_cur_freq-8750)/10;
		}
		else
		{
    		ch = 0;
		}
	}
	else
	{
    	ch = (ui_fm_cur_freq-7600)/10;
	}
	uc_fm_write_bytes[0] = (uc8)((ch>>8)|0x80);	//set high ch and ture
	uc_fm_write_bytes[1] = (uc8)(ch);				//set low ch 
	_fm_i2c_write(3);

	uc_fm_write_bytes[0] &= 0x7f;				//set disable ture
	_fm_i2c_write(3);	
	
	_delay_1ms(100); 		// delay 100ms

	_fm_i2c_read(0x07);
	freqd = (ui16) (uc_fm_read_bytes[0] <<8)|(uc_fm_read_bytes[1]); 
	freqd = freqd >>4 ;
    snr = uc_fm_read_bytes[1] & 0x0f;

	_fm_i2c_read(0x0a);
	rssi = uc_fm_read_bytes[1];
	afc  = uc_fm_read_bytes[0]&0x10;
	
	if(!afc)
	{
		if((rssi>=10)&&(snr>=2))	
		{
			if((ui_fm_cur_freq - ui_fm_cur_freq_bk) == 1)
			{
				return 0;
		   	}
			else if((ui_fm_cur_freq_bk - ui_fm_cur_freq) == 1)
			{
				return 0;
			}
			seekover=1;
		}
	  	else 
		{
			seekover=0;
			ui_fm_cur_freq_bk =	 ui_fm_cur_freq;
		}
    }
  	else
	{
 	   seekover=0;	    
	   ui_fm_cur_freq_bk = ui_fm_cur_freq;
	
	} 
	if((freqd>=ui_seek_value)&&(freqd<=(ui_envelop_value-ui_seek_value)))
	{
		seekover=0;	 
		ui_fm_cur_freq_bk =	ui_fm_cur_freq;
	}
    
	if(ui_fm_cur_freq ==9600)
	{
		seekover=0;	  
	 	ui_fm_cur_freq_bk = ui_fm_cur_freq;
	}
	 
	return seekover;
}
 
#endif
