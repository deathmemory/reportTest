#pragma once
#include <gdiplus.h>
#include <Gdiplusbase.h>
using namespace Gdiplus;

typedef struct TagBtnImage 
{
	Image * pImage;
	int nXOffSet;
	int nYOffSet;
	BOOL IsFourBlack;
	TagBtnImage()
	{
		pImage = NULL;
		nXOffSet = 0;
		nYOffSet = 0;
		IsFourBlack = FALSE;
	}
	TagBtnImage(Image * pimage,int nX,int nY,BOOL FourBlack)
	{
		pImage = pimage;
		nXOffSet = nX;
		nYOffSet = nY;
		IsFourBlack = FourBlack;
	}
}BtnImage,BTN_IAMGE;

typedef struct TagBtnText 
{
	CString sText;
	CRect  rcText; 
	Color cNormal;
	Color cHigh;
	Color cPress;
	Color cDisable;
	HFONT hFont;
	BOOL  IsTextCenter;
	TagBtnText()
	{
		sText = _T("");
		rcText = CRect(0,0,0,0);
		IsTextCenter = TRUE;
	}
	TagBtnText(HFONT font,CString Text,CRect  rText,Color Normal,Color High,Color Press,Color Disable ,BOOL TextCenter = TRUE)
	{
		sText = Text;
		rcText = rText; 
		cNormal = Normal;
		cHigh = High;
		cPress = Press;
		cDisable = Disable;
		hFont = font;
		IsTextCenter = TextCenter;
	}
}BtnText,BTN_TEXT;

class CImageHelp
{
public:
	CImageHelp(void);
	~CImageHelp(void);

	static Image * MakeDoubleBtn(HDC hDC,Image*pBK,BtnImage btnImage,BtnText btnText = BtnText());
};
