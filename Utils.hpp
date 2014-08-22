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

#ifndef UTILS_HPP_INCLUDED
#define UTILS_HPP_INCLUDED

#include <cstdlib>
#include <png.h>

namespace Utils
{
    struct Image
    {
        int colorFormat;
        float width;
        float height;
        png_byte* pixels;
    };

    Image* LoadImage(const char* fileName);
    void DeleteImage(Image* image);

    const int EXIT_S = EXIT_SUCCESS;
    const int EXIT_F = EXIT_FAILURE;
    const int SUBJECT_COUNT = 2;
    const int TRAINING_IMAGES_PER_SUBJECT = 4;
    const int IMAGE_WIDTH = 20;
    const int IMAGE_HEIGHT = 20;
    const int PIXEL_COUNT = IMAGE_WIDTH * IMAGE_HEIGHT;
    const int HIDDEN_LAYER_COUNT = 1;
    const int VALIDATION_IMAGES_PER_SUBJECT = 2;
    const int CHAR_OFFSET = 97;
    const int BLACK_CUTOFF = 128;
    const int TRAINING_TOTAL_INPUT = SUBJECT_COUNT * TRAINING_IMAGES_PER_SUBJECT;
    const int VALIDATION_TOTAL_INPUT = SUBJECT_COUNT * VALIDATION_IMAGES_PER_SUBJECT;
    const char RESOURCE_STRING[] = "Resources/%c%d.png";
    const char BEGIN_TRAINING[] = "Begining training, this may take a while.\n";
    const char END_TRAINING[] = "Training complete!\n";
    const char ERROR_BEGIN[] = "ERROR: ";
    const char ERROR_PROBLEM_LOADING[] = "Failed to load image properly.";
    const char ERROR_WRONG_FORMAT[] = "Image is not a gray scale image.";
    const char ERROR_WRONG_DIMENSIONS[] = "Image is not a 160 x 400 image.";
}

#endif // UTILS_HPP_INCLUDED
