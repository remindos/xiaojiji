/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_resident.h
Creator: zj.zong					Date: 2009-04-14
Description:  Macro Define (These macro define for all mode:music,rec,voice...)
Others:
Version: V0.1
History:
	V0.1	2009-04-14		zj.zong
			1. build this module
********************************************************************************/
#ifndef PM1_RESIDENT_DEF
#define PM1_RESIDENT_DEF

#include "sh86278_memory_assign.h"




/*******************Battery level define*********************/

#ifdef LDO_1650mv				/*** 1.65V ***/
	#define BATTERY_LEVEL_0		 		163	//1.06V 3.365V
	#define BATTERY_LEVEL_1		 		173		//1.12V 3.555V
	#define BATTERY_LEVEL_2		 		183		//1.18V 3.745V
	#define BATTERY_LEVEL_3		 		192		//1.24V 3.935V
	#define BATTERY_LEVEL_4		 		201		//1.30V 4.126V
	#define BATTERY_CHARGE_FULL			206		//1.33V 4.22V
#endif
#ifdef LDO_1700mv				/*** 1.70V ***/
	#define BATTERY_LEVEL_0		 		158		//1.06V 3.365V
	#define BATTERY_LEVEL_1		 		169		//1.12V 3.555V
	#define BATTERY_LEVEL_2		 		178		//1.18V 3.745V
	#define BATTERY_LEVEL_3		 		187		//1.24V 3.935V
	#define BATTERY_LEVEL_4		 		196		//1.30V 4.126V
	#define BATTERY_CHARGE_FULL			200		//1.33V 4.22V
#endif
#ifdef LDO_1800mv				/*** 1.80V ***/		
	#define BATTERY_LEVEL_0		 		149		//1.06V 3.365V
	#define BATTERY_LEVEL_1		 		159		//1.12V 3.555V
	#define BATTERY_LEVEL_2		 		167		//1.18V 3.745V
	#define BATTERY_LEVEL_3		 		176		//1.24V 3.935V
	#define BATTERY_LEVEL_4		 		184		//1.30V 4.126V
	#define BATTERY_CHARGE_FULL			188		//1.33V 4.22V
#endif

#ifdef LDO_1900mv				/*** 1.90V ***/
	#define BATTERY_LEVEL_0		 		133//141//158		//1.053V 3.295V
	#define BATTERY_LEVEL_1		 		150//153//169		//1.12V 3.555V
	#define BATTERY_LEVEL_2		 		159//160//178		//1.18V 3.745V
	#define BATTERY_LEVEL_3		 		165//165//187		//1.24V 3.935V
	#define BATTERY_LEVEL_4		 		172//171//177//196		//1.30V 4.126V
	#define BATTERY_CHARGE_FULL			180//180//181//200		//1.33V 4.22V
#endif

#define BATTERY_VALID_CNT		5
#define	BATTERY_LOW_CNT			3


/*******************define timer counter*********************/
#define TIME_DETECT_VOLTAGE			2		//2*100ms
#define TIME_EXIT_VOLUME			50		//50*100ms	for music & voice & fm
#define TIME_EXIT_MENU				50		//50*100ms
#define TIME_REFRESH_FLG			6		//6*100ms
#define TIME_REFRESH_REC			3		//300ms
#define TIME_REFRESH_CURRENT_TIME	5		//500ms
#define TIME_REFRESH_VOICE_FREQ		5		//500ms
#define TIME_REFRESH_FREQ			3		//300ms

#define	TIME_POWERON_DIS			120		//150*1ms	for power on display
#define TIME_REFRESH_LRC			10		//10*100ms	for music
#define TIME_REFRESH_ID3			7		//7*100ms	for music
#define TIME_FLICKER_AB				4		//4*100ms	for music & voice
#define TIME_1S						10		//10*100ms	for music & voice
#define TIME_FAST					1		//1*100ms	for music & voice
#define TIME_PREVIEW				10		//10s		for music & voice
#define	TIME_DETECT_HEADPHONE		8		//8*100ms	for music & voice & movie & FM
#define	VOLUME_LEVEL_MAX			31		//0~31

/*******************Notice String Index **********************/

