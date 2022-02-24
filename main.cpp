

#include <assert.h>
#include "dyn/obj_import.h"
#include <iostream>



static TRI_CB_DEF( tri_cb ){
    std::cout 
        <<   "(" << a[0] << " " << a[1] << " " << a[2]
        << ") (" << b[0] << " " << b[1] << " " << b[2]
        << ") (" << c[0] << " " << c[1] << " " << c[2]
        << ") (" << d[0] << " " << d[1] << " " << d[2]
        << ") (" << e[0] << " " << e[1] << " " << e[2]
        << ")" << std::endl;
}



int main( int argc , char *argv[] ){
    assert( argc >= 2 );
    obj_import( argv[1], tri_cb );
    return 0;
}

