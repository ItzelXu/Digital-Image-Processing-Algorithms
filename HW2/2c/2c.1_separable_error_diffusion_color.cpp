// This sample code reads in image data from a RAW image file and 
// writes it into another file

// NOTE:	The code assumes that the bird input image is of size 500 x 375 and 
//			is in the RAW format. You will need to make corresponding changes to
//			accommodate images of different sizes and/or types

//EE 569: Homework 2 - By Brinal Bheda
//USC ID - 8470655102
//Email - bbheda@usc.edu
//Submission Date - 02/12/2019

/*
Separable error diffusion:-
Algorithm implemented in C++:
• Read the color input image “bird.raw” whose dimensions are height_size, width_size, BytesPerPixel using fread () function
• The input image is normalized for values between 0-1
• Get C, M, Y values using the R, G, B values in the color image
• Calculate the error diffusion matrix using Floyd-Steinberg’s (FS) matrix with and without serpentine scanning
• For raster scanning, assign a value to the output pixel by using thresholding.
  If the value of input image <= 0.5 or 127 depends on normalizing, assign a value of 255 or else assign a value of 0
• For serpentine scanning, if the row is even, check by using the modular 2 operation, assign a value to the output pixel by using thresholding. If the value of input image <= 0.5 or 127 depends on normalizing, assign a value of 255 or else assign a value of 0
• Calculate the error by subtracting new value assigned (binarized value) to the pixel after thresholding from the old value (error = output – input)
• Diffuse the error to the future neighboring pixels as shown in the matrices above
• Repeat the procedure for scanning of odd rows when modular 2 operation fails
• Combine the C, M and Y channels into the output image
• Covert this CMY image into RGB output image
• Write the computed image data array on output.raw file using the fwrite() function
*/

// To run the code, open terminal, add path and run using following commands
// g++ -o 2c1 2c.1_separable_error_diffusion_color.cpp
// ./2c1 bird.raw output_2c1_FS_serp.raw 3

#include <stdio.h>
#include <iostream>
#include <stdlib.h>

using namespace std;

int main(int argc, char *argv[])