#define ALL_MENU_NUMBER				512				//Menu
#define	STRING_START_IDX			0

#define NORM_STR_START_IDX					STRING_START_IDX + 0

#define NORM_STR_SD_NOFILE_IDX				NORM_STR_START_IDX + 32
#define NORM_STR_FLASH_NOFILE_IDX			NORM_STR_START_IDX + 33	  
#define	NORM_STR_RECOVERFLOW_IDX			NORM_STR_START_IDX + 34	
#define	NORM_STR_LOW_VOL_IDX				NORM_STR_START_IDX + 35
#define	NORM_STR_NOSPACE_IDX				NORM_STR_START_IDX + 36
#define	NORM_STR_DELETE_FILE_IDX			NORM_STR_START_IDX + 37
#define	NORM_STR_DELETING_IDX				NORM_STR_START_IDX + 38
#define	NORM_STR_LOADING_IDX				NORM_STR_START_IDX + 39
#define NORM_STR_FILEERR_IDX				NORM_STR_START_IDX + 40	  
#define NORM_STR_FLASH_ERR_IDX				NORM_STR_START_IDX + 41
#define NORM_STR_NO_CHANNEL_IDX				NORM_STR_START_IDX + 42
#define NORM_STR_SAVING_IDX					NORM_STR_START_IDX + 43
#define NORM_STR_NOCONTENT_IDX				NORM_STR_START_IDX + 44

#define NORM_STR_IN_IDX						NORM_STR_START_IDX + 45
#define NORM_STR_OUT_IDX					NORM_STR_START_IDX + 46
#define NORM_STR_IMPORTING_IDX				NORM_STR_START_IDX + 47
#define NORM_STR_EXPORTING_IDX				NORM_STR_START_IDX + 48
#define NORM_STR_IMPORT_OK_IDX				NORM_STR_START_IDX + 49
#define NORM_STR_EXPORT_OK_IDX				NORM_STR_START_IDX + 50
#define NORM_STR_NO_TELFILE_IDX				NORM_STR_START_IDX + 51
#define NORM_STR_PROTECT_CARD_IDX			NORM_STR_START_IDX + 53
#define NORM_STR_DRAW_CARD_IDX			    NORM_STR_START_IDX + 52

/**************Menu String Index In display menu****************/
#define MENU_STR_START_IDX						STRING_START_IDX + 0
/*MainMenu*/
#define MENU_STR_MAIN_MENU_IDX					MENU_STR_START_IDX + 70


/*music*/
#define MENU_STR_MUSIC_MENU_IDX					MENU_STR_START_IDX + 80
#define MENU_STR_MUSIC_MENU_EQ_IDX				MENU_STR_START_IDX + 86
#define MENU_STR_MUSIC_MENU_REPEAT_IDX			MENU_STR_START_IDX + 94
#define	MENU_STR_MUSIC_SYS_MENU_IDX				MENU_STR_START_IDX + 90
#define	MENU_STR_MUSIC_RTC_IDX					MENU_STR_START_IDX + 97
#define MENU_STR_MUSIC_STOP_MENU_IDX			MENU_STR_START_IDX + 146
#define MENU_STR_MUSIC_FLASH_MENU_IDX			MENU_STR_START_IDX + 210  //allen
#define MENU_STR_MUSIC_SD_MENU_IDX			    MENU_STR_START_IDX + 209
/*rec*/
#define MENU_REC_QUALITY_BASE_IDX				MENU_STR_START_IDX + 101
#define MENU_STR_REC_MENU_SD_IDX				MENU_STR_START_IDX + 214   //allen
#define MENU_STR_REC_MENU_FLASH_IDX		     	MENU_STR_START_IDX + 216 //allen
/*voice*/
#define MENU_STR_VOICE_MENU_REPEAT_IDX			MENU_STR_START_IDX + 96
#define MENU_STR_VOICE_MENU_PLAY_IDX			MENU_STR_START_IDX + 103
#define MENU_STR_VOICE_MENU_STOP_IDX			MENU_STR_START_IDX + 106
#define MENU_STR_VOICE_MENU_STOP_SD_IDX			MENU_STR_START_IDX + 209
#define MENU_STR_VOICE_MENU_STOP_FLASH_IDX		MENU_STR_START_IDX + 211
/*** EBOOK ***/
#define MENU_STR_EBOOK_MENU_SD_IDX				MENU_STR_START_IDX + 218 //allen
#define MENU_STR_EBOOK_MENU_FLASH_IDX			MENU_STR_START_IDX + 219						
/*system*/
#define MENU_STR_SYS_MENU_LANGUAGE_IDX			MENU_STR_START_IDX + 0
#define MENU_STR_SYS_MENU_IDX					MENU_STR_START_IDX + 116
#define MENU_STR_SYS_MENU_POWEROFF_IDX			MENU_STR_START_IDX + 127
#define MENU_STR_SYS_MENU_REPLAY_IDX			MENU_STR_START_IDX + 130
#define PIC_SYS_STR_MENU_AMT_IDX				MENU_STR_START_IDX + 132
#define MENU_STR_SYS_MENU_FACTORY_IDX			MENU_STR_START_IDX + 134
#define MENU_STR_SYS_MENU_DISK_TYPE_IDX			MENU_STR_START_IDX + 136
#define MENU_STR_SYS_MENU_BKLIGHT_TIME_IDX		MENU_STR_START_IDX + 170
#define MENU_STR_SYS_MENU_POWEROFF_IDLE_IDX		MENU_STR_START_IDX + 177
#define MENU_STR_SYS_MENU_POWEROFF_SLEEP_IDX	MENU_STR_START_IDX + 184
#define	MENU_STR_SYS_MENU_LOCKKEY_IDX			MENU_STR_START_IDX + 150	//WANG

