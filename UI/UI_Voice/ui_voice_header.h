/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: voice_header.h
Creator: zj.zong					Date: 2007-12-18
Description: define macro in voice mode
Others:
Version: V0.1
History:
	V0.1	2007-12-18		zj.zong
			1. build this module
********************************************************************************/
#ifndef VOICE_HEADER
#define VOICE_HEADER

/***************************voice play state**************************/
#define STATE_PLAY			1
#define STATE_PAUSE			2
#define STATE_STOP			3
#define STATE_FORWARD		4
#define STATE_BCKWARD		5
#define MPEG_STATE_ERROR	0xA5

/****************************repleat mode*****************************/
#define REPEAT_FOLDER		0
#define REPEAT_FOLDER_NOR	1
#define REPEAT_ONE	 		2
#define REPEAT_RANDOM	 	3
#define REPEAT_PREVIEW	 	4

/*********************Window Define in voice mode**********************/
#define	WINDOW_VOICE			    	0
#define	WINDOW_VOICE_PLAY_MENU	    	1
#define	WINDOW_VOICE_STOP_MENU	    	2
#define	WINDOW_VOICE_STOP_MENU_FLASH	3
#define	WINDOW_VOICE_STOP_MENU_SD   	4
#define	WINDOW_VOICE_MENU_REPEAT    	5
#define WINDOW_VOICE_VOLUME		    	6
#define	WINDOW_VOICE_MENU_DEL	    	7
#define	WINDOW_VOICE_MENU_DELALL	    8


/*****************Voice Display Column & Row Address*******************/
#define	DisMenu_MaxNum				3	
#define	DIS_VOICE_FREQ_NUM			8	
#define	DIS_VOICE_NAME_X			14
#define DIS_NAME_X_END				DIS_VOICE_NAME_X + 96 

#endif