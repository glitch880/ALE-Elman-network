//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "FeedForwardNetwork.h"
#include <assert.h>


typedef std::vector<float> fVector;

FeedForwardNetwork::FeedForwardNetwork(unsigned int nInput, unsigned int nHidden, unsigned int nOutput, bool randomizeWeights)
	: num_input(nInput), num_hidden(nHidden), num_output(nOutput),  wih(nInput + 1, fVector(nHidden)),
	who(nHidden + 1, fVector(nOutput)), inputs_val(nInput), hidden_val(nHidden), output_val(nOutput)
{
	if (randomizeWeights)
		assignRandomWeights();
	calculateTotalConnections();
}


FeedForwardNetwork::~FeedForwardNetwork()
{
}

std::vector<float>& FeedForwardNetwork::feedForward(std::vector<float>& input)
{
	float sum;

	for (size_t i = 0; i < input.size(); i++)
	{
		inputs_val[i] = input[i];
	}

	//Calculate input connections to hidden layer.
	for (int hid = 0; hid <= (num_hidden - 1); hid++) 
	{
		sum = 0.0;
		//from input to hidden...
		for (int inp = 0; inp <= (num_input - 1); inp++)
		{
			sum += inputs_val[inp] * wih[inp][hid];
		} // inp
		
		  //Add in bias.
		sum += bias * wih[num_input][hid];
		
		hidden_val[hid] = sigmoid(sum);
	} // hid

	  //Calculate the hidden to output layer.
	for (int out = 0; out <= (num_output - 1); out++)
	{
		sum = 0.0;
		for (int hid = 0; hid <= (num_hidden - 1); hid++)
		{
			sum += hidden_val[hid] * who[hid][out];
		} // hid

		  //Add in bias.
		sum += bias * who[num_hidden][out];
		output_val[out] = sigmoid(sum);
	} // out	 

	return output_val;
}

void FeedForwardNetwork::assignRandomWeights() //TODO unused/remove
{
	for (int inp = 0; inp <= num_input; inp++) {
		for (int hid = 0; hid <= (num_hidden - 1); hid++) {
			//Assign a random weight value between --5 and 5
			wih[inp][hid] = rand() % 11 - 5;
		} // hid
	} // inp
	
	for (int hid = 0; hid <= num_hidden; hid++) {
		for (int out = 0; out <= (num_output - 1); out++) {
			//Assign a random weight value between -10 and 10
			who[hid][out] = rand() % 11 - 5;
		} // out
	} // hid
}

void FeedForwardNetwork::assignweights(const std::vector<float>& weights)
{
	assert(weights.size() == num_connections);
	int globalIndex = 0;
	for (size_t i = 0; i < wih.size(); i++)
	{
		for (size_t j = 0; j < wih[i].size(); j++)
		{
			wih[i][j] = weights[globalIndex];
			globalIndex++;
		}
	}

	for (size_t i = 0; i < who.size(); i++)
	{
		for (size_t j = 0; j < who[i].size(); j++)
		{
			who[i][j] = weights[globalIndex];
			globalIndex++;
		}
	}
	
	assert(globalIndex == weights.size());
}

int FeedForwardNetwork::getConnectionTotal()
{
	return num_connections;
}

float FeedForwardNetwork::sigmoid(float val)
{
	return (1.0 / (1.0 + exp(-val)));
}

float FeedForwardNetwork::sigmoidDerivative(float val)
{
	return val * (1.0 - val);
}

void FeedForwardNetwork::calculateTotalConnections()
{
	int totalConnections = 0;
	for (size_t i = 0; i < wih.size(); i++)
	{
		totalConnections += wih[i].size();
	}

	for (size_t i = 0; i < who.size(); i++)
	{
		totalConnections += who[i].size();
	}
	
	assert(num_input * num_hidden + num_hidden * num_output + 2);

	num_connections = totalConnections;
}