/*FM*/		  
#define MENU_STR_FM_MENU_IDX					MENU_STR_START_IDX + 197

/*TEL*/
#define	MENU_TEL_STR_MENU_IDX				   	MENU_STR_START_IDX + 205

/************************8x6_ZIKU Index**************************/
#define ADRESS_8x6_ZIKU				   			0x4800

/*****************Power On and Poweroff picture*********************/
#define PIC_POWERON_BASE_IDX					0x4100
#define PIC_POWEROFF_BASE_IDX					0x4110

/************************Picture Index**************************/

#define	PIC_MAIN_MENU_NO_EBOOK_TEL_BASE_IDX		 0x422C
#define	PIC_MAIN_MENU_NO_FM_EBOOK_TEL_BASE_IDX	 0x4233
#define	PIC_MAIN_MENU_HAVE_MUSIC_FM_SYS_BASE_IDX 0x423A
#define	PIC_MAIN_MENU_HAVE_MUSIC_SYS_BASE_IDX	 0x4241
#define	PIC_MAIN_MENU_NO_VOICE_REC_BASE_IDX		 0x421E
#define	PIC_MAIN_MENU_NO_VOICE_REC_FM_BASE_IDX	 0x4225
#define	PIC_MAIN_MENU_BASE_IDX				     0x4210
#define	PIC_MAIN_MENU_NO_FM_BASE_IDX			 0x4217
#define	PIC_BATTERY_COMMON_BASE_IDX		0x4080
/*music*/
#define PIC_NUMBER_BASE_IDX			  	0x4000
#define PIC_VOLUME_FLAG_IDX			  	0x4060

#define PIC_MUSIC_FLAG_IDX			  	0x4061
#define PIC_MUSICLRC_FLAG_IDX		  	0x4062
#define PIC_STATE_BASE_IDX				0x4086
#define PIC_MUSIC_REPEAT_BASE_IDX		0x408B
#define PIC_MUSIC_AB_BASE_IDX			0x4092
#define PIC_MUSIC_EQ_BASE_IDX			0x4095
#define PIC_MUSIC_FORMAT_IDX			0x40C0
#define PIC_ROOT_NAME_IDX				0x40E4
#define PIC_MUSIC_FREQ_BASE_IDX			0x4140
#define PIC_MUSIC_SD_NOFLIE_IDX			0x4141
#define PIC_MUSIC_FLASH_NOFLIE_IDX	    0x4142

