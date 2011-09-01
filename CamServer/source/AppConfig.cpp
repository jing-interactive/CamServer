#include "AppConfig.h"
#include "ofxArgs.h"
#include <stdio.h>

using cv::FileStorage;

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE 1
#endif

AppConfig theConfig;

AppConfig::AppConfig():CLIENT("localhost")
{
	PORT = 3333;
	fixed_back_mode = TRUE;
	tuio_mode = TRUE;
	face_track = FALSE;
	hand_track = FALSE;
	finger_track = FALSE;
	hull_mode = FALSE;
	gray_detect_mode = TRUE;
	minim_window = FALSE;
	delay_for_run= 0;

	bg_mode = DIFF_BG;

	paramFlipX = 0;
	paramFlipY = 0;
	paramDark = 220;
	paramBright = 40;
	paramAuto = 8;
	paramBlur1 = 35;
	paramBlur2 = 1;
	paramNoise = 0;
	paramMinArea = 10;
	paramMaxArea = 800; 
};

bool using_debug_file = true;

void AppConfig::parse_args(int argc, char** argv)
{
	ofxArgs args(argc, argv);
	//if (args.contains("-a"))
	//	fixed_back_mode = false;
	//else
	//	fixed_back_mode = true;

	if (args.contains("-client"))
		CLIENT = args.getString("-client");

#ifndef _DEBUG
	using_debug_file = args.contains("-log");
#endif

	minim_window = args.contains("-minim");

	if (args.contains("-delay"))
		delay_for_run = args.getInt("-delay");

	finger_track = args.contains("-finger");
	hand_track = args.contains("-hand");
	if (args.contains("-port"))
		PORT = args.getInt("-port");
}

bool AppConfig::load_from(char* filename)
{
	FileStorage fs(filename, FileStorage::READ);
	if (!fs.isOpened())
	{
		printf("config.xml does not exist, application start with default value.\n");
		return false;
	}
	else
	{
#define READ_(id, var) fs[id]>>var
#define READ_FS(var) fs[#var]>>(var)

		READ_("corners0_x",corners[0].x);READ_("corners0_y",corners[0].y);
		READ_("corners1_x",corners[1].x);READ_("corners1_y",corners[1].y);
		READ_("corners2_x",corners[2].x);READ_("corners2_y",corners[2].y);
		READ_("corners3_x",corners[3].x);READ_("corners3_y",corners[3].y);
		READ_FS(paramFlipX);
		READ_FS(paramFlipY);
		READ_FS(paramDark);
		READ_FS(paramBright);
		READ_FS(paramBlur1);
		READ_FS(paramBlur2);
		READ_FS(face_track);
		READ_FS(paramMinArea);
		READ_FS(paramMaxArea);
		READ_FS(hull_mode);
		READ_FS(gray_detect_mode);
		READ_FS(bg_mode);
		READ_FS(tuio_mode);
		printf("config.xml loaded.\n");

		return true;
	}
}

bool AppConfig::save_to(char* filename)
{
	FileStorage fs("config.xml", FileStorage::WRITE);
	if (!fs.isOpened())
	{
		printf("failed to open config.xml for writing.\n");
		return false;
	}
	else
	{
#define WRITE_(id, var) fs<<id<<var
#define WRITE_FS(var) fs<<#var<<(var)

		WRITE_("corners0_x",corners[0].x);WRITE_("corners0_y",corners[0].y);
		WRITE_("corners1_x",corners[1].x);WRITE_("corners1_y",corners[1].y);
		WRITE_("corners2_x",corners[2].x);WRITE_("corners2_y",corners[2].y);
		WRITE_("corners3_x",corners[3].x);WRITE_("corners3_y",corners[3].y);
		WRITE_FS(paramFlipX);
		WRITE_FS(paramFlipY);
		WRITE_FS(paramDark);
		WRITE_FS(paramBright);
		WRITE_FS(paramBlur1);
		WRITE_FS(paramBlur2);
		WRITE_FS(face_track);
		WRITE_FS(paramMinArea);
		WRITE_FS(paramMaxArea);
		WRITE_FS(hull_mode);
		WRITE_FS(gray_detect_mode);
		WRITE_FS(bg_mode);
		WRITE_FS(tuio_mode);
		printf("config.xml saved.\n");
		return true;
	}
}
