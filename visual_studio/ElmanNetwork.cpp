//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#include "ElmanNetwork.h"
#include <iostream>
#include <iomanip>
#include <cmath>
#include <string>
#include <ctime>
#include <cstdlib>
#include <assert.h>

using namespace std;

typedef std::vector<float> fVector;

ElmanNetwork::ElmanNetwork(unsigned int nInput, unsigned int nHidden, unsigned int nOutput, bool randomizeWeights) 
	: num_input(nInput), num_hidden(nHidden), num_output(nOutput), num_context(nHidden), wih (nInput+1, fVector(nHidden)), 
	wch (nHidden+1, fVector(nHidden)), who(nHidden+ 1, fVector(nOutput)),
	inputs_val(nInput),hidden_val(nHidden), output_val(nOutput), context_val(nHidden)
{
	if(randomizeWeights)
		assignRandomWeights();
	calculateTotalConnections();
}


ElmanNetwork::~ElmanNetwork()
{

}

fVector& ElmanNetwork::propogate(fVector& input)
{
	float sum;

	for (size_t i = 0; i < input.size(); i++) //TODO unnessecary loop
	{
		inputs_val[i] = input[i];
	}

	//Calculate input and context connections to hidden layer.
	for (int hid = 0; hid <= (num_hidden - 1); hid++) {
		sum = 0.0;
		//from input to hidden...
		for (int inp = 0; inp <= (num_input - 1); inp++) {
			sum += inputs_val[inp] * wih[inp][hid];
		} // inp
		  //from context to hidden...
		for (int con = 0; con <= (num_context - 1); con++) {
			sum += context_val[con] * wch[con][hid];
		} // con
		  //Add in bias.
		sum += bias * wih[num_input][hid];
		sum += bias * wch[num_context][hid];
		hidden_val[hid] = sigmoid(sum);
	} // hid

	  //Calculate the hidden to output layer.
	for (int out = 0; out <= (num_output - 1); out++) {
		sum = 0.0;
		for (int hid = 0; hid <= (num_hidden - 1); hid++) {
			sum += hidden_val[hid] * who[hid][out];
		} // hid

		  //Add in bias.
		sum += bias * who[num_hidden][out];
		output_val[out] = sigmoid(sum);
	} // out

	  //Copy outputs of the hidden to context layer.
	for (int con = 0; con <= (num_context - 1); con++) {
		context_val[con] = hidden_val[con];
	} // con

	return output_val;
}

void ElmanNetwork::assignRandomWeights()  
{
	for (int inp = 0; inp <= num_input; inp++) {
		for (int hid = 0; hid <= (num_hidden - 1); hid++) {
			//Assign a random weight value between --5 and 5
			wih[inp][hid] = rand() % 11 - 5;
		} // hid
	} // inp

	for (int con = 0; con <= num_context; con++) {
		for (int hid = 0; hid <= (num_hidden - 1); hid++) {
			//Assign a random weight value between -10 and 10
			wch[con][hid] = rand() % 11 - 5;
		} // hid
	} // con

	for (int hid = 0; hid <= num_hidden; hid++) {
		for (int out = 0; out <= (num_output - 1); out++) {
			//Assign a random weight value between -10 and 10
			who[hid][out] = rand() % 11 - 5;
		} // out
	} // hid
	
}

void ElmanNetwork::assignweights(const std::vector<float>& weights)
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

	for (size_t i = 0; i < wch.size(); i++)
	{
		for (size_t j = 0; j < wch[i].size(); j++)
		{
			wch[i][j] = weights[globalIndex];
			globalIndex++;
		}
	}
	assert(globalIndex == weights.size());
}

int ElmanNetwork::getConnectionTotal()
{
	return num_connections;
}

void ElmanNetwork::clearContext()
{
	for (size_t i = 0; i < context_val.size(); i++)
	{
		context_val[i] = 0;
	}
}

float ElmanNetwork::sigmoid(float val)
{
	return (1.0 / (1.0 + exp(-val)));
}

void ElmanNetwork::calculateTotalConnections()
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

	for (size_t i = 0; i < wch.size(); i++)
	{
		totalConnections += wch[i].size();
	}

	assert(num_input * num_hidden + num_hidden * num_output + num_context * num_hidden + 3);

	num_connections = totalConnections;
}