#define PIC_REPLAY_GAP_TIME_BASE_IDX	0x41F0
#define PIC_MENU_YES_NO_IDX				0x415E
#define PIC_MENU_ALL_YES_NO_IDX			0x415C
#define PIC_VOLUME_BAST_IDX				0x4180
#define PIC_MUSIC_FILEBROW_IDX			0x4156
/*record*/
#define PIC_REC_REFRESH_IDX			    0x4069
#define PIC_REC_FALG_IDX			    0x4063
#define PIC_REC_STATE_BASE_IDX		    0x4066

/*VOICE*/
#define PIC_VOICE_FLAG_IDX				0x4064
#define PIC_VOICE_AB_BASE_IDX			PIC_MUSIC_AB_BASE_IDX
#define PIC_VOICE_REPEAT_BASE_IDX		PIC_MUSIC_REPEAT_BASE_IDX+2
#define PIC_VOICE_MENU_REPEAT_IDX		0x4092
#define PIC_VOICE_DRI_IDX				0x40AF
#define PIC_VOICE_FREQ_IDX				0x4130
#define PIC_VOICE_MENU_DEL_ALL_IDX		PIC_MENU_ALL_YES_NO_IDX
#define PIC_VOICE_MENU_DEL_IDX			PIC_MENU_YES_NO_IDX
#define PIC_VOICE_8K_IDX				0x4170
#define PIC_VOICE_16K_IDX				0x4173

/*usb*/
#define PIC_USB_STATUS_IDEL_IDX			0x40C1
#define PIC_USB_DOWN_LOAD_IDX			0x40C3
#define PIC_USB_UP_LOAD_IDX				0x40C2
/*FM*/
#define	PIC_FM_FLAG_IDX					0x4065
#define	PIC_FM_NOR_BAND_LOW_IDX			0x4150
#define	PIC_FM_JP_BAND_LOW_IDX			0x4151
#define	PIC_FM_POINT_IDX				0x4152
#define	PIC_FM_IDX						0x4153
#define	PIC_FM_MHZ_IDX					0x4154
#define	PIC_FM_CH_YES_IDX				0x4155
#define	PIC_FM_DELCH_IDX				0x4159
#define	PIC_FM_ALL_YES_NO_IDX			0x415A
#define	PIC_SIGNAL_BAST_IDX			  	0x4200
/*ebook*/
#define PIC_FILEBROW_FOLDER_IDX		  	0x40E8
#define PIC_FILEBROW_FILE_IDX			0x40EA
#define PIC_NOTICE_YES_NO_IDX			PIC_MENU_YES_NO_IDX

/*system*/
#define PIC_MENU_NOSEL_IDX				0x40e5
#define PIC_MENU_SEL_IDX				0x40e6
#define PIC_SYS_MEMORYINFO_IDX			0x40CE
#define PIC_SYS_VERSION_IDX				0x40D8
#define PIC_SELECT_FLAG_IDX				0x4138
#define PIC_SYS_BKGRADE_BASE_IDX		0x41C0

/*tel*/
#define PIC_TEL_INFO_LIST_IDX		  	0x4250

/*****************IO Assigned Define********************/
#define BIT0_MASK	(1<<0)
#define BIT1_MASK	(1<<1)
#define BIT2_MASK	(1<<2)
#define BIT3_MASK	(1<<3)
#define BIT4_MASK	(1<<4)
#define BIT5_MASK	(1<<5)
#define BIT6_MASK	(1<<6)
#define BIT7_MASK	(1<<7)

#define	SPEAKER_CONTROL_MASK	BIT1_MASK		//PA1

//#define	BKLIGHT_MASK			BIT4_MASK		//PB4
//#define MIC_POWER_MASK			BIT4_MASK		//PC4

#define PLAYKEY_MASK			BIT0_MASK		//PA0
#define	DISK1_MASK				BIT0_MASK		//disk 1
#define DISK2_MASK				BIT1_MASK		//disk 2
/**************************************/
#define HEADPHONE_DETECT_MASK	BIT7_MASK		//
#define IR_PA1PIN_MASK			BIT7_MASK		//PA7
#define	RLED_PA3PIN_MASK		BIT3_MASK		//PA3
#define	GLED_PA4PIN_MASK		BIT4_MASK		//PA4
#define	BLED_PA5PIN_MASK		BIT5_MASK		//PA5
#define	WLED_PA6PIN_MASK		BIT6_MASK		//PA6
#define MODE1KEY_MASK			BIT1_MASK
#define MODE2KEY_MASK			BIT3_MASK




