// Surface.cpp-IGAD/NHTV-Jacco Bikker-2006-2009
// modified by Rick de Bruijne

#include "Surface.h"
#include "Debug.h"
#include "MathHelper.h"
#include "Declarations.h"
#include "FreeImage.h"

#include <stdio.h>
#include <cstdarg>
#include <iostream>

unsigned int	Surface::mFont[256];

Surface::Surface(int inWidth, int inHeight, Pixel* inBuffer, int inPitch) :
	mWidth(inWidth),
	mHeight(inHeight),
	mBuffer(inBuffer),
	mPitch(inPitch)
{
}



Surface::Surface(int inWidth, int inHeight, Pixel inColor) :
	mWidth(inWidth),
	mHeight(inHeight),
	mPitch(inWidth)
{

	mBuffer = (Pixel*)MALLOC64(inWidth*inHeight*sizeof(Pixel));
	Clear(inColor);
}



Surface::Surface(int inWidth, int inHeight) :
	mWidth(inWidth),
	mHeight(inHeight),
	mPitch(inWidth)
{
	mBuffer = (Pixel*)MALLOC64(inWidth*inHeight*sizeof(Pixel));
}



Surface::Surface(Surface* inSurface) :
	mWidth(inSurface->mWidth),
	mHeight(inSurface->mHeight),
	mPitch(inSurface->mPitch)
{
	mBuffer = (Pixel*)MALLOC64(mWidth*mHeight*sizeof(Pixel));
	inSurface->CopyTo(this, 0, 0);
}



Surface::Surface(const String& inFile) :
	mBuffer(NULL),
	mWidth(0), mHeight(0)
{
	Load_Image(inFile);
}



Surface::~Surface()
{
	FREE64(mBuffer);
}



void Surface::SetSize(int inWidth, int inHeight, bool inKeepOldImage)
{
	if(inWidth==mWidth && inHeight==mHeight)
		return;

	// create new buffer
	Pixel* tmp = (Pixel*)MALLOC64(inWidth*inHeight*sizeof(Pixel));

	if(inKeepOldImage)
	{
		// get min sizes
		int w = MIN(mWidth, inWidth);
		int h = MIN(mHeight, inHeight);

		// copy old buffer to new buffer
		Pixel* src = mBuffer;
		Pixel* dst = tmp;
		for(int y=0; y<h; y++)
		{
			memcpy(dst, src, w*4);
			dst += inWidth;
			src += mWidth;
		}
	}

	// remove old buffer and set pointer
	FREE64(mBuffer);
	mBuffer = tmp;

	// set data
	mWidth	= inWidth;
	mHeight	= inHeight;
	mPitch	= inWidth;
}



//------------------------------------------------------------------------------
// color functions
Pixel Surface::AlphaBlend(Pixel inSrc, Pixel inDst)
{
	const Pixel sa		= inSrc >> 24;
	const Pixel da		= inDst >> 24;

	if(sa == 0xFF)	return inSrc;		// src has zero transparency
	if(sa == 0x00)	return inDst;		// src has full transparency

	const Pixel srb		= inSrc & 0x00FF00FF;
	const Pixel sg		= inSrc & 0x0000FF00;
	const Pixel drb		= inDst & 0x00FF00FF;
	const Pixel dg		= inDst & 0x0000FF00;

	const Pixel rb		= (drb+(((srb-drb)*sa+0x00800080) >> 8)) & 0x00FF00FF;
	const Pixel g		= (dg +(((sg - dg)*sa+0x00008000) >> 8)) & 0x00FF00FF;
	const Pixel a		= (da +(((sa - da)*sa+0x00000080) >> 8)) + da;

	return ((a < 0xFF ? a << 24 : 0xFF000000) | rb | g);
}



