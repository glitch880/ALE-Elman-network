//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "GeneticBase.h"
#include <random>
#include <assert.h>
#include <iostream>
#include <fstream>
#include <string>

using namespace std;

static float getRandom(std::mt19937& gen, std::uniform_real_distribution<float>& dis) 
{
	return dis(gen);
}

GeneticBase::GeneticBase(int numberOfChromosomes, int numberOfGenes, float minWeight, float maxWeight) : nrOfGenes(numberOfGenes), bestChromosome(), population(numberOfChromosomes), gen(rd()), valueRange(minWeight, maxWeight)
{
	for (size_t i = 0; i < population.size(); i++)
	{
		population[i].genes.resize(numberOfGenes);
	}
	generation = 0;
}

GeneticBase::GeneticBase(int numberOfChromosomes, int numberOfGenes, float minWeight, float maxWeight, bool createFirstGen) : nrOfGenes(numberOfGenes), bestChromosome(), population(numberOfChromosomes), gen(rd()), valueRange(minWeight,maxWeight)
{
	if(createFirstGen)
		createFirstGeneration();
}

GeneticBase::~GeneticBase()
{
}

void GeneticBase::evolve()
{
	cout << "Evolving...." << endl;

	ensureNoNegative();
	sortPopulation();
	calcAverageFitness();
	std::vector<GeneticChromosome> newPop;

	if (!currentSelection) //wheel
	{
		sigmaScaleFitness();
	}

	for (size_t i = 0; newPop.size() < population.size() - numElites; i++)
	{
		GeneticChromosome childA;
		GeneticChromosome childB;

		GeneticChromosome parentA;
		GeneticChromosome parentB;

		//select two parents
		if (currentSelection) //tourney
		{ 
			parentA.genes = tourneySelect().genes;
			parentB.genes = tourneySelect().genes;
		}
		else			//wheel
		{
			parentA.genes = rouletteWheelSelect().genes;
			parentB.genes = rouletteWheelSelect().genes;
		}


		//combine
		if(!currentCombine)
			combineMultiPoint(parentA, parentB, childA, childB);
		else
			combineTwoPoint(parentA, parentB, childA, childB);

		//mutate
		mutate(childA);
		mutate(childB);

		newPop.push_back(childA);
		newPop.push_back(childB);
	}

	//add elites
	for (size_t j = 0; j < numElites; j++)
	{
		newPop.push_back(population[j]);
	}

	
	assert(newPop.size() == population.size());
	cout << "Evolution complete. Average fitness of previous gen was " << averageFitness << endl;
	population = newPop;
	generation++;
}

std::vector<GeneticChromosome>& GeneticBase::getPopulation()
{
	return population;
}

const GeneticChromosome & GeneticBase::getBestChromosome()
{
	return bestChromosome;	
}

const GeneticChromosome & GeneticBase::getChromosome(int index)
{	
	return population[index];
}

int GeneticBase::getPopulationSize()
{
	return population.size();
}

int GeneticBase::getGeneration()
{
	return generation;
}

void GeneticBase::setMutationChance(float mutChance)
{
	mutationChance = mutChance;
}

void GeneticBase::setCombinationChance(float combChance)
{
	combinationChance = combChance;
}

void GeneticBase::setMPCombinationChance(float combinationChance)
{
	mPCombinationChance = combinationChance;
}

void GeneticBase::setElites(int elites)
{
	numElites = elites;
}

void GeneticBase::setSelectionMethod(bool useTourney)
{
	currentSelection = useTourney;
}

void GeneticBase::setCombinationMethod(bool useTwoPoint)
{
	currentCombine = useTwoPoint;
}

