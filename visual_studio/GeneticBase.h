//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#pragma once
#include <vector>
#include <random>

struct GeneticChromosome
{
	GeneticChromosome() : genes(), scaledFitness(0), fitness(0) {};
		
	std::vector<float> genes;
	float scaledFitness; //only used by roulette
	int fitness;
};

class GeneticBase
{
public:

	GeneticBase(int numberOfChromosomes, int numberOfGenes, float minWeight, float maxWeight);
	GeneticBase(int numberOfChromosomes, int numberOfGenes, float minWeight, float maxWeight, bool createFirstGen);
	~GeneticBase();
	
	void evolve();
	std::vector<GeneticChromosome>& getPopulation();
	const GeneticChromosome& getBestChromosome();
	const GeneticChromosome& getChromosome(int index);
	int getPopulationSize();
	int getGeneration();
	void setMutationChance(float mutationChance);
	void setCombinationChance(float combinationChance);
	void setMPCombinationChance(float combinationChance);
	//needs to be even
	void setElites(int elites);
	//sets the selection method. true = tourney, false = roulette
	void setSelectionMethod(bool useTourney);
	//True = twoPoint false = Multi-point
	void setCombinationMethod(bool useTwoPoint);

	void saveGeneration(std::string fileName);
	void loadGeneration(std::string fileName);
	void saveResults(std::string fileName);
	void saveResultsGeneration(std::string fileName);

	void createFirstGeneration();
	void sortPopulation();
	float calcAverageFitness();
	void ensureNoNegative();

private:

	GeneticChromosome& rouletteWheelSelect();
	GeneticChromosome& tourneySelect();
	void combineMultiPoint(GeneticChromosome& parentA, GeneticChromosome& parentB, GeneticChromosome& childA, GeneticChromosome& childB);
	void combineTwoPoint(GeneticChromosome& parentA, GeneticChromosome& parentB, GeneticChromosome& childA, GeneticChromosome& childB);
	void mutate(GeneticChromosome& chromosomeToMutate);
	void sigmaScaleFitness();

	int nrOfGenes;
	GeneticChromosome bestChromosome;
	std::vector<GeneticChromosome> population;
	float combinationChance = 0.75;
	float mPCombinationChance = 0.5;
	float mutationChance = 0.01;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_real_distribution<float> valueRange;
	int numElites = 4;
	int tourneySelectionAmount = 10;
	// 0 = wheel. 1 = tourney
	bool currentSelection = 0;
	// 0 = MP, 1 = twoPoint
	bool currentCombine = 0;
	int generation = 0;
	float averageFitness;
};

