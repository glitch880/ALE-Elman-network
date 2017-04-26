//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "DownSampler.h"
#include <assert.h>
#include <math.h>

using namespace std; 

void resizePixels(vector<PixelRep>& pixelsIn, vector<PixelRep>& pixelsOut, int w1, int h1, int w2, int h2)
{
	pixelsOut.resize(w2*h2);
	double x_ratio = w1 / (double)w2;
	double y_ratio = h1 / (double)h2;
	double px, py;
	for (int i = 0; i<h2; i++) 
	{
		for (int j = 0; j<w2; j++) 
		{
			px = floor(j*x_ratio);
			py = floor(i*y_ratio);
			pixelsOut[(i*w2) + j] = pixelsIn[(int)((py*w1) + px)];
		}
	}
}

std::vector<PixelRep> DownSampler::getDownsampledNeighbour(std::vector<PixelRep>& pixels, int downScaler)
{
	//screen is 210 x 160; using nearest neighbour
	int controlSize = pixels.size() / (std::pow(downScaler, 2));
	std::vector<PixelRep> newVector(controlSize);
	
	int w1 = 160;
	int h1 = 210;
	int w2 = 160 / downScaler;
	int h2 = 210 / downScaler;

	double x_ratio = w1 / (double)w2;
	double y_ratio = h1 / (double)h2;
	double px, py;
	for (int i = 0; i < h2; i++)
	{
		for (int j = 0; j < w2; j++)
		{
			px = floor(j*x_ratio);
			py = floor(i*y_ratio);
			newVector[(i*w2) + j] = pixels[(int)((py*w1) + px)];
		}
	}
	//TODO add fail catch
	
	return newVector;
}

std::vector<PixelRep> DownSampler::getDownsampledSkip(std::vector<PixelRep> pixels, int downScaler)
{
	//screen is 210 x 160;
	std::vector<PixelRep> newVector;
	for (size_t i = 0; i < 210; i++)
	{
		if (i % downScaler == 0) 
		{
			for (size_t j = 0; j < 160; j++)
			{
				if (j % downScaler == 0) 
				{
					newVector.push_back(pixels[160 * i + j]);
				}
			}
		}
	}

	int controlSize = pixels.size() / (std::pow(downScaler, 2));
	assert(newVector.size() == controlSize);  //size needs to match
	return newVector;
}

std::vector<PixelRep> DownSampler::getDownsampledSkip(std::vector<PixelRep*> pixels, int downScaler)
{
	//assume screen is 210 x 160;
	std::vector<PixelRep> newVector;
	for (size_t i = 0; i < 210; i++)
	{
		if (i % downScaler == 0)
		{
			for (size_t j = 0; j < 160; j++)
			{
				if (j % downScaler == 0)
				{
					newVector.push_back(*pixels[160 * i + j]);
				}
			}
		}
	}

	int controlSize = pixels.size() / (std::pow(downScaler, 2));
	assert(newVector.size() == controlSize);  //size needs to match
	return newVector;
}

std::vector<PixelRep> DownSampler::getDownsampledGrayScale(std::vector<PixelRep>& pixels, int downScaler, bool useRBGGreyScale)
{
	//assume screen is 210 x 160;
	std::vector<PixelRep> newVector;

	int controlSize = pixels.size() / (std::pow(downScaler, 2));

	//new size is pixels.height / downscaler * pixels.width * downscaler
	int width = 160 / downScaler;
	int height = 210 / downScaler;


	//average a downscaler x downscaler area into one pixel 

	for (size_t downSampledI = 0; downSampledI < height; downSampledI++)
	{
		for (size_t downSampledJ = 0; downSampledJ < width; downSampledJ++)
		{
			float grayScale = 0;
			
			for (size_t i = 0; i < downScaler; i++) //row
			{
				for (size_t j = 0; j < downScaler; j++) //column
				{
					if (useRBGGreyScale)			
						grayScale += pixels[(160 * downSampledI * downScaler) + (160 * i) + (j)+downScaler * downSampledJ].getRGBGrayScale();
					else
						grayScale += pixels[(160 * downSampledI * downScaler) + (160 * i) + (j)+downScaler * downSampledJ].getGrayScale();

				}

			}
			grayScale =  grayScale / std::pow(downScaler, 2);
			newVector.push_back(PixelRep(grayScale));
		}
	}

	
	assert(newVector.size() == controlSize);  //size needs to match
	return newVector;
}

DownSampler::DownSampler()
{
}


DownSampler::~DownSampler()
{
}
