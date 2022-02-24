
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>

#include <string.h>

#include <assert.h>


// importer per wavefront obj





#define STREQ(A,B) (0==strcmp(A,B))


void parse( FILE *f ){

    std::string mtllib_path;
    std::string obj_name;
    std::string mtl;

    int face_id;
    int vert_id;
    int norm_id;
    int uv_id;

    while(1){
        char line[256];
        if( 0 == fgets( line, sizeof(line), f ))break;

        // brucio i commenti
        char *c = strchr(line,'#');
        if(c)*c=0;  // termino la stringa al primo commento, se c'è

        // linee vuote non le caghiamo
        if(line[0]==0)continue;

        // parse
        std::string item = strtok(line," \n");

        if( item == "mtllib" ){
            mtllib_path = strtok(0," \n");
            std::cout << "mtllib_path " << mtllib_path << std::endl;
            continue;
        }

        if( item == "usemtl" ){
            mtl = strtok(0," \n");
            std::cout << "mtl " << mtl << std::endl;
            continue;
        }

        if( item == "vt" ){
            ++uv_id;
            float u = atof(strtok(0," \n"));
            float v = atof(strtok(0," \n"));
            printf("uv %d uv %f %f\n",uv_id,u,v);
            continue;
        }

        if( item == "vn" ){
            ++norm_id;
            float x = atof(strtok(0," \n"));
            float y = atof(strtok(0," \n"));
            float z = atof(strtok(0," \n"));
            printf("norm %d xyz %f %f %f\n",norm_id,x,y,z);
            continue;
        }

        if( item == "v" ){
            ++vert_id;
            float x = atof(strtok(0," \n"));
            float y = atof(strtok(0," \n"));
            float z = atof(strtok(0," \n"));
            printf("vert %d xyz %f %f %f\n",vert_id,x,y,z);
            continue;
        }

        if( item == "o" ){
            obj_name = strtok(0," \n");
            std::cout << "obj_name " << obj_name << std::endl;
            face_id=0;
            vert_id=0;
            norm_id=0;
            uv_id=0;
            continue;
        }

        if( item == "f" ){
            // face def
            // f 1/1/1 2/2/1 3/3/1 4/4/1
            ++face_id;
            std::cout << "face " << face_id << " vtn ";
            while(1){
                char *t = strtok(0," \n");
                if(!t)break;
                std::string vtn = t;
                std::cout << vtn << " ";
            }
            std::cout << std::endl;
            continue;
        }

        std::cout << "* item " << item << " skippato" << std::endl;
    }
}



int main( int argc , char *argv[] ){
    if(argc<2)return 1;
    FILE *f;
    assert( f = fopen( argv[1], "rb" ));
    parse(f);
    fclose(f);
    return 0;
}
