//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include <iostream>
#include <fstream>
#include <vector>
#include <ale_interface.hpp>
#include <SDL.h>
#include "PixelRep.h"
#include "FeedForwardNetwork.h"
#include "GeneticBase.h"
#include "DownSampler.h"

using namespace std;

void aleThreadUser(GeneticChromosome& chromosome, int index, string rom);

const unsigned int DOWNSAMPLING_FACTOR = 5;  //1 / 2 / 5 / 10 are valid factors

const unsigned int NUM_INPUT = 33600 / pow(DOWNSAMPLING_FACTOR, 2); //downscaled by downsampling_factor
const unsigned int NUM_HIDDEN = 100; //configurable

const unsigned int NUM_CHROMOSOMES = 100;  // must be even

const unsigned int MAX_FRAMES = 50000;

int main(int argc, char** argv) {
	if (argc < 5) {
		std::cerr << "Usage: " << argv[0] << " rom_file" << " load location" << " save location " << "Frame skip" <<  "Show gameplay (0/1)" << std::endl;

		return 1;
	}

	std::ios::sync_with_stdio();
	srand((unsigned)time(0));
	
	ALEInterface ale;
		
	ale.setFloat("repeat_action_probability", 0.0);

	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));

	//set frame skip. Means we dont execute on every frame
	ale.setInt("frame_skip", stoi(argv[4]));

	ale.setInt("max_num_frames_per_episode", 0);

	ale.setBool("display_screen", stoi(argv[5]));
	ale.setBool("sound", false);


	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(argv[1]);

	//get vector of actions
	ActionVect minimal_actions = ale.getMinimalActionSet();

	std::vector<pixel_t> greyScaleVector;
	std::vector<pixel_t> RGBVector(100800);
	std::vector<PixelRep> pixelVector;

	ale.theOSystem->colourPalette().setPalette("standard", "SECAM");

	//create FF network
	FeedForwardNetwork feedForwardNet(NUM_INPUT, NUM_HIDDEN, minimal_actions.size(), false);

	std::vector<float> inputVector(NUM_INPUT);
	std::vector<float> outputVector(minimal_actions.size());

	//Genetics
	GeneticBase ga(NUM_CHROMOSOMES, feedForwardNet.getConnectionTotal(), -10, 10);

	
	cout << "Loading from file " << argv[2] << endl;
	ga.loadGeneration(argv[2]);
		
	// Play populationSize episodes
	for (int episode = 0; episode < ga.getPopulationSize(); episode++)
	{
		//set new weights
		feedForwardNet.assignweights(ga.getPopulation()[episode].genes);
			

		reward_t totalReward = 0;
		while (!ale.game_over())
		{
			pixelVector.clear();
			ale.getScreenGrayscale(greyScaleVector);
			ale.getScreenRGB(RGBVector);
			const ALEScreen& temp = ale.getScreen();

			//Convert pixelvalues/ greyscale to full pixelreps
			for (size_t i = 0; i < temp.height(); i++)
			{
				for (size_t j = 0; j < temp.width(); j++)
				{
					pixelVector.push_back(PixelRep(greyScaleVector[160 * i + j], RGBVector[(160 * 3)*i + j * 3], RGBVector[(160 * 3)*i + j * 3 + 1], RGBVector[(160 * 3)*i + j * 3 + 2]));
				}
			}

			std::vector<PixelRep> downsampledInput(DownSampler::getDownsampledGrayScale(pixelVector, DOWNSAMPLING_FACTOR, false));  //downsample the screen info

			assert(downsampledInput.size() == inputVector.size());

			for (size_t i = 0; i < downsampledInput.size(); i++)
			{
				inputVector[i] = downsampledInput[i].getNormalizedGrayScale(false);
			}

			outputVector = feedForwardNet.feedForward(inputVector);

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

	}
	
	ga.sortPopulation();
	//save test results
	//ga.saveGeneration(argv[2]);
	ga.saveResults(argv[3]);
	

	return 0;

}
