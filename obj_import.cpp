

//
// importer per wavefront obj
//



#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#undef NDEBUG
#include <assert.h>

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>





// non voglio esportare strutture
// quindi uso solo float arrays

// HEADERBEG
#define TRI_CB_DEF(NAME) void NAME( float a[], float b[], float c[], float d[], float e[] )
// HEADEREND




struct V3f {
    float v[3];
    // get
    auto x(){ return v[0]; }
    auto y(){ return v[1]; }
    auto z(){ return v[2]; }
    auto r(){ return v[0]; }
    auto g(){ return v[1]; }
    auto b(){ return v[2]; }
    // set
    auto x( float x ){ return v[0]=x; }
    auto y( float x ){ return v[1]=x; }
    auto z( float x ){ return v[2]=x; }
    auto r( float x ){ return v[0]=x; }
    auto g( float x ){ return v[1]=x; }
    auto b( float x ){ return v[2]=x; }
};


struct MTL {
    struct V3f diff;
    struct V3f emit;
};

std::unordered_map<std::string,struct MTL> mtl_list;
// le liste di vertici/uv/ecc sono globali nel file
// inoltre son 1-based, gestito
std::vector<struct V3f> vert_list;

TRI_CB_DEF((*global_tri_cb));



static void parse( FILE *f );  // fwd

static void import_mtl( std::string &mtllib_path ){
    FILE *f;
    assert( f = fopen( mtllib_path.c_str(), "rb" ));
    parse(f);   // usiamo lo stesso parser, i tokens son cmq univoci (ci han pensato)
    fclose(f);
}


static char *tok( char *line = 0 ){
    // giusto per wrappare lo split sep 
    return strtok(line," \n");
}



static void out_raw_tri( struct MTL &mtl ){
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
        global_tri_cb(
            vert_list[vun[0].v].v,
            vert_list[vun[1].v].v,
            vert_list[vun[2].v].v,
            mtl.diff.v,
            mtl.emit.v
        );
        vun[1] = vun[2];
    }
}



static void parse( FILE *f ){

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
        if(c)*c=0;  // termino la stringa sul #

        // parse
        c = tok(line);
        if(!c)continue;

        std::string item = c;
//        std::cout << item << std::endl;

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
            mtl.diff.r(atof(tok()));
            mtl.diff.g(atof(tok()));
            mtl.diff.b(atof(tok()));
            continue;
        }

        if( item == "Ke" ){
            mtl.emit.r(atof(tok()));
            mtl.emit.g(atof(tok()));
            mtl.emit.b(atof(tok()));
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
            struct V3f xyz;
            xyz.x(atof(tok()));
            xyz.y(atof(tok()));
            xyz.z(atof(tok()));
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



void obj_import( char *path, TRI_CB_DEF((*tri_cb))){    // HEADER
    assert( path );
    assert( tri_cb );

    global_tri_cb = tri_cb;

    FILE *f;
    assert( f = fopen( path, "rb" ));

    // nel .obj gli indici son tutti 1-based, quindi aggiungo un dummy
    // per evitare di fare brutta aritmetica dopo
    struct V3f dummy;
    vert_list.push_back(dummy);

    parse(f);

    mtl_list.clear();
    vert_list.clear();

    fclose(f);
}


