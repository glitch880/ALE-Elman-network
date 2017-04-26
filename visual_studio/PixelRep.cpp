//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "PixelRep.h"
#include <iostream>
#include <string>
#include <assert.h>

typedef unsigned char pixel_t;

const int NUMBER_OF_COLORS = 8;


PixelRep::PixelRep(): red(0), green(0), blue(0), indexColor(0)
{

}

PixelRep::PixelRep(int grayscale) : red(0), green(0), blue(0), grayScale(grayscale)
{
	switch (grayscale)
	{
	case 0:    
		indexColor = 0;   //black //or white????
		break;
	case 58:  //0 33 255  blue or 33 33 255
		indexColor = 1;
		break;
	case 63:   //pink?
		indexColor = 2;
		break;
	case 121:  // 240 60 121  //red
		indexColor = 3;
		break;
	case 152:  //0 80 255 Blue  OR 255 80 255
		indexColor = 4;
		break;
	case 188: // 127 255 0 Green
		indexColor = 5;
		break;
	case 217:  //0 255 255  teal OR 127 255 255
		indexColor = 6;
		break;
	case 233: // 0 255 63  Green  OR 255 255 63
		indexColor = 7;
		break;
	case 255:  // 0 255 255 teal  OR 233 233 233 OR 188 188 188
		indexColor = 8;
		break;
	



	default:
		
		std::cout << "unmapped greyscale" << std::to_string(grayscale) << std::endl;
		throw "Unmapped greyscale";
		indexColor = 0;
		break;
	}
}

PixelRep::PixelRep(float grayscale) : red(0), green(0), blue(0), grayScale(grayscale)
{
	switch ((int)grayscale)
	{
	case 0:
		indexColor = 0;   //black
		break;
	case 58:  //0 33 255  blue
		indexColor = 1;
		break;
	case 63:   //pink?
		indexColor = 2;
		break;
	case 121:  // 0 0 0  //red
		indexColor = 3;
		break;
	case 152:  //0 80 255 Blue
		indexColor = 4;
		break;
	case 188: //
		indexColor = 5;
		break;
	case 217:  //0 255 255  teal
		indexColor = 6;
		break;
	case 233: // 0 255 63  Green
		indexColor = 7;
		break;
	case 255:  // 0 255 255 teal
		indexColor = 8;
		break;

	default:
		indexColor = 0;
		break;
	}
}

PixelRep::PixelRep(pixel_t grayscale) : red(0), green(0), blue(0), grayScale(grayscale)
{
	switch (grayscale)
	{
	case 0:
		indexColor = 0;   //black
		break;
	case 58:  //0 33 255  blue
		indexColor = 1;
		break;
	case 63:   //pink?
		indexColor = 2;
		break;
	case 121:  // 0 0 0  //red
		indexColor = 3;
		break;
	case 152:  //0 80 255 Blue
		indexColor = 4;
		break;
	case 188: //
		indexColor = 5;
		break;
	case 217:  //0 255 255  teal
		indexColor = 6;
		break;
	case 233: // 0 255 63  Green
		indexColor = 7;
		break;
	case 255:  // 0 255 255 teal
		indexColor = 8;
		break;

	default:
		std::cout << "unmapped greyscale" << std::to_string(grayscale) << std::endl;
		throw "Unmapped greyscale";
		assert(false);
		indexColor = 0;
		break;
	}
}

PixelRep::PixelRep(pixel_t grayscale, pixel_t r, pixel_t g, pixel_t b) :  red(r), green (g), blue(b), grayScale(grayscale)
{
	switch (grayscale)
	{
	case 0:
		indexColor = 0;   //black
		break;
	case 58:  //0 33 255  blue
		indexColor = 1;
		break;
	case 63:   //pink?
		indexColor = 2;
		break;
	case 121:  // 0 0 0  //red
		indexColor = 3;
		break;
	case 152:  //0 80 255 Blue
		indexColor = 4;
		break;
	case 188: //
		indexColor = 5;
		break;
	case 217:  //0 255 255  teal
		indexColor = 6;
		break;
	case 233: // 0 255 63  Green
		indexColor = 7;
		break;
	case 255:  // 0 255 255 teal
		indexColor = 8;
		break;

	default:
		std::cout << "unmapped greyscale" << std::to_string(grayscale) << std::endl;
		throw "Unmapped greyscale";
		indexColor = 0;
		break;
	}
}

//TODO unused
PixelRep::PixelRep(pixel_t r, pixel_t g, pixel_t b) : red(r), green(g), blue(b)
{
	if (red == 0 && green == 0 && blue == 0)
		indexColor = 0;
	else if (red == 255 && green == 255 && blue == 255)
		indexColor = 1;
	else if (red == 33 && green == 33 && blue == 255) //blue
		indexColor = 2;
	else if (red == 255 && green == 80 && blue == 255) //purple
		indexColor = 3;
	else if (red == 0 && green == 80 && blue == 255) //blue
		indexColor = 4;
	else if (red == 0 && green == 33 && blue == 255) //blue
		indexColor = 5;
	else if (red == 0 && green == 255 && blue == 255) // teal
		indexColor = 6;
	else if (red == 255 && green == 255 && blue == 255)
		indexColor = 7;


}

PixelRep::~PixelRep()
{
}

int PixelRep::getIndexColor()
{
	return indexColor;

}

int PixelRep::getRed()
{
	return red;
}

int PixelRep::getGreen()
{
	return green;
}

int PixelRep::getBlue()
{
	return blue;
}

int PixelRep::getGrayScale()
{
	return grayScale;
}

int PixelRep::getRGBGrayScale() //todo change to better method?
{
	return ((double)red * 0.2989 + (double)green * 0.5870 + (double)blue * 0.1140);
}

float PixelRep::getNormalizedIndexColor()
{
	return (float)indexColor / (float)NUMBER_OF_COLORS;
}

float PixelRep::getNormalizedGrayScale(bool useRGB)
{
	float maxGray = 255;
	if (useRGB)
	{
		float grayScale = (red + green + blue) / 3;

		return grayScale / maxGray;
	}
	return (float)((float)grayScale / maxGray);

}
