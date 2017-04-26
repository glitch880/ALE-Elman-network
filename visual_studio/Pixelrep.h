//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#pragma once
#include <vector>

class PixelRep
{
	typedef unsigned char pixel_t;

public:
	PixelRep();
	PixelRep(int grayscale);
	PixelRep(float grayscale);
	PixelRep(pixel_t grayscale);
	PixelRep(pixel_t grayscale, pixel_t r, pixel_t g, pixel_t b);
	PixelRep(pixel_t r, pixel_t g, pixel_t b);
	~PixelRep();

	int getIndexColor();
	int getRed();
	int getGreen();
	int getBlue();
	int getGrayScale();
	int getRGBGrayScale();
	float getNormalizedIndexColor();
	float getNormalizedGrayScale(bool useRGB);

private:

	int red;
	int green;
	int blue;
	int grayScale;
	int indexColor;


};

