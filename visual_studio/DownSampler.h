//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#pragma once
#include <vector>
#include "PixelRep.h"

class DownSampler  //TODO optimize these functions
{
public:
	//return vector with nearest neighbour 
	static std::vector<PixelRep> getDownsampledNeighbour(std::vector<PixelRep>& pixels, int downScaler);
	//return downscaled pixel array using skipped method
	static std::vector<PixelRep> getDownsampledSkip(std::vector<PixelRep> pixels, int downScaler);
	static std::vector<PixelRep> getDownsampledSkip(std::vector<PixelRep*> pixels, int downScaler);

	//
	static std::vector<PixelRep> getDownsampledGrayScale(std::vector<PixelRep>& pixels, int downScaler, bool useRGBGrayScale);

private:
	DownSampler();
	~DownSampler();

};

