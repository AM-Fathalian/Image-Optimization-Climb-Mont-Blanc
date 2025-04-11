#include <math.h>
#include <string.h>
#include <stdlib.h>

#include <omp.h>

#include "ppm.h"

// Image from:
// http://7-themes.com/6971875-funny-flowers-pictures.html

typedef struct {
     
	// //double red,green,blue;
	// double red, green, blue; // [1] double TO float change
    //[5]
    float *red;   // Separate contiguous arrays
    float *green;
    float *blue;
} AccuratePixel;

typedef struct {
     int x, y;
     AccuratePixel *data;
} AccurateImage;

/////
AccurateImage *convertToAccurateImage(PPMImage *image) {
    int size = image->x * image->y;

    // Allocate memory for AccurateImage
    AccurateImage *imageAccurate = (AccurateImage *)malloc(sizeof(AccurateImage));
    imageAccurate->x = image->x;
    imageAccurate->y = image->y;

    // Allocate separate arrays for red, green, and blue
    imageAccurate->data = (AccuratePixel *)malloc(sizeof(AccuratePixel));
    imageAccurate->data->red = (float *)malloc(size * sizeof(float));
    imageAccurate->data->green = (float *)malloc(size * sizeof(float));
    imageAccurate->data->blue = (float *)malloc(size * sizeof(float));

    // Populate the arrays with data from the PPM image
    for (int i = 0; i < size; i++) {
        imageAccurate->data->red[i] = (float)image->data[i].red;
        imageAccurate->data->green[i] = (float)image->data[i].green;
        imageAccurate->data->blue[i] = (float)image->data[i].blue;
    }

    return imageAccurate;
}
/////

//[5] changed
PPMImage *convertToPPPMImage(AccurateImage *imageIn) {
    PPMImage *imageOut;
    imageOut = (PPMImage *)malloc(sizeof(PPMImage));
    imageOut->data = (PPMPixel *)malloc(imageIn->x * imageIn->y * sizeof(PPMPixel));

    imageOut->x = imageIn->x;
    imageOut->y = imageIn->y;

    // Populate the PPM image data from the SoA AccurateImage
    for (int i = 0; i < imageIn->x * imageIn->y; i++) {
        imageOut->data[i].red = (unsigned char)round(imageIn->data->red[i]);
        imageOut->data[i].green = (unsigned char)round(imageIn->data->green[i]);
        imageOut->data[i].blue = (unsigned char)round(imageIn->data->blue[i]);
    }

    return imageOut;
}
/////


// //blur one color channel
// void blurIteration(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
	
// 	// Iterate over each pixel
// 	for(int senterX = 0; senterX < imageIn->x; senterX++) {

// 		for(int senterY = 0; senterY < imageIn->y; senterY++) {

// 			// For each pixel we compute the magic number
// 			double sum = 0;
// 			int countIncluded = 0;
// 			for(int x = -size; x <= size; x++) {

// 				for(int y = -size; y <= size; y++) {
// 					int currentX = senterX + x;
// 					int currentY = senterY + y;

// 					// Check if we are outside the bounds
// 					if(currentX < 0)
// 						continue;
// 					if(currentX >= imageIn->x)
// 						continue;
// 					if(currentY < 0)
// 						continue;
// 					if(currentY >= imageIn->y)
// 						continue;

// 					// Now we can begin
// 					int numberOfValuesInEachRow = imageIn->x;
// 					int offsetOfThePixel = (numberOfValuesInEachRow * currentY + currentX);
// 					if(colourType == 0)
// 						sum += imageIn->data[offsetOfThePixel].red;
// 					if(colourType == 1)
// 						sum += imageIn->data[offsetOfThePixel].green;
// 					if(colourType == 2)
// 						sum += imageIn->data[offsetOfThePixel].blue;

// 					// Keep track of how many values we have included
// 					countIncluded++;
// 				}

// 			}

// 			// Now we compute the final value
// 			double value = sum / countIncluded;


// 			// Update the output image
// 			int numberOfValuesInEachRow = imageOut->x; // R, G and B
// 			int offsetOfThePixel = (numberOfValuesInEachRow * senterY + senterX);
// 			if(colourType == 0)
// 				imageOut->data[offsetOfThePixel].red = value;
// 			if(colourType == 1)
// 				imageOut->data[offsetOfThePixel].green = value;
// 			if(colourType == 2)
// 				imageOut->data[offsetOfThePixel].blue = value;
// 		}