//------------------------------------------------------------------------------
// output
void Surface::Print(int inX, int inY, Pixel inColor, char* inString, ...)
{
	if(inY+6 > mHeight) return;
	if(inY < 0) return;

	char buffer[2048];
	va_list args;
	va_start(args, inString);
	vsnprintf(buffer, 2047, inString, args);
	va_end(args);

	Pixel* t = mBuffer+inX+inY*mPitch;
	int x = inX;
	for(int i = 0; i < (int)(strlen(buffer)); i++)
	{
		if(buffer[i] == '\n')
		{
			if(inY+12 > mHeight) return;
			inY += 6;
			x = inX;
			t = mBuffer+inX+inY*mPitch;
			continue;
		}
		long pos = 0;
		if ((buffer[i] >= 'a') && (buffer[i] <= 'z'))
		{
			pos = buffer[i] + ('A'-'a');
		}
		else
		{
			pos = buffer[i];
		}
		Pixel* a = t;
		x += 6;
		if(x > mWidth) continue;
		unsigned int b=0x80000000;
		for(int v=0; v<5; v++)
		{
			for(int h=0; h<5; h++)
			{
				if(mFont[pos]&b)
					*(a+h) = inColor, *(a+h+mPitch+1) = 0;
				b >>= 1;
			}
			a += mPitch;
		}
		t += 6;
	}
}



void Surface::PrintCenter(int inY, Pixel inColor, char* inString, ...)
{
	char buffer[2048];
	va_list args;
	va_start(args, inString);
	vsnprintf(buffer, 2047, inString, args);
	va_end(args);

	int x = (mWidth-(int)strlen(buffer)*6) >> 1;
	Print(x, inY, inColor, buffer);
}



void Surface::PrintCenter(Pixel inColor, char* inString, ...)
{
	char buffer[2048];
	va_list args;
	va_start(args, inString);
	vsnprintf(buffer, 2047, inString, args);
	va_end(args);

	int x = (mWidth-(int)strlen(buffer)*6) >> 1;
	int y = (mHeight >> 1)-3;
	Print(x, y, inColor, buffer);
}



void Surface::Clear(Pixel inColor)
{
	int s = mWidth*mHeight;
	for (int i = 0; i < s; i++) mBuffer[i] = inColor;
}



void Surface::Plot(int inX, int inY, Pixel inColor)
{
	if ((inX >= 0) && (inY >= 0) && (inX < mWidth) && (inY < mHeight)) mBuffer[inX+inY*mPitch] = inColor;
}



void Surface::HLine(int inX, int inY, int inLength, Pixel inColor)
{
	if(inY < 0 || inY >= mHeight) return;
	inX = (inX < 0) ? inLength += inX, 0 : (inX > mWidth) ? mWidth : inX;
	inLength  = (inX+inLength < 0) ? inX : (inX+inLength > mWidth) ? mWidth-inX : inLength;
	if(inLength < 0) { inX += inLength; inLength = -inLength; }
	Pixel* a = mBuffer+inX+inY*mPitch;
	for (int i = 0; i < inLength; i++, a++) *a = inColor;
}



void Surface::VLine(int inX, int inY, int inLength, Pixel inColor)
{
	if(inX < 0 || inX >= mWidth) return;
	inY = (inY < 0) ? inLength += inY, 0 : (inY > mHeight) ? mHeight : inY;
	inLength  = (inY+inLength < 0) ? inY : (inY+inLength > mHeight) ? mHeight-inY : inLength;
	if(inLength < 0) { inY += inLength; inLength = -inLength; }
	Pixel* a = mBuffer+inX+inY*mPitch;
	for (int i = 0; i < inLength; i++, a += mPitch) *a = inColor;
}



