#include "opencv2/opencv.hpp"
#include <iostream>
#include <opencv2/videoio/videoio_c.h>
 
using namespace std;
using namespace cv;
 
int main(){
 
  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap("../../../video15.mp4"); 
 
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
  printf("frame width %d", frame_width);
  printf("frame height %d", frame_height);
  printf("frame size %d", frame_width * frame_height);
  //printf("%lf %lf \n", fps, num_frames);
   
  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file. 
  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height)); 
  while(1)
  { 
    Mat frame; 
     
    // Capture frame-by-frame 
    cap >> frame;
  
    // If the frame is empty, break immediately
    if (frame.empty())
      break;
     
    Mat result;
    result.create(frame.size(), frame.type());
    const int nChannels = frame.channels();

    for(int i = 0; i < frame.rows; i++){
        for(int j = 0; j < frame.cols; j++){
            Vec3b intensity = frame.at<Vec3b>(i, j);
            //int mean = (intensity.val[0] + intensity.val[1] + intensity.val[2])/3;
            int gray = .299f * intensity.val[0]  + .587f * intensity.val[1] + .114f * intensity.val[2];
            result.at<Vec3b>(i,j) = Vec3b(gray, gray, gray);
            //result.at<uchar>(i,j) = 255 - frame.at<uchar>(i,j);
            //result.at<uchar>(i,j) = .299f * intensity.val[0]  + .587f * intensity.val[1] + .114f * intensity.val[2];
        } 
    }

    
    // Write the frame into the file 'outcpp.avi'
    video.write(result);
    
    // Display the resulting frame    
    //imshow( "Frame", result );
  
    // Press  ESC on keyboard to  exit
    //char c = (char)waitKey(1);
    //if( c == 27 ) 
    //  break;
  }
 
  // When everything done, release the video capture and write object
  cap.release();
  video.release();
 
  // Closes all the windows
  destroyAllWindows();
  return 0;
}

