#include "opencv2/opencv.hpp"
#include <iostream>
#include <opencv2/videoio/videoio_c.h>
#include <mpi.h>
 
using namespace std;
using namespace cv;

/*
 MPI_Datatype frame_type;
  MPI_Type_contiguous(1080 *1920 * 3, MPI_UNSIGNED_CHAR, &frame_type);
  MPI_Type_commit(&frame_type);
*/
 
int main(int argc, char *argv[]){

  // Video properties
  int frame_width; 
  int frame_height; 
  double fps;
  double num_frames;
  int frame_size;
  int chunk_img;
  VideoCapture cap;

  VideoCapture test("../video15.mp4"); 
 
  // Check if camera opened successfully
  if(!test.isOpened())
  {
    cout << "Error opening video stream" << endl; 
    return -1; 
  } 
  num_frames = test.get(CV_CAP_PROP_FRAME_COUNT);
  //num_frames = 240;
  frame_width = test.get(CV_CAP_PROP_FRAME_WIDTH); 
  frame_height = test.get(CV_CAP_PROP_FRAME_HEIGHT); 
  fps = test.get(CV_CAP_PROP_FPS);
  test.release();
  frame_size = frame_height * frame_width * 3;

  int i, tag = 0, tasks, iam, root = 0, namelen;

  char processor_name[MPI_MAX_PROCESSOR_NAME];

  VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height)); 

  MPI_Status status;
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &iam);

  if (iam == root){
    cap.open("../video15.mp4");  

    if(!cap.isOpened()){
      cout << "Error opening video stream" << endl; 
      return -1; 
    } 
  }

  

  unsigned char *frame_ptr = NULL;
  unsigned char *image_fragment = NULL;
  Mat frame;
  Mat result;
  chunk_img = frame_size / tasks;


  for (int i = 0; i < num_frames; i++){    
    //printf("node %d, i: %d\n", iam, i);
    
    if (iam == root){
      cap >> frame;
      if (frame.empty())
        break;

      frame_ptr = (unsigned char *) malloc(frame_size);
      assert(frame_ptr != NULL);
      frame_ptr = frame.ptr<unsigned char>(0);
    }

    image_fragment = (unsigned char *) malloc(chunk_img);
    assert(image_fragment != NULL);

    MPI_Scatter(frame_ptr, chunk_img, MPI_UNSIGNED_CHAR,
                image_fragment, chunk_img, MPI_UNSIGNED_CHAR, root, MPI_COMM_WORLD);
    // ------
    for(int i = 0; i < chunk_img; i+= 3){
        int gray = .299f * image_fragment[i+2]  + .587f * image_fragment[i+1] + .114f * image_fragment[i];
        image_fragment[i] = gray;
        image_fragment[i+1] = gray;
        image_fragment[i+2] = gray;
    }

    // ------
    if (iam == root) {
      //free(frame_ptr);
      frame_ptr = (unsigned char *) malloc(chunk_img * tasks);
      assert(frame_ptr != NULL);
    }

    MPI_Gather(image_fragment, chunk_img, MPI_UNSIGNED_CHAR,
               frame_ptr, chunk_img, MPI_UNSIGNED_CHAR, root, MPI_COMM_WORLD);

    if (iam == root){
      cv::Mat tmp(frame_height, frame_width, CV_8UC3, (void*) frame_ptr);
      //result = tmp.clone();
      // Write the frame into the file 'outcpp.avi'
      video.write(tmp);
    }
    /*
    // Clean up
    if (iam == root) {
      free(frame_ptr);
      free(r_frame_ptr);
    }

    free(image_fragment);
    */
    //printf("fin node %d, i: %d\n", iam, i);
  }
 
  // When everything done, release the video capture and write object
  //cap.release();
  //video.release();

//  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  return 0;
}

