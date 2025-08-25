
#include "tk_array.h"
#include "tk_slotlist.h"

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


    // Slotlist test
    tk::SlotList<int> table = tk::SlotList<int>( alloc, 256 );

    tk::Handle h0 = table.AddItem(  99 );
    tk::Handle h1 = table.AddItem( 100 );
    tk::Handle h2 = table.AddItem( 101 );

    printf("h0 lookup %d\n", table.Lookup( h0));
    printf("h1 lookup %d\n", table.Lookup( h1));
    printf("h2 lookup %d\n", table.Lookup( h2));

    printf("\n---\n\n");

    table.Update(h0, 5);

    printf("h0 maps to %d\n", table.Lookup(h0));
    printf("h1 maps to %d\n", table.Lookup(h1));
    printf("h2 maps to %d\n", table.Lookup(h2));
    printf("\n---\n\n");

    table.DropItem(h1);

    printf("h0 is %s\n", table.Validate(h0) ? "alive" : "dead");
    printf("h1 is %s\n", table.Validate(h1) ? "alive" : "dead");
    printf("h2 is %s\n", table.Validate(h2) ? "alive" : "dead");
    printf("\n---\n\n");

    tk::Handle h3 = table.AddItem(700);

    printf("h0 maps to %d\n", table.Lookup(h0));
    printf("h2 maps to %d\n", table.Lookup(h2));
    printf("h3 maps to %d\n", table.Lookup(h3));
    printf("\n---\n\n");

    printf("Now let us check out the internals of the Handle for h1 and h3...\n\n");
    printf("\th1 = { index : %d, version : %d }\n", (int)h1.u.index, (int)h1.u.version);
    printf("\th3 = { index : %d, version : %d }\n\n", (int)h3.u.index, (int)h3.u.version);
    printf("Notice that the index was re-used, but count incremented!\n");
    printf("This is how lifetime is tracked for unique elements while reusing the same memory.\n");
    printf("\n---\n\n");


    // Check that clear resets the freelist properly
    table.Clear();
    assert( table.CountUsage() == 0 );


    table.Free( alloc );
    return 1;
}