//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include <iostream>
#include <fstream>
#include <vector>
#include <ale_interface.hpp>
#include <thread>

#include <SDL.h>
#include "PixelRep.h"
#include "FeedForwardNetwork.h"
#include "GeneticBase.h"
#include "DownSampler.h"

using namespace std;

void WriteScreen(const ALEScreen& screen) 
{
	// writing on a text file
		
	ofstream myfile("frameoutput.txt");
	if (myfile.is_open(), ios::trunc)
	{
		for (int i = 0; i < screen.height(); i++)
		{
			for (int j = 0; j <screen.width(); j++)
			{
				int p = screen.get(i,j);
				string textio = "pixel " + to_string( i);
				textio += ", " + to_string( j) ;
				textio += " :" + to_string( p) + " ";


				//if (p != 0)
					myfile <<  to_string(p) + " ";
					//myfile << "reRo";
			}
			myfile << "\n";
		}

		myfile.close();
	}
	else cout << "Unable to open file";
		
}

void WriteScreen(std::vector<PixelRep*>& pixels, const ALEScreen& screen)
{
	// writing on a text file

	ofstream myfile("frameoutputPIXEL.txt");
	if (myfile.is_open(), ios::trunc)
	{
		for (int i = 0; i < screen.height(); i++)
		{
			for (int j = 0; j <screen.width(); j++)
			{
				string textio = " Pixel (" + to_string(i) + "," + to_string(j) + ") has RGB: " + to_string(pixels[160 * i + j]->getRed()) + " " + to_string(pixels[160 * i + j]->getGreen()) + " " + to_string(pixels[160 * i + j]->getBlue()) + "\n";
				if (pixels[160 * i + j]->getRed() != 0 || pixels[160 * i + j]->getGreen() != 0 || pixels[160 * i + j]->getBlue() != 0)
					myfile << textio;
				else
					myfile << "Black pixel at (" + to_string(i) + "," + to_string(j) + ") \n";
			}
			myfile << "\n";
		}

		myfile.close();
	}
	else cout << "Unable to open file";

}

void WriteGrayScreen(std::vector<pixel_t> greys, const ALEScreen& screen)
{
	// writing on a text file
	int val = 0;
	ofstream myfile("frameoutputPIXEL.txt");
	if (myfile.is_open(), ios::trunc)
	{
		for (int i = 0; i < screen.height(); i++)
		{
			for (int j = 0; j <screen.width(); j++)
			{
				string textio = " Pixel (" + to_string(i) + "," + to_string(j) + ") has grey: " + to_string(greys[160 * i + j]) + "\n";
				val = greys[160 * i + j];
				if(greys[160 * i + j] != 0 && greys[160 * i + j] != 255 && greys[160 * i + j] != 58 && greys[160 * i + j] != 121 && greys[160 * i + j] != 152 && val != 188 && val != 217 && val != 233 && val != 63)
				myfile << textio;
			}
			if (val != 0 && val != 255 && val != 58 && val != 121 && val != 152)
			myfile << "\n";
		}

		myfile.close();
	}
	else cout << "Unable to open file";

}

void WriteGrayScreen(std::vector<pixel_t> greys, std::vector<PixelRep*>& pixels, const ALEScreen& screen)
{
	// writing on a text file
	int val = 0;
	PixelRep pval(0);
	ofstream myfile("frameoutputPIXEL.txt");
	if (myfile.is_open(), ios::trunc)
	{
		for (int i = 0; i < screen.height(); i++)
		{
			for (int j = 0; j <screen.width(); j++)
			{
				val = greys[160 * i + j];
				pval = *pixels[160 * i + j];
				string textio = " Pixel (" + to_string(i) + "," + to_string(j) + ") has grey: " + to_string(greys[160 * i + j]) + " and color :" + to_string( pval.getRed()) + " " + to_string( pval.getGreen()) + " " + to_string(pval.getBlue()) + "\n";
				
				if (val != 0 && val != 255 && val != 58 && val != 121 && val != 152 && val != 188 && val != 217 && val != 233 && val != 63)
					myfile << textio;
			}
			if (val != 0 && val != 255 && val != 58 && val != 121 && val != 152)
				myfile << "\n";
		}

		myfile.close();
	}
	else cout << "Unable to open file";

}

//creates a new ALE environment to play Atari 
void aleThreadUser(GeneticChromosome& chromosome, int index, string rom, bool grayscaleCustom, bool useLegalSet);



const unsigned int FPS = 60;
const unsigned int MIN_WITHOUT_SCORE = 3;