/***********************Main Mode(11)*************************/
#define MODE_0_MAIN_MENU			0
#define MODE_1_MUSIC				1
#define MODE_2_RECORD				2
#define MODE_3_VOICE				3
#define MODE_4_FM			    	4
#define MODE_5_SYSTEM				5
#define MODE_6_EBOOK				6
#define MODE_7_TELBOOK				7


/*****************Dynamic loading Code idx*******************/

#define	CODE_POWERON_MODE_IDX			1
#define CODE_POWEROFF_MODE_IDX			2
#define CODE_MAINMENU_IDX			    3
#define	CODE_SYS_MODE_IDX		     	4
#define CODE_USB_MODE_IDX			    5
#define CODE_USB_MODE_CPM_IDX			6


#define	CODE_MUSIC_0_MAIN_IDX			10
#define CODE_MUSIC_1_INIT_IDX			11
#define CODE_MUSIC_2_GETID3_IDX			12
#define CODE_MUSIC_3_PROID3_IDX			13
#define CODE_MUSIC_4_LRC_IDX			14
#define	CODE_MUSIC_5_GET_ITEM_NUM_IDX	15
#define CODE_MUSIC_6_GET_ITEM_NAME_IDX	16
#define CODE_MUSIC_7_SELFILE_IDX		17
#define CODE_MUSIC_8_DELETE_IDX			18
#define	CODE_MUSIC_9_FSEEK_IDX			19
#define	CODE_MUSIC_A_SAVE_PARA_IDX		20
#define	CODE_MUSIC_B_SUBFUNC_IDX		21
#define CODE_MUSIC_C_SWITCH_CARD_IDX    22
#define	CODE_MUSIC_D_MAIN_IDX			23
#define	CODE_MUSIC_0_MAIN_CPM_IDX		24 //allen


#define	CODE_RECORD_0_MAIN_IDX			27
#define	CODE_RECORD_1_INIT1_IDX		    28
#define	CODE_RECORD_2_INIT2_IDX		    29
#define	CODE_RECORD_4_START_IDX			30
#define	CODE_RECORD_5_STOP_IDX			31
#define	CODE_RECORD_6_SAVE_IDX			32
#define	CODE_RECORD_7_GET_NEW_FILE_IDX  33
#define	CODE_RECORD_8_DYNAMIC_DISPLAY_IDX  34
#define	CODE_RECORD_A_SWITCH_CARD_IDX	 35 //allen
#define	CODE_RECORD_9_MAIN_IDX			 36
#define	CODE_RECORD_0_MAIN_CPM_IDX	     37 //allen


#define	CODE_VOICE_0_MAIN_IDX			40
#define	CODE_VOICE_1_INIT1_IDX			41
#define	CODE_VOICE_2_INIT2_IDX			42	
#define	CODE_VOICE_3_INIT_PLAY_IDX		43
#define	CODE_VOICE_4_SEL_FILE_IDX		44
#define	CODE_VOICE_5_FSEEK_IDX			45
#define	CODE_VOICE_6_DEL_FILE_IDX		46
#define	CODE_VOICE_7_SAVE_IDX			47
#define	CODE_VOICE_9_SWITCH_CARD_IDX	48	  //ALLEN
#define	CODE_VOICE_8_MAIN_IDX			49
#define	CODE_VOICE_0_MAIN_CPM_IDX		50 //allen


#define	CODE_FM_0_MAIN_IDX		        53
#define	CODE_FM_1_RESTORE_PARA_IDX		54
#define CODE_FM_2_SAVE_PARA_IDX			55
#define CODE_FM_3_DRVIER_TEA5767_IDX	56
#define CODE_FM_4_DRVIER_5800C_IDX		57
#define CODE_FM_5_DRVIER_AR1010_IDX		58
#define CODE_FM_6_DRVIER_CL6017S_IDX	59
#define CODE_FM_7_DRVIER_SI4702_IDX		60
#define CODE_FM_8_DRVIER_Rda5807SP_IDX	61
#define	CODE_FM_9_DRVIER_BK1080_IDX		62
#define	CODE_FM_A_MAIN_IDX		        63

