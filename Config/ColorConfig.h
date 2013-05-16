#pragma once


#define BTN_COLOR_NORMAL         			RGB(255,255,255)
#define BTN_COLOR_PRESS          			BTN_COLOR_NORMAL
#define BTN_COLOR_HIGHLIGHT      			BTN_COLOR_NORMAL
#define BTN_COLOR_DISABLE        			RGB(112,123,141)

#define LAB_COLOR_VERY_IMPORTANT 			RGB(223,239,247)
#define LAB_COLOR_GROUP          			RGB(180,218,239)
#define LAB_COLOR_NORMAL         			RGB(153,185,204)
#define LAB_COLOR_NOT_IMPORTANT  			RGB(85,103,126)
#define LAB_COLOR_B              			RGB(180,218,239)
#define LAB_COLOR_W              			RGB(255,255,255)
#define LAB_COLOR_IMPORTANT      			RGB(180,218,239)
#define LAB_COLOR_BLUE           			RGB(0,104,183)
#define LAB_COLOR_TIP            			RGB(128,136,152)

enum SCREENSHORTTYPE
{
	BMP = 1,
	JPG = 2,
	PNG = 3,
	TGA = 4
};