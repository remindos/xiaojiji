/*******************************************************************************
Copyright: Sinowealth Tech. Co. Ltd.
File name: ui_sys_header.h
Creator: zj.zong			Date: 2009-05-31
Description: define Macro in system mode
Others:
Version: V0.1
History:
	V0.1	2009-05-31		zj.zong
			1. build this module
********************************************************************************/
#ifndef SYS_HEADER
#define SYS_HEADER

/*********************Window Define In system mode**********************/
#define	WINDOW_SYS_MENU						0x00
#define	WINDOW_SYS_MENU_BKLIGHT_TIME		0x01
#define	WINDOW_SYS_MENU_POWEROFF			0x02
#define	WINDOW_SYS_MENU_POWEROFF_IDLE		0x03
#define	WINDOW_SYS_MENU_POWEROFF_SLEEP		0x04
#define WINDOW_SYS_MENU_REPLAY				0x05
#define	WINDOW_SYS_MENU_CONTRAST			0x06
#define	WINDOW_SYS_MENU_LANGUAGE			0x07
#define WINDOW_SYS_MENU_MEMORY				0x08
#define WINDOW_SYS_MENU_VERSION				0x09
#define	WINDOW_SYS_MENU_FACTORY				0x0a
#define	WINDOW_SYS_MENU_ONLINE			   	0x0b
#define WINDOW_SYS_MENU_REC				  	0x0c
#define	WINDOW_SYS_MENU_AMT			  	 	0x0d

/*****************system Display Column & Row Address*******************/
#define	SYS_MENU_NUM				11
#define	DisMenu_MaxNum				3

#endif