const unsigned int DOWNSAMPLING_FACTOR = 5;  //1 / 2 / 5 / 10 are valid factors

const unsigned int NUM_INPUT = 33600 / pow(DOWNSAMPLING_FACTOR, 2); //downscaled by downsampling_factor
const unsigned int NUM_HIDDEN = 100; //configurable

const float MAX_WEIGHT = 1;
const float MIN_WEIGHT = -1;

const unsigned int NUM_GENERATIONS = 150;
const unsigned int NUM_CHROMOSOMES = 100;  // must be even

const unsigned int FRAME_SKIP = 15;
const unsigned int MAX_FRAMES = 50000;


int main(int argc, char** argv) {
	if (argc < 5) { // argv[1] = rom , 2 = save FileName, 3 = how often to save, 4 = If gameplay should be shown or not (faster if not) , 5 = to resume evolution from file, 6 = where to load evolution from
		std::cerr << "Usage: " << argv[0] << " rom_file" << " Save location" << " How often to save " << " Show gameplay (0/1) " <<" Wether to load from file or start from gen 1 (0 / 1)" << " <file to load from>" << std::endl;

		return 1;
	}


	bool customGrayscale = 0;
	bool useLegalSet = 0;
	int customGenerationStop = 0;
	//to not make the args list massive, change rest to cin input

	//use ALE GRAY or custom  (0/1) "use custom grayscale calc?"
	cout << "Use ALE grayScale or custom (0/1): ";
	cin >> customGrayscale;
	//What gen to stop at
	cout << "Stop at generation(0 uses standard 150 gens): ";
	cin >> customGenerationStop;

	//to use minimal or legal action set
	cout << "Minimal or legal action set (0/1): ";
	cin >> useLegalSet;
	cout << endl;


	int totalGensToRun = customGenerationStop <= 0 ? NUM_GENERATIONS : customGenerationStop;

	std::ios::sync_with_stdio();
	srand((unsigned)time(0));

	ALEInterface ale;

	ale.setFloat("repeat_action_probability", 0.0);

	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));

	//set frame skip. Means we dont execute on every frame
	ale.setInt("frame_skip", FRAME_SKIP);

	ale.setInt("max_num_frames_per_episode", MAX_FRAMES);

	ale.setBool("display_screen", stoi(argv[4]));
	ale.setBool("sound", false);


	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(argv[1]);

	//get vector of actions
	ActionVect actions;
	if (!useLegalSet)
		actions = ale.getMinimalActionSet();
	else
		actions = ale.getLegalActionSet();

	std::vector<pixel_t> greyScaleVector(33600);
	std::vector<pixel_t> RGBVector(100800);
	std::vector<PixelRep> pixelVector;
		
	ale.theOSystem->colourPalette().setPalette("standard", "SECAM");	

	//create FF network
	FeedForwardNetwork feedForwardNet(NUM_INPUT, NUM_HIDDEN, actions.size(), false);

	std::vector<float> inputVector(NUM_INPUT);
	std::vector<float> outputVector(actions.size());

	//Genetics
	GeneticBase ga(NUM_CHROMOSOMES, feedForwardNet.getConnectionTotal(), MIN_WEIGHT, MAX_WEIGHT);
	ga.setSelectionMethod(false);
	

	if (stoi(argv[5]) == 0) // no load
	{
		cout << "no loading" << endl;
		ga.createFirstGeneration();		
	}
	else           //load generation to continue training
	{
		cout << "Loading from file" << endl;
		ga.loadGeneration(argv[6]);		
	}

	//evolution loop
	for (size_t evoLoop = ga.getGeneration(); evoLoop < NUM_GENERATIONS; evoLoop++)
	{
		cout << endl << "Generation: " << to_string(ga.getGeneration()) << " Starting..." << endl;
		// Play populationSize episodes
		for (int episode = 0; episode < ga.getPopulationSize(); episode++)
		{
			//set new weights
			feedForwardNet.assignweights(ga.getPopulation()[episode].genes);

			int currentEp = episode;

			//Create thread to speed up execution
			episode++;
			std::thread threadTest1(aleThreadUser, ref(ga.getPopulation()[episode]), episode, argv[1], customGrayscale, useLegalSet);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			episode++;
			std::thread threadTest2(aleThreadUser, ref(ga.getPopulation()[episode]), episode, argv[1], customGrayscale, useLegalSet);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			episode++;
			std::thread threadTest3(aleThreadUser, ref(ga.getPopulation()[episode]), episode, argv[1], customGrayscale, useLegalSet);
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			episode++;
			std::thread threadTest4(aleThreadUser, ref(ga.getPopulation()[episode]), episode, argv[1], customGrayscale, useLegalSet);

			reward_t totalReward = 0;

			//timer to stop agents if no change in score in X time
			reward_t lastFrameTotalReward = 0;
			int frameCounter = 0;

			while (!ale.game_over())
			{
				pixelVector.clear();
				ale.getScreenGrayscale(greyScaleVector);
				ale.getScreenRGB(RGBVector);
				const ALEScreen& temp = ale.getScreen();

				//output_rgb_buffer[(160 * 3) * 1 + 52 * 3 + 1]    = 2nd row 53 column green

				//Convert pixelvalues/ greyscale to full pixelreps
				for (size_t i = 0; i < temp.height(); i++)
				{
					for (size_t j = 0; j < temp.width(); j++)
					{					
						//greyscale
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

				lastFrameTotalReward = totalReward;
				Action bestA = actions[indexBestCommand];
				reward_t reward = ale.act(bestA);
				totalReward += reward;

				//used to terminate early if no score changes
				if (lastFrameTotalReward == totalReward)
				{
					frameCounter += FRAME_SKIP;
					if (frameCounter >= FPS * 60 * MIN_WITHOUT_SCORE)
					{
						cout << currentEp << " skipped further play because of no score change in " << MIN_WITHOUT_SCORE << " minutes" << endl;
						break;
					}
				}
				else
					frameCounter = 0;

			}
			cout << "Episode " << currentEp << " ended with score: " << totalReward << endl;
			ga.getPopulation()[currentEp].fitness = totalReward;

			threadTest1.join();
			threadTest2.join();
			threadTest3.join();
			threadTest4.join();
			ale.reset_game();

		}
		ga.saveResultsGeneration(argv[2]);
		ga.evolve();  
		
		if (ga.getGeneration() % stoi(argv[3]) == 0)  //save incase of crash or having to quit app
			ga.saveGeneration(argv[2]);
	}
		
	return 0;

}

//just create more consoles to play simulataenous
void aleThreadUser(GeneticChromosome& chromosomes, int index, string rom, bool grayscaleCustom, bool useLegalSet)
{
	ALEInterface ale;

	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));
	
	ale.setFloat("repeat_action_probability", 0.0);

	//set frame skip. Means we dont execute on every frame
	ale.setInt("frame_skip", FRAME_SKIP);

	ale.setInt("max_num_frames_per_episode", MAX_FRAMES);

	ale.setBool("display_screen", false);
	ale.setBool("sound", false);

	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(rom);

	//get vector of actions
	ActionVect actions;
	if (!useLegalSet)
		actions = ale.getMinimalActionSet();
	else
		actions = ale.getLegalActionSet();

	std::vector<pixel_t> greyScaleVector(33600);
	std::vector<pixel_t> RGBVector(100800);
	std::vector<PixelRep> pixelVector;

	ale.theOSystem->colourPalette().setPalette("standard", "SECAM");

	std::vector<float> inputVector(NUM_INPUT);
	std::vector<float> outputVector(actions.size());

	// elman
	FeedForwardNetwork feedForwardNet(NUM_INPUT, NUM_HIDDEN, actions.size(), false);

	feedForwardNet.assignweights(chromosomes.genes);

	reward_t totalReward = 0;
	//timer to stop agents if no change in score in X time
	reward_t lastFrameTotalReward = 0;
	int frameCounter = 0;

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
				pixelVector.push_back(PixelRep(greyScaleVector[160 * i + j], RGBVector[(160 * 3)*i + j * 3], RGBVector[(160 * 3)*i + j * 3 + 1], RGBVector[(160 * 3)*i + j * 3 + 2]));
			}
		}

		std::vector<PixelRep> downsampledInput(DownSampler::getDownsampledGrayScale(pixelVector, DOWNSAMPLING_FACTOR, false));  //downsample the screen info

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
		lastFrameTotalReward = totalReward;
		Action bestA = actions[indexBestCommand];
		reward_t reward = ale.act(bestA);
		totalReward += reward;

		if (lastFrameTotalReward == totalReward)
		{
			frameCounter += FRAME_SKIP;
			if (frameCounter >= FPS * 60 * MIN_WITHOUT_SCORE)
			{
				cout << index << " skipped further play because of no score change in " << MIN_WITHOUT_SCORE << " minutes" << endl;
				break;
			}
		}
		else
			frameCounter = 0;

	}
	cout << "Episode " << "Thread " << index << " ended with score: " << totalReward << endl;  
	chromosomes.fitness = totalReward;
}