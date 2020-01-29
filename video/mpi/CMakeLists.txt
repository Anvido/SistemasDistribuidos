cmake_minimum_required(VERSION 2.8)
project( video )
find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( video video.cpp )
target_link_libraries( video ${OpenCV_LIBS} )

find_package(MPI REQUIRED)

target_include_directories(video PRIVATE ${MPI_CXX_INCLUDE_PATH})
target_compile_options(video PRIVATE ${MPI_CXX_COMPILE_FLAGS})
target_link_libraries(video ${MPI_CXX_LIBRARIES} ${MPI_CXX_LINK_FLAGS})