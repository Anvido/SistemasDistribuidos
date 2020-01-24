#include <stdio.h>
#include <opencv2/opencv.hpp>
using namespace cv;

int main(int argc, char** argv )
{
    if ( argc != 2 )
    {
        printf("usage: DisplayImage.out <Image_Path>\n");
        return -1;
    }
    Mat image;
    image = imread( argv[1], 1 );
    if ( !image.data )
    {
        printf("No image data \n");
        return -1;
    }

    Mat result;
    result.create(image.size(), image.type());
    const int nChannels = image.channels();

    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            Vec3b intensity = image.at<Vec3b>(i, j);
            int mean = (intensity.val[0] + intensity.val[1] + intensity.val[2])/3;
            result.at<Vec3b>(i,j) = Vec3b(mean, mean, mean);
            //result.at<uchar>(i,j) = 255 - image.at<uchar>(i,j);
        } 
    }
    


    namedWindow("Display Image", WINDOW_AUTOSIZE );
    namedWindow("Output", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    imshow( "Output", result );
    waitKey(0);
    return 0;
}