// 	}
// }

//**Experminet [3]; transpose implementation
//////////////////////
// void transposeImage(AccurateImage *imageOut, AccurateImage *imageIn) {
//     for (int y = 0; y < imageIn->y; y++) {
//         for (int x = 0; x < imageIn->x; x++) {
//             int inOffset = y * imageIn->x + x;
//             int outOffset = x * imageIn->y + y;
//             imageOut->data[outOffset] = imageIn->data[inOffset];
//         }
//     }
//     imageOut->x = imageIn->y;
//     imageOut->y = imageIn->x;
// }

// void horizontalBlur(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
//     for (int y = 0; y < imageIn->y; y++) {
//         for (int x = 0; x < imageIn->x; x++) {
//             double sum = 0.0;
//             int count = 0;
//             for (int dx = -size; dx <= size; dx++) {
//                 int currentX = x + dx;
//                 // Handle boundary conditions by clamping [4] we are using another if condition
//                 // if (currentX < 0) currentX = 0;
//                 // if (currentX >= imageIn->x) currentX = imageIn->x - 1;
// 				if (currentX < 0 || currentX >= imageIn->x)
//     				continue;
//                 int offset = y * imageIn->x + currentX;
//                 sum += (colourType == 0) ? imageIn->data[offset].red :
//                        (colourType == 1) ? imageIn->data[offset].green : imageIn->data[offset].blue;
//                 count++;
//             }
//             int outOffset = y * imageOut->x + x;
//             double avg = sum / count;
//             if (colourType == 0) imageOut->data[outOffset].red = avg;
//             else if (colourType == 1) imageOut->data[outOffset].green = avg;
//             else imageOut->data[outOffset].blue = avg;
//         }
//     }
// }

// void blurIteration(AccurateImage *out, AccurateImage *in, int colourType, int size) {
//     AccurateImage *temp = (AccurateImage *)malloc(sizeof(AccurateImage));
//     temp->data = (AccuratePixel *)malloc(in->x * in->y * sizeof(AccuratePixel));
//     temp->x = in->x;
//     temp->y = in->y;

//     // Horizontal blur
//     horizontalBlur(temp, in, colourType, size);

//     // Transpose for vertical blur
//     AccurateImage *transposedIn = (AccurateImage *)malloc(sizeof(AccurateImage));
//     transposedIn->data = (AccuratePixel *)malloc(temp->x * temp->y * sizeof(AccuratePixel));
//     transposeImage(transposedIn, temp);

//     AccurateImage *transposedOut = (AccurateImage *)malloc(sizeof(AccurateImage));
//     transposedOut->data = (AccuratePixel *)malloc(temp->x * temp->y * sizeof(AccuratePixel));
//     transposedOut->x = transposedIn->x;
//     transposedOut->y = transposedIn->y;

//     // Vertical blur (as horizontal blur on transposed image)
//     horizontalBlur(transposedOut, transposedIn, colourType, size);

//     // Transpose back
//     transposeImage(out, transposedOut);

//     // Free temporary images
//     free(temp->data);
//     free(temp);
//     free(transposedIn->data);
//     free(transposedIn);
//     free(transposedOut->data);
//     free(transposedOut);
// }


//////////////////////

//**Experminet [2]; changing blur algorithm w/o parallelization

// //which is Split the 2D blur into two 1D passes (horizontal + vertical):
// // Horizontal blur pass
// void horizontalBlur(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
//     for (int y = 0; y < imageIn->y; y++) {
//         for (int x = 0; x < imageIn->x; x++) {
//             float sum = 0.0f;
//             int count = 0;
//             for (int dx = -size; dx <= size; dx++) {
//                 int currentX = x + dx;
//                 if (currentX < 0 || currentX >= imageIn->x) continue;
//                 int offset = y * imageIn->x + currentX;
//                 sum += (colourType == 0) ? imageIn->data[offset].red :
//                        (colourType == 1) ? imageIn->data[offset].green : imageIn->data[offset].blue;
//                 count++;
//             }
//             int outOffset = y * imageOut->x + x;
//             float avg = sum / count;
//             if (colourType == 0) imageOut->data[outOffset].red = avg;
//             else if (colourType == 1) imageOut->data[outOffset].green = avg;
//             else imageOut->data[outOffset].blue = avg;
//         }
//     }
// }