void Surface::Line(float inX1, float inY1, float inX2, float inY2, Pixel inColor)
{
	if ((inX1 < 0) || (inY1 < 0) || (inX1 >= mWidth) || (inY1 >= mHeight) ||
		(inX2 < 0) || (inY2 < 0) || (inX2 >= mWidth) || (inY2 >= mHeight))
	{
		return;
	}
	float b = inX2-inX1;
	float h = inY2-inY1;
	float l = fabsf(b);
	if (fabsf (h) > l) l = fabsf(h);
	int il = (int)l;
	float dx = b/(float)l;
	float dy = h/(float)l;
	for (int i = 0; i <= il; i++)
	{
		*(mBuffer+(int)inX1+(int)inY1*mPitch) = inColor;
		inX1 += dx, inY1 += dy;
	}
}



void Surface::LineClipped(float inX1, float inY1, float inX2, float inY2, Pixel inColor)
{
	float w = inX2-inX1;
	float h = inY2-inY1;
	float l = fabsf(w);
	if (fabsf (h) > l) l = fabsf(h);
	float dx = w/(float)l;
	float dy = h/(float)l;
	int il = (int)l;
	int a = 0;
	for (int i = 0; i <= il; i++)
	{
		if(!(inX1 < 0 || inX1 >= mWidth || inY1 < 0 || inY1 >= mHeight))
			*(mBuffer+(int)inX1+(int)inY1*mPitch) = inColor;
		inX1 += dx, inY1 += dy;
	}
}



void Surface::Bar(int inX1, int inY1, int inX2, int inY2, Pixel inColor)
{
	if(inX1 > inX2) SWAP(inX1, inX2);
	if(inY1 > inY2) SWAP(inY1, inY2);
	if((inX1 >= mWidth) || (inX2 < 0) || (inY1 >= mHeight) || (inY2 < 0)) return;
	inX1 = CLAMP(inX1, 0, mWidth-1);	inY1 = CLAMP(inY1, 0, mHeight-1);
	inX2 = CLAMP(inX2, 0, mWidth-1);	inY2 = CLAMP(inY2, 0, mHeight-1);
	Pixel* a = inX1+inY1*mPitch+mBuffer;
	for (int y = inY1; y <= inY2; y++)
	{
		for (int x = 0; x <= (inX2-inX1); x++) a[x] = inColor;
		a += mPitch;
	}
}



void Surface::Box(int inX1, int inY1, int inX2, int inY2, Pixel inColor)
{
	HLine(inX1, inY1, inX2-inX1, inColor);
	HLine(inX1, inY2, inX2-inX1, inColor);
	VLine(inX1, inY1, inY2-inY1, inColor);
	VLine(inX2, inY1, inY2-inY1, inColor);
}



void Surface::Circle(int inX, int inY, int inRadius, Pixel inColor)
{
	int cx=inX, cy=inY;
	int x=0, y=inRadius, e=1-inRadius;
	int incrE=3, incrSE=-inRadius-inRadius+5;
	Plot(cx+x, cy+y, inColor);
	Plot(cx-x, cy-y, inColor);
	Plot(cx+y, cy+x, inColor);
	Plot(cx-y, cy-x, inColor);
	while(y > x)
	{
		if(e < 0)
		{
			e += incrE;
			incrE += 2;
			incrSE += 2;
		}
		else
		{
			e += incrSE;
			incrE += 2;
			incrSE += 4;
			y--;
		}
		x++;
		Plot(cx+x, cy+y, inColor);
		Plot(cx+x, cy-y, inColor);
		Plot(cx+y, cy+x, inColor);
		Plot(cx+y, cy-x, inColor);

		Plot(cx-x, cy+y, inColor);
		Plot(cx-x, cy-y, inColor);
		Plot(cx-y, cy+x, inColor);
		Plot(cx-y, cy-x, inColor);
	}
}



