#include "object.h"
#include "../util.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "vertex.h"



static int
count(char* s, char c) {
	int i = 0;
	for (i = 0; s[i]; s[i] == c ? i++ : *s++);
	return i;
}

obj3d_t
obj3dopen(char* filename) {
	obj3d_t object = {};
	object.filename = filename;

	FILE* file;
	file = fopen(filename, "r");
	if (file == NULL) {
		fprintf(stderr, "[object.c] Error loading object file '%s'\n", filename);
	}

	int vcount = 0;
	int fcount = 0;
	int vncount = 0;
	int vtcount = 0;
	int lines = 0;
	// We need to figure out everything we need, so we
	// read over the file, line by line frist.
	char line[256];
	while (fgets(line, sizeof(line), file)) {
		lines++; // The number of lines increases every line, of course

		// Now we determine what the line is
		if (strncmp("v ", line, 2) == 0) vcount++;
		if (strncmp("vn ", line, 2) == 0) vncount++;
		if (strncmp("vt ", line, 2) == 0) vtcount++;
		if (strncmp("f ", line, 2) == 0) fcount++;
	}

	vector3_t* vertices = xmalloc(sizeof(vertex_t) * vcount);
	vector3_t* normals = xmalloc(sizeof(vertex_t) * vncount);
	vector3_t* textures = xmalloc(sizeof(vector3_t) * vtcount);
	face_t* faces = xmalloc(sizeof(face_t) * fcount);

	// Current indexes in the different arrays of data.
	int vi = 0;
	int vni = 0;
	int vti = 0;
	int fi = 0;
	// Now that we have the data allocated, we read the file again.
	rewind(file);
	while (fgets(line, sizeof(line), file)) {
		if (strncmp("v ", line, 2) == 0) {
			float x, y, z;
			// printf("%s", line);
			sscanf(line, "v%f%f%f", &x, &y, &z);
			vertices[vi].x = x;
			vertices[vi].y = y;
			vertices[vi].z = z;
			vi++;
			continue;
		}

		if (strncmp("vn ", line, 2) == 0) {
			float x, y, z;
			// printf("%s", line);
			sscanf(line, "vn%f%f%f", &x, &y, &z);
			normals[vni].x = x;
			normals[vni].y = y;
			normals[vni].z = z;
			vni++;
			continue;
		}

		if (strncmp("vt ", line, 2) == 0) {
			float x, y, z;
			// printf("%s", line);
			sscanf(line, "vn%f%f%f", &x, &y, &z);
			textures[vti].x = x;
			textures[vti].y = y;
			textures[vti].z = z;
			vti++;
			continue;
		}


		if (strncmp("f ", line, 2) == 0) {
			face_t face;
			if (count(line, ' ') == 4) {
				// Quad
				face.count = 4;
				face.vertices = xmalloc(sizeof(vertex_t) * face.count);
				int v0, n0;
				int v1, n1;
				int v2, n2;
				int v3, n3;
				// Scan into the data, with this ugly, terrible, disgraceful sscanf...
				sscanf(line, "f %d//%d %d//%d %d//%d %d//%d",
					&v0, &n0,
					&v1, &n1,
					&v2, &n2,
					&v3, &n3
				);
				face.vertices[0].position = vertices[v0 - 1];
				face.vertices[0].normal = normals[n0 - 1];

				face.vertices[1].position = vertices[v1 - 1];
				face.vertices[1].normal = normals[n1 - 1];

				face.vertices[2].position = vertices[v2 - 1];
				face.vertices[2].normal = normals[n2 - 1];

				face.vertices[3].position = vertices[v3 - 1];
				face.vertices[3].normal = normals[n3 - 1];
			} else {
				// Triangle
				face.count = 3;
				face.vertices = xmalloc(sizeof(vertex_t) * face.count);
				int v0, n0;
				int v1, n1;
				int v2, n2;
				// Scan into the data, with this ugly, terrible, disgraceful sscanf...
				sscanf(line, "f %d//%d %d//%d %d//%d",
					&v0, &n0,
					&v1, &n1,
					&v2, &n2
				);
				face.vertices[0].position = vertices[v0 - 1];
				face.vertices[0].normal = normals[n0 - 1];

				face.vertices[1].position = vertices[v1 - 1];
				face.vertices[1].normal = normals[n1 - 1];

				face.vertices[2].position = vertices[v2 - 1];
				face.vertices[2].normal = normals[n2 - 1];
			}
			faces[fi++] = face;
			continue;
		}

		// printf("UNHANDLED: %s", line);
	}

	object.facec = fcount;
	object.facev = faces;
	
	free(vertices);
	free(normals);
	free(textures);
	fclose(file);

	return object;
}