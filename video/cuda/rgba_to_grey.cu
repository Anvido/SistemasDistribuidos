#include "utils.h"
#include <stdio.h>
#include <math.h>       /* ceil */
//  /usr/local/cuda-10.2/samples/1_Utilities/deviceQuery
// 2 Muliprocesadores
// 128 Cores por mp
// 256 Cores
// Maximum number of threads per multiprocessor:  2048
// Maximum number of threads per block:           1024
// Max dimension size of a thread block (x,y,z): (1024, 1024, 64)
// Max dimension size of a grid size    (x,y,z): (2147483647, 65535, 65535)

#define TxB 1024

__global__
void rgba_to_grey_kernel(const uchar4* const rgbaImage,
                       unsigned char* const greyImage,
                       int numRows, int numCols)
{
  // The mapping from components of a uchar4 to RGBA is:
  // .x -> R ; .y -> G ; .z -> B ; .w -> A
  //
  //The output (greyImage) at each pixel should be the result of
  //applying the formula: output = .299f * R + .587f * G + .114f * B;
  //Note: We will be ignoring the alpha channel for this conversion
  int i = blockIdx.x * blockDim.x + threadIdx.x;
  uchar4 px = rgbaImage[i]; // thread pixel to process
  greyImage[i] = .299f * px.x +
                 .587f * px.y +
                 .114f * px.z;
}

void rgba_to_grey(uchar4 * const d_rgbaImage,
                  unsigned char* const d_greyImage, size_t numRows, size_t numCols)
{

  // Since it does not matter the relative position of a pixel
  // the block - grid assign strategy will simply be to cover
  // all pixels secuencially in 'x' axis
  long long int total_px = numRows * numCols;  // total pixels
  long int grids_n = ceil(total_px / TxB); // grids numer -- numero de bloques
  // tamaño bloque
  const dim3 blockSize(TxB, 1, 1);
  const dim3 gridSize(grids_n, 1, 1);
  rgba_to_grey_kernel<<<gridSize, blockSize>>>(d_rgbaImage, d_greyImage, numRows, numCols);
  
  cudaDeviceSynchronize(); checkCudaErrors(cudaGetLastError());
}