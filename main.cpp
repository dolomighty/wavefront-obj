

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

#include <vector>
#include <string>
#include <iostream>
#include <iomanip>
#include <unordered_map>



// importer per wavefront obj

struct RGB {
    float r,g,b;
};

struct XYZ {
    float x,y,z;
};




struct MTL {
    struct RGB diff;
    struct RGB emit;
};


std::unordered_map<std::string,struct MTL> mtl_list;
// le liste di vertici/uv/ecc sono globali nel file
// inoltre son 1-based, gestito
std::vector<struct XYZ> vert_list;



void out_xyz( struct XYZ &xyz ){
    std::cout 
    << "(" << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << xyz.x
    << " " << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << xyz.y
    << " " << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << xyz.z
    << ") ";
}

void out_rgb( struct RGB &rgb, float scale=1 ){
    std::cout 
    << "(" << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << scale*rgb.r
    << " " << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << scale*rgb.g
    << " " << std::fixed << std::setw(5) << std::setprecision(3) << std::setfill('0') << scale*rgb.b
    << ") ";
}




void parse( FILE *f );  // fwd

void import_mtl( std::string &mtllib_path ){
    FILE *f;
    assert( f = fopen( mtllib_path.c_str(), "rb" ));
    parse(f);   // usiamo lo stesso parser, i tokens son cmq univoci (ci han pensato)
    fclose(f);
}


char *tok( char *line = 0 ){
    // giusto per wrappare lo split sep 
    return strtok(line," \n");
}



void out_raw_tri( struct MTL &mtl ){
    // ogni token letto descrive un vertice
    // "vert_id/uv_id/norm_id"
    // non sappiamo in anticipop quanti vertici ci sono, ma sono minimo 3
    // vogliamo cmq esportare triangoli

    struct VUN {
        int v,u,n;
    };

    struct VUN vun[3];
    int i=0;

    while(1){
        char *t = tok();
        if(!t)break;
        assert( 3 == sscanf( t, "%d/%d/%d", &vun[i].v, &vun[i].u, &vun[i].n ));
        if(i<2){ ++i; continue; }

        out_xyz( vert_list[vun[0].v]);
        out_xyz( vert_list[vun[1].v]);
        out_xyz( vert_list[vun[2].v]);
        out_rgb( mtl.diff );
        out_rgb( mtl.emit, 1000 );
        std::cout << std::endl;

        vun[1] = vun[2];
    }
}



void parse( FILE *f ){

    std::string usemtl;

    // usate sono nel parsing dell'mtl
    std::string mtl_name;
    struct MTL mtl;


    bool store_mtl = false;

    while(1){
        // visto che obj non è freeform
        // processiamo per linea
        char line[256];
        if(!fgets( line, sizeof(line), f ))break;

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
            continue;
        }

        if( item == "mtllib" ){
            assert(c=tok());
            std::string mtllib_path = c;
            import_mtl(mtllib_path);
            continue;
        }

        if( item == "usemtl" ){
            assert(c=tok());
            usemtl = c;
            continue;
        }

        if( item == "Kd" ){
            mtl.diff.r = atof(tok());
            mtl.diff.g = atof(tok());
            mtl.diff.b = atof(tok());
            continue;
        }

        if( item == "Ke" ){
            mtl.emit.r = atof(tok());
            mtl.emit.g = atof(tok());
            mtl.emit.b = atof(tok());
            continue;
        }

        if( item == "vt" ){
            float u = atof(tok());
            float v = atof(tok());
            continue;
        }

        if( item == "vn" ){
            float x = atof(tok());
            float y = atof(tok());
            float z = atof(tok());
            continue;
        }

        if( item == "v" ){
            struct XYZ xyz;
            xyz.x = atof(tok());
            xyz.y = atof(tok());
            xyz.z = atof(tok());
            vert_list.push_back(xyz);
            continue;
        }

        if( item == "o" ){
            assert(c=tok());
            std::string obj_name = c;
            // ripartiamo con una lista di vertici
            // obj è 1-based, inseriamo un dummy vert per non far aritmetica dopo
            continue;
        }

        if( item == "f" ){
            out_raw_tri( mtl_list[usemtl]);
            continue;
        }

//        std::cout << "* item " << item << " skippato" << std::endl;
    }

    if(store_mtl) mtl_list[mtl_name]=mtl;
}


void import_obj( char *path ){
    FILE *f;
    assert( f = fopen( path, "rb" ));
    parse(f);
    fclose(f);
}



int main( int argc , char *argv[] ){
    if(argc<2)return 1;
    // nel .obj gli indici son tutti 1-based, quindi aggiungo un dummy
    // per evitare di fare aritmetica inelegante dopo
    struct XYZ dummy;
    vert_list.push_back(dummy);
    import_obj(argv[1]);
    return 0;
}


