#ifndef HELPER_FUNCS
#define HELPER_FUNCS

#include "gl\glew.h"
#include "glm\glm.hpp"
#include <stdio.h>

const int g_SpheresCount = 20;
struct Spheres_t
{
	glm::vec3		centers[g_SpheresCount];
	float			radii[g_SpheresCount];
};
extern Spheres_t g_Spheres;

//Allocates a GL_ARRAY_BUFFER and initialzes it with contents from pData. 
//If pData is null the initialization is skippped.
//This has to be deallocated using glDeleteBuffers()
GLuint			AllocateBuffer(GLenum bufferType, const void* pData, size_t size);

//Allocates a GL_TEXTURE_2D and initializes it with contents from pData
//If pData is null the initialization is skipped.
//This has to be deallocated using glDeleteTextures()
GLuint			AllocateTexture(const void* pData, int width, int height);

typedef struct
{
    unsigned char imageTypeCode;
    short int imageWidth;
    short int imageHeight;
    unsigned char bitCount;
    unsigned char *imageData;
} TGAFILE;


bool LoadTGAFile(const char *filename, TGAFILE *tgaFile);

#endif