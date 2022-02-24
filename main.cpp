
#include <stdlib.h>
#include <stdio.h>

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>

#include <string.h>

#include <assert.h>


// importer per wavefront obj

struct RGB {
    float r,g,b;
} RGB;

struct MTL {
    struct RGB diff;
    struct RGB emit;
} MTL;

std::unordered_map<std::string,struct MTL> mtl_list;



void parse( FILE *f );  // fwd

void import_mtl( std::string &mtllib_path ){
    FILE *f;
    assert( f = fopen( mtllib_path.c_str(), "rb" ));
    std::cout << "parsing " << mtllib_path << std::endl;
    parse(f);   // usiamo lo stesso parser, i tokens son cmq univoci (ci han pensato)
    fclose(f);
    for( auto i=mtl_list.begin(); i!=mtl_list.end(); ++i ){
        std::cout << i->first << std::endl;
    }
}


char *tok( char *line = 0 ){
    // giusto per wrappare lo split sep 
    return strtok(line," \n");
}





void parse( FILE *f ){

    std::string mtllib_path;
    std::string obj_name;
    std::string usemtl;

    std::string mtl_name;
    struct MTL mtl;

    int face_id;
    int vert_id;
    int norm_id;
    int uv_id;

    bool store_mtl=false;

    while(1){
        // visto che obj non è freeform
        // processiamo per linea
        char line[256];
        if( 0 == fgets( line, sizeof(line), f ))break;

        // brucio i commenti
        char *c = strchr(line,'#');
        if(c)*c=0;  // termino la stringa

        // parse
        c = tok(line);
        if(!c)continue;

        std::string item = c;

        if( item == "newmtl" ){
            if(store_mtl) mtl_list[mtl_name]=mtl;
            assert(c=tok());
            mtl_name = c;
            store_mtl=true;
            std::cout << "mtl_name " << mtl_name << std::endl;
            continue;
        }

        if( item == "mtllib" ){
            assert(c=tok());
            mtllib_path = c;
            std::cout << "mtllib_path " << mtllib_path << std::endl;
            import_mtl(mtllib_path);
            continue;
        }

        if( item == "usemtl" ){
            assert(c=tok());
            usemtl = c;
            std::cout << "usemtl " << usemtl << std::endl;
            continue;
        }

        if( item == "Kd" ){
            mtl.diff.r = atof(tok());
            mtl.diff.g = atof(tok());
            mtl.diff.b = atof(tok());
            std::cout << "diff rgb " << mtl.diff.r << " " << mtl.diff.g << " " << mtl.diff.b << std::endl;
            continue;
        }

        if( item == "Ke" ){
            mtl.emit.r = atof(tok());
            mtl.emit.g = atof(tok());
            mtl.emit.b = atof(tok());
            std::cout << "emit rgb " << mtl.emit.r << " " << mtl.emit.g << " " << mtl.emit.b << std::endl;
            continue;
        }

        if( item == "vt" ){
            ++uv_id;
            float u = atof(tok());
            float v = atof(tok());
            printf("uv %d uv %f %f\n",uv_id,u,v);
            continue;
        }

        if( item == "vn" ){
            ++norm_id;
            float x = atof(tok());
            float y = atof(tok());
            float z = atof(tok());
            printf("norm %d xyz %f %f %f\n",norm_id,x,y,z);
            continue;
        }

        if( item == "v" ){
            ++vert_id;
            float x = atof(tok());
            float y = atof(tok());
            float z = atof(tok());
            printf("vert %d xyz %f %f %f\n",vert_id,x,y,z);
            continue;
        }

        if( item == "o" ){
            assert(c=tok());
            obj_name = c;
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
                char *t = tok();
                if(!t)break;
                std::string vtn = t;
                std::cout << vtn << " ";
            }
            std::cout << std::endl;
            continue;
        }

        std::cout << "* item " << item << " skippato" << std::endl;
    }

    if(store_mtl) mtl_list[mtl_name]=mtl;
}


void import_obj( char *path ){
    FILE *f;
    assert( f = fopen( path, "rb" ));
    std::cout << "parsing " << path << std::endl;
    parse(f);
    fclose(f);
}



int main( int argc , char *argv[] ){
    if(argc<2)return 1;
    import_obj(argv[1]);
    return 0;
}


