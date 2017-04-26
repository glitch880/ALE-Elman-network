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

const unsigned int NUM_INPUT = 33600 / pow(DOWNSAMPLING_FACTOR, 2); //downscaled by downsampling_factor
const unsigned int NUM_HIDDEN = 100; //configurable
const unsigned int NUM_CONTEXT = NUM_HIDDEN;

const unsigned int NUM_GENERATIONS = 150;
const unsigned int NUM_CHROMOSOMES = 100;

const unsigned int MAX_FRAMES = 50000;

int main(int argc, char** argv) {
	if (argc < 5) {
		std::cerr << "Usage: " << argv[0] << " rom_file " << " File to Load" << " SaveLocation " << "frameskip" << "Show gameplay (0/1)" << ". You used " << argc << std::endl;
		return 1;
	}

	std::ios::sync_with_stdio();

	srand((unsigned)time(0));

	std::cout << endl << "Creating network." << endl;

	ALEInterface ale;

	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));

	ale.setFloat("repeat_action_probability", 0.0);

	//set frame skip. Means we dont execute on every frame
	ale.setInt("frame_skip", stoi(argv[4]));

	ale.setInt("max_num_frames_per_episode", 0);

	ale.setBool("display_screen", stoi(argv[5]));
	ale.setBool("sound", false);

	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(argv[1]);

	cout << endl << "Filename to save in: " << argv[2] << endl;
		
	ActionVect minimal_actions = ale.getMinimalActionSet();

	std::vector<pixel_t> greyScaleVector;
	std::vector<PixelRep> pixelVector;

	ale.theOSystem->colourPalette().setPalette("standard", "SECAM");

	// elman
	ElmanNetwork elman(NUM_INPUT, NUM_HIDDEN, minimal_actions.size(), false);

	std::vector<float> inputVector(NUM_INPUT);
	std::vector<float> outputVector(minimal_actions.size());

	// Genetics
	GeneticBase ga(NUM_CHROMOSOMES, elman.getConnectionTotal(), -10, 10);

	ga.loadGeneration(argv[2]);
	
	// Play 1 episode per agent
	for (int episode = 0; episode < ga.getPopulationSize(); episode++)
	{
		//set new weights
		elman.assignweights(ga.getPopulation()[episode].genes);

		reward_t totalReward = 0;
		
		

		while (!ale.game_over())
		{
			pixelVector.clear();
			ale.getScreenGrayscale(greyScaleVector);
			const ALEScreen& temp = ale.getScreen();

			//output_rgb_buffer[(160 * 3) * 1 + 52 * 3 + 1]    = 2nd row 53 column green

			//Convert pixelvalues/ greyscale to full pixelreps
			for (size_t i = 0; i < temp.height(); i++)
			{
				for (size_t j = 0; j < temp.width(); j++)
				{
					//create pixel with r,g,b
					//pixelVector.push_back(new PixelRep(colorVec[(160 * 3)*i + j * 3], colorVec[(160 * 3)*i + j * 3 + 1], colorVec[(160 * 3)*i + j * 3 + 2]));

					//greyscale
					pixelVector.push_back(PixelRep(greyScaleVector[160 * i + j]));
				}
			}

			std::vector<PixelRep> downsampledInput(DownSampler::getDownsampledGrayScale(pixelVector, DOWNSAMPLING_FACTOR, false));  //downsample the screen info

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

			std::string yes;

		}
		cout << "Episode " << episode << " ended with score: " << totalReward << endl;
		ga.getPopulation()[episode].fitness = totalReward;
		ale.reset_game();
		elman.clearContext();
	}
	
	ga.sortPopulation();
	//save results
//	ga.saveGeneration(argv[3]);
	ga.saveResults(argv[3]);

	return 0;
}