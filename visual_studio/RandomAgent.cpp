//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "ale_interface.hpp"
#include <iostream>
#include <ale_interface.hpp>

using namespace std;

void saveResults(string fileName, vector<reward_t>& scores);

const unsigned int FRAME_SKIP = 15;
const unsigned int MAX_FRAMES = 50000;

int main(int argc, char** argv) {
	if (argc < 4) {
		std::cerr << "Usage: " << argv[0] << " rom_file" << " SaveResult" << "How many runs"  << std::endl;
		return 1;
	}

	ALEInterface ale;
	
	srand((unsigned)time(0));

	// Get & Set the desired settings
	ale.setInt("random_seed", time(0));

	ale.setFloat("repeat_action_probability", 0.0);

	//set frame skip. Means we dont execute on every frame
	ale.setInt("frame_skip", FRAME_SKIP);

	ale.setInt("max_num_frames_per_episode", MAX_FRAMES);

	ale.setBool("display_screen", true);
	ale.setBool("sound", false);
	
	// Load the ROM file. (Also resets the system for new settings to
	// take effect.)
	ale.loadROM(argv[1]);

	// Get the vector of legal actions
	ActionVect actions = ale.getMinimalActionSet();

	int test_runs = stoi(argv[3]);

	//results
	vector<reward_t> scores(test_runs);


	for (int episode = 0; episode < test_runs; episode++)
	{
		float totalReward = 0;
		while (!ale.game_over())
		{
			Action a = actions[rand() % actions.size()];
			// Apply the action and get the resulting reward
			float reward = ale.act(a);
			totalReward += reward;
		}
		cout << "Episode " << episode << " ended with score: " << totalReward << endl;
		scores[episode] = totalReward;
		ale.reset_game();
	}

	saveResults(argv[2],scores);

	return 0;
}


void saveResults(string fileName, vector<reward_t>& scores)
{
	//calc avg fitness
	float avgFitness = 0;

	for (size_t i = 0; i < scores.size(); i++)
	{
		avgFitness += scores[i];
	}
	avgFitness = avgFitness / scores.size();

	sort(scores.begin(), scores.end());


	ofstream SaveFile(fileName + "_results", ios::trunc);
	if (SaveFile.is_open())
	{
		SaveFile << "Average score: " << avgFitness << "\n";
		SaveFile << "Lowest score: " << scores[0] << "\n";
		SaveFile << "Highest score: " << scores[scores.size() - 1] << "\n";
		SaveFile << "Easy copy-pase below:" << endl;

		for (size_t i = 0; i < scores.size(); i++)
		{
			SaveFile << to_string(scores[i]) << "\n";
		}
		

		SaveFile.close();
	}
	else cout << "Unable to open file :" << fileName;


	cout << endl << "Saving results successful" << endl;
}