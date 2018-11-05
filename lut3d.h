#ifndef _LUT_3D_H_
#define _LUT_3D_H_

#ifndef _MAX_DRIVE
#define _MAX_DRIVE 3
#endif
#ifndef _MAX_FNAME
#define _MAX_FNAME 256
#endif
#ifndef _MAX_EXT
#define _MAX_EXT 256
#endif
#ifndef _MAX_DIR
#define _MAX_DIR 256
#endif
#ifdef _MSC_VER
#endif
#ifndef MIN
#define MIN(a, b)    ( (a) > (b) ? (b) : (a) )
#endif
#ifndef _NEAR
#define _NEAR(x)    ( (int) ( (x) + .5) )
#endif
#ifndef PREV
#define PREV(x)    ( (int) (x) )
#endif
#ifndef NEXT
#define NEXT(x)    (MIN( (int) (x) + 1, lut3d->lutsize - 1 ) )
#endif
#ifndef R
#define R    0
#endif
#ifndef G
#define G    1
#endif
#ifndef B
#define B    2
#endif
#ifndef A
#define A    3
#endif
#ifndef MAX_LEVEL
#define MAX_LEVEL 64
#endif
#ifndef NEXT_LINE
#define NEXT_LINE(loop_cond) do {                  \
        if ( !fgets( line, sizeof(line), f ) ) {            \
            printf( "Unexpected EOF\n" );    fclose( f );  if ( lut3d ) free( lut3d );          \
            return NULL;                 \
        }                            \
} while ( loop_cond )
#endif

#ifndef MAX_LINE_SIZE
#define MAX_LINE_SIZE 512
#endif

struct rgbvec {
    float r, g, b;
};

typedef struct LUT3DContext {
    unsigned char rgba_map[4];
    int step;
    struct rgbvec lut[MAX_LEVEL][MAX_LEVEL][MAX_LEVEL];
    int lutsize;
} LUT3DContext;

//only support cube file
LUT3DContext *lut3d_load(char *filename);
int interp_tetrahedral_uint8( LUT3DContext * lut3d, const unsigned char * indata, 
        unsigned char * outdata, int width, int height, int linesize);


#endif



