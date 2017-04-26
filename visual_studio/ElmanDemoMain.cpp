//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include <iostream>
#include <fstream>
#include <vector>
#include <ale_interface.hpp>

#include <SDL.h>
#include "PixelRep.h"
#include "ElmanNetwork.h"
#include "DownSampler.h"
#include "GeneticBase.h"
#include <math.h>

using namespace std;

const unsigned int DOWNSAMPLING_FACTOR = 5;
const unsigned int MAX_FRAMES = 50000;

const unsigned int NUM_INPUT = 33600 / pow(DOWNSAMPLING_FACTOR, 2); //downscaled by downsampling_factor
const unsigned int NUM_HIDDEN = 100; //configurable
const unsigned int NUM_CONTEXT = NUM_HIDDEN;

const unsigned int NUM_GENERATIONS = 150;
const unsigned int NUM_CHROMOSOMES = 100;

int main(int argc, char** argv) {
	if (argc < 5) {
		std::cerr << "Usage: " << argv[0] << " rom_file " << " File to Load" << "faked frame skip" << "Which index to play" << std::endl;
		return 1;
	}

	std::ios::sync_with_stdio();

	srand((unsigned)time(0));

	ALEInterface ale;


	//set frame skip. 
	ale.setInt("frame_skip", stoi(argv[3]));
	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));
	
	ale.setFloat("repeat_action_probability", 0.0);

	ale.setInt("max_num_frames_per_episode", MAX_FRAMES);

	ale.setBool("display_screen", true);
	ale.setBool("sound", false);

	//save frames to make movie
	string recordPath = argv[2];
	recordPath += "_frames";

	// Set record flags
	ale.setString("record_screen_dir", recordPath.c_str());

	// Not completely portable, but will work in most cases
	string cmd = "mkdir ";
	cmd += recordPath;
	system(cmd.c_str());

	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(argv[1]);

	ActionVect minimal_actions = ale.getMinimalActionSet();

	std::vector<pixel_t> greyScaleVector;
	std::vector<pixel_t> colorVec;
	std::vector<PixelRep> pixelVector;

	ale.theOSystem->colourPalette().setPalette("standard", "SECAM");


	std::cout << endl << "Creating network." << endl;
	// elman
	ElmanNetwork elman(NUM_INPUT, NUM_HIDDEN, minimal_actions.size(), false);

	std::vector<float> inputVector(NUM_INPUT);
	std::vector<float> outputVector(minimal_actions.size());

	// Genetics
	GeneticBase ga(NUM_CHROMOSOMES, elman.getConnectionTotal(), -1, 1);
	cout << "Loading..." << endl;
	ga.loadGeneration(argv[2]);

	//set new weights
	elman.assignweights(ga.getPopulation()[stoi(argv[4])].genes);

	reward_t totalReward = 0;
	while (!ale.game_over())
	{
		//decide if "skipping" frame
		pixelVector.clear();
		
			ale.getScreenGrayscale(greyScaleVector);
			ale.getScreenRGB(colorVec);
			const ALEScreen& temp = ale.getScreen();

			//output_rgb_buffer[(160 * 3) * 1 + 52 * 3 + 1]    = 2nd row 53 column green

			//Convert pixelvalues/ greyscale to full pixelreps
			for (size_t i = 0; i < temp.height(); i++)
			{
				for (size_t j = 0; j < temp.width(); j++)
				{
					pixelVector.push_back(PixelRep(greyScaleVector[160 * i + j], colorVec[(160 * 3)*i + j * 3], colorVec[(160 * 3)*i + j * 3 + 1], colorVec[(160 * 3)*i + j * 3 + 2]));
				}
			}

			std::vector<PixelRep> downsampledInput(DownSampler::getDownsampledGrayScale(pixelVector, DOWNSAMPLING_FACTOR,false));  //downsample the screen info

			for (size_t i = 0; i < downsampledInput.size(); i++)
			{
				inputVector[i] = downsampledInput[i].getNormalizedGrayScale(false);
			}

			outputVector = elman.propogate(inputVector);

			float bestCommand = 0;
			int indexBestCommand = 0;

			for (size_t i = 0; i < outputVector.size(); i++)
			{
				if (outputVector[i] > bestCommand)
				{
					bestCommand = outputVector[i];
					indexBestCommand = i;
				}
			}

			Action bestA = minimal_actions[indexBestCommand];
		
			reward_t reward = ale.act(bestA);
			totalReward += reward;
			
	}

	cout << "Episode " << argv[4] << " ended with score: " << totalReward << endl;
	


	return 0;
}