// // Vertical blur pass (similar structure)
// void verticalBlur(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
//     for (int x = 0; x < imageIn->x; x++) {
// 		for (int y = 0; y < imageIn->y; y++) {
// 			float sum = 0.0f;
// 			int count = 0;
// 			for (int dy = -size; dy <= size; dy++) {
// 				int currentY = y + dy;
// 				if (currentY < 0 || currentY >= imageIn->y) continue;
// 				int offset = currentY * imageIn->x + x;
// 				sum += (colourType == 0) ? imageIn->data[offset].red :
// 					   (colourType == 1) ? imageIn->data[offset].green : imageIn->data[offset].blue;
// 				count++;
// 			}
// 			int outOffset = y * imageOut->x + x;
// 			float avg = sum / count;
// 			if (colourType == 0) imageOut->data[outOffset].red = avg;
// 			else if (colourType == 1) imageOut->data[outOffset].green = avg;
// 			else imageOut->data[outOffset].blue = avg;
// 		}
// 	}
// }

// // Replace blurIteration with:
// void blurIteration(AccurateImage *out, AccurateImage *in, int colourType, int size) {
//     horizontalBlur(out, in, colourType, size);
//     verticalBlur(out, out, colourType, size); // Reuse output as input for vertical pass
// }

// ////////////////////////////////////////


//[5]////////*******///////////
void blurIteration(AccurateImage *imageOut, AccurateImage *imageIn, int colourType, int size) {
    float *inputChannel;
    float *outputChannel;

    // Select the appropriate color channel
    if (colourType == 0) {
        inputChannel = imageIn->data->red;
        outputChannel = imageOut->data->red;
    } else if (colourType == 1) {
        inputChannel = imageIn->data->green;
        outputChannel = imageOut->data->green;
    } else {
        inputChannel = imageIn->data->blue;
        outputChannel = imageOut->data->blue;
    }

    // Iterate over each pixel
    for (int senterX = 0; senterX < imageIn->x; senterX++) {
        for (int senterY = 0; senterY < imageIn->y; senterY++) {
            double sum = 0.0;
            int countIncluded = 0;

            for (int x = -size; x <= size; x++) {
                for (int y = -size; y <= size; y++) {
                    int currentX = senterX + x;
                    int currentY = senterY + y;

                    // Check if we are outside the bounds
                    if (currentX < 0 || currentX >= imageIn->x || currentY < 0 || currentY >= imageIn->y)
                        continue;

                    int offset = currentY * imageIn->x + currentX;
                    sum += inputChannel[offset];
                    countIncluded++;
                }
            }

            // Compute the final value
            int offset = senterY * imageOut->x + senterX;
            outputChannel[offset] = sum / countIncluded;
        }
    }
}
//[5]////////*******///////////

//[5] rebuilt for SoA
// Perform the final step, and return it as ppm.
PPMImage *imageDifference(AccurateImage *imageInSmall, AccurateImage *imageInLarge) {
    PPMImage *imageOut;
    imageOut = (PPMImage *)malloc(sizeof(PPMImage));
    imageOut->data = (PPMPixel *)malloc(imageInSmall->x * imageInSmall->y * sizeof(PPMPixel));

    imageOut->x = imageInSmall->x;
    imageOut->y = imageInSmall->y;

    for (int i = 0; i < imageInSmall->x * imageInSmall->y; i++) {
        // Process the red channel
        double value = imageInLarge->data->red[i] - imageInSmall->data->red[i];
        if (value > 255)
            imageOut->data[i].red = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            if (value > 255)
                imageOut->data[i].red = 255;
            else
                imageOut->data[i].red = floor(value);
        } else if (value > -1.0 && value < 0.0) {
            imageOut->data[i].red = 0;
        } else {
            imageOut->data[i].red = floor(value);
        }

        // Process the green channel
        value = imageInLarge->data->green[i] - imageInSmall->data->green[i];
        if (value > 255)
            imageOut->data[i].green = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            if (value > 255)
                imageOut->data[i].green = 255;
            else
                imageOut->data[i].green = floor(value);
        } else if (value > -1.0 && value < 0.0) {
            imageOut->data[i].green = 0;
        } else {
            imageOut->data[i].green = floor(value);
        }

        // Process the blue channel
        value = imageInLarge->data->blue[i] - imageInSmall->data->blue[i];
        if (value > 255)
            imageOut->data[i].blue = 255;
        else if (value < -1.0) {
            value = 257.0 + value;
            if (value > 255)
                imageOut->data[i].blue = 255;
            else
                imageOut->data[i].blue = floor(value);
        } else if (value > -1.0 && value < 0.0) {
            imageOut->data[i].blue = 0;
        } else {
            imageOut->data[i].blue = floor(value);
        }
    }

    return imageOut;
}


