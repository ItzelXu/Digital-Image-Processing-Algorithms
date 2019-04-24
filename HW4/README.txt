#EE 569 - Homework Assignment #4
#Date - 03/19/2019
#Name - Brinal Chimanlal Bheda
#USC ID - 8470655102
#Email - bbheda@usc.edu

Version of OS - macOS Mojave (version 10.14.2)
IDE - Visual Studio Code for C++
IDE - MATLAB v2016b
Compiler - g++
Image Viewing Software - ImageJ (http://rsb.info.nih.gov/ij/)

-->How to run the codes? (For C++)
1.  Open terminal.
2.  Go to the directory where the source code files are stored along with the input image files in .cpp and .raw format respectively
2a. cd "path"
3.  Compile the codes using:
3a. g++ -o program source_code.cpp
4.  Run and execute the code and the output images are generated and stored in the same folder as the source codes and input images.
4a. ./program input_image.raw output_image.raw 1 or ./program input_image.raw output_image.raw 3
4b. Last argument denotes the bytes_per_pixel

-->How to choose arguments for compilation?
input_image.raw output_image.raw bytes_per_pixel
bytes_per_pixel is 1 for grayscale image and 2 for color images

-->File name of output images
output_image.raw

-->How to run the codes? (For MATLAB)
1. Open Matlab.
2. Add the path where the source code files are stored along with the input image files in .m, .jpg, .mat and .raw format respectively
3. Open the code .m file 
4. Set the threshold or the number of ground truth file in the code
5. Press the run button

-->How to run the codes? (For Python)
1. Open Anaconda-Spyder.
2. Open the .py file.
3. Make sure all the dependencies are downloaded or provided in the same directory folder.
4. Include the input images in the same folder as .py file.
5. Run the code, providing any arguments like image width and height and BytesPerPixel.


