// Implementation File ////////////////////////////////////////////////////////
/*
 * @author Raymond Pistoresi
 *
 * @file mandel_dynamic.cpp
 *
 * @date 20 March 2017
 *
 * @brief A parallel implementation to calculate the Mandelbrot set.
 *        The set is stored as a greyscale image in dynamic.pgm file
 *        and the time to calculate the set is outputted to screen.
 *        This alogrithm incorporates dynamic load balancing.
 */
//Header Files ////////////////////////////////////////////////////////////////
#include <iostream>    /* outputting to screen captured in stat_res.txt      */
#include <stdlib.h>    /* atoi()                                             */
#include "PIMFuncs.h"  /* writing black & white image to file                */
#include "mpi.h"       /* Message Passing Interface library                  */

using namespace std;

// Precomipler Directives /////////////////////////////////////////////////////
#define MASTER     0
#define MAX_WIDTH  32000
#define MAX_HEIGHT 32000

// Structs ////////////////////////////////////////////////////////////////////
struct complex{
  float real;
  float imag;
};

// Functional Prototypes //////////////////////////////////////////////////////
int cal_pixel( complex );
bool valid( int, char *[], int&, int& );
bool inBounds( int, int );
void outputResults( unsigned char **, double, int, int );

// Main Function //////////////////////////////////////////////////////////////
int main( int argc, char *argv[] )
{
  // declare initial variables
  int comm_sz, my_rank, tag;
  int kill, proc, count, rcvdRow;
  int width, height;
  int row, col, index, greyScaleMod;
  double radius, radSq;
  double startTime, endTime, deltaTime;
  complex c;
  bool go;
  unsigned char *pixels;

  // assign default parameters
  radius = 2.0;
  radSq = ( radius * radius );
  width = height = 0;
  tag = 0;
  row = 0;
  count = 0;
  kill = -1;
  greyScaleMod = 256;

  // initialize MPI
  MPI_Init( &argc, &argv );
  MPI_Comm_size( MPI_COMM_WORLD, &comm_sz );
  MPI_Comm_rank( MPI_COMM_WORLD, &my_rank );

  // test for correct user input, report and quit on error
  if( !valid( argc, argv, width, height ) )
  {
    MPI_Finalize();
    return 0;
  }

  // 1D array to hold single row data
   pixels = ( unsigned char * ) calloc(width, sizeof( unsigned char ) );

  // parallelization portion of Mandelbrot set algorithm
  if( my_rank == MASTER )
  {
    // 2D array to store entire image data
    unsigned char **p = new unsigned char *[ height ];
    for( index = 0; index < height; index++ )
    {
      p[ index ] = new unsigned char [ width ];
    }

    // report image dimensions and start the wall clock
    cout << "width x height = " << width << " x " << height << endl;
    startTime = MPI_Wtime();

    // give all processors one row each to start
    proc = 1;
    while( proc < comm_sz ) /* not including lazy master  */
    {
      MPI_Send( &row, 1, MPI_INT, proc, tag, MPI_COMM_WORLD );
      proc++;
      row++;
      count++;
    }
    // give out one row at a time as processors become available
    MPI_Status status;
    do
    {
      MPI_Recv( pixels,
                width,
                MPI_UNSIGNED_CHAR,
                MPI_ANY_SOURCE,
                MPI_ANY_TAG,
                MPI_COMM_WORLD,
                &status );
      proc = status.MPI_SOURCE;
      rcvdRow = status.MPI_TAG;
      count--;
      if( row < height )
      {
        MPI_Send( &row, 1, MPI_INT, proc, tag, MPI_COMM_WORLD );
        row++;
        count++;
      }
      else
      {
        MPI_Send( &kill, 1, MPI_INT, proc, tag, MPI_COMM_WORLD );
      }

      // store recieved row in 2D image array
      for( col = 0; col < width; col++ )
      {
        p[ rcvdRow ][ col ] = pixels[ col ];
      }
    } while( count > 0 );

    // stop timer, store elapsed time, and report to user
    endTime = MPI_Wtime();
    deltaTime = endTime - startTime;

    // report elapsed time and write image to file
    outputResults( p, deltaTime, width, height );

    // deallocate 2D array
    for( index = 0; index < height; index++ )
    {
      delete [] p[ index ];
    }
    delete [] p;
  }
  else /* slaves process one row of pixels sent from MASTER and send back */
  {
    go = true;
    while( go )
    {
      MPI_Recv( &row,
                1,
                MPI_INT,
                MASTER,
                tag,
                MPI_COMM_WORLD,
                MPI_STATUS_IGNORE );
      if( row == -1 )
      {
        go = false;
      }
      else
      {
        for( col = 0; col < width; col++ )
        {
          c.real = ( col - width / radius ) * ( radSq / width );
          c.imag = ( row - height / radius) * ( radSq / width );

          // individual pixel stored as 8-bit color value (0-255)
          pixels[ col ] = ( cal_pixel( c ) * 35 ) % greyScaleMod;
        }
        MPI_Send( pixels,
                  width,
                  MPI_UNSIGNED_CHAR,
                  MASTER,
                  row,
                  MPI_COMM_WORLD );
      }
    }
  }
  // deallocate, wait, and shutdown
  free( pixels );
  MPI_Barrier( MPI_COMM_WORLD );
  MPI_Finalize();
  return 0;
}
// Functional Implementation //////////////////////////////////////////////////
/**
  * @brief Function calculate if pixel is in Mandelbrot set
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
  float temp, lengthsq;
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
bool inBounds( int w, int h )
{
  if( ( w < 0 ) || ( w > MAX_WIDTH ) || ( h < 0 ) || ( h > MAX_HEIGHT ) )
  {
    cout << "ERROR: Invalid image dimensions." << endl;
    cout << "Height and width range: [1,32000]" << endl;
    return false;
  }
  return true;
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
  if( argc == 3 )
  {
    w = atoi( argv[ 1 ] );
    h = atoi( argv[ 2 ] );
    if( inBounds(w, h) )
    {
      return true;
    }
  }
  cout << "ERROR: Invalid input." << endl;
  cout << "Arguments should be <width> <height>" << endl;
  cout << "Shutting down..." << endl;
  return false;
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
  * @note MAX_WIDTH and MAX_HEIGHT above as 10000
  */
void outputResults( unsigned char ** pixs, double time, int w, int h )
{
  const unsigned char **p;
  p = ( const unsigned char ** ) pixs;
  cout.precision( 6 );
  cout << "Set calculation took " << fixed << time << "s." << endl;
  cout << "Writing image to file 'dynamic.pgm'" << endl;

  if( pim_write_black_and_white( "dynamic.pgm", w, h, p ) )
  {
    cout << "SUCCESS: image written to file." << endl;
  }
  else
  {
    cout << "FAILED: image NOT written to file." << endl;
  }
}