void Surface::CircleFill(int inX, int inY, int inRadius, Pixel inColor)
{
	int cx=inX, cy=inY;
	int x=0, y=inRadius, e=1-inRadius;
	int incrE=3, incrSE=-inRadius-inRadius+5;
	HLine(inX-inRadius, inY, inRadius+inRadius, inColor);
	while(y > x)
	{
		if(e < 0)
		{
			e += incrE;
			incrE += 2;
			incrSE += 2;
		}
		else
		{
			e += incrSE;
			incrE += 2;
			incrSE += 4;
			y--;
			HLine(cx-x, cy-y, x+x, inColor);
			HLine(cx-x, cy+y, x+x, inColor);
		}
		x++;
		HLine(cx-y, cy-x, y+y, inColor);
		HLine(cx-y, cy+x, y+y, inColor);
	}
}



Pixel Surface::GetColor(int inX, int inY)
{
	// clamp mode
	inX = CLAMP(inX, 0, mWidth-1);
	inY = CLAMP(inY, 0, mHeight-1);
	return mBuffer[inX+inY*mPitch];
}



Pixel Surface::GetUVColor(float inU, float inV)
{
	float cu = inU-(int)inU;
	float cv = inV-(int)inV;
	cu = cu < 0 ? 1.f+cu : cu;
	cv = 1.f-(cv < 0 ? 1.f+cv : cv);
	return GetColor((int)(cu*(mWidth-1)), (int)(cv*(mHeight-1)));
}



void Surface::Load_Image(const String& inFile)
{
	FILE* f = fopen((const char*)inFile, "rb");
	if (!f)
	{
		gAssertT(false, "Cannot open file: %s", (const char*)inFile);
		return;
	}
	else fclose(f);

	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	fif = FreeImage_GetFileType((const char*)inFile, 0);
	if (fif == FIF_UNKNOWN) fif = FreeImage_GetFIFFromFilename((const char*)inFile);
	FIBITMAP* tmp = FreeImage_Load(fif, (const char*)inFile);
	FIBITMAP* dib = FreeImage_ConvertTo32Bits(tmp);
	FreeImage_Unload(tmp);
	unsigned char* bits = FreeImage_GetBits(dib);
	mWidth = mPitch = FreeImage_GetWidth(dib);
	mHeight = FreeImage_GetHeight(dib);
	mBuffer = (Pixel*)MALLOC64(mWidth*mHeight*sizeof(Pixel));

	for(int y = 0; y < mHeight; y++)
	{
		unsigned char* line = FreeImage_GetScanLine(dib, mHeight-1-y);
		memcpy(mBuffer+y*mPitch, line, mWidth*sizeof(Pixel));
	}
	FreeImage_Unload(dib);
}



void Surface::Save_Image(const String& inFile, bool inFlipX, bool inFlipY)
{
	// create FIBITMAP
	FIBITMAP *bitmap = FreeImage_Allocate(mWidth, mHeight, 24);
	RGBQUAD tempPixel;

	for(int y=0; y<mHeight; y++)
	{
		const int saveY			= inFlipY ? y : (mHeight-1-y);
		const int yPitch		= y*mPitch;
		for(int x=0; x<mWidth; x++)
		{
			const int saveX		= inFlipX ? (mWidth-1-x) : x;
			const Pixel c		= mBuffer[ x+yPitch ];
			tempPixel.rgbRed	= (BYTE)((c & REDMASK) >> 16);
			tempPixel.rgbGreen	= (BYTE)((c & GREENMASK) >> 8);
			tempPixel.rgbBlue	= (BYTE)(c & BLUEMASK);
			FreeImage_SetPixelColor(bitmap, saveX, saveY, &tempPixel);
		}
	}

	FREE_IMAGE_FORMAT fif = FreeImage_GetFIFFromFilename((const char*)inFile);
	if(fif != FIF_UNKNOWN) FreeImage_Save(fif, bitmap, (const char*)inFile);

	// unload bitmap
	FreeImage_Unload(bitmap);
}



