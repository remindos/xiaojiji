/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: drv_fm_var.asm
Creator: andyliu					Date: 2009-07-18
Description:
Others:
Version: V0.1
History:
	V0.1	2009-07-18		andyliu
			1. build this module
********************************************************************************/
#include "sh86278_compiler.h"
#include "drv_fm.h"
#ifdef DRV_FM_VAR_DEF


PUBLIC	uc_fm_high_injection_flg
PUBLIC	uc_fm_ch_total_num
PUBLIC	uc_fm_cur_ch_idx
PUBLIC	uc_fm_valid_station_flg
PUBLIC	uc_fm_valid_cnt
PUBLIC	uc_fm_signal_strength
PUBLIC	uc_fm_ifc_er_cnt
PUBLIC	uc_fm_signal_er_cnt
PUBLIC	uc_fm_signal_th_acc
PUBLIC	uc_fm_signal_th_acc_max
PUBLIC	uc_fm_mode
PUBLIC	uc_fm_signal_strength_val
PUBLIC	uc_fm_accumulative_val
PUBLIC	uc_radio_status

PUBLIC	ui_fm_freq_limit_low
PUBLIC	ui_fm_freq_limit_high
PUBLIC	ui_fm_first_valid_freq
PUBLIC	ui_fm_cur_freq_bk
PUBLIC	ui_fm_cur_freq
PUBLIC	ui_seek_value
PUBLIC	ui_envelop_value

PUBLIC	uc_fm_write_bytes
PUBLIC	uc_fm_read_bytes
PUBLIC	ui_fm_preset_buf
PUBLIC	uc_fm_band
PUBLIC	uc_fm_band_bk

/*******************************************************************************
Dont Modify These Variable
the xdata address is mapping fm driver
********************************************************************************/
	XSEG	AT	7250H
uc_radio_status:			DS		1
uc_fm_mode:					DS		1
ui_fm_cur_freq:				DS		2
uc_fm_cur_ch_idx:			DS		1
uc_fm_ch_total_num:			DS		1
uc_fm_signal_strength_val:	DS		1
uc_fm_accumulative_val:		DS		1
ui_fm_freq_limit_low:		DS		2
ui_fm_freq_limit_high:		DS		2
ui_fm_preset_buf:			DS		FM_CH_NUM_MAX*2
uc_fm_band:					DS		1
uc_fm_band_bk:				DS		1

/*User add variable in below*/
	XSEG	AT	7300H

uc_fm_write_bytes:			DS		FM_READ_BYTES_NUM
uc_fm_read_bytes:			DS		FM_WRITE_BYTES_NUM
uc_fm_high_injection_flg:	DS		1
uc_fm_valid_station_flg:	DS		1
uc_fm_valid_cnt:			DS		1
uc_fm_signal_strength:		DS		1
uc_fm_ifc_er_cnt:			DS		1
uc_fm_signal_er_cnt:		DS		1
uc_fm_signal_th_acc:		DS		1
uc_fm_signal_th_acc_max:	DS		1
ui_fm_first_valid_freq:		DS		2
ui_fm_cur_freq_bk:			DS		2
ui_seek_value:				DS		2
ui_envelop_value:			DS		2

#endif
	END