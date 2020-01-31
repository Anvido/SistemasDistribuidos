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

    int img_size = image.rows * image.cols * nChannels;

    unsigned char *img = (unsigned char *) malloc(img_size);

    Mat imagecp = image.clone();

    img = imagecp.ptr<unsigned char>(0);

    for(int i = 0; i < image.rows; i++){
        for(int j = 0; j < image.cols; j++){
            Vec3b intensity = image.at<Vec3b>(i, j);
            int mean = (intensity.val[0] + intensity.val[1] + intensity.val[2])/3;
            result.at<Vec3b>(i,j) = Vec3b(mean, mean, mean);
            //result.at<uchar>(i,j) = 255 - image.at<uchar>(i,j);
        } 
    }

    for(int i = 0, index = 0; i < img_size; i+= 3, index++){
        int gray = .299f * img[i+2]  + .587f * img[i+1] + .114f * img[i];
        img[i] = gray;
        img[i+1] = gray;
        img[i+2] = gray;
    }

    cv::Mat output2(720, 1280, CV_8UC3, (void*) img);

    printf("\n");
    


    namedWindow("Display Image", WINDOW_AUTOSIZE );
    namedWindow("Output", WINDOW_AUTOSIZE);
    namedWindow("Pointer", WINDOW_AUTOSIZE);
    imshow("Display Image", image);
    imshow( "Output", result );
    imshow( "Pointer", output2 );
    waitKey(0);
    return 0;
}
