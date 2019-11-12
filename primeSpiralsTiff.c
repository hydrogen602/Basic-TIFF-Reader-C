#include <stdlib.h>
#include <stdio.h>

int readOnce(const char* filename)
{
	// returns -1 upon failure
	// prints error

    FILE * f = fopen(filename, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
        return -1;
    }

    int ch;
    int count = 0;
    while ((ch = fgetc(f)) != EOF)
    {
        count += 1;
    }
    return count;
}

int writeToTiff(int width, int height, int dataSize, unsigned char *rgb_data)
{
    if (width * height != dataSize) {
        fprintf(stderr, "Width, height, and dataSize don't fit together\n");
        return -1;
    }

	const char* filename = "spirals/template1M.tiff";

	// Getting template
    int sizeofTemplate = readOnce(filename);
	if (sizeofTemplate < 0) {
		return -1;
	}
    //printf("sizeofTemplate = %d\n", sizeofTemplate);

	unsigned char *buffer = malloc(sizeofTemplate);
	if (buffer == NULL)
	{
		printf("Failed to allocate memory\n");
		return -1;
	}

	FILE *f = fopen(filename, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "Cannot open file %s\n", filename);
		free(buffer);
        return -1;
    }

    int ch;
    int count = 0;
    while ((ch = fgetc(f)) != EOF)
    {
		if (count >= sizeofTemplate) {
			fprintf(stderr, "File grew in size during program execution\n");
			free(buffer);
			return -1;
		}
		*(buffer + count) = (unsigned char)(ch);
        count += 1;
    }

	if (count < sizeofTemplate) {
		fprintf(stderr, "File shrunk in size during program execution\n");
		free(buffer);
		return -1;
	}

	return -1;

	// Put the primes data into the image

	int indexBegin = 22869; // these values represent the beginning and end of
	int indexEnd = 1022724; // the pixel data in the image. These values where found with a python program

	if (dataSize < indexEnd - indexBegin)
	{
		printf("Not enought data given\n");
		free(buffer);
		return -1;
	}

	// int j;
	// for (int i = indexBegin; i < indexEnd; i++)
	// {
	// 	j = i - indexBegin; // this counts from 0 to (indexEnd-indexBegin)
	// 	if (data[j] == 1)
	// 	{
	// 		buffer[i] = 0;
	// 	}
	// 	else if (data[j] == 0)
	// 	{
	// 		buffer[i] = 255;
	// 	}
	// 	else
	// 	{
	// 		printf("Value Error: expected 0 or 1\n");
	// 		free(c);
	// 		return -1;
	// 	}
	// }

	// write to tiff file

	FILE *f2;
	f2 = fopen("result.tiff","wb");


	for (int i = 0; i < sizeofTemplate; i++)
	{
		fputc(buffer[i], f2);
	}

	fclose(f2);

	free(buffer);

	return 0;
}

int main(int argc, char const *argv[])
{
    
    writeToTiff(10, 10, 100, NULL);
    return 0;
}
