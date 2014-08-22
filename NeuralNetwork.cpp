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

#include "NeuralNetwork.hpp"

NeuralNetwork::NeuralNetwork(int inputCount, int hiddenLayerCount, int hiddenNeuronCounts[], int outputCount)
{
    /*Although C++11 introduces delgating constructors, they cannot be
    placed within the body of the constuctor so Construct is used instead*/
    Construct(inputCount, hiddenLayerCount, hiddenNeuronCounts, outputCount);
}

NeuralNetwork::NeuralNetwork(const char* fileName)
{
    std::ifstream weightsInStream;
    weightsInStream.open(fileName);

    std::string tmp;

    if (weightsInStream.good())
        getline(weightsInStream, tmp);
    int layersInFile = std::stoi(tmp);
    int neuronsPerLayer[layersInFile];

    for (int i = 0; i < layersInFile; i++)
    {
        if (weightsInStream.good())
            getline(weightsInStream, tmp);
        neuronsPerLayer[i] = std::stoi(tmp);
    }

    Construct(neuronsPerLayer[0], layersInFile - 2, &neuronsPerLayer[1], neuronsPerLayer[layersInFile - 1]);
    //Now that we used the normal constructor we can use the regular variables like layerCount
    char weightString[16];
    int weightLength = 0;
    int index = 0;
    for (int n = 0; n < layerCount - 1; n++)
    {
        std::getline(weightsInStream, tmp);
        index = 0;
        for (int x = 0; x < neuronLayers[n].size; x++)
        {
            for (int y = 0; y < neuronLayers[n + 1].size; y++)
            {
                for (int m = 0; tmp[index + m] != ' '; m++)
                {
                    weightString[m] = tmp[index + m];
                    if (tmp[index + m + 1] == ' ')
                    {
                        weightString[m + 1] = '\0';
                        weightLength = m + 1;
                    }
                }
                double weightValue = atof(weightString);
                neuronLayers[n].weights[x][y] = weightValue;
                index += weightLength + 1;

                if (x == neuronLayers[n].size - 1 && y == neuronLayers[n + 1].size - 1)
                {
                    for (int i = 0; i < neuronLayers[n + 1].size; i++)
                    {
                        for (int m = 0; tmp[index + m] != ' '; m++)
                        {
                            weightString[m] = tmp[index + m];
                            if (tmp[index + m + 1] == ' ')
                            {
                                weightString[m + 1] = '\0';
                                weightLength = m + 1;
                            }
                        }
                        weightValue = atof(weightString);
                        neuronLayers[n].biasWeights[i] = weightValue;
                        index += weightLength + 1;
                    }
                }
            }
        }
    }
    weightsInStream.close();
}

NeuralNetwork::~NeuralNetwork()
{
    for (int i = 0; i < layerCount; i++)
    {
        if (neuronLayers[i].weights)
        {
            for (int j = 0; j < neuronLayers[i].size; j++)
                delete[] neuronLayers[i].weights[j];
            delete[] neuronLayers[i].weights;
        }

        if (neuronLayers[i].biasWeights)
            delete[] neuronLayers[i].biasWeights;
        if (neuronLayers[i].deltas)
            delete[] neuronLayers[i].deltas;
        if (neuronLayers[i].dotProducts)
            delete[] neuronLayers[i].dotProducts;
        //Every layer has this node, so no need to check if its a nullptr
        delete[] neuronLayers[i].values;
    }
    delete[] neuronLayers;
}

void NeuralNetwork::Construct(int inputCount, int hiddenLayerCount, int hiddenNeuronCounts[], int outputCount)
{
    double low = 0.0;
    double high = 1.0;
    std::uniform_real_distribution<double> RandUnif(low, high);
    std::default_random_engine randEngine(time(0));

    //Input and ouput layers plus hidden layers
    layerCount = 2 + hiddenLayerCount;

    neuronLayers = new NeuronLayer[layerCount];

    lastIndex = layerCount - 1;

    neuronLayers[0].size = inputCount;
    neuronLayers[lastIndex].size = outputCount;

    for (int i = 0; i < hiddenLayerCount; i++)
        neuronLayers[i + 1].size = hiddenNeuronCounts[i];//i + 1 to skip input layer

    neuronLayers[0].deltas = nullptr;
    neuronLayers[0].values = new double[neuronLayers[0].size];
    neuronLayers[0].biasWeights = new double[neuronLayers[1].size];
    neuronLayers[0].weights = new double*[neuronLayers[0].size];
    neuronLayers[0].dotProducts = new double[neuronLayers[1].size];

    for (int x = 0; x < neuronLayers[0].size; x++)
    {
        neuronLayers[0].weights[x] = new double[neuronLayers[1].size];
        for (int y = 0; y < neuronLayers[1].size; y++)
            neuronLayers[0].weights[x][y] = RandUnif(randEngine);
    }
    for (int n = 0; n < neuronLayers[1].size; n++)
    {
        neuronLayers[0].biasWeights[n] = RandUnif(randEngine);
        neuronLayers[0].dotProducts[n] = 0;
    }

    neuronLayers[lastIndex].deltas = new double[neuronLayers[lastIndex].size];
    neuronLayers[lastIndex].values = new double[neuronLayers[lastIndex].size];
    //Output layer does not need these values
    neuronLayers[lastIndex].biasWeights = nullptr;
    neuronLayers[lastIndex].weights = nullptr;
    neuronLayers[lastIndex].dotProducts = nullptr;

    for (int i = 0; i < hiddenLayerCount; i++)
    {
        //i + 1 to skip input layer and currentIndex + 1 for the size of the following layer
        int currentIndex = i + 1;
        int nextIndex = currentIndex + 1;

        neuronLayers[currentIndex].deltas = new double[neuronLayers[currentIndex].size + 1];
        neuronLayers[currentIndex].values = new double[neuronLayers[currentIndex].size];
        neuronLayers[currentIndex].biasWeights = new double[neuronLayers[nextIndex].size];
        neuronLayers[currentIndex].weights = new double*[neuronLayers[currentIndex].size];
        neuronLayers[currentIndex].dotProducts = new double[neuronLayers[nextIndex].size];

        for (int x = 0; x < neuronLayers[currentIndex].size; x++)
        {
            neuronLayers[currentIndex].weights[x] = new double[neuronLayers[nextIndex].size];
            for (int y = 0; y < neuronLayers[nextIndex].size; y++)
                neuronLayers[currentIndex].weights[x][y] = RandUnif(randEngine);
        }
        for (int n = 0; n < neuronLayers[nextIndex].size; n++)
        {
            neuronLayers[currentIndex].biasWeights[n] = RandUnif(randEngine);
            neuronLayers[currentIndex].dotProducts[n] = 0;
        }
    }
}

