/*
Copyright (c) 2014 Joshua Bowren

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not
   claim that you wrote the original software. If you use this software
   in a product, an acknowledgment in the product documentation would be
   appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be
   misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/

#ifndef NEURALNETWORK_HPP_INCLUDED
#define NEURALNETWORK_HPP_INCLUDED

#include <random>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

const static double E = 2.718281828;
const static double LEARNING_RATE = 1.0;
const static double ACCEPTED_ERROR = 0.006;

struct NeuronLayer
{
    int size;
    double* biasWeights;
    double* deltas;
    double* values;
    double* dotProducts;
    double** weights;
};

class NeuralNetwork
{
    public:
        NeuralNetwork(int inputCount, int hiddenLayerCount, int hiddenNeuronCounts[], int outputCount);
        NeuralNetwork(const char* fileName);
        ~NeuralNetwork();
        void CalculateOutputs();
        void Train(double** trainingInputs, double** labels, int size);
        void Save(const char* fileName);
        double GetGuess(double* inputs, int index);
    private:
        void Construct(int inputCount, int hiddenLayerCount, int hiddenNeuronCounts[], int outputCount);
        inline double Sigmoid(double x)
        {
            return 1.0 / (1.0 + std::pow(E, -x));
        }
        inline bool AboveAcceptedError(double** trainingInputs, double** labels, int size)
        {
            for (int x = 0; x < size; x++)
            {
                for (int y = 0; y < neuronLayers[0].size; y++)
                    neuronLayers[0].values[y] = trainingInputs[x][y];
                CalculateOutputs();

                for (int n = 0; n < neuronLayers[lastIndex].size; n++)
                {
                    double error = labels[x][n] - neuronLayers[lastIndex].values[n];
                    if (std::abs(error) > ACCEPTED_ERROR)
                        return true;
                }
            }
            return false;
        }
        int layerCount;
        int lastIndex;
        NeuronLayer* neuronLayers;
};

#endif // NEURALNETWORK_HPP_INCLUDED
