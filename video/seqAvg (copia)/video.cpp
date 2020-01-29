#include "opencv2/opencv.hpp"
#include <iostream>
#include <opencv2/videoio/videoio_c.h>
 
using namespace std;
using namespace cv;
 
int main(){
 
  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap("../video15.mp4"); 
 
  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl; 
    return -1; 
  } 
 
  // Default resolution of the frame is obtained.The default resolution is system dependent. 
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH); 
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT); 
  double fps = cap.get(CV_CAP_PROP_FPS);
  double num_frames = cap.get(CV_CAP_PROP_FRAME_COUNT);
  // printf("frame width %d", frame_width);
  // printf("frame height %d", frame_height);
  // printf("frame size %d", frame_width * frame_height);
  //printf("%lf %lf \n", fps, num_frames);
   
  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file. 
  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height)); 
  
  unsigned char *uframes[(int)num_frames];
  
  for(int i = 0; i < num_frames; i++)
    uframes[i] = (unsigned char *) malloc(1080 *1920 * 3);
  
  int i = 0;

  Mat frame; 

  while(1)
  { 
    Mat result;
     
    // Capture frame-by-frame 
    cap >> frame;
  
    // If the frame is empty, break immediately
    if (frame.empty())
      break;
    
    result.create(frame.size(), frame.type());
    const int nChannels = frame.channels();

    //printf("%d %d\n", frame.rows, frame.cols);

    for(int i = 0; i < frame.rows; i++){
        for(int j = 0; j < frame.cols; j++){
            Vec3b intensity = frame.at<Vec3b>(i, j);
            int gray = .299f * intensity.val[0]  + .587f * intensity.val[1] + .114f * intensity.val[2];
            result.at<Vec3b>(i,j) = Vec3b(gray, gray, gray);
        } 
    }
   
    memcpy ( uframes[i], result.ptr<unsigned char>(0), sizeof(unsigned char) * 1920 * 1080 * 3 );

    //uframes[i] =  result.ptr<unsigned char>(0);
    i++;
  }

  for (int i =0; i < num_frames; i++){
    cv::Mat output(1080, 1920, CV_8UC3, (void*) uframes[i]);
    //cv::cvtColor(output, result, cv::COLOR_RGBA2BGR);
    video.write(output);
  }


  // When everything done, release the video capture and write object
  cap.release();
  video.release();
 
  // Closes all the windows
  destroyAllWindows();
  return 0;
}

