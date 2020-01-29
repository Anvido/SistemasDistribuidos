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

bool show_video(Mat output){
  imshow( "Frame", output );
  
  // Press  ESC on keyboard to  exit
  char c = (char)waitKey(1);
  if( c == 27 ) 
    return true;
}

int main(int argc, char *argv[]){
  int i, tag = 0, tasks, iam, root = 0, namelen;
  // Video properties
  VideoCapture cap;
  int frame_width, frame_height;
  int chunk_frames;
  double fps, num_frames;

  VideoCapture test("../video15.mp4"); 
 
  // Check if camera opened successfully
  if(!test.isOpened())
  {
    cout << "Error opening video stream" << endl; 
    return -1; 
  } 
  num_frames = test.get(CV_CAP_PROP_FRAME_COUNT);
  num_frames = 125;
  frame_width = test.get(CV_CAP_PROP_FRAME_WIDTH); 
  frame_height = test.get(CV_CAP_PROP_FRAME_HEIGHT); 
  fps = test.get(CV_CAP_PROP_FPS);
  test.release();


  //VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height));

  char processor_name[MPI_MAX_PROCESSOR_NAME];
  MPI_Status status;

  MPI_Datatype frame_type;

  MPI_Init(&argc, &argv);

  MPI_Type_contiguous(1080 *1920 * 3, MPI_UNSIGNED_CHAR, &frame_type);
  MPI_Type_commit(&frame_type);

  MPI_Comm_size(MPI_COMM_WORLD, &tasks);
  MPI_Comm_rank(MPI_COMM_WORLD, &iam);

  unsigned char *v_frames = NULL;

  if (iam == root){
    cap.open("../video15.mp4"); 

    if(!cap.isOpened())
    {
      cout << "Error opening video stream" << endl; 
      return -1; 
    } 

    v_frames = (unsigned char *) malloc(1080 *1920 * 3 * (num_frames));
    assert(v_frames != NULL);

    for (int i = 0; i < num_frames; i++){
      Mat frame;
      cap >> frame;
      /*
      imshow( "Frame", frame );
  
      // Press  ESC on keyboard to  exit
      char c = (char)waitKey(1);
      if( c == 27 ) 
        break;
      */
      long pos =  1920 * 1080 * 3 * i;
      memcpy ( v_frames + pos, frame.ptr<unsigned char>(0), sizeof(unsigned char) * 1920 * 1080 * 3 );
    }
  }
  

  //printf("node %d: tasks: %d\n", iam, tasks);
  chunk_frames = num_frames/ tasks;
  //printf("node %d: chunkframes: %d\n", iam, chunk_frames);
  
  unsigned char *p_frames = NULL;
  p_frames = (unsigned char *) malloc(1080 *1920 * 3 * chunk_frames);
  assert(p_frames != NULL);

  MPI_Scatter(v_frames, chunk_frames, frame_type,
              p_frames, chunk_frames, frame_type, root, MPI_COMM_WORLD);
  //printf("scatteernode %d: chunkframes: %d\n", iam, chunk_frames);

  // proceso
  /*
  unsigned char *p_frames2 = (unsigned char *) malloc(1080 *1920 * 3 * chunk_frames);
  assert(p_frames != NULL);
  memcpy ( p_frames2, p_frames, sizeof(unsigned char) * 1920 * 1080 * 3 * chunk_frames );
  printf("un chunk %u\n", p_frames2[1000]);
  */
  
  unsigned char *temp_frame = (unsigned char *) malloc(1920 * 1080 * 3);
  for (int i = 0; i < chunk_frames; i++){
    long pos = i * sizeof(unsigned char) * 1920 * 1080 * 3 ;
    memcpy ( temp_frame, p_frames + pos, sizeof(unsigned char) * 1920 * 1080 * 3 );
    cv::Mat output(1080, 1920, CV_8UC3, (void*) temp_frame);

    for(int i = 0; i < output.rows; i++){
        for(int j = 0; j < output.cols; j++){
            Vec3b intensity = output.at<Vec3b>(i, j);
            int gray = .299f * intensity.val[0]  + .587f * intensity.val[1] + .114f * intensity.val[2];
            output.at<Vec3b>(i,j) = Vec3b(gray, gray, gray);
        } 
    }
    memcpy ( p_frames + pos, output.ptr<unsigned char>(0), sizeof(unsigned char) * 1920 * 1080 * 3 );
  }
  

  unsigned char *r_frames = NULL; 

  if (iam == 0) {
    //printf("llegue en 0 chunkframes %d, tasks %d\n", chunk_frames, tasks);
    r_frames = (unsigned char *) malloc(1080 *1920 * 3 * (chunk_frames) * tasks);
    assert(r_frames != NULL);
  }

  MPI_Gather(p_frames, chunk_frames, frame_type,
             r_frames, chunk_frames, frame_type, root, MPI_COMM_WORLD);


  if (iam == 0) {
    //printf("llegue :v %d", iam);
    VideoWriter video("outcpp.avi",VideoWriter::fourcc('M','J','P','G'), fps, Size(frame_width,frame_height));
    
    unsigned char *temp_frame2 = (unsigned char *) malloc(1920 * 1080 * 3);

    for (int i = 0; i < num_frames; i++){
      long pos = i * sizeof(unsigned char) * 1920 * 1080 * 3 ;
      memcpy ( temp_frame2, r_frames + pos, sizeof(unsigned char) * 1920 * 1080 * 3 );
      cv::Mat output(1080, 1920, CV_8UC3, (void*) temp_frame2);

      //cv::cvtColor(output, result, cv::COLOR_RGBA2BGR);
      video.write(output);
    }
    
  }

  // Clean up
  if (iam == 0) {
    free(v_frames);
    free(r_frames);
  }

  free(p_frames);

  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  
  return 0;
}

