#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <omp.h>
#include "main.h"

#define SLOW_THREAD 2
int twork[16] ;

extern void FillPascalTriangle( int arr[MAXDIM][MAXDIM+1] ) ;

void PTUpdateEntry( int arr[MAXDIM][MAXDIM+1], int i, int j, int value  )
{
  int tnum = omp_get_thread_num() ;
  //printf( "%d,%d\n", i, j ) ;
  #pragma omp critical
  twork[tnum]++ ;

  arr[i][j] = value ;

  if ( ( (tnum == SLOW_THREAD) && (rand()%1 == 0) ) || ( (tnum != SLOW_THREAD) && (rand()%100 == 0) ) )
  {
     double timex = time(NULL) ;
     while ( time(NULL) < (timex+.01) ) ;
  }
}

int main( int argc, char **argv )
{
    int arr[MAXDIM][MAXDIM+1] ;
    int row, col, numt=4, i ;

    double time1, time2, time3, time4 ;
    char status[100] ;
    int pf ;

    for( row = 0 ; row < MAXDIM ; row++ )
        for( col = 0 ; col <= MAXDIM ; col++ )
            arr[row][col] = 0 ;

    for( numt = 1 ; numt < 8 ; numt *= 2 )
    {
        omp_set_num_threads( numt ) ;

        for( i = 0 ; i < 16 ; i++ )
            twork[i] = 0 ;

        #pragma omp parallel
        {
          time1 = time(NULL) ;

          #pragma omp single    
          FillPascalTriangle( arr ) ;

          #pragma omp barrier
          time2 = time(NULL) ;
        }

        pf = 1 ;
        for( row = 0 ; row < MAXDIM ; row++ )
            for( col = 0 ; col <= row ; col++ )
            {
                int ccol ;
                if (col>row/2)
                  ccol = row-col ;
                else
                  ccol = col ;

                //printf( "C(%d,%d)=%d\n", row, col, arr[row][col] ) ;
                unsigned long ans = 1 ;
                if ( ( row > 0 ) && ( col > 0 ) && ( col < row ) )
                {
                  for( i = row ; i > (row-ccol) ; i-- )
                    ans *= i ;
                  for( i = ccol ; i > 0 ; i-- )
                    ans /= i ;
                }

                if ( ans != arr[row][col] )
                {
                   printf( "row=%d,col=%d,ans=%lu,arr=%d\n", row, col, ans, arr[row][col] ) ;
                   pf = 0;
                }
            }

        if (pf == 0)
            strcpy( status, "Failed" ) ;
        else
            strcpy( status, "Passed" ) ;

        printf( "numt=%d, status=%s, workload=[%d", numt, status, twork[0] ) ;
        for( i = 1 ; i < numt ; i++ )
            printf( ",%d", twork[i] ) ;
        printf( "], time=%g\n", time2-time1 ) ;
    }
}





