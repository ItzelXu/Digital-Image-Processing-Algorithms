// This sample code reads in image data from a RAW image file and
// writes it into another file

// NOTE:    The code assumes that the bird input image is of size 500 x 375 and
//            is in the RAW format. You will need to make corresponding changes to
//            accommodate images of different sizes and/or types

//EE 569: Homework 2 - By Brinal Bheda
//USC ID - 8470655102
//Email - bbheda@usc.edu
//Submission Date - 02/12/2019

/*
 MBVQ based error diffusion:-
 Algorithm implemented in C++:
 • Read the color input image “bird.raw” whose dimensions are height_size, width_size, BytesPerPixel using fread () function
 • The input image is normalized for values between 0-1
 • Get C, M, Y values using the R, G, B values in the color image
 • Calculate the error diffusion matrix using Floyd-Steinberg’s (FS) matrix with and without serpentine scanning
 • For raster scanning, assign a value to the output pixel by using thresholding.
 If the value of input image <= 0.5 or 127 depends on normalizing, assign a value of 255 or else assign a value of 0
 • For serpentine scanning, if the row is even, check by using the modular 2 operation, assign a value to the output pixel by using thresholding. If the value of input image <= 0.5 or 127 depends on normalizing, assign a value of 255 or else assign a value of 0
 • Get the R, G, B values for every pixel location
 • Using the above algorithm/code, find the tetrahedron/quadruple in which the pixel lies
 • Find the Euclidian distance of the pixel from the 4 vertices of the quadruple
 • Select the vertex which corresponds to the minimum distance compared to others using the C, M, Y values
 • Calculate the error by subtracting new value assigned (binarized value) to the pixel after thresholding from the old value (error = output – input)
 • Diffuse the error to the future neighboring pixels as shown in the matrices above
 • Repeat the procedure for scanning of odd rows when modular 2 operation fails Combine the C, M and Y channels into the output image
 • Covert this CMY image into RGB output image
 • Write the computed image data array on output.raw file using the fwrite() function
 */

// To run the code, open terminal, add path and run using following commands
// g++ -o 2c2 2c.2_mbvq_error_diffusion_color.cpp
// ./2c2 bird.raw output_2c2_mbvq_serp.raw 3


#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <math.h>


using namespace std;
// define sturcture to store the CMY information and error information
struct {
    double r;
    double g;
    double b;
}cmy[512][512];

struct {
    double r;
    double g;
    double b;
}error[512][512];

// find the closest vertex of the MBVQ
int find (double x1, double y1, double z1, double array[][3])
{
    double dis1,dis2,dis3,dis4;
    int pp=0;
    // calculate the distance
    dis1 = (x1-array[0][0])*(x1-array[0][0])+(y1-array[0][1])*(y1-array[0][1])+(z1-array[0][2])*(z1-array[0][2]);
    dis2 = (x1-array[1][0])*(x1-array[1][0])+(y1-array[1][1])*(y1-array[1][1])+(z1-array[1][2])*(z1-array[1][2]);
    dis3 = (x1-array[2][0])*(x1-array[2][0])+(y1-array[2][1])*(y1-array[2][1])+(z1-array[2][2])*(z1-array[2][2]);
    dis4 = (x1-array[3][0])*(x1-array[3][0])+(y1-array[3][1])*(y1-array[3][1])+(z1-array[3][2])*(z1-array[3][2]);
    // compare the distance and find the smallest one
    if(dis1<dis2&&dis1<dis3&&dis1<dis4){pp = 0;}
    if(dis2<dis1&&dis2<dis3&&dis2<dis4){pp = 1;}
    if(dis3<dis1&&dis3<dis2&&dis3<dis4){pp = 2;}
    if(dis4<dis1&&dis4<dis2&&dis4<dis3){pp = 3;}
    return pp;
}
int main(int argc, char *argv[])