void Surface::CopyTo(Surface* inDst, int inDstX, int inDstY, int inXOffset, int inYOffset)
{
	if(inDst==NULL) return;
	Pixel* dst = inDst->GetBuffer();
	Pixel* src = mBuffer+(inXOffset+(inYOffset*mHeight));
	if ((src) && (dst))
	{
		int srcwidth = mWidth-inXOffset;
		int srcheight = mHeight-inYOffset;
		int srcpitch = mPitch;
		int dstwidth = inDst->GetWidth();
		int dstheight = inDst->GetHeight();
		int dstpitch = inDst->GetPitch();
		if ((srcwidth+inDstX) > dstwidth) srcwidth = dstwidth-inDstX;
		if ((srcheight+inDstY) > dstheight) srcheight = dstheight-inDstY;
		if (inDstX < 0) src -= inDstX, srcwidth += inDstX, inDstX =0;
		if (inDstY < 0) src -= inDstY*srcpitch, srcheight += inDstY, inDstY = 0;
		if ((srcwidth > 0) && (srcheight > 0))
		{
			dst += inDstX+dstpitch*inDstY;
			for (int y = 0; y < srcheight; y++)
			{
				memcpy(dst, src, srcwidth*4);
				dst += dstpitch;
				src += srcpitch;
			}
		}
	}
}



void Surface::PartCopyTo(Surface* inDst, int inDstX, int inDstY, int inStartX, int inStartY, int inEndX, int inEndY)
{
	if(inDst==NULL) return;
	Pixel* dst = inDst->GetBuffer();
	Pixel* src = mBuffer+(inStartX+(inStartY*mHeight));
	if ((src) && (dst))
	{
		int srcwidth = inEndX-inStartX;
		int srcheight = inEndY-inStartY;
		int srcpitch = mPitch;
		int dstwidth = inDst->GetWidth();
		int dstheight = inDst->GetHeight();
		int dstpitch = inDst->GetPitch();
		if ((srcwidth+inDstX) > dstwidth) srcwidth = dstwidth-inDstX;
		if ((srcheight+inDstY) > dstheight) srcheight = dstheight-inDstY;
		if (inDstX < 0) src -= inDstX, srcwidth += inDstX, inDstX =0;
		if (inDstY < 0) src -= inDstY*srcpitch, srcheight += inDstY, inDstY = 0;
		if ((srcwidth > 0) && (srcheight > 0))
		{
			dst += inDstX+dstpitch*inDstY;
			for (int y = 0; y < srcheight; y++)
			{
				memcpy(dst, src, srcwidth*4);
				dst += dstpitch;
				src += srcpitch;
			}
		}
	}
}



void Surface::ConvertToGrayscale()
{
	int r,g,b,c;
	Pixel* src = mBuffer;
	for(int i=0; i<mWidth*mHeight; i++)
	{
		r = (*src)&0xFF0000;	r >>= 16;
		g = (*src)&0x00FF00;	g >>= 8;
		b = (*src)&0x0000FF;
#if 0
		c = (((r*30) + (g*59) + (b*11)) / 100) & 0xFF;
#else
		// values slightly altered for performance
		c = (((r<<8) + (g<<9) + (b<<7)) >> 10) & 0xFF;
#endif
		(*src) = c | c<<8 | c<<16;
		src++;
	}
}



void Surface::Binarize(unsigned char inTreshhold)
{
	Pixel* src = mBuffer;
	for(int i=0; i<mWidth*mHeight; i++, src++)
		(*src) = ((*src)&0xFF)>inTreshhold ? 0xFFFFFF : 0x000000;
}



