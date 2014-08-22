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

#ifndef APPLICATION_HPP_INCLUDED
#define APPLICATION_HPP_INCLUDED

#include "NeuralNetwork.hpp"
#include "Utils.hpp"

class Application
{
    public:
        Application();
        int Execute();
    private:
        bool LoadData(Utils::Image* imgSet[], int imageCount, bool isTraining);
        void InitData(double*** inputs, double*** labels, int count, Utils::Image* imgSet[]);
        void Clean();
        double** trainingInputs;
        double** trainingLabels;
        double** validationInputs;
        double** validationLabels;
        Utils::Image* trainingImgs[Utils::TRAINING_TOTAL_INPUT];
        Utils::Image* validationImgs[Utils::VALIDATION_TOTAL_INPUT];
};

#endif // APPLICATION_HPP_INCLUDED
