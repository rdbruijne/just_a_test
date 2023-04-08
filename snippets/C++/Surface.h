// Surface.h - IGAD/NHTV - Jacco Bikker - 2006-2009
#pragma once

#include <stdint.h>

class Surface
{
	// disable copying
	Surface(const Surface&) = delete;
	Surface& operator =(const Surface&) = delete;

public:
	typedef uint32_t Pixel;

	// constructor/destructor
	Surface(int inWidth, int inHeight, Pixel* inBuffer, int inPitch);
	Surface(int inWidth, int inHeight, Pixel inColor);
	Surface(int inWidth, int inHeight);
	Surface(Surface* inSurface);
	Surface(const std::string& inFile);
	~Surface();

	void SetSize(int inWidth, int inHeight, bool inKeepOldImage=false);

	// member data access
	Pixel* GetBuffer() { return mBuffer; }
	void SetBuffer(Pixel* inBuffer) { mBuffer = inBuffer; }
	int GetWidth() const { return mWidth; }
	int GetHeight() const { return mHeight; }
	int GetArea() const { return mWidth*mHeight; }
	int GetPitch() const { return mPitch; }
	void SetPitch(int inPitch) { mPitch = inPitch; }

	void ConvertToGrayscale();
	void Binarize(unsigned char inTreshhold);
	void GetNextMIPLevel(Surface* ioSurface);		// Scaled(0.5f, 0.5f, this);

	static Pixel AlphaBlend(Pixel inSrc, Pixel inDst);

	// output
	void Print(int inX, int inY, Pixel inColor, char* inString, ...);
	void PrintCenter(int inY, Pixel inColor, char* inString, ...);
	void PrintCenter(Pixel inColor, char* inString, ...);
	void Clear(Pixel inColor = 0x000000);
	void Plot(int inX, int inY, Pixel inColor);

	void HLine(int inX, int inY, int inLength, Pixel inColor);
	void VLine(int inX, int inY, int inLength, Pixel inColor);
	void Line(float inX1, float inY1, float inX2, float inY2, Pixel inColor);
	void LineClipped(float inX1, float inY1, float inX2, float inY2, Pixel inColor);
	void Bar(int inX1, int inY1, int inX2, int inY2, Pixel inColor);
	void Box(int inX1, int inY1, int inX2, int inY2, Pixel inColor);
	void Circle(int inX, int inY, int inRadius, Pixel inColor);
	void CircleFill(int inX, int inY, int inRadius, Pixel inColor);

	Pixel GetColor(int inX, int inY);
	Pixel GetUVColor(float inU, float inV);

	void Load_Image(const std::string& inFile);
	void Save_Image(const std::string& inFile, bool inFlipX=false, bool inFlipY=false);

	void CopyTo(Surface* inDst, int inDstX = 0, int inDstY = 0, int inXOffset = 0, int inYOffset = 0);
	void PartCopyTo(Surface* inDst, int inDstX, int inDstY, int inStartX, int inStartY, int inEndX, int inEndY);

	static void LoadAnimatedGif(const std::string& inFile, int& outImageCount, Surface** outSurfaces);
	static void SaveAnimatedGif(const std::string& inFile, int inImageCount, Surface** inSurfaces, float inFps, bool inFlipX = false, bool inFlipY = false);

	// Static attributes for the building font
	static void InitCharset();

private:
	// Attributes
	Pixel* mBuffer = nullptr;
	int mWidth = 0;
	int mHeight = 0;
	int mPitch = 0;

	// Static attributes for the building font
	static unsigned int mFont[256];
};
