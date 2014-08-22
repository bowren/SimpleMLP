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

#include "Application.hpp"

Application::Application()
{
    trainingInputs = nullptr;
    trainingLabels = nullptr;
    validationInputs = nullptr;
    validationLabels = nullptr;
    for (int i = 0; i < Utils::TRAINING_TOTAL_INPUT; i++)
        trainingImgs[i] = nullptr;
    for (int i = 0; i < Utils::VALIDATION_TOTAL_INPUT; i++)
        validationImgs[i] = nullptr;
}

int Application::Execute()
{
    if (!LoadData(trainingImgs, Utils::TRAINING_IMAGES_PER_SUBJECT, true))
        return Utils::EXIT_F;
    if (!LoadData(validationImgs, Utils::VALIDATION_IMAGES_PER_SUBJECT, false))
        return Utils::EXIT_F;

    InitData(&trainingInputs, &trainingLabels, Utils::TRAINING_TOTAL_INPUT, trainingImgs);
    InitData(&validationInputs, &validationLabels, Utils::VALIDATION_TOTAL_INPUT, validationImgs);

    int hiddenLayerNeurons[] = { 5 };
    NeuralNetwork ann(Utils::PIXEL_COUNT, Utils::HIDDEN_LAYER_COUNT, hiddenLayerNeurons, Utils::SUBJECT_COUNT);

    printf(Utils::BEGIN_TRAINING);
    ann.Train(trainingInputs, trainingLabels, Utils::TRAINING_TOTAL_INPUT);
    printf(Utils::END_TRAINING);

    ann.Save("ocr.ann");

    for (int x = 0; x < Utils::VALIDATION_TOTAL_INPUT; x++)
    {
        for (int y = 0; y < Utils::SUBJECT_COUNT; y++)
            validationLabels[x][y] = ann.GetGuess(validationInputs[x], y);
    }

    for (int x = 0; x < Utils::VALIDATION_TOTAL_INPUT; x++)
    {
        for (int y = 0; y < Utils::SUBJECT_COUNT; y++)
            std::printf("%5.5f\n", validationLabels[x][y]);
        std::printf("\n");
    }

    Clean();
    return Utils::EXIT_S;
}

bool Application::LoadData(Utils::Image* imgSet[], int imageCount, bool isTraining)
{
    char resource[33];
    int currentIndex = 0;
    for (int x = 0; x < Utils::SUBJECT_COUNT; x++)
    {
        for (int y = 0; y < imageCount; y++)
        {
            char letter = x + Utils::CHAR_OFFSET;
            if (isTraining)
                std::sprintf(&resource[0], Utils::RESOURCE_STRING, letter, y);
            else
                std::sprintf(&resource[0], Utils::RESOURCE_STRING, letter, y + Utils::TRAINING_IMAGES_PER_SUBJECT);
            currentIndex = (x * imageCount) + y;
            imgSet[currentIndex] = Utils::LoadImage(resource);

            if (!imgSet[currentIndex])
            {
                printf(Utils::ERROR_BEGIN);
                printf(Utils::ERROR_PROBLEM_LOADING);
                Clean();
                return false;
            }
            if (imgSet[currentIndex]->height != Utils::IMAGE_HEIGHT && imgSet[currentIndex]->width != Utils::IMAGE_WIDTH)
            {
                printf(Utils::ERROR_BEGIN);
                printf(Utils::ERROR_WRONG_DIMENSIONS);
                Clean();
                return false;
            }
            if (imgSet[currentIndex]->colorFormat != PNG_COLOR_TYPE_GRAY)
            {
                printf(Utils::ERROR_BEGIN);
                printf(Utils::ERROR_WRONG_FORMAT);
                Clean();
                return false;
            }
        }
    }
    return true;
}

void Application::InitData(double*** inputs, double*** labels, int count, Utils::Image* imgSet[])
{
    *inputs = new double*[count];
    *labels = new double*[count];
    for (int i = 0; i < count; i++)
    {
        (*inputs)[i] = new double[Utils::PIXEL_COUNT];
        (*labels)[i] = new double[Utils::SUBJECT_COUNT];
    }

    for (int x = 0; x < count; x++)
    {
        for (int y = 0; y < Utils::PIXEL_COUNT; y++)
        {
            if (imgSet[x]->pixels[y] < Utils::BLACK_CUTOFF)
                (*inputs)[x][y] = -0.5;
            else
                (*inputs)[x][y] = 0.5;
        }

        for (int y = 0; y < Utils::SUBJECT_COUNT; y++)
        {
            if (y == x / 4)
                (*labels)[x][y] = 1.0;
            else
                (*labels)[x][y] = 0.0;
        }
    }
}

void Application::Clean()
{
    for (int i = 0; i < Utils::TRAINING_TOTAL_INPUT; i++)
    {
        if (trainingImgs[i])
            Utils::DeleteImage(trainingImgs[i]);
        if (trainingInputs)
        {
            if (trainingInputs[i])
                delete[] trainingInputs[i];
        }
        if (trainingLabels)
        {
            if (trainingLabels[i])
                delete[] trainingLabels[i];
        }
    }
    for (int i = 0; i < Utils::VALIDATION_TOTAL_INPUT; i++)
    {
        if (validationImgs[i])
            Utils::DeleteImage(validationImgs[i]);
        if (validationInputs)
        {
            if (validationInputs[i])
                delete[] validationInputs[i];
        }
        if (validationLabels)
        {
            if (validationLabels[i])
                delete[] validationLabels[i];
        }
    }
    if (trainingInputs)
        delete[] trainingInputs;
    if (trainingLabels)
        delete[] trainingLabels;
    if (validationInputs)
        delete[] validationInputs;
    if (validationLabels)
        delete[] validationLabels;
}