#define	CODE_EBOOK_0_MAIN_IDX			67
#define CODE_EBOOK_1_INIT_IDX			68
#define CODE_EBOOK_2_INIT_PLAY_IDX		69
#define CODE_EBOOK_3_GET_PAGE_NUM_IDX   70
#define CODE_EBOOK_4_SAVE_IDX			71
#define CODE_EBOOK_5_DEL_IDX			72
#define CODE_EBOOK_6_GET_NUM_IDX		73
#define CODE_EBOOK_7_GET_NAME_IDX		74
#define	CODE_EBOOK_8_SWITCHCARD_IDX		75
#define	CODE_EBOOK_8_MAIN_IDX			76
#define	CODE_EBOOK_0_MAIN_CPM_IDX		77 //allen

#define CODE_TEL_0_MAIN_IDX		     	80
#define CODE_TEL_1_DEL_OLD_FILE_IDX		81
#define CODE_TEL_2_IMPORT_IDX			82
#define CODE_TEL_3_EXPORT_FILE_IDX		83
#define CODE_TEL_4_CHECK_FILE_IDX		84
#define CODE_TEL_5_MAIN_IDX		    	85


/*these code index has not be changed*/
#define CODE_MPEG_VOICE_AUDIO_DCM_SB_IDX		96
#define CODE_MPEG_VOICE_AUDIO_DCM_IDX			97
#define CODE_MPEG_VOICE_AUDIO_CBUF_IDX			98

#define CODE_MPEG_AUDIO_DCM_SB_IDX		102
#define CODE_MPEG_AUDIO_DCM_IDX			103
#define CODE_MPEG_AUDIO_CBUF_IDX		104



/****************System Event Define*******************/
#define EVT_BATTERY				0x0001
#define	EVT_TIMER				0x0002
#define	EVT_KEY					0x0004
#define	EVT_USB					0x0008
#define	EVT_DISPLAY				0x0010
#define EVT_MSI					0x0020

#define	EVT_MUSIC_FILE_ERR		0x0100
#define	EVT_MUSIC_FILE_END		0x0200
#define	EVT_MUSIC_FILL_DECBUF	0x0400
#define	EVT_MUSIC_AB_REPLAY		0x0800

#define	EVT_REC_ENCODE			0x0100
#define	EVT_REC_SEEK_NEXT_PAGE	0x0200

#define	EVT_VOICE_FILE_ERR		0x0100
#define	EVT_VOICE_FILE_END		0x0200
#define	EVT_VOICE_FILL_DECBUF	EVT_MUSIC_FILL_DECBUF	//0x0400	(for lcd_api.asm)
#define	EVT_VOICE_AB_REPLAY		0x0800

#define	EVT_FM_SEARCH			0x0100

#define	EVT_PHOTO_FILE_ERR		0x0100
#define	EVT_PHOTO_AUTO_PLAY		0x0200
#define	EVT_PHOTO_FILL_DECBUF	0x0400

#define	EVT_MOVIE_FILE_ERR		0x0100
#define	EVT_MOVIE_FILE_END		0x0200
#define	EVT_MOVIE_FILL_DECBUF	0x0400

/*****************Key Valuee Define********************/
#define	KEY_STATE_S		00H		//Key State
#define	KEY_STATE_L		20H
#define	KEY_STATE_LC	40H
#define	KEY_STATE_LE	80H

#define	STATE_S			0x00
#define STATE_L			0x20
#define STATE_LC		0x40
#define STATE_LE		0x80
						//Key Value		 1800mv			1650mv
#define	KEYVAL_1		01H				//0.212			0.194
#define	KEYVAL_2		02H				//0.485			0.446
#define	KEYVAL_3		03H				//0.778			0.713
#define	KEYVAL_4		04H				//1.195			1.095
#define	KEYVAL_5		05H				//1.462			1.340
#define	KEYVAL_6		06H
#define	KEYVAL_7		07H
#define	KEYVAL_8		08H
#define	KEYVAL_9		09H
#define	KEYVAL_B		0BH
#define	KEYVAL_PLAY		0AH
#define	KEYVAL_HOLD		0BH
#define	KEYVAL_NULL		1FH