{
    // Define file pointer and variables
    FILE *file;
    int bytesPerPixel = 3;
    int height_size = 375;
    int width_size = 500;

    // Allocate image data array
    unsigned char imagedata[height_size][width_size][bytesPerPixel];
    // Read image (filename specified by first argument) into image data matrix
    if (!(file=fopen(argv[1],"rb"))) {
        cout << "Cannot open file: " << argv[1] <<endl;
        exit(1);
    }
    fread(imagedata, sizeof(unsigned char), height_size*width_size*bytesPerPixel, file);
    fclose(file);

    unsigned char imagedata_out[height_size][width_size][bytesPerPixel];
    int tmp[height_size][width_size][bytesPerPixel];
    int i,j;
    double x,y,z;
    int id;
    // define the 6 MBVQ in the CMYK space
    double v1[4][3]={{0,0,0},{0,0,1},{1,0,0},{0,1,0}};
    double v2[4][3]={{0,0,1},{1,0,0},{1,0,1},{0,1,0}};
    double v3[4][3]={{0,0,1},{1,0,1},{0,1,1},{0,1,0}};
    double v4[4][3]={{1,1,1},{1,0,1},{0,1,1},{1,1,0}};
    double v5[4][3]={{0,1,1},{1,1,0},{1,0,1},{0,1,0}};
    double v6[4][3]={{1,0,1},{1,0,0},{0,1,0},{1,1,0}};
    // initialize the error
    for (i=0;i<height_size;i++) {
        for (j=0;j<width_size;j++){
            error[i][j].r = 0;
            error[i][j].g = 0;
            error[i][j].b = 0;
        }
    }
    // CMY space
    for (i=0;i<height_size;i++) {
        for (j=0;j<width_size;j++){
            cmy[i][j].r = 1-(double)imagedata[i][j][0]/255;
            cmy[i][j].g = 1-(double)imagedata[i][j][1]/255;
            cmy[i][j].b = 1-(double)imagedata[i][j][2]/255;
            tmp[i][j][0] = (int)imagedata[i][j][0];
            tmp[i][j][1] = (int)imagedata[i][j][1];
            tmp[i][j][2] = (int)imagedata[i][j][2];
        }
    }
    // apply vector color half-toning
    for (i=0;i<height_size;i++) {
        if (i%2==0) {       // from left to right
            for (j=0;j<width_size;j++) {
                // CMY value plus error
                x = cmy[i][j].r+error[i][j].r;
                y = cmy[i][j].g+error[i][j].g;
                z = cmy[i][j].b+error[i][j].b;
                if (x+y<=1){
                    if (y+z<=1){
                        if(x+y+z<=1){
                            // CMYW
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v1);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v1[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v1[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v1[id][2];
                            // error diffusion
                            if (j+1<width_size){
                                error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                                error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                                error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                            if (j-1>=0&&i+1<height_size){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j+1<width_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                            }
                        else{
                            // MYGC
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v2);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v2[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v2[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v2[id][2];
                            // error diffusion
                            if (j+1<width_size){
                                error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                                error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                                error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                            if (j-1>=0&&i+1<height_size){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j+1<width_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                        }
                    }
                    else{
                        // RGMY
                        if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        // find the vertex
                        id = find(x,y,z,v3);
                        // get the error
                        error[i][j].r=cmy[i][j].r+error[i][j].r-v3[id][0];
                        error[i][j].g=cmy[i][j].g+error[i][j].g-v3[id][1];
                        error[i][j].b=cmy[i][j].b+error[i][j].b-v3[id][2];
                        // error diffusion
                        if (j+1<width_size){
                            error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                            error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                            error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                        if (j-1>=0&&i+1<height_size){
                            error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                            error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                            error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                        if (i+1<height_size){
                            error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                            error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                            error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                        if (i+1<height_size&&j+1<width_size){
                            error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                            error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                            error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                    }
                }
                else{
                    if(y+z>1){
                        if(x+y+z>2){
                            // KRGB
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v4);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v4[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v4[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v4[id][2];
                            // error diffusion
                            if (j+1<width_size){
                                error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                                error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                                error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                            if (j-1>=0&&i+1<height_size){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j+1<width_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                        }
                        else{
                            // RGBM
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v5);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v5[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v5[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v5[id][2];
                            // error diffusion
                            if (j+1<width_size){
                                error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                                error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                                error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                            if (j-1>=0&&i+1<height_size){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j+1<width_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                        }
                    }
                    else{
                        // CMGB
                        if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        // find the vertex
                        id = find(x,y,z,v6);
                        // get the error
                        error[i][j].r=cmy[i][j].r+error[i][j].r-v6[id][0];
                        error[i][j].g=cmy[i][j].g+error[i][j].g-v6[id][1];
                        error[i][j].b=cmy[i][j].b+error[i][j].b-v6[id][2];
                        // error diffusion
                        if (j+1<width_size){
                            error[i][j+1].r=error[i][j+1].r+error[i][j].r*7/16;
                            error[i][j+1].g=error[i][j+1].g+error[i][j].g*7/16;
                            error[i][j+1].b=error[i][j+1].b+error[i][j].b*7/16;}
                        if (j-1>=0&&i+1<height_size){
                            error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*3/16;
                            error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*3/16;
                            error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*3/16;}
                        if (i+1<height_size){
                            error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                            error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                            error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                        if (i+1<height_size&&j+1<width_size){
                            error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*1/16;
                            error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*1/16;
                            error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*1/16;}
                    }
                }
            }
        }
        else{       // from right to left
            for (j=width_size-1;j>=0;j--) {
                x = cmy[i][j].r+error[i][j].r;
                y = cmy[i][j].g+error[i][j].g;
                z = cmy[i][j].b+error[i][j].b;
                if (x+y<=1){
                    if (y+z<=1){
                        if(x+y+z<=1){
                            // CMYW
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v1);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v1[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v1[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v1[id][2];
                            // error diffusion
                            if (j-1>=0){
                                error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                                error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                                error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                            if (j+1<width_size&&i+1<height_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j-1>=0){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                        }
                        else{
                            // MYGC
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v2);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v2[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v2[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v2[id][2];
                            // error duffusion
                            if (j-1>=0){
                                error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                                error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                                error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                            if (j+1<width_size&&i+1<height_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j-1>=0){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                        }
                    }
                    else{
                        // RGMY
                        if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        // find the vertex
                        id = find(x,y,z,v3);
                        // get the error
                        error[i][j].r=cmy[i][j].r+error[i][j].r-v3[id][0];
                        error[i][j].g=cmy[i][j].g+error[i][j].g-v3[id][1];
                        error[i][j].b=cmy[i][j].b+error[i][j].b-v3[id][2];
                        // error diffusion
                        if (j-1>=0){
                            error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                            error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                            error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                        if (j+1<width_size&&i+1<height_size){
                            error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                            error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                            error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                        if (i+1<height_size){
                            error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                            error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                            error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                        if (i+1<height_size&&j-1>=0){
                            error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                            error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                            error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                    }
                }
                else{
                    if(y+z>1){
                        if(x+y+z>2){
                            // KRGB
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v4);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v4[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v4[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v4[id][2];
                            // error diffusion
                            if (j-1>=0){
                                error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                                error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                                error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                            if (j+1<width_size&&i+1<height_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j-1>=0){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                        }
                        else{
                            // RGBM
                            if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                            if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                            if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                            // find the vertex
                            id = find(x,y,z,v5);
                            // get the error
                            error[i][j].r=cmy[i][j].r+error[i][j].r-v5[id][0];
                            error[i][j].g=cmy[i][j].g+error[i][j].g-v5[id][1];
                            error[i][j].b=cmy[i][j].b+error[i][j].b-v5[id][2];
                            // error diffusion
                            if (j-1>=0){
                                error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                                error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                                error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                            if (j+1<width_size&&i+1<height_size){
                                error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                                error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                                error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                            if (i+1<height_size){
                                error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                                error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                                error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                            if (i+1<height_size&&j-1>=0){
                                error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                                error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                                error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                        }
                    }
                    else{
                        // CMGB
                        if(x>=0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x>=0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x>=0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)0;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y>=0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y>=0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)0;imagedata_out[i][j][2]=(unsigned char)255;}
                        if(x<0.5&&y<0.5&&z>=0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)0;}
                        if(x<0.5&&y<0.5&&z<0.5){imagedata_out[i][j][0]=(unsigned char)255;imagedata_out[i][j][1]=(unsigned char)255;imagedata_out[i][j][2]=(unsigned char)255;}
                        // find the vertex
                        id = find(x,y,z,v6);
                        // get the error
                        error[i][j].r=cmy[i][j].r+error[i][j].r-v6[id][0];
                        error[i][j].g=cmy[i][j].g+error[i][j].g-v6[id][1];
                        error[i][j].b=cmy[i][j].b+error[i][j].b-v6[id][2];
                        // error diffusion
                        if (j-1>=0){
                            error[i][j-1].r=error[i][j-1].r+error[i][j].r*7/16;
                            error[i][j-1].g=error[i][j-1].g+error[i][j].g*7/16;
                            error[i][j-1].b=error[i][j-1].b+error[i][j].b*7/16;}
                        if (j+1<width_size&&i+1<height_size){
                            error[i+1][j+1].r=error[i+1][j+1].r+error[i][j].r*3/16;
                            error[i+1][j+1].g=error[i+1][j+1].g+error[i][j].g*3/16;
                            error[i+1][j+1].b=error[i+1][j+1].b+error[i][j].b*3/16;}
                        if (i+1<height_size){
                            error[i+1][j].r=error[i+1][j].r+error[i][j].r*5/16;
                            error[i+1][j].g=error[i+1][j].g+error[i][j].g*5/16;
                            error[i+1][j].b=error[i+1][j].b+error[i][j].b*5/16;}
                        if (i+1<height_size&&j-1>=0){
                            error[i+1][j-1].r=error[i+1][j-1].r+error[i][j].r*1/16;
                            error[i+1][j-1].g=error[i+1][j-1].g+error[i][j].g*1/16;
                            error[i+1][j-1].b=error[i+1][j-1].b+error[i][j].b*1/16;}
                    }
                }
            }

        }

    }
    // output the image
    if (!(file=fopen(argv[2],"wb"))) {
        cout << "Cannot open file: " << argv[2] << endl;
        exit(1);
    }
    fwrite(imagedata_out, sizeof(unsigned char), width_size*height_size*bytesPerPixel, file);
    fclose(file);

    return 0;
}


/*
 to observe output on ImageJ,
 Use: image type - 24-bit RGB
 width - 500
 height - 375
 */