void Surface::GetNextMIPLevel(Surface* ioSurface)
{
	if(ioSurface == NULL) return;
	ioSurface->SetSize(mWidth>>1, mHeight>>1);
	Pixel *dst = ioSurface->GetBuffer();
	int w = ioSurface->GetPitch();
	for(int x = 0, i = 0; x < mWidth; x += 2, i++) for(int y = 0, j = 0; y < mHeight; y += 2, j++)
	{
		int rb = 0, g = 0, c = 0;

		c = mBuffer[ x  +(y  )*mPitch ]; rb += c & (REDMASK|BLUEMASK); g += c & GREENMASK;
		c = mBuffer[ x+1+(y  )*mPitch ]; rb += c & (REDMASK|BLUEMASK); g += c & GREENMASK;
		c = mBuffer[ x  +(y+1)*mPitch ]; rb += c & (REDMASK|BLUEMASK); g += c & GREENMASK;
		c = mBuffer[ x+1+(y+1)*mPitch ]; rb += c & (REDMASK|BLUEMASK); g += c & GREENMASK;

		dst[i+j*w] = ((rb >> 2) & (REDMASK+BLUEMASK))+((g >> 2) & GREENMASK);
	}
}



//------------------------------------------------------------------------------
// animated gif
void Surface::LoadAnimatedGif(const String& inFile, int& outImageCount, Surface** outSurfaces)
{
	gAssertT(false, "Function not implemented!");
}



void Surface::SaveAnimatedGif(const String& inFile, int inImageCount, Surface** inSurfaces, float inFps, bool inFlipX, bool inFlipY)
{
	// create bitmaps
	FIBITMAP** bitmaps = new FIBITMAP*[inImageCount];
	for(int i=0; i<inImageCount; i++)
	{
		FIBITMAP* tmp = FreeImage_Allocate(inSurfaces[i]->GetWidth(), inSurfaces[i]->GetHeight(), 24);
		Pixel* buffer = inSurfaces[i]->GetBuffer();
		RGBQUAD tempPixel;

		for(int y=0; y<inSurfaces[i]->GetHeight(); y++)
		{
			const int saveY			= inFlipY ? y : (inSurfaces[i]->GetHeight()-1-y);
			const int yPitch		= y*inSurfaces[i]->GetPitch();
			for(int x=0; x<inSurfaces[i]->GetWidth(); x++)
			{
				const int saveX		= inFlipX ? (inSurfaces[i]->GetWidth()-1-x) : x;
				const Pixel c		= buffer[ x+yPitch ];
				tempPixel.rgbRed	= (BYTE)((c & REDMASK) >> 16);
				tempPixel.rgbGreen	= (BYTE)((c & GREENMASK) >> 8);
				tempPixel.rgbBlue	= (BYTE)(c & BLUEMASK);
				FreeImage_SetPixelColor(tmp, saveX, saveY, &tempPixel);
			}
		}

		bitmaps[i] = FreeImage_ColorQuantize(tmp, FIQ_NNQUANT);
		FreeImage_Unload(tmp);
	}

	// create gif image
	FIMULTIBITMAP* multi	= FreeImage_OpenMultiBitmap(FIF_GIF, (const char*)inFile, TRUE, FALSE);
	DWORD frameTime			= (DWORD)((1000.0f / inFps)+0.5f);
	for(int i=0; i<inImageCount; i++)
	{
		// clear any animation data used by this bitmap as we ad our new ones
		FreeImage_SetMetadata(FIMD_ANIMATION, bitmaps[i], NULL, NULL);
		// add animation tage to the bitmap[i]
		FITAG* tag = FreeImage_CreateTag();
		if(tag)
		{
			FreeImage_SetTagKey(tag, "FrameTime");
			FreeImage_SetTagType(tag, FIDT_LONG);
			FreeImage_SetTagCount(tag, 1);
			FreeImage_SetTagLength(tag, 4);
			FreeImage_SetTagValue(tag, &frameTime);
			FreeImage_SetMetadata(FIMD_ANIMATION, bitmaps[i], FreeImage_GetTagKey(tag), tag);
			FreeImage_DeleteTag(tag);
		}
		FreeImage_AppendPage(multi, bitmaps[i]);
	}
	FreeImage_CloseMultiBitmap(multi);

	// unload bitmaps
	for(int i=0; i<inImageCount; i++)
		FreeImage_Unload(bitmaps[i]);
	delete[] bitmaps;
}