#define KEY_S_TIME		2		//(2+1)*20ms
#define KEY_L_TIME		40		//40*20ms
#define KEY_LC_TIME		60		//60*20ms



/****************Key State | Key Value*****************/
/*#define KEY_MODE1_S		0x01|STATE_S
#define KEY_MODE1_L		0x01|STATE_L
#define KEY_MODE1_LC	0x01|STATE_LC
#define KEY_MODE1_LE	0x01|STATE_LE

#define KEY_MODE2_S		0x02|STATE_S
#define KEY_MODE2_L		0x02|STATE_L
#define KEY_MODE2_LC	0x02|STATE_LC
#define KEY_MODE2_LE	0x02|STATE_LE
*/
#define KEY_LASTKEY_S	0x03|STATE_S
#define KEY_LASTKEY_L	0x03|STATE_L
#define KEY_LASTKEY_LC	0x03|STATE_LC
#define KEY_LASTKEY_LE	0x03|STATE_LE

#define KEY_NEXTKEY_S	0x04|STATE_S
#define KEY_NEXTKEY_L	0x04|STATE_L
#define KEY_NEXTKEY_LC	0x04|STATE_LC
#define KEY_NEXTKEY_LE	0x04|STATE_LE


#define KEY_NEXT_S		0x06|STATE_S
#define KEY_NEXT_L		0x06|STATE_L
#define KEY_NEXT_LC		0x06|STATE_LC
#define KEY_NEXT_LE		0x06|STATE_LE

#define KEY_LAST_S		0x07|STATE_S
#define KEY_LAST_L		0x07|STATE_L
#define KEY_LAST_LC		0x07|STATE_LC
#define KEY_LAST_LE		0x07|STATE_LE

#define KEY_VOL_INC_S		0x08|STATE_S 
#define KEY_VOL_INC_L		0x08|STATE_L 
#define KEY_VOL_INC_LC		0x08|STATE_LC
#define KEY_VOL_INC_LE		0x08|STATE_LE

#define KEY_VOL_S			KEY_VOL_INC_S
#define KEY_VOL_L			KEY_VOL_INC_L
#define KEY_VOL_LC			KEY_VOL_INC_LC
#define KEY_VOL_LE			KEY_VOL_INC_LE

#define KEY_VOL_DEC_S		0x05|STATE_S 
#define KEY_VOL_DEC_L		0x05|STATE_L 
#define KEY_VOL_DEC_LC		0x05|STATE_LC
#define KEY_VOL_DEC_LE		0x05|STATE_LE
/*
#define	KEY_MODE_S		0x01|STATE_S 
#define	KEY_MODE_L		0x01|STATE_L 
#define	KEY_MODE_LC		0x01|STATE_LC
#define	KEY_MODE_LE		0x01|STATE_LE
/
#define	KEY_S1_S		0x09|STATE_S 
#define	KEY_S1_L		0x09|STATE_L 
#define	KEY_S1_LC		0x09|STATE_LC
#define	KEY_S1_LE		0x09|STATE_LE

#define	KEY_S2_S		0x04|STATE_S 
#define	KEY_S2_L		0x04|STATE_L 
#define	KEY_S2_LC		0x04|STATE_LC
#define	KEY_S2_LE		0x04|STATE_LE

#define	KEY_S3_S		0x02|STATE_S 
#define	KEY_S3_L		0x02|STATE_L 
#define	KEY_S3_LC		0x02|STATE_LC
#define	KEY_S3_LE		0x02|STATE_LE

#define	KEY_S4_S		0x03|STATE_S 
#define	KEY_S4_L		0x03|STATE_L 
#define	KEY_S4_LC		0x03|STATE_LC
#define	KEY_S4_LE		0x03|STATE_LE

#define	KEY_REC_S		0x06|STATE_S 
#define	KEY_REC_L		0x06|STATE_L 
#define	KEY_REC_LC		0x06|STATE_LC
*/
#define	KEY_PLAY_S		0x0A 
#define KEY_PLAY_L		0x2A
#define KEY_PLAY_LC		0x4A
/*
#define	KEY_MUTE_S		0x0B|STATE_S 
#define KEY_MUTE_L		0x0B|STATE_L 
#define KEY_MUTE_LC		0x0B|STATE_LC

#define KEY_NUM0			0x0C
#define KEY_NUM1			0x0D
#define KEY_NUM2			0x0E
#define KEY_NUM3			0x0F
#define KEY_NUM4			0x10
#define KEY_NUM5			0x11
#define KEY_NUM6			0x12
#define KEY_NUM7			0x13
#define KEY_NUM8			0x14
#define KEY_NUM9			0x15


#define HOLD_0			0xBB
#define HOLD_1			0x0B

#define IR_PLAY_S		0x40
#define IR_MODE_S		0x09
#define IR_MUTE_S		0x47

#define IR_VOL_DEC_S	0x19
#define IR_VOL_INC_S	0x0d

#define IR_LAST_S		0x44
#define IR_NEXT_S		0x43

#define IR_NUM0			0x16
#define IR_NUM1			0x0C
#define IR_NUM2			0x18
#define IR_NUM3			0x5E
#define IR_NUM4			0x08
#define IR_NUM5			0x1C
#define IR_NUM6			0x5A
#define IR_NUM7			0x42
#define IR_NUM8			0x52
#define IR_NUM9			0x4A

#define	IR_S1_S			0x45
#define	IR_S2_S			0x46
#define	IR_S3_S			0x07
#define	IR_S4_S			0x15
*/

