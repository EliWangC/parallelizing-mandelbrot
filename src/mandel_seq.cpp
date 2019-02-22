// Implementation File ////////////////////////////////////////////////////////
/*
 * @author Raymond Pistoresi
 *
 * @file mandel_seq.cpp
 *
 * @date 20 March 2017
 *
 * @brief A sequential implementation to caldulate the Mandelbrot set.
 *        The set is stored as a greyscale image in sequential.pgm file
 *        and the time to calculate the set is outputted to screen.
 */
//Header Files ////////////////////////////////////////////////////////////////
#include <iostream>
#include <stdlib.h>
#include "PIMFuncs.h"
#include "mpi.h"

using namespace std;

// Precompiler Directives /////////////////////////////////////////////////////
#define MAX_WIDTH  32000
#define MAX_HEIGHT 32000

// Structs ////////////////////////////////////////////////////////////////////
struct complex{
    double real;
    double imag;
};

// Functional Prototypes //////////////////////////////////////////////////////
int cal_pixel( complex c );
bool valid( int, char *[], int&, int& );
bool inBounds( int, int, int );
void outputResults( unsigned char **, double, int, int );

// Main Function //////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
  // declare and assign variables
  int comm_sz, my_rank;
  int width, height, row, col;
  int greyScaleMod, index;
  double startTime, endTime, deltaTime, radius, radSq;
  unsigned char **pixels;
  complex c;

  // initialize MPI
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &comm_sz );
  MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

  // assing default parameters
  radius = 2.0;
  radSq = ( radius * radius );
  width = height = 0;
  greyScaleMod = 256;
  
  // test for correct user input and report, passes back height and width
  if( !valid( argc, argv, width, height ) )
  {
    return 0;
  }
  
  // initialize 2D array for all image pixels
  pixels = new unsigned char *[ height ];
  for( index = 0; index < height; index++ )
  {
    pixels[ index ] = new unsigned char [ width ];
  }

  startTime = MPI_Wtime();
  // loop through each pixel calculation and write to file (Mandelbrot set)
  for( row = 0; row < height; row++ )
  {
    for( col = 0; col < width; col++ )
    {
      c.real = ( col - width / radius ) * ( radSq / width );
      c.imag = ( row - height / radius ) * ( radSq / width) ;
      pixels[ row ][ col ] = ( cal_pixel( c ) * 35 ) % greyScaleMod;
    }
  }
  // stop wall clock and store time
  endTime = MPI_Wtime();
  deltaTime = endTime - startTime;

  // report elapsed time and write image to file
  outputResults( pixels, deltaTime, width, height );

  // deallocate memory and exit
  for( index = 0; index < height; index++ )
  {
    delete [] pixels[ index ];
  }
  return 0;
}

// Functional Implementation //////////////////////////////////////////////////
/**
  * @brief Function to calculate if pixel is in Mandelbrot set
  *
  * @param[in] c
  *            complex number containing real and imaginary part
  *
  * @pre complex The complex number c with valid real and imaginary parts
  *
  * @return int count value containing pixel value (0-256)
  *
  * @note If count returns as 0, the pixel will be counted in Mandelbrot set.
  */
int cal_pixel( complex c )
{
  complex z;
  int count, max;
  double temp, lengthsq;
  max = 256;
  z.real = 0; z.imag = 0;
  count = 0; /* number of iterations */
  do
  {
    temp = z.real * z.real - z.imag * z.imag + c.real;
    z.imag = 2 * z.real * z.imag + c.imag;
    z.real = temp;
    lengthsq = z.real * z.real + z.imag * z.imag;
    count++;
  } while ( ( lengthsq < 4.0 ) && ( count < max ) );
  return count;
}
/**
  * @brief Function checking if image size is within program bounds
  *
  * @param[in] w
  *            image width
  *
  * @param[in] h
  *            image height
  *
  * @return bool returns false if fails boundary test, otherwise true
  *
  * @note MAX_WIDTH and MAX_HEIGHT set above as 10000
  */
bool inBounds( int argc, int w, int h )
{
  if( ( w < 0 ) || ( w > MAX_WIDTH ) || ( h < 0 ) || ( h > MAX_HEIGHT ) )
  {
    cout << "ERROR: Invalid image dimensions." << endl;
    cout << "Height and width range: [1,1000]" << endl;
    cout << "Shutting down..." << endl;
    return true;
  }
  return false;
}
/**
  * @brief Function checking if user input is valid
  *
  * @param[in] argc
  *            int containing command line argument count
  *
  * @param[in] argv
  *            char *[] containing user argument values
  *
  * @param[in] w
  *            int width size to be passed back to calling function
  *
  * @param[in] h
  *            int height size to be passed back to calling function
  *
  * @return bool returns true is valid dimensions, otherwise false.
  *
  * @note MAX_WIDTH and MAX_HEIGHT above as 10000
  */
bool valid( int argc, char *argv[], int &w, int &h )
{
  // test user input for validity
  if( argc == 3 )
  {
    w = atoi( argv[ 1 ] );
    h = atoi( argv[ 2 ] );
    if( inBounds( argc, w, h ) )
    {
        return false;
    }
    cout << "Width x Height = " << w << " x " << h << endl;
    return true;
  }
  else
  {
    cout << "ERROR: Invalid input." << endl;
    cout << "Arguments should be <width> <height>" << endl;
    cout << "Shutting down..." << endl;
    return false;
  }
}
/**
  * @brief Function checking if image size is within program bounds
  *
  * @param[in] argc
  *            int containing command line argument count
  *
  * @param[in] argv
  *            char *[] containing user argument values
  *
  * @param[in] w
  *            int width size to be passed back to calling function
  *
  * @param[in] h
  *            int height size to be passed back to calling function
  *
  * @return bool returns true is valid dimensions, otherwise false.
  *
  * @note MAX_WIDTH and MAX_HEIGHT above as 100000
  */
void outputResults( unsigned char ** pixs, double time, int w, int h )
{
  const unsigned char **p;
  p = ( const unsigned char ** ) pixs;
  cout.precision( 6 );
  cout << "Set calculation took " << fixed << time << "s." << endl;
  cout << "Writing image to file 'sequential.pgm'" << endl;

  if( pim_write_black_and_white( "sequential.pgm", w, h, p ) )
  {
    cout << "SUCCESS: image written to file." << endl;
  }
  else
  {
    cout << "FAILED: image NOT written to file." << endl;
  }
}