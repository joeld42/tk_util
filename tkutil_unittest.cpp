
#include "tk_util.h"
#include <stdio.h>


int main( int argc, const char **argv )
{
    tk::DefaultAllocator defaultAlloc;
    tk::IAllocator *alloc = &defaultAlloc;

    tk::Array<int> arrTest;
    arrTest.Add( 5, alloc );
    arrTest.Add( 6, alloc );
    arrTest.Add( 7, alloc );

    printf("Size %d, Capacity is %d\n", arrTest.Length(), arrTest.Capacity() );
    for (int i=0; i < arrTest.Length(); i++) {
        printf("Array %d = %d\n", i, arrTest._elems[i] );
    }

    return 1;
}