#include "Helpers.h"
#include <cassert>
#include "stdio.h"
#include "stdlib.h"
#include <vector>

GLuint AllocateBuffer(GLenum bufferType, const void* pData, size_t size)
{
    GLuint bufferObject = 0;
    
    //Generate a handle
    glGenBuffers(1, &bufferObject); 

    //Bind
    glBindBuffer(bufferType, bufferObject); 

    //Store data
    if(pData != nullptr)
        glBufferData(bufferType, size, pData, GL_STATIC_DRAW); 

    //Unbind it
    glBindBuffer(bufferType, 0);
    return bufferObject;
}

GLuint AllocateTexture(const void* pData, int width, int height)
{
    GLuint textureID = 0;

    //Generate a handle
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0);

    //Bind
    glBindTexture(GL_TEXTURE_2D, textureID);

    //Init
    //glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);

    //Store data
    if(pData != nullptr)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pData);//glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_BYTE, pData);
    
    glGenerateMipmap(GL_TEXTURE_2D);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 4.0f

    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    //Unbind it.
    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}


bool LoadTGAFile(const char *filename, TGAFILE *tgaFile)
{
    FILE *filePtr;
    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;

    // Open the TGA file.
    filePtr = fopen(filename, "rb");
    if (filePtr == NULL)
    {
        return false;
    }

    // Read the two first bytes we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Which type of image gets stored in imageTypeCode.
    fread(&tgaFile->imageTypeCode, sizeof(unsigned char), 1, filePtr);

    // For our purposes, the type code should be 2 (uncompressed RGB image)
    // or 3 (uncompressed black-and-white images).
    if (tgaFile->imageTypeCode != 2 && tgaFile->imageTypeCode != 3)
    {
        fclose(filePtr);
        return false;
    }

    // Read 13 bytes of data we don't need.
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);

    // Read the image's width and height.
    fread(&tgaFile->imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile->imageHeight, sizeof(short int), 1, filePtr);

    // Read the bit depth.
    fread(&tgaFile->bitCount, sizeof(unsigned char), 1, filePtr);

    // Read one byte of data we don't need.
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);

    // Color mode -> 3 = BGR, 4 = BGRA.
    colorMode = tgaFile->bitCount / 8;
    imageSize = tgaFile->imageWidth * tgaFile->imageHeight * colorMode;

    // Allocate memory for the image data.
    tgaFile->imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);

    // Read the image data.
    fread(tgaFile->imageData, sizeof(unsigned char), imageSize, filePtr);

    // Change from BGR to RGB so OpenGL can read the image data.
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile->imageData[imageIdx];
        tgaFile->imageData[imageIdx] = tgaFile->imageData[imageIdx + 2];
        tgaFile->imageData[imageIdx + 2] = colorSwap;
    }

    fclose(filePtr);
    return true;
}