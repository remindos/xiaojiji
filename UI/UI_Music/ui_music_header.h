/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: music_header.h
Creator: zj.zong			Date: 2009-04-29
Description: define Macro in music mode
Others:
Version: V0.1
History:
	V0.1	2009-04-29		zj.zong
			1. build this module
********************************************************************************/
#ifndef UI_MUSIC_HEADER
#define UI_MUSIC_HEADER


// FOR NEXT ACTION 
#define	ACTION_POWEROFF	1
#define ACTION_STOP		2
#define	ACTION_RECORD	3
#define ACTION_NEXT_BOOK	4
#define ACTION_SCAN_DISK	5
#define ACTION_VOICE	6
#define ACTION_RET_PLAY	7
#define	ACTION_BAT_LOW	8

#define	KEY_SOUND_BASE		0x003
#define KEY_SOUND_PLAY		3
#define KEY_SOUND_DIR		4
#define KEY_SOUND_NEXT		6
#define KEY_SOUND_LAST		5
#define	KEY_SOUND_VOL_INC	7
#define	KEY_SOUND_VOL_DEC	8


#define KEY_SOUND_POWEROFF	2
#define KEY_SOUND_ER		3
#define KEY_SOUND_GS		4
#define KEY_SOUND_TS		5
#define KEY_SOUND_GX		6
#define KEY_SOUND_BAT_LOW	KEY_SOUND_BASE+6

/***********************System Menu Number Define***********************/
#define	MUSIC_PLAY_MENU_NUM			5	//6
#define MUSIC_STOP_MENU_NUM			4
#define	MUSIC_EQ_MENU_NUM			4	//8
#define	MUSIC_REPEAT_MENU_NUM		3	//7
#define	MUSIC_RRL_TIME_MENU_NUM		10
#define	MUSIC_RRL_GAP_MENU_NUM		10
#define	MUSIC_DELETE_MENU_NUM		2
#define MUSIC_LRC_OPEN_MENU_NUM		2

/************************lrc macro define*****************************/
#define LRC_BUFFER_MAX		6144		//Common buffer 6*1024 word
#define LRC_LABEL_NUM_MAX	150			//100			//the maximum number of time label
#define LRC_ONE_LABEL_MAX	65			//the maximum word in one label
//#define LRC_FLASH_ADDRESS	0			//0000 0000~0000 0063
#define LRC_FLASH_ADDRESS	0x0300			//0000 0000~0000 0063



/***************************music play state**************************/
#define STATE_PLAY			1
#define STATE_PAUSE			2
#define STATE_STOP			3
#define STATE_FORWARD		4
#define STATE_BCKWARD		5
#define MPEG_STATE_ERROR	0xA5


/****************************repleat mode*****************************/
#define REPEAT_ALL			0
#define REPEAT_NORMAL	 	4
#define REPEAT_FOLDER	 	2
#define	REPEAT_FOLDER_NOR	3
#define REPEAT_ONE			1
#define REPEAT_RANDOM	 	5
#define REPEAT_PREVIEW	 	6



/*********************Window Define In Music mode**********************/
#define	WINDOW_MUSIC				0
#define	WINDOW_MUSIC_PLAY_MENU		1
//#define	WINDOW_MUSIC_STOP_MENU		2
#define	WINDOW_MUSIC_MENU_SD		2	 //allen
#define	WINDOW_MUSIC_MENU_FLASH		3	 //allen
#define	WINDOW_MUSIC_MENU_EQ		4
#define	WINDOW_MUSIC_MENU_REPEAT	5
#define	WINDOW_MUSIC_MENU_RTC_TIME	6
#define	WINDOW_MUSIC_MENU_RPL_GAP	7
#define WINDOW_MUSIC_VOLUME			8
#define	WINDOW_MUSIC_MENU_BROWSER	9
#define	WINDOW_MUSIC_MENU_DEL		10
#define WINDOW_MUSIC_MENU_DELALL	11

#define	WINDOW_SYS_MENU						12
#define	WINDOW_SYS_MENU_BKLIGHT_TIME		13
#define	WINDOW_SYS_MENU_CONTRAST			14

#define WINDOW_SYS_POWEROFF			15
#define	WINDOW_SYS_LOCKKEY			16
#define	WINDOW_SYS_RTCADJ			17

#define WINDOW_MUSIC_TOP_MENU		18
#define WINDOW_MUSIC_INPUT			19

#define WINDOW_MUSIC_LOGO			20
#define WINDOW_MUSIC_BYEBYE			21
#define WINDOW_MUSIC_SHOW_LOCK		22
#define WINDOW_MUSIC_NUM			23

#define	WINDOW_MUSIC_BOOK			24
/*****************Music Display Column & Row Address*******************/
#define DisMenu_MaxNum			3//6
#define	Dis_BrowserItem_MaxNum	3
#define	Dis_Music_Freq_Num		7


#endif