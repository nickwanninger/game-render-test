// #include "shader.h"
// #include "gl/gllib.h"
// #include <stdio.h>
// #include <stdlib.h>

// unsigned int
// compileShader(unsigned int type, const char* source) {
// 	unsigned int id = glCreateShader(type);
// 	glShaderSource(id, 1, &source, NULL);
// 	glCompileShader(id);
// 	// TODO error handle
// 	int result;
// 	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
// 	if (result == GL_FALSE) {
// 		int length;
// 		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
// 		char* message = malloc(length * sizeof(char));
// 		glGetShaderInfoLog(id, length, &length, message);
// 		char* stype = type == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader";
// 		printf("[GL] %s Failed to compile %s\n", stype, message);
// 		free(message);
// 	}

// 	return id;
// }

// unsigned int
// createshader(char* vertexshader, char* fragmentshader) {
// 	unsigned int program = glCreateProgram();
// 	unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexshader);
// 	unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentshader);
// 	glAttachShader(program, vs);
// 	glAttachShader(program, fs);
// 	glLinkProgram(program);
// 	glValidateProgram(program);
// 	glDeleteShader(vs);
// 	glDeleteShader(fs);
// 	return program;
// }


// unsigned int
// openshader(char* vertexshaderfile, char* fragmentshaderfile) {
// 	FILE* vsf = fopen(vertexshaderfile, "r");
// 	fseek(vsf, 0, SEEK_END);
// 	long vsfsize = ftell(vsf);
// 	fseek(vsf, 0, SEEK_SET); // rewind the file.

// 	char* vertexshadersource = malloc(vsfsize + 1);
// 	fread(vertexshadersource, vsfsize, 1, vsf);
// 	fclose(vsf);

// 	FILE* fsf = fopen(fragmentshaderfile, "r");
// 	fseek(fsf, 0, SEEK_END);
// 	long fsfsize = ftell(fsf);
// 	fseek(fsf, 0, SEEK_SET); // rewind the file.

// 	char* fragmentshadersource = malloc(fsfsize + 1);
// 	fread(fragmentshadersource, fsfsize, 1, fsf);
// 	fclose(fsf);
// 	unsigned int program = createshader(vertexshadersource, fragmentshadersource);
// 	free(vertexshadersource);
// 	free(fragmentshadersource);
// 	return program;
// }