void NeuralNetwork::CalculateOutputs()
{
    //Exclude output layer
    for (int i = 0; i < layerCount - 1; i++)
    {
        for (int x = 0; x < neuronLayers[i + 1].size; x++)
        {
            for (int y = 0; y < neuronLayers[i].size; y++)
                neuronLayers[i].dotProducts[x] += neuronLayers[i].values[y] * neuronLayers[i].weights[y][x];
            neuronLayers[i].dotProducts[x] += neuronLayers[i].biasWeights[x];
            neuronLayers[i + 1].values[x] = Sigmoid(neuronLayers[i].dotProducts[x]);
            neuronLayers[i].dotProducts[x] = 0.0f;
        }
    }
}

void NeuralNetwork::Train(double** trainingInputs, double** labels, int size)
{
    while (AboveAcceptedError(trainingInputs, labels, size))
    {
        for (int x = 0; x < size; x++)
        {
            for (int y = 0; y < neuronLayers[0].size; y++)
                neuronLayers[0].values[y] = trainingInputs[x][y];
            CalculateOutputs();

            for (int n = 0; n < neuronLayers[lastIndex].size; n++)
            {
                double error = labels[x][n] - neuronLayers[lastIndex].values[n];
                double sigmoidDerivative = neuronLayers[lastIndex].values[n] * (1.0f - neuronLayers[lastIndex].values[n]);
                neuronLayers[lastIndex].deltas[n] = sigmoidDerivative * error;
            }

            /*Exclude the input layer and skip the output
            layer because it was already calculated*/
            for (int n = layerCount - 2; n > 0; n--)
            {
                for (int m = 0; m < neuronLayers[n].size + 1; m++)
                {
                    double error = 0.0;
                    for (int j = 0; j < neuronLayers[n + 1].size; j++)
                    {
                        if (m != neuronLayers[n].size)
                            error += neuronLayers[n].weights[m][j] * neuronLayers[n + 1].deltas[j];
                        else
                            error += neuronLayers[n].biasWeights[j] * neuronLayers[n + 1].deltas[j];
                    }
                    neuronLayers[n].deltas[m] = neuronLayers[n].values[m] * (1.0 - neuronLayers[n].values[m]) * error;
                }
            }

            for (int n = layerCount - 1; n > 0; n--)
            {
                for (int m = 0; m < neuronLayers[n].size; m++)
                {
                    for (int j = 0; j < neuronLayers[n - 1].size + 1; j++)
                    {
                        //use momentum (delta values from last pass),
                        //to ensure moved in correct direction
                        if (j < neuronLayers[n - 1].size)//The size is last index becuase of the bias neuron
                        {
                            neuronLayers[n - 1].weights[j][m] +=
                            LEARNING_RATE * neuronLayers[n].deltas[m] * neuronLayers[n - 1].values[j];
                        }
                        else
                        {
                            neuronLayers[n - 1].biasWeights[m] +=
                            LEARNING_RATE * neuronLayers[n].deltas[m];// * neuronLayers[n - 1].values[j];
                        }
                    }
                }
            }
        }
    }
}

void NeuralNetwork::Save(const char* fileName)
{
    std::ofstream weightsOutStream;
    weightsOutStream.open(fileName);

    weightsOutStream << layerCount;
    weightsOutStream << "\n";

    for (int n = 0; n < layerCount; n++)
    {
        weightsOutStream << neuronLayers[n].size;
        weightsOutStream << "\n";
    }

    for (int n = 0; n < layerCount - 1; n++)
    {
        for (int x = 0; x < neuronLayers[n].size; x++)
        {
            for (int y = 0; y < neuronLayers[n + 1].size; y++)
            {
                weightsOutStream << neuronLayers[n].weights[x][y];
                weightsOutStream << " ";
                if (x == neuronLayers[n].size - 1 && y == neuronLayers[n + 1].size - 1)
                {
                    for (int i = 0; i < neuronLayers[n + 1].size; i++)
                    {
                        weightsOutStream << neuronLayers[n].biasWeights[i];
                        weightsOutStream << " ";
                        if (i == neuronLayers[n + 1].size - 1)
                            weightsOutStream << "\n";
                    }
                }
            }
        }
    }
    weightsOutStream.close();
}

double NeuralNetwork::GetGuess(double* inputs, int index)
{
    for (int i = 0; i < neuronLayers[0].size; i++)
        neuronLayers[0].values[i] = inputs[i];

    CalculateOutputs();

    return neuronLayers[lastIndex].values[index];
}
