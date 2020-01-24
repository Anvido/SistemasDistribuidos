#include <iostream>
#include "utils.h"
#include <string>
#include <stdio.h>
#include "opencv2/opencv.hpp"
#include <opencv2/videoio/videoio_c.h>


// Function calling the kernel to operate
void rgba_to_grey(uchar4 * const d_rgbaImage,
                  unsigned char* const d_greyImage, 
                  size_t numRows, size_t numCols);

//include the definitions of the above functions for this homework
#include "preprocess.cpp"

using namespace std;
using namespace cv;

Mat frame, result;

int main(int argc, char **argv) {
  uchar4        *h_rgbaImage, *d_rgbaImage;
  unsigned char *h_greyImage, *d_greyImage;

  std::string input_file;
  std::string output_file;

  VideoCapture cap("../video60.mp4");

  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl;
    return -1;
  }

  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);
  double fps = cap.get(CV_CAP_PROP_FPS);
  double num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);

  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height));

  //make sure the context initializes ok
  checkCudaErrors(cudaFree(0));

  while(1)
  {
    // Capture frame-by-frame
    cap >> frame;

    // If the frame is empty, break immediately
    if (frame.empty())
      break;  

    result.create(frame.size(), frame.type());

    //load the image and give us our input and output pointers
    preProcess(&h_rgbaImage, &h_greyImage, &d_rgbaImage, &d_greyImage, frame);

    //call the cuda code
    rgba_to_grey(d_rgbaImage, d_greyImage, numRows(), numCols());

    size_t numPixels = numRows()*numCols();
    checkCudaErrors(cudaMemcpy(h_greyImage, d_greyImage, sizeof(unsigned char) * numPixels, cudaMemcpyDeviceToHost));
    
    cv::Mat output(numRows(), numCols(), CV_8UC1, (void*)h_greyImage);

    cv::cvtColor(output, result, cv::COLOR_RGBA2BGR);

    //output.copyTo(result);

    video.write(result);

    /* Cleanup */
    cudaFree(d_rgbaImage__);
    cudaFree(d_greyImage__);

    /*
    // Display the resulting frame
    cv::imshow( "Frame", result );

    // Press  ESC on keyboard to  exit
    char c = (char)waitKey(1);
    if( c == 27 )
      break;
    */


  }
  
  /* Output the grey image */
  //cv::Mat output(numRows(), numCols(), CV_8UC1, (void*)h_greyImage);
  // Open the window
  //cv::namedWindow("to_bw");
  // Display the image m in this window
  //cv::imshow("to_bw", output);
  //cv::waitKey (0);
  //cv::destroyWindow ("to_bw");
  //output the image
  //cv::imwrite(output_file.c_str(), output);
  
  
  // When everything done, release the video capture and write object
  cap.release();
  video.release();

  // Closes all the windows
  destroyAllWindows();
  return 0;
}