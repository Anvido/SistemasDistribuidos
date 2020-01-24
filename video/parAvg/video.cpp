#include "opencv2/opencv.hpp"
#include <iostream>
#include <opencv2/videoio/videoio_c.h>
#include <omp.h>

using namespace std;
using namespace cv;

void chunkFilter(int beginRow, int endRow, int beginCol, int endCol);

Mat frame, result;
int g_num_procs;

int main(){
  //g_num_procs = omp_get_num_procs();
  g_num_procs = 16;
  omp_set_num_threads(g_num_procs);
  printf ("Procesando con %d hilos\n", g_num_procs);

  // Create a VideoCapture object and use camera to capture the video
  VideoCapture cap("../../../video60.mp4");

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

  // Define the codec and create VideoWriter object.The output is stored in 'outcpp.avi' file.
  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height));
  while(1)
  {
    // Capture frame-by-frame
    cap >> frame;

    // If the frame is empty, break immediately
    if (frame.empty())
      break;


    result.create(frame.size(), frame.type());
    const int nChannels = frame.channels();

    #pragma omp parallel
    {
      int threadId = omp_get_thread_num();
      int numProcs = g_num_procs;
      int rows = 2;
      int cols = numProcs / 2;

      // printf("%d", threadId);

      int beginRow = threadId /  cols * frame.rows / rows;
      int endRow = (threadId / cols + 1)  * frame.rows / rows;
      
      int beginCol = threadId % cols * frame.cols / cols ;
      int endCol = (threadId % cols + 1) * frame.cols / cols ;

      chunkFilter(beginRow, endRow, beginCol, endCol);
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

void chunkFilter(int beginRow, int endRow, int beginCol, int endCol) {

  for(int i = beginRow ; i < endRow; i++){
    for(int j = beginCol; j < endCol; j++){
      Vec3b intensity = frame.at<Vec3b>(i, j);
      int gray = .299f * intensity.val[0]  + .587f * intensity.val[1] + .114f * intensity.val[2];
      result.at<Vec3b>(i,j) = Vec3b(gray, gray, gray);
    }
  }
}

