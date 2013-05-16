#include "StdAfx.h"
#include "ImageHelp.h"

CImageHelp::CImageHelp(void)
{

}

CImageHelp::~CImageHelp(void)
{

}

Image * CImageHelp::MakeDoubleBtn(HDC hDC,Image*pBK,BtnImage btnImage,BtnText btnText)
{
	Image* lpImageBoxFrame = pBK->Clone();
	Image *lpImageUp = NULL;
	if (btnImage.pImage)
	{
		lpImageUp = btnImage.pImage->Clone();
	}
	

	if(lpImageBoxFrame == NULL)
	{
		return NULL;
	}
	Graphics loBtnGraphics(lpImageBoxFrame);
	int nBkWidth =  lpImageBoxFrame->GetWidth();
	int nBkHeight = lpImageBoxFrame->GetHeight();
	if (lpImageUp)
	{
		int nUpWidth =  lpImageUp->GetWidth();
		int nUpHeight = lpImageUp->GetHeight();
		//-------------------------------------------
		Rect lrcBox1(nBkWidth/8 - nUpWidth/8 - btnImage.nXOffSet, nBkHeight/2 - nUpHeight/2-btnImage.nYOffSet, nUpWidth/4 , nUpHeight);
		Rect lrcBox2(nBkWidth*3/8 - nUpWidth/8 - btnImage.nXOffSet, nBkHeight/2 - nUpHeight/2-btnImage.nYOffSet, nUpWidth/4 , nUpHeight);
		Rect lrcBox3(nBkWidth*5/8 - nUpWidth/8 - btnImage.nXOffSet, nBkHeight/2 - nUpHeight/2-btnImage.nYOffSet, nUpWidth/4 , nUpHeight);
		Rect lrcBox4(nBkWidth*7/8 - nUpWidth/8 - btnImage.nXOffSet, nBkHeight/2 - nUpHeight/2-btnImage.nYOffSet, nUpWidth/4 , nUpHeight);

		loBtnGraphics.DrawImage(lpImageUp, lrcBox1, 0, 0, nUpWidth/4, nUpHeight,UnitPixel);
		loBtnGraphics.DrawImage(lpImageUp, lrcBox2, nUpWidth/4,   0, nUpWidth/4, nUpHeight,UnitPixel);
		loBtnGraphics.DrawImage(lpImageUp, lrcBox3, nUpWidth*2/4, 0, nUpWidth/4, nUpHeight,UnitPixel);
		if (btnImage.IsFourBlack)
		{
			ColorMatrix cm= {0.3f, 0.3f, 0.3f, 0, 0,
				0.59f,0.59f,0.59f,0, 0,
				0.11f,0.11f,0.11f,0, 0,
				0,    0,    0,    1, 0,
				0,    0,    0,    0, 1};
			ImageAttributes ia;
			ia.SetColorMatrix(&cm);
			loBtnGraphics.DrawImage(lpImageUp, lrcBox4, nUpWidth*3/4, 0, nUpWidth/4, nUpHeight,UnitPixel,&ia);
		}
		else
		{
			loBtnGraphics.DrawImage(lpImageUp, lrcBox4, nUpWidth*3/4, 0, nUpWidth/4, nUpHeight,UnitPixel);
		}
		delete lpImageUp;
	}

	//-------------------------------------------
	if (btnText.sText !=  _T(""))
	{
		StringFormat drawFormat;
		if (btnText.IsTextCenter)
		{
			drawFormat.SetAlignment(StringAlignmentCenter);
		}
		else
		{
			drawFormat.SetAlignment(StringAlignmentNear);        
		}
		Gdiplus::Font font(hDC,btnText.hFont);
		Gdiplus::RectF rcc(btnText.rcText.left,btnText.rcText.top,btnText.rcText.Width(),btnText.rcText.Height());
		//Color colText(Color(153,185,204));
		SolidBrush brush(btnText.cNormal);
		CString text = btnText.sText;
		WCHAR * pText = text.GetBuffer(text.GetLength());

		loBtnGraphics.DrawString(pText,text.GetLength(),&font,rcc,&drawFormat , &brush);
		rcc.X += nBkWidth/4;
		SolidBrush brush2(btnText.cHigh);

		loBtnGraphics.DrawString(pText,text.GetLength(),&font,rcc,&drawFormat , &brush2);
		rcc.X += nBkWidth/4;
		//Color colText3(Color(255,255,202));
		SolidBrush brush3(btnText.cPress);

		loBtnGraphics.DrawString(pText,text.GetLength(),&font,rcc,&drawFormat , &brush3);
		rcc.X += nBkWidth/4;
		//Color colText4(Color(212,255,236));
		SolidBrush brush4(btnText.cDisable);

		loBtnGraphics.DrawString(pText,text.GetLength(),&font,rcc,&drawFormat , &brush4);
		text.ReleaseBuffer(text.GetLength());
	}
	return lpImageBoxFrame;
}