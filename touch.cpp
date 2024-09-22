#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <chrono>
/*
 * OpenCV reference counting system
 * Each Mat object has its own header, a matrix may be shared between two Mat objects
 * by having their matrix pointers point to the same address.
 * Copy operators only copy headers and the pointers to large matrix, not data itself.
 *
 * all headers below point to the same data matrix, making a modification will affect others
 * Mat A, C; -> headers
 * A = imread(argv[1], IMREAD_COLOR); allocate matrix
 *
 * Mat B(A); -> copy constructor
 * C = A; -> assignment
 * 
 * To avoid this (createa  new header and copy of Matrix)
 * Mat F = A.clone();
 * Mat G;
 * A.copyTo(G);
 *
 *
 * Region of interest (creating headers which refer to only a subsection of the full data.
 * Mat D(A, Rect(10, 10, 100, 100);
 * Mat E = A(Range::all(), Range(1,3));
 * 
 * Mat M (2, 2, CV_8UC3, Scalar(0,0,255));
 * 
 */
using namespace cv;

void grayScale(Mat& image){
    for(int row = 0; row < image.rows; ++row){
        for(int col = 0; col < image.cols; ++col){
            Vec3b& pixels = image.at<Vec3b>(row, col);
           
            uchar gray = static_cast<uchar>((pixels[0] + pixels[1] + pixels[2]) / 3);
            pixels[0] = gray;
            pixels[1] = gray;
            pixels[2] = gray;
        }
    }

}


void invertColors(Mat& image){
    for(int row = 0; row < image.rows; ++row){
        for(int col = 0; col < image.cols; ++col){
            Vec3b& pixels = image.at<Vec3b>(row, col);

            pixels[0] = 255 - pixels[0];
            pixels[1] = 255 - pixels[1];
            pixels[2] = 255 - pixels[2];
        }
    }

}

/* grayscale it first */
void applyThreshold(Mat& image, double threshold){
    for(int row = 0; row < image.rows; ++row){
        for(int col = 0; col < image.cols; ++col){
            Vec3b& pixels = image.at<Vec3b>(row, col);

            if (pixels[0] > threshold){
                pixels[0] = 255;
                pixels[1] = 255;
                pixels[2] = 255;
            } else {
                pixels[0] = 0;
                pixels[1] = 0;
                pixels[2] = 0;
            }

        }
    }
}

/* assume kernel size = 1
 * center pixel = n
 * you need all pixels in pixel(row, col)
 *          all pixels in pixel(row, col - 1)
 *          all pixels in pixel(row, col + 1)
 * sum color values
 * divide result by 9 to compute box blur effect
 */



void gaussianBlur(Mat& image, size_t kernel_size){
    if(kernel_size > image.rows || kernel_size > image.cols) throw std::runtime_error("Invalid kernel size.");
    

    Mat temp = image.clone();
    int half_kernel = kernel_size / 2;
    
    auto start = std::chrono::system_clock::now();
    /*for(int row = half_kernel; row < image.rows - half_kernel; ++row){
        for(int col = half_kernel; col < image.cols - half_kernel; ++col){
            Vec3i values = Vec3i(0,0,0);

            for(int i = -half_kernel; i <= half_kernel; ++i){
                for(int j = -half_kernel; j <= half_kernel; ++j){
                    Vec3b neighbor = temp.at<Vec3b>(row + i, col + j);

                    values[0] += neighbor[0];
                    values[1] += neighbor[1];
                    values[2] += neighbor[2];
                }

            }

            Vec3b& pixel = image.at<Vec3b>(row, col);
            pixel[0] = static_cast<uchar>(values[0] / (kernel_size * kernel_size));
            pixel[1] = static_cast<uchar>(values[1] / (kernel_size * kernel_size));
            pixel[2] = static_cast<uchar>(values[2] / (kernel_size * kernel_size));
        }
    }*/
    GaussianBlur(image, image, Size(kernel_size, kernel_size), 1);
    auto end = std::chrono::system_clock::now();
    auto elapsed = end - start;
    std::cout << "Blurred in " << elapsed.count() << "ms." << std::endl;
}

Mat loadImageColor(const std::string& img){
    std::string imagePath = samples::findFile(img);
    Mat image = imread(imagePath, IMREAD_COLOR);
    
    if(image.empty()){
        throw std::runtime_error("Could not read img: " + img + ".");
    }
    return image;
}

void displayImage(Mat& image, const std::string& windowName){
    Mat original = image.clone();
    imshow(windowName, image);
    while(true){
        int k = waitKey(30);
        if(k == 'q'){
            break;
        } else if(k == 'g'){
            grayScale(image);
            imshow(windowName,image);
        } else if(k == 'i'){
            invertColors(image);
            imshow(windowName, image);
        } else if(k == 'o'){
            image = original.clone();
            imshow(windowName, original);
        } else if(k == 'b'){
            gaussianBlur(image, 5);
            imshow(windowName, image);
        }
    }
    destroyWindow(windowName);
}

void saveImage(const Mat& image, const std::string& outputPath){
    imwrite(outputPath, image);
    std::cout << outputPath << " saved." << std::endl;
}


int main(){
    Mat a = loadImageColor("../imgs/dog.jpg");
    
    displayImage(a, "Window!");    
    return EXIT_SUCCESS;
}