//------------------------------------------------------------------------------
// Static attributes for the building font
void Surface::InitCharset()
{
	for(int i=0; i<256; i++) mFont[i] = 0x00000000;

	mFont['A']  = 0x747F1880; // A
	mFont['B']  = 0xF47D1F00; // B
	mFont['C']  = 0x7C210780; // C
	mFont['D']  = 0xF4631F00; // D
	mFont['E']  = 0xFC3D0F80; // E
	mFont['F']  = 0xFC390800; // F
	mFont['G']  = 0x7C2F1700; // G
	mFont['H']  = 0x8C7F1880; // H
	mFont['I']  = 0x71084700; // I
	mFont['J']  = 0x10842E00; // J
	mFont['K']  = 0x95314900; // K
	mFont['L']  = 0x84210F80; // L
	mFont['M']  = 0xD56B1880; // M
	mFont['N']  = 0x8E6B3880; // N
	mFont['O']  = 0x74631700; // O
	mFont['P']  = 0xF47D0800; // P
	mFont['Q']  = 0x74633780; // Q
	mFont['R']  = 0xF47D4900; // R
	mFont['S']  = 0x7C1C1F00; // S
	mFont['T']  = 0xF9084200; // T
	mFont['U']  = 0x8C631780; // U
	mFont['V']  = 0x8C54A200; // V
	mFont['W']  = 0x8C6B5500; // W
	mFont['X']  = 0x8A88A880; // X
	mFont['Y']  = 0x8C5E1700; // Y
	mFont['Z']  = 0xF8888F80; // Z
	mFont['0']  = 0x74EB9700; // 0
	mFont['1']  = 0x23084700; // 1
	mFont['2']  = 0x744C8F80; // 2
	mFont['3']  = 0xF04C1F00; // 3
	mFont['4']  = 0x84BE2100; // 4
	mFont['5']  = 0xFC3C1F00; // 5
	mFont['6']  = 0x7C3D1700; // 6
	mFont['7']  = 0xF8444200; // 7
	mFont['8']  = 0x745D1700; // 8
	mFont['9']  = 0x745E1700; // 9
	mFont['!']  = 0x21080200; // !
	mFont['?']  = 0x70440200; // ?
	mFont[':']  = 0x01004000; // :
	mFont['=']  = 0x0380E000; // =
	mFont[',']  = 0x00002200; // ,
	mFont['.']  = 0x00000200; // .
	mFont['-']  = 0x001C0000; // -
	mFont['(']  = 0x11084100; // (
	mFont[')']  = 0x20842200; // )
	mFont[' ']  = 0x00000000; // <space>
	mFont['#']  = 0x57D5F500; // #
	mFont['*']  = 0xABBEEA80; // *
	mFont['/']  = 0x08888800; // /
	mFont['+']  = 0x213E4200; // +
	mFont['_']  = 0x00000F80; // _
	mFont['[']  = 0x31084300; // [
	mFont[']']  = 0x30842300; // ]
	mFont['%']  = 0x48888900; // %
	mFont['^']  = 0x22A20000; // ^
	mFont[';']  = 0x01004400; // ;
	mFont['<']  = 0x11104100; // <
	mFont['>']  = 0x41044400; // >
	mFont['\\'] = 0x82082080; // '\'
	mFont['~']  = 0x6C800000; // ~
	mFont['@']  = 0x746F5380; // @
	mFont['$']  = 0x7D1C5F00; // $
	mFont['&']  = 0x62192680; // &
	mFont['{']  = 0x31184300; // {
	mFont['}']  = 0x30862300; // }
	mFont['\''] = 0x21000000; // '
	mFont['"']  = 0x52800000; // "
	mFont['|']  = 0x21084200; // |
	mFont['`']  = 0x41000000; // `
}
