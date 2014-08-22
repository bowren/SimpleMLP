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

#include "Utils.hpp"

namespace Utils
{
    Image* LoadImage(const char* fileName)
    {
        png_byte header[8];

        /*Open the image file with read premissions on the file
        as a binary because the png_init_io takes a binary file only*/
        FILE* imageFile = fopen(fileName, "rb");
        if (imageFile == 0)
        {
            printf("Error: Problem opening file\n");
            return nullptr;
        }

        fread(header, 1, 8, imageFile);
        if (png_sig_cmp(header, 0, 8))
        {
            printf("Error: File loaded is not a png\n");
            fclose(imageFile);
            return nullptr;
        }

        png_structp pngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
        if (!pngPtr)
        {
            printf("Error: Problem creating png read struct\n");
            fclose(imageFile);
            return nullptr;
        }

        png_infop infoPtr = png_create_info_struct(pngPtr);
        if (!infoPtr)
        {
            printf("Error: Problem creating png info struct\n");
            png_destroy_read_struct(&pngPtr, 0, 0);
            fclose(imageFile);
            return nullptr;
        }

        png_infop endInfo = png_create_info_struct(pngPtr);
        if (!endInfo)
        {
            printf("Error: Problem creating png info struct for end info\n");
            png_destroy_read_struct(&pngPtr, &infoPtr, 0);
            fclose(imageFile);
            return nullptr;
        }

        if (setjmp(png_jmpbuf(pngPtr)))
        {
            printf("Error: error setting jmp to pngPtr\n");
            png_destroy_read_struct(&pngPtr, &infoPtr, &endInfo);
            fclose(imageFile);
            return nullptr;
        }

        png_init_io(pngPtr, imageFile);

        png_set_sig_bytes(pngPtr, 8);

        png_read_info(pngPtr, infoPtr);

        int bitDepth;
        int colorFormat;
        png_uint_32 width;
        png_uint_32 height;

        png_get_IHDR(pngPtr, infoPtr, &width, &height, &bitDepth, &colorFormat, 0, 0, 0);

        png_read_update_info(pngPtr, infoPtr);

        int rowBytes = png_get_rowbytes(pngPtr, infoPtr);

        png_byte* imageData = new png_byte[rowBytes * height];

        png_bytep* rowPointers = new png_bytep[height];

        for (unsigned int i = 0; i < height; ++i)
            rowPointers[height - 1 - i] = imageData + i * rowBytes;

        png_read_image(pngPtr, rowPointers);

        Image* parsedImage = new Image;
        parsedImage->colorFormat = colorFormat;
        parsedImage->width = width;
        parsedImage->height = height;
        parsedImage->pixels = imageData;

        png_destroy_read_struct(&pngPtr, &infoPtr, &endInfo);
        delete[] rowPointers;
        fclose(imageFile);

        return parsedImage;
    }

    void DeleteImage(Image* image)
    {
        delete[] image->pixels;
        delete image;
    }
}
