//Created by Elias Granfelt 
// 2017 - thesis project at University of Skovde

#pragma once
#include <vector>

class FeedForwardNetwork
{
public:
	FeedForwardNetwork(unsigned int nInput, unsigned int nHidden, unsigned int nOutput, bool randomizeWeights);
	~FeedForwardNetwork();

	std::vector<float>& feedForward(std::vector<float>& input);
	void assignRandomWeights();
	void assignweights(const std::vector<float>& weights);
	int getConnectionTotal();
	float sigmoid(float val);
	float sigmoidDerivative(float val);

private:

	unsigned int num_input;
	unsigned int num_hidden;
	unsigned int num_output;
	unsigned int num_connections;
	const float bias = -1.f;

	//Input to Hidden weights (with biases).
	std::vector<std::vector<float>> wih;

	//Hidden to Output weights (with biases).
	std::vector<std::vector<float>> who;

	//Activations.
	std::vector<float> inputs_val;
	std::vector<float> hidden_val;
	std::vector<float> output_val;

	void calculateTotalConnections();

};