#define IR_PLAY_S		0x19//0x04
//#define IR_MODE_S		0x1E//0x1E
//#define IR_MUTE_S		0xfe
//#define IR_POWER		0x0A//0x12

#define IR_VOL_DEC_S	0x18//0x08
#define IR_VOL_INC_S	0x15//0x09

#define IR_LAST_S		0x16//0x05
#define IR_NEXT_S		0x0d//0x06
/*
#define IR_NUM0			0x16//0x07
#define IR_NUM1			0x0F//0x0A
#define IR_NUM2			0x1F//0x1B
#define IR_NUM3			0x15//0x1F
#define IR_NUM4			0x00//0x0C
#define IR_NUM5			0x1D//0x0D
#define IR_NUM6			0x01//0x0E
#define IR_NUM7			0x1C//0x00
#define IR_NUM8			0x11//0x0F
#define IR_NUM9			0x03//0x19

#define	IR_S1_S			0x0E//0x1A
#define	IR_S2_S			0x0B//0x01
#define	IR_S3_S			0x0C//0x02
#define	IR_S4_S			0x10//0x03
*/
/****************Power Off Mode Define******************/
#define POWEROFF_HOLD		0x01
#define POWEROFF_LOW_VOL	0x02
#define POWEROFF_FLASH_ERR	0x03
#define POWEROFF_NORMAL		0x04



/************File system File Type Define***************/
#define FS_FILE_TYPE_MP3		0x0001
#define FS_FILE_TYPE_MMV		0x0002
#define FS_FILE_TYPE_AMV		0x0004
#define FS_FILE_TYPE_WAV		0x0008
#define FS_FILE_TYPE_BMP		0x0010
#define FS_FILE_TYPE_JPG		0x0020
#define FS_FILE_TYPE_TXT		0x0040
#define FS_FILE_TYPE_LRC		0x0080
#define FS_FILE_TYPE_BIN		0x0100
#define FS_FILE_TYPE_AVI		0x0200
#define FS_MAX_SUBDIR_DEPTH		8


#define FS_OPENFILE_TYPE_MP3	0x01
#define FS_OPENFILE_TYPE_MMV	0x02
#define FS_OPENFILE_TYPE_AMV	0x03
#define FS_OPENFILE_TYPE_WAV	0x04
#define FS_OPENFILE_TYPE_BMP	0x05
#define FS_OPENFILE_TYPE_JPG	0x06
#define FS_OPENFILE_TYPE_TXT	0x07
#define FS_OPENFILE_TYPE_LRC	0x08
#define FS_OPENFILE_TYPE_BIN	0x09
#define FS_OPENFILE_TYPE_AVI	0x0A

#endif