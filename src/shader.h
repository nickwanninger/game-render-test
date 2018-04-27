#pragma once

#include "gl/gllib.h"


unsigned int compileShader(unsigned int type, const char* source);

unsigned int createshader(char* vertexshader, char* fragmentshader);
unsigned int openshader(char* vertexshaderfile, char* fragmentshaderfile);