void GeneticBase::saveGeneration(std::string fileName)
{
	//save fitness and weights
	using namespace std; 

	//save weights in fileName
	ofstream myfile(fileName, ios::trunc);
	if (myfile.is_open())
	{		
		for (size_t i = 0; i < population.size(); i++)
		{
			for (size_t j = 0; j < population[i].genes.size(); j++)
			{
				myfile << to_string(population[i].genes[j]) << "\n";
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file :" << fileName;

	//save fitness in FileName_Fitness
	ofstream fitnessFile   (fileName + "_fitness", ios::trunc);
	if (fitnessFile.is_open())
	{
		fitnessFile << generation << "\n";

		for (size_t i = 0; i < population.size(); i++)
		{
			fitnessFile << to_string(population[i].fitness) << "\n";
		}
		fitnessFile.close();
	}
	else cout << "Unable to open file :" << fileName + "_fitness";


	cout << endl <<  "Saving successful" << endl;
}

void GeneticBase::loadGeneration(std::string fileName)
{

	using namespace std;

	//Load weights from fileName
	string nextLine;

	ifstream myfile(fileName);
	if (myfile.is_open())
	{
		for (size_t i = 0; i < population.size(); i++)
		{
			for (size_t j = 0; j < population[i].genes.size(); j++)
			{
				if(getline(myfile, nextLine)) 
				{
					//set weight
					population[i].genes[j] = stof(nextLine);
				}

			}
		}
		myfile.close();
	}
	else 
	{
		cout << "Unable to open file :" << fileName;
		throw "Can't open file";
	}
	//load fitness from FileName_Fitness
	ifstream fitnessFile(fileName + "_fitness");
	if (fitnessFile.is_open())
	{
		if (getline(fitnessFile, nextLine))
			generation = stoi(nextLine);

		for (size_t i = 0; i < population.size(); i++)
		{

			if (getline(fitnessFile, nextLine))
			{
				population[i].fitness = stoi(nextLine);
			}
		}
		fitnessFile.close();
	}
	else 
	{
		cout << "Unable to open file :" << fileName + "_fitness";
		throw "Cant open";
	}
	cout << endl << "Load Successful" << endl;
}

void GeneticBase::saveResults(std::string fileName)
{
	//save fitness and weights
	using namespace std;

	//save results
	ofstream fitnessFile(fileName + "_results", ios::trunc);
	if (fitnessFile.is_open())
	{
		fitnessFile << "On generation " << generation << " with " << population.size() << " individuals" << "\n";
		fitnessFile << "Average fitness: " <<  calcAverageFitness() <<"\n";
		fitnessFile << "Lowest fitness: " << population[population.size() - 1].fitness << "\n";

		for (size_t i = 0; i < population.size(); i++)
		{
			fitnessFile << "Individual: " << i << " fitness score: " << to_string(population[i].fitness) << "\n";
		}
		fitnessFile.close();
	}
	else cout << "Unable to open file :" << fileName + "_fitness";


	cout << endl << "Saving results successful" << endl;

}

void GeneticBase::saveResultsGeneration(std::string fileName)
{
	sortPopulation();
	//save results
	ofstream fitnessFile(fileName + "_results_" + to_string(generation), ios::trunc);
	if (fitnessFile.is_open())
	{
		fitnessFile << "On generation " << generation << " with " << population.size() << " individuals" << "\n";
		fitnessFile << "Average fitness: " << calcAverageFitness() << "\n";
		fitnessFile << "Lowest fitness: " << population[population.size() - 1].fitness << "\n";
		fitnessFile << "Highest fitness: " << population[0].fitness << "\n";

		for (size_t i = 0; i < population.size(); i++)
		{
			fitnessFile << "Individual: " << i << " fitness score: " << to_string(population[i].fitness) << "\n";
		}
		fitnessFile.close();
	}
	else cout << "Unable to open file :" << fileName + "_fitness";


	cout << endl << "Saving results for generation " << to_string(generation) << " successful" << endl;
}

void GeneticBase::createFirstGeneration()
{
	
	for (size_t i = 0; i < population.size(); i++)
	{
		population[i].genes.clear();
		for (size_t j = 0; j < nrOfGenes; j++)
		{
			population[i].genes.push_back(valueRange(gen));
		}
	}
	generation = 0;
}

void GeneticBase::sortPopulation()
{
	//insert sort used

	int j;
	GeneticChromosome temp;

	for (int i = 1; i < population.size(); i++) {
		j = i;

		while (j > 0 && population[j].fitness < population[j - 1].fitness) 
		{
			temp = population[j];
			population[j] = population[j - 1];
			population[j - 1] = temp;
			j--;
		}
	}

	std::reverse(population.begin(), population.end());

	bestChromosome = population[0];
}

float GeneticBase::calcAverageFitness()
{

	float sum = 0;

	for (size_t i = 0; i < population.size(); i++)
	{
		sum += population[i].fitness;
	}

	sum /= population.size();

	averageFitness = sum;

	return sum;
}

void GeneticBase::ensureNoNegative()
{
	for (size_t i = 0; i < population.size(); i++)
	{
		if (population[i].fitness < 0)
			population[i].fitness = 0;
	}
}

GeneticChromosome& GeneticBase::rouletteWheelSelect()
{
	float totalFitness = 0;

	for each (GeneticChromosome var in population)
	{
		totalFitness += var.scaledFitness;
	}
	
	int chosenIndex = 0;

	if ((int)totalFitness <= 0)  // no fitness on any individual = random individual chosen
	{
		std::uniform_int_distribution<int> rand(0, population.size()-1);
		chosenIndex = rand(gen);
		cout << "Parent " << chosenIndex << " chosen randomly" << endl;
		return population[chosenIndex];
	}

	std::uniform_real_distribution<float> range(0, totalFitness);
	float chosenFitness = range(gen);

	for (size_t i = 0; i < population.size(); i++)
	{
		chosenFitness -= population[i].scaledFitness;
		if (chosenFitness <= 0)
		{
			chosenIndex = i;
			break;
		}
	}

	return population[chosenIndex];
}

GeneticChromosome & GeneticBase::tourneySelect()
{
	std::uniform_int_distribution<int> range(0, population.size()-1);
	int chosenIndex = 0;
	int HighestFitness = 0;

	for (size_t i = 0; i < numElites; i++)
	{
		if (i != 0)
		{
			int tempIndex = range(gen);
			if (HighestFitness < population[tempIndex].fitness)
			{
				HighestFitness = population[tempIndex].fitness;
				chosenIndex = tempIndex;
			}
		}
		else 
		{
			int tempIndex = range(gen);
			HighestFitness = population[tempIndex].fitness;
			chosenIndex = tempIndex;
		}
	}


	return population[chosenIndex];
}

void GeneticBase::combineMultiPoint(GeneticChromosome & parentA, GeneticChromosome & parentB, GeneticChromosome & childA, GeneticChromosome & childB)
{

	childA.genes = parentA.genes;
	childB.genes = parentB.genes;

	std::uniform_real_distribution<float> range(0, 1);
	
	for (size_t i = 0; i < childA.genes.size(); i++)
	{
		if (range(gen) < mPCombinationChance)
		{
			childA.genes[i] = parentB.genes[i];
		}
	}
	for (size_t i = 0; i < childB.genes.size(); i++)
	{
		if (range(gen) < mPCombinationChance)
		{
			childB.genes[i] = parentA.genes[i];
		}
	}

}

void GeneticBase::combineTwoPoint(GeneticChromosome & parentA, GeneticChromosome & parentB, GeneticChromosome & childA, GeneticChromosome & childB)
{
	childA.genes = parentA.genes;
	childB.genes = parentB.genes;

	std::uniform_real_distribution<float> chanceToCombine(0, 1);

	if (chanceToCombine(gen) < combinationChance) 
	{

		std::uniform_int_distribution<int> range(0, nrOfGenes);

		int firstIndex = range(gen);
		int lastIndex = range(gen);
	
		//replace values between the index
		for (size_t i = firstIndex; i < lastIndex; i++)
		{
			childA.genes[i] = parentB.genes[i];
		}
		for (size_t i = firstIndex; i < lastIndex; i++)
		{
			childB.genes[i] = parentA.genes[i];
		}

	}
}

void GeneticBase::mutate(GeneticChromosome & chromosomeToMutate)
{
	std::uniform_real_distribution<float> range(0, 1);

	for (size_t i = 0; i < chromosomeToMutate.genes.size(); i++)
	{
		if (range(gen) < mutationChance)
			chromosomeToMutate.genes[i] = valueRange(gen);
	}
}

void GeneticBase::sigmaScaleFitness()
{
	double RunningTotal = 0;
	//first iterate through the population to calculate the standard
	//deviation
	for (int i = 0; i < population.size(); ++i)
	{
		RunningTotal += (population[i].fitness - averageFitness) * (population[i].fitness - averageFitness);
	}

	if (RunningTotal <= 0)
	{
		for (int i = 0; i < population.size(); ++i)
		{			
				population[i].scaledFitness = 0.0;
		}
		return;
	}

	double variance = RunningTotal / (double)population.size();

	//standard deviation is the square root of the variance
	float sigma = sqrt(variance);

	for (int i = 0; i < population.size(); ++i)
	{
		double OldFitness = population[i].fitness;
		population[i].scaledFitness = (OldFitness - averageFitness) / (2 * sigma);
	
		if (population[i].scaledFitness < 0.0)
			population[i].scaledFitness = 0.0;
	}


}
