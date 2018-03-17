{

	FILE* img = fopen(filename, "rb");   //read the file


	unsigned char header[54];
	fread(header, sizeof(unsigned char), 54, img); // read the 54-byte header


	// extract image height and width from header 
	int width = *(int*)&header[18];     
	int height = *(int*)&header[22];     
	int padding=0; while (width*3+padding) % 4!=0 padding++;

	int widthnew=width*3+padding;

	unsigned char* data = new unsigned char[widthnew]; 

	for (int i=0; i<height; i++ ) {                                     

		fread( data, sizeof(unsigned char), widhthnew, img);

		for (int j=0; j<width*3; j+=3)                                 

		{ //Convert BGR to RGB and store                       

			array[i*width+j+0] = data[j+2];                               

			array[i*width+j+1] = data[j+1];                               

			array[i* width+j+2] = data[j+0]; }
	}                         

	fclose(img); //close the file 
}