int main(int argc, char** argv) {
    // read image
    PPMImage *image;
    // select where to read the image from
    if(argc > 1) {
        // from file for debugging (with argument)
        image = readPPM("flower.ppm");
    } else {
        // from stdin for cmb
        image = readStreamPPM(stdin);
    }
	
	
	AccurateImage *imageAccurate1_tiny = convertToAccurateImage(image);
	AccurateImage *imageAccurate2_tiny = convertToAccurateImage(image);
	
	// Process the tiny case:
	for(int colour = 0; colour < 3; colour++) {
		int size = 2;
        blurIteration(imageAccurate2_tiny, imageAccurate1_tiny, colour, size);
        blurIteration(imageAccurate1_tiny, imageAccurate2_tiny, colour, size);
        blurIteration(imageAccurate2_tiny, imageAccurate1_tiny, colour, size);
        blurIteration(imageAccurate1_tiny, imageAccurate2_tiny, colour, size);
        blurIteration(imageAccurate2_tiny, imageAccurate1_tiny, colour, size);
	}
	
	
	AccurateImage *imageAccurate1_small = convertToAccurateImage(image);
	AccurateImage *imageAccurate2_small = convertToAccurateImage(image);
	
	// Process the small case:
	for(int colour = 0; colour < 3; colour++) {
		int size = 3;
        blurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
        blurIteration(imageAccurate1_small, imageAccurate2_small, colour, size);
        blurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
        blurIteration(imageAccurate1_small, imageAccurate2_small, colour, size);
        blurIteration(imageAccurate2_small, imageAccurate1_small, colour, size);
	}

    // an intermediate step can be saved for debugging like this
//    writePPM("imageAccurate2_tiny.ppm", convertToPPPMImage(imageAccurate2_tiny));
	
	AccurateImage *imageAccurate1_medium = convertToAccurateImage(image);
	AccurateImage *imageAccurate2_medium = convertToAccurateImage(image);
	
	// Process the medium case:
	for(int colour = 0; colour < 3; colour++) {
		int size = 5;
        blurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
        blurIteration(imageAccurate1_medium, imageAccurate2_medium, colour, size);
        blurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
        blurIteration(imageAccurate1_medium, imageAccurate2_medium, colour, size);
        blurIteration(imageAccurate2_medium, imageAccurate1_medium, colour, size);
	}
	
	AccurateImage *imageAccurate1_large = convertToAccurateImage(image);
	AccurateImage *imageAccurate2_large = convertToAccurateImage(image);
	
	// Do each color channel
	for(int colour = 0; colour < 3; colour++) {
		int size = 8;
        blurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
        blurIteration(imageAccurate1_large, imageAccurate2_large, colour, size);
        blurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
        blurIteration(imageAccurate1_large, imageAccurate2_large, colour, size);
        blurIteration(imageAccurate2_large, imageAccurate1_large, colour, size);
	}
	// calculate difference
	PPMImage *final_tiny = imageDifference(imageAccurate2_tiny, imageAccurate2_small);
    PPMImage *final_small = imageDifference(imageAccurate2_small, imageAccurate2_medium);
    PPMImage *final_medium = imageDifference(imageAccurate2_medium, imageAccurate2_large);
	// Save the images.
    if(argc > 1) {
        writePPM("flower_tiny.ppm", final_tiny);
        writePPM("flower_small.ppm", final_small);
        writePPM("flower_medium.ppm", final_medium);
    } else {
        writeStreamPPM(stdout, final_tiny);
        writeStreamPPM(stdout, final_small);
        writeStreamPPM(stdout, final_medium);
    }
	
}

