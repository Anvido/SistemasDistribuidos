#include "opencv2/opencv.hpp"
#include <iostream>
#include <opencv2/videoio/videoio_c.h>
 
using namespace std;
using namespace cv;
 
int main(){
 
  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap("../video.mp4"); 
 
  // Check if camera opened successfully
  if(!cap.isOpened())
  {
    cout << "Error opening video stream" << endl; 
    return -1; 
  } 
 
  // Default resolution of the frame is obtained.The default resolution is system dependent. 
  int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH); 
  int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT); 
   
  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file. 
  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'),30, Size(frame_width,frame_height)); 
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
            int mean = (intensity.val[0] + intensity.val[1] + intensity.val[2])/3;
            result.at<Vec3b>(i,j) = Vec3b(mean, mean, mean);
            //result.at<uchar>(i,j) = 255 - frame.at<uchar>(i,j);
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