{
	// Define file pointer and variables
	FILE *file;
	int BytesPerPixel = 3;
	int width_size = 500;
	int height_size = 375;
	
	// Check for proper syntax
	if (argc < 3){
		cout << "Syntax Error - Incorrect Parameter Usage:" << endl;
		cout << "program_name input_image.raw output_image.raw [BytesPerPixel = 1] [Size = 256]" << endl;
		return 0;
	}
	
	// Check if image is grayscale or color
	if (argc < 4){
		BytesPerPixel = 3; // default is color image
	} 
	else {
		BytesPerPixel = atoi(argv[3]);
		// Check if size is specified
		if (argc >= 5){
			//Size = atoi(argv[4]);
		}
	}
	
	//golden gate bridge.raw grayscale input image
	// Allocate image data array
	unsigned char Imagedata[height_size][width_size][BytesPerPixel];

	// Read image (filename specified by first argument) into image data matrix
	if (!(file=fopen(argv[1],"rb"))) {
		cout << "Cannot open file: " << argv[1] <<endl;
		exit(1);
	}
	fread(Imagedata, sizeof(unsigned char), height_size*width_size*BytesPerPixel, file);
	fclose(file);

	///////////////////////// INSERT YOUR PROCESSING CODE HERE /////////////////////////

	//Digital Halftoning

	//Problem 2c - Color Halftoning with Error Diffusion

	//1. Separable Error Diffusion

	double error = 0;

	//defining output 3d image arrays

	// Allocate image processing data array
	unsigned char Imagedata_output[height_size][width_size][BytesPerPixel];

	// Allocate image processing data array
	unsigned char Imagedata_output_cyan[height_size][width_size][1];

	// Allocate image processing data array
	unsigned char Imagedata_output_magenta[height_size][width_size][1];

	// Allocate image processing data array
	unsigned char Imagedata_output_yellow[height_size][width_size][1];

	//defining temporary 3d image arrays for processing of matrices

	// Allocate image processing data array
	double Imagedata_temp_cyan[height_size][width_size][1];

	// Allocate image processing data array
	double Imagedata_temp_magenta[height_size][width_size][1];

	// Allocate image processing data array
	double Imagedata_temp_yellow[height_size][width_size][1];

	

	//binarize the temp image which has values transferred from original image

	for (int i=0; i<height_size; i++)
	{
		for (int j=0; j<width_size; j++)
		{
			Imagedata_temp_cyan[i][j][0] = 1 - (double)Imagedata[i][j][0] / 255;
			Imagedata_temp_magenta[i][j][1] = 1 - (double)Imagedata[i][j][1] / 255;
			Imagedata_temp_yellow[i][j][2] = 1 - (double)Imagedata[i][j][2] / 255;
			
		}
	}

	//if do not normalize to 0~1, then declare variables long double as dealing with high values between 0~255

	


	//calculate error diffusion matrix using Floyd-Steinberg’s matrix with and without serpentine scanning

	//for 3 channels - CMY


	//for Cyan
/*
	//without serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		for (int j=0; j<width_size; j++)
		{
			for (int k=0; k<1; k++)
			{
				//normalize this temp image to 0~1
				//subtract the binarized value to estimated value to get error and output
				if (Imagedata_temp_cyan[i][j][k] <= 0.5)
				{
					error = Imagedata_temp_cyan[i][j][k] - 0;
					Imagedata_output_cyan[i][j][k] = (unsigned char)255;
				}
				else
				{
					error = Imagedata_temp_cyan[i][j][k] - 1;
					Imagedata_output_cyan[i][j][k] = (unsigned char)0;
				}

				if (j+1 < width_size)
				{
					Imagedata_temp_cyan[i][j+1][k] += error*7/16;
				}
				if (j > 0 && i+1 < height_size)
				{
					Imagedata_temp_cyan[i+1][j-1][k] += error*3/16;
				}
				if (i+1 < height_size)
				{
					Imagedata_temp_cyan[i+1][j][k] += error*5/16;
				}
				if (i+1 < height_size && j+1 < width_size)
				{
					Imagedata_temp_cyan[i+1][j+1][k] += error*1/16;
				}

			}
		}
	}
*/

	//with serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		//from left to right
		if (i%2 == 0)
		{
			for (int j=0; j<width_size; j++)
			{
				for (int k=0; k<1; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_cyan[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_cyan[i][j][k] - 0;
						Imagedata_output_cyan[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_cyan[i][j][k] - 1;
						Imagedata_output_cyan[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_cyan[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_cyan[i+1][j-1][k] += error*3/16;
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_cyan[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_cyan[i+1][j+1][k] += error*1/16;
					}
					
				}
			}
		}
		//from right to left
		if (i%2 != 0) 
		{
			for (int j=width_size - 1; j>=0; j--)
			{
				for (int k=0; k<1; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_cyan[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_cyan[i][j][k] - 0;
						Imagedata_output_cyan[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_cyan[i][j][k] - 1;
						Imagedata_output_cyan[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_cyan[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_cyan[i+1][j-1][k] += error*1/16;              
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_cyan[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_cyan[i+1][j+1][k] += error*3/16;
					}
					
				}
			}
		}
		
	}



	//for Magenta
/*
	//without serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		for (int j=0; j<width_size; j++)
		{
			for (int k=1; k<2; k++)
			{
				//normalize this temp image to 0~1
				//subtract the binarized value to estimated value to get error and output
				if (Imagedata_temp_magenta[i][j][k] <= 0.5)
				{
					error = Imagedata_temp_magenta[i][j][k] - 0;
					Imagedata_output_magenta[i][j][k] = (unsigned char)255;
				}
				else
				{
					error = Imagedata_temp_magenta[i][j][k] - 1;
					Imagedata_output_magenta[i][j][k] = (unsigned char)0;
				}

				if (j+1 < width_size)
				{
					Imagedata_temp_magenta[i][j+1][k] += error*7/16;
				}
				if (j > 0 && i+1 < height_size)
				{
					Imagedata_temp_magenta[i+1][j-1][k] += error*3/16;
				}
				if (i+1 < height_size)
				{
					Imagedata_temp_magenta[i+1][j][k] += error*5/16;
				}
				if (i+1 < height_size && j+1 < width_size)
				{
					Imagedata_temp_magenta[i+1][j+1][k] += error*1/16;
				}

			}
		}
	}
*/

	//with serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		//from left to right
		if (i%2 == 0)
		{
			for (int j=0; j<width_size; j++)
			{
				for (int k=1; k<2; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_magenta[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_magenta[i][j][k] - 0;
						Imagedata_output_magenta[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_magenta[i][j][k] - 1;
						Imagedata_output_magenta[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_magenta[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_magenta[i+1][j-1][k] += error*3/16;
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_magenta[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_magenta[i+1][j+1][k] += error*1/16;
					}
					
				}
			}
		}
		//from right to left
		if (i%2 != 0) 
		{
			for (int j=width_size - 1; j>=0; j--)
			{
				for (int k=1; k<2; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_magenta[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_magenta[i][j][k] - 0;
						Imagedata_output_magenta[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_magenta[i][j][k] - 1;
						Imagedata_output_magenta[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_magenta[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_magenta[i+1][j-1][k] += error*1/16;              
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_magenta[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_magenta[i+1][j+1][k] += error*3/16;
					}
					
				}
			}
		}
		
	}



	//for Yellow
/*
	//without serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		for (int j=0; j<width_size; j++)
		{
			for (int k=2; k<3; k++)
			{
				//normalize this temp image to 0~1
				//subtract the binarized value to estimated value to get error and output
				if (Imagedata_temp_yellow[i][j][k] <= 0.5)
				{
					error = Imagedata_temp_yellow[i][j][k] - 0;
					Imagedata_output_yellow[i][j][k] = (unsigned char)255;
				}
				else
				{
					error = Imagedata_temp_yellow[i][j][k] - 1;
					Imagedata_output_yellow[i][j][k] = (unsigned char)0;
				}

				if (j+1 < width_size)
				{
					Imagedata_temp_yellow[i][j+1][k] += error*7/16;
				}
				if (j > 0 && i+1 < height_size)
				{
					Imagedata_temp_yellow[i+1][j-1][k] += error*3/16;
				}
				if (i+1 < height_size)
				{
					Imagedata_temp_yellow[i+1][j][k] += error*5/16;
				}
				if (i+1 < height_size && j+1 < width_size)
				{
					Imagedata_temp_yellow[i+1][j+1][k] += error*1/16;
				}

			}
		}
	}
*/

	//with serpentine scanning
	for (int i=0; i<height_size; i++)
	{
		//from left to right
		if (i%2 == 0)
		{
			for (int j=0; j<width_size; j++)
			{
				for (int k=2; k<3; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_yellow[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_yellow[i][j][k] - 0;
						Imagedata_output_yellow[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_yellow[i][j][k] - 1;
						Imagedata_output_yellow[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_yellow[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_yellow[i+1][j-1][k] += error*3/16;
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_yellow[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_yellow[i+1][j+1][k] += error*1/16;
					}
					
				}
			}
		}
		//from right to left
		if (i%2 != 0) 
		{
			for (int j=width_size - 1; j>=0; j--)
			{
				for (int k=2; k<3; k++)
				{
					//normalize this temp image to 0~1
					//subtract the binarized value to estimated value to get error and output
					if (Imagedata_temp_yellow[i][j][k] <= 0.5)
					{
						error = Imagedata_temp_yellow[i][j][k] - 0;
						Imagedata_output_yellow[i][j][k] = (unsigned char)255;
					}
					else
					{
						error = Imagedata_temp_yellow[i][j][k] - 1;
						Imagedata_output_yellow[i][j][k] = (unsigned char)0;
					}

					if (j+1 < width_size)
					{
						Imagedata_temp_yellow[i][j+1][k] += error*7/16;
					}
					if (j > 0 && i+1 < height_size)
					{
						Imagedata_temp_yellow[i+1][j-1][k] += error*1/16;              
					}
					if (i+1 < height_size)
					{
						Imagedata_temp_yellow[i+1][j][k] += error*5/16;
					}
					if (i+1 < height_size && j+1 < width_size)
					{
						Imagedata_temp_yellow[i+1][j+1][k] += error*3/16;
					}
					
				}
			}
		}
		
	}



	//adding CMY 3 separate channels into one output image

	for (int i=0; i<height_size; i++)
	{
		for (int j=0; j<width_size; j++)
		{
			Imagedata_output[i][j][0] = Imagedata_output_cyan[i][j][0];
			Imagedata_output[i][j][1] = Imagedata_output_magenta[i][j][1];
			Imagedata_output[i][j][2] = Imagedata_output_yellow[i][j][2];
			
		}
	}


	// Write image data (filename specified by second argument) from image data matrix

	if (!(file=fopen(argv[2],"wb"))) {
		cout << "Cannot open file: " << argv[2] << endl;
		exit(1);
	}
	fwrite(Imagedata_output, sizeof(unsigned char), height_size*width_size*BytesPerPixel, file);
	fclose(file);

	return 0;
}


/*
to observe output on ImageJ, 
Use: image type - 24-bit RGB
width - 500
height - 375
*/

