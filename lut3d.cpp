/*
 * Copyright (c) 2013 Clément Bœsch
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/**
 * 3D Lookup table filter
 */
#include "browse.h"
#include <stdio.h> 

#define USE_SHELL_OPEN

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION


#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image.h"
#include "stb_image_write.h"
#include "lut3d.h"
/* ref:https://github.com/nothings/stb/blob/master/stb_image.h */

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include "timing.h"
#include <stdint.h>
#include <assert.h>

enum interp_mode {
    INTERPOLATE_NEAREST,
    INTERPOLATE_TRILINEAR,
    INTERPOLATE_TETRAHEDRAL,
    NB_INTERP_MODE
};




/* 3D LUT don't often go up to level 32 */
#ifdef _MSC_VER
int strcasecmp(const char *s1, char *s2) {
    while (toupper((unsigned char)*s1) == toupper((unsigned char)*s2++))
        if (*s1++ == 0x00)
            return (0);
    return (toupper((unsigned char)*s1) - toupper((unsigned char) *--s2));
}
#endif

// static inline float lerpf(float v0, float v1, float f) {
//     return (v0 + (v1 - v0) * f);
// }

// static inline struct rgbvec lerp(const struct rgbvec *v0, const struct rgbvec *v1, float f) {
//     static int i6 = 0;
//     if(i6 == 0)
//     {
//         printf("in func:%s line:%d\n", __FILE__,__LINE__); 
//         i6++;
//     }
//     struct rgbvec v = {
//             lerpf(v0->r, v1->r, f), lerpf(v0->g, v1->g, f), lerpf(v0->b, v1->b, f)
//     };
//     return (v);
// }


/**
 * Get the nearest defined point
 */
// static inline struct rgbvec interp_nearest(const LUT3DContext *lut3d,
//                                            const struct rgbvec *s) {

//     //printf("in interp_nearest\n");
//     return (lut3d->lut[_NEAR(s->r)][_NEAR(s->g)][_NEAR(s->b)]);
// }


// /**
//  * Interpolate using the 8 vertices of a cube
//  * @see https://en.wikipedia.org/wiki/Trilinear_interpolation
//  */
// static inline struct rgbvec interp_trilinear(const LUT3DContext *lut3d,
//                                              const struct rgbvec *s) {

//     //printf("in interp_trilinear\n");
//     const int prev[] = {PREV(s->r), PREV(s->g), PREV(s->b)}; //临近前像素点坐标位置
//     const int next[] = {NEXT(s->r), NEXT(s->g), NEXT(s->b)}; //临近后像素点坐标位置
//     const struct rgbvec d = {s->r - prev[0], s->g - prev[1], s->b - prev[2]}; //小数部分
//     const struct rgbvec c000 = lut3d->lut[prev[0]][prev[1]][prev[2]];
//     const struct rgbvec c001 = lut3d->lut[prev[0]][prev[1]][next[2]];
//     const struct rgbvec c010 = lut3d->lut[prev[0]][next[1]][prev[2]];
//     const struct rgbvec c011 = lut3d->lut[prev[0]][next[1]][next[2]];
//     const struct rgbvec c100 = lut3d->lut[next[0]][prev[1]][prev[2]];
//     const struct rgbvec c101 = lut3d->lut[next[0]][prev[1]][next[2]];
//     const struct rgbvec c110 = lut3d->lut[next[0]][next[1]][prev[2]];
//     const struct rgbvec c111 = lut3d->lut[next[0]][next[1]][next[2]];
//     const struct rgbvec c00 = lerp(&c000, &c100, d.r);
//     const struct rgbvec c10 = lerp(&c010, &c110, d.r);
//     const struct rgbvec c01 = lerp(&c001, &c101, d.r);
//     const struct rgbvec c11 = lerp(&c011, &c111, d.r);
//     const struct rgbvec c0 = lerp(&c00, &c10, d.g);
//     const struct rgbvec c1 = lerp(&c01, &c11, d.g);
//     const struct rgbvec c = lerp(&c0, &c1, d.b);
//     return (c);
// }


/**
 * Tetrahedral interpolation. Based on code found in Truelight Software Library paper.
 * @see http://www.filmlight.ltd.uk/pdf/whitepapers/FL-TL-TN-0057-SoftwareLib.pdf
 */



/**
 * Locale-independent conversion of ASCII isspace.
 */



/**
 * Clip a signed integer value into the 0-65535 range.
 * @param a value to clip
 * @return clipped value
 */
static uint16_t clip_uint16(int a) {    
    printf("in func:%s line:%d\n", __FILE__,__LINE__); 
    if (a & (~0xFFFF))
        return ((~a) >> 31);
    else return (a);
}




static unsigned clip_uintp2(int a, int p) {
    printf("in func:%s line:%d\n", __FILE__,__LINE__); 
    if (a & ~((1 << p) - 1))
        return (-a >> 31 & ((1 << p) - 1));
    else return (a);
}


// #define DEFINE_INTERP_FUNC_PLANAR(name, nbits, depth)                             \
//     static int interp_ ## nbits ## _ ## name ## _p ## depth( const LUT3DContext * lut3d, uint8_t * indata_g, uint8_t * indata_b, uint8_t * indata_r, uint8_t * indata_a, uint8_t * outdata_g, uint8_t * outdata_b, uint8_t * outdata_r, uint8_t * outdata_a, int width, int height, int linesize ) \
//     {                                                       \
//         int        x, y;                                             \
//         int        direct        = (outdata_g == indata_g);                                    \
//         uint8_t        *grow        = outdata_g  ;                   \
//         uint8_t        *brow        = outdata_b  ;                   \
//         uint8_t        *rrow        = outdata_r  ;                   \
//         uint8_t        *arow        = outdata_a  ;                   \
//         const uint8_t    *srcgrow    = indata_g  ;               \
//         const uint8_t    *srcbrow    = indata_b  ;               \
//         const uint8_t    *srcrrow    = indata_r  ;               \
//         const uint8_t    *srcarow    = indata_a  ;               \
//         const float    scale        = (1.f / ( (1 << (depth) ) - 1) ) * (lut3d->lutsize - 1);                \
//         for ( y = 0; y < height; y++ ) {                            \
//             uint ## nbits ## _t * dstg    = (uint ## nbits ## _t *)grow;                             \
//             uint ## nbits ## _t * dstb    = (uint ## nbits ## _t *)brow;                             \
//             uint ## nbits ## _t * dstr    = (uint ## nbits ## _t *)rrow;                             \
//             uint ## nbits ## _t * dsta    = (uint ## nbits ## _t *)arow;                             \
//             const    uint ## nbits ## _t *srcg = (const uint ## nbits ## _t *)srcgrow;                 \
//             const    uint ## nbits ## _t *srcb = (const uint ## nbits ## _t *)srcbrow;                 \
//             const    uint ## nbits ## _t *srcr = (const uint ## nbits ## _t *)srcrrow;                 \
//             const    uint ## nbits ## _t *srca = (const uint ## nbits ## _t *)srcarow;                 \
//             for ( x = 0; x < width; x++ ) {                                     \
//                 const struct rgbvec    scaled_rgb = { srcr[x] * scale,                        \
//                                        srcg[x] * scale,                        \
//                                        srcb[x] * scale };                     \
//                 struct rgbvec        vec = interp_ ## name( lut3d, &scaled_rgb );                     \
//                 dstr[x] = clip_uintp2( vec.r * (float) ( (1 << (depth) ) - 1), depth );                 \
//                 dstg[x] = clip_uintp2( vec.g * (float) ( (1 << (depth) ) - 1), depth );                 \
//                 dstb[x] = clip_uintp2( vec.b * (float) ( (1 << (depth) ) - 1), depth );                 \
//                 if ( !direct && linesize )                                  \
//                     dsta[x] = srca[x];                                       \
//             }                                                        \
//             grow    += linesize;                                      \
//             brow    += linesize;                                       \
//             rrow    += linesize;                                       \
//             arow    += linesize;                                        \
//             srcgrow += linesize;                                     \
//             srcbrow += linesize;                                     \
//             srcrrow += linesize;                                      \
//             srcarow += linesize;                                     \
//         }                                                        \
//         return 0;                                               \
//     }

// DEFINE_INTERP_FUNC_PLANAR(nearest, 8, 8)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 8, 8)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 8, 8)

// DEFINE_INTERP_FUNC_PLANAR(nearest, 16, 9)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 16, 9)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 16, 9)

// DEFINE_INTERP_FUNC_PLANAR(nearest, 16, 10)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 16, 10)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 16, 10)

// DEFINE_INTERP_FUNC_PLANAR(nearest, 16, 12)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 16, 12)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 16, 12)

// DEFINE_INTERP_FUNC_PLANAR(nearest, 16, 14)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 16, 14)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 16, 14)

// DEFINE_INTERP_FUNC_PLANAR(nearest, 16, 16)

// DEFINE_INTERP_FUNC_PLANAR(trilinear, 16, 16)

// DEFINE_INTERP_FUNC_PLANAR(tetrahedral, 16, 16)

// #define DEFINE_INTERP_FUNC(name, nbits)                                  \
//     static int interp_ ## nbits ## _ ## name( LUT3DContext * lut3d, const uint8_t * indata, uint8_t * outdata, int width, int height, int linesize )     \
//     {                                                    \
//         int        x, y;                                                 \
//         const int    direct    = outdata == indata;                                       \
//         const int    step    = lut3d->step;                                     \
//         const uint8_t    r    = lut3d->rgba_map[R];                                \
//         const uint8_t    g    = lut3d->rgba_map[G];                                \
//         const uint8_t    b    = lut3d->rgba_map[B];                                \
//         const uint8_t    a    = lut3d->rgba_map[A];                                \
//         uint8_t        *dstrow = outdata;                \
//         const uint8_t    *srcrow = indata;               \
//         const float    scale    = (1.f / ( (1 << nbits) - 1) ) * (lut3d->lutsize - 1);                  \
//                                                     \
//         for ( y = 0; y < height; y++ ) {                             \
//             uint ## nbits ## _t * dst = (uint ## nbits ## _t *)dstrow;                         \
//             const uint ## nbits ## _t *src = (const uint ## nbits ## _t *)srcrow;                    \
//             for ( x = 0; x < width * step; x += step ) {                          \
//                 const struct rgbvec    scaled_rgb = { src[x + r] * scale,                     \
//                                        src[x + g] * scale,                     \
//                                        src[x + b] * scale };                      \
//                 struct rgbvec        vec = interp_ ## name( lut3d, &scaled_rgb );                      \
//                 dst[x + r]    = clip_uint ## nbits( vec.r * (float) ( (1 << nbits) - 1) );              \
//                 dst[x + g]    = clip_uint ## nbits( vec.g * (float) ( (1 << nbits) - 1) );              \
//                 dst[x + b]    = clip_uint ## nbits( vec.b * (float) ( (1 << nbits) - 1) );              \
//                 if ( !direct && step == 4 )                                  \
//                     dst[x + a] = src[x + a];                                \
//             }                                                \
//             dstrow    += linesize;                                     \
//             srcrow    += linesize;                                     \
//         }                                                \
//         return 0;                                            \
//     }



// DEFINE_INTERP_FUNC(nearest, 8)

// DEFINE_INTERP_FUNC(trilinear, 8)

// DEFINE_INTERP_FUNC(tetrahedral, 8)

// DEFINE_INTERP_FUNC(nearest, 16)

// DEFINE_INTERP_FUNC(trilinear, 16)

// DEFINE_INTERP_FUNC(tetrahedral, 16)



/* Basically r g and b float values on each line, with a facultative 3DLUTSIZE
 * directive; seems to be generated by Davinci */
// LUT3DContext *parse_dat(char *filename) {
//     FILE *f = fopen(filename, "r");
//     if (f == NULL) return NULL;
//     LUT3DContext *lut3d = NULL;
//     char line[MAX_LINE_SIZE];
//     int i, j, k, size;

//     int lutsize = size = 33;

//     NEXT_LINE(skip_line(line));
//     if (!strncmp(line, "3DLUTSIZE ", 10)) {
//         size = strtol(line + 10, NULL, 0);
//         if (size < 2 || size > MAX_LEVEL) {
//             printf("Too large or invalid 3D LUT size\n");
//             fclose(f);
//             return (NULL);
//         }
//         lutsize = size;
//         NEXT_LINE(skip_line(line));
//     }
//     if (size != 0 && lut3d == NULL) {
//         lut3d = (LUT3DContext *) calloc(1, sizeof(LUT3DContext));
//     }
//     lut3d->lutsize = lutsize;
//     for (k = 0; k < size; k++) {
//         for (j = 0; j < size; j++) {
//             for (i = 0; i < size; i++) {
//                 struct rgbvec *vec = &lut3d->lut[k][j][i];
//                 if (k != 0 || j != 0 || i != 0)
//                     NEXT_LINE(skip_line(line));
//                 if (sscanf(line, "%f %f %f", &vec->r, &vec->g, &vec->b) != 3) {
//                     fclose(f);
//                     free(lut3d);
//                     return (NULL);
//                 }
//             }
//         }
//     }
//     fclose(f);
//     return (lut3d);
// }





/* Assume 17x17x17 LUT with a 16-bit depth   */
// LUT3DContext *parse_3dl(char *filename) {
//     printf("in parse_3dl\n");
//     FILE *f = fopen(filename, "r");
//     if (f == NULL) return NULL;
//     char line[MAX_LINE_SIZE];
//     int i, j, k;
//     const int size = 17;
//     const float scale = 16 * 16 * 16;

//     int lutsize = size;
//     LUT3DContext *lut3d = (LUT3DContext *) calloc(1, sizeof(LUT3DContext));

//     lut3d->lutsize = lutsize;
//     NEXT_LINE(skip_line(line));
//     for (k = 0; k < size; k++) {
//         for (j = 0; j < size; j++) {
//             for (i = 0; i < size; i++) {
//                 int r, g, b;
//                 struct rgbvec *vec = &lut3d->lut[k][j][i];

//                 NEXT_LINE(skip_line(line));
//                 if (sscanf(line, "%d %d %d", &r, &g, &b) != 3) {
//                     fclose(f);
//                     free(lut3d);
//                     return (NULL);
//                 }
//                 vec->r = r / scale;
//                 vec->g = g / scale;
//                 vec->b = b / scale;
//             }
//         }
//     }
//     fclose(f);
//     return (lut3d);
// }


/* Pandora format */
//LUT3DContext *parse_m3d(char *filename) {
//     printf("in parse_m3d\n");
//     FILE *f = fopen(filename, "r");
//     if (f == NULL) return NULL;
//     float scale;
//     int i, j, k, size, in = -1, out = -1;
//     char line[MAX_LINE_SIZE];
//     uint8_t rgb_map[3] = {0, 1, 2};

//     while (fgets(line, sizeof(line), f)) {
//         if (!strncmp(line, "in", 2))
//             in = strtol(line + 2, NULL, 0);
//         else if (!strncmp(line, "out", 3))
//             out = strtol(line + 3, NULL, 0);
//         else if (!strncmp(line, "values", 6)) {
//             const char *p = line + 6;
// #define SET_COLOR(id) do {              \
//         while ( _isspace( *p ) )          \
//             p++;                    \
//         switch ( *p ) {                  \
//         case 'r': rgb_map[id]    = 0; break;      \
//         case 'g': rgb_map[id]    = 1; break;      \
//         case 'b': rgb_map[id]    = 2; break;      \
//         }                    \
//         while ( *p && !_isspace( *p ) )          \
//             p++;                    \
// } \
//     while ( 0 )
//             SET_COLOR(0);
//             SET_COLOR(1);
//             SET_COLOR(2);
//             break;
//         }
//     }

//     if (in == -1 || out == -1) {
//         printf("in and out must be defined\n");
//         fclose(f);
//         return (NULL);
//     }
//     if (in < 2 || out < 2 ||
//         in > MAX_LEVEL * MAX_LEVEL * MAX_LEVEL ||
//         out > MAX_LEVEL * MAX_LEVEL * MAX_LEVEL) {
//         printf("invalid in (%d) or out (%d)\n", in, out);
//         fclose(f);
//         return (NULL);
//     }
//     for (size = 1; size * size * size < in; size++);
//     {}
//     int lutsize = size;
//     scale = 1.f / (out - 1);
//     LUT3DContext *lut3d = NULL;
//     if (size != 0) {
//         lut3d = (LUT3DContext *) calloc(1, sizeof(LUT3DContext));
//     }
//     lut3d->lutsize = lutsize;
//     for (k = 0; k < size; k++) {
//         for (j = 0; j < size; j++) {
//             for (i = 0; i < size; i++) {
//                 struct rgbvec *vec = &lut3d->lut[k][j][i];

//                 float val[3];
//                 NEXT_LINE(0);
//                 if (sscanf(line, "%f %f %f", val, val + 1, val + 2) != 3) {
//                     fclose(f);
//                     free(lut3d);
//                     return (NULL);
//                 }
//                 vec->r = val[rgb_map[0]] * scale;
//                 vec->g = val[rgb_map[1]] * scale;
//                 vec->b = val[rgb_map[2]] * scale;
//             }
//         }
//     }
//     fclose(f);
//     return (lut3d);
// }



typedef int (action_planar_func)(const LUT3DContext *lut3d, uint8_t *indata_g, uint8_t *indata_b, uint8_t *indata_r,
                                 uint8_t *indata_a, uint8_t *outdata_g, uint8_t *outdata_b, uint8_t *outdata_r,
                                 uint8_t *outdata_a, int width, int height, int linesize);

static int apply_planar_lut(char *filename, uint8_t *indata_g, uint8_t *indata_b, uint8_t *indata_r,
                            uint8_t *indata_a, uint8_t *outdata_g, uint8_t *outdata_b, uint8_t *outdata_r,
                            uint8_t *outdata_a, int width, int height, int linesize, int depth, int interpolation
) {
//     action_planar_func *interp_func = 0;

//     LUT3DContext *lut3d = lut3d_load(filename);
//     if (lut3d == NULL)
//         return (-1);
//     lut3d->step = depth;
//     int planar = 1;     \

// #define SET_PLANAR_FUNC(name) do {                     \
//         if ( planar ) {                              \
//             switch ( depth ) {                      \
//             case  8: interp_func    = interp_8_ ## name ## _p8;   break; \
//             case  9: interp_func    = interp_16_ ## name ## _p9;  break; \
//             case 10: interp_func    = interp_16_ ## name ## _p10; break; \
//             case 12: interp_func    = interp_16_ ## name ## _p12; break; \
//             case 14: interp_func    = interp_16_ ## name ## _p14; break; \
//             case 16: interp_func    = interp_16_ ## name ## _p16; break; \
//             }                            \
//         }      \
// } while ( 0 )

//     switch (interpolation) {
//         case INTERPOLATE_NEAREST:
//             SET_PLANAR_FUNC(nearest);
//             break;
//         case INTERPOLATE_TRILINEAR:
//             SET_PLANAR_FUNC(trilinear);
//             break;
//         case INTERPOLATE_TETRAHEDRAL:
//             SET_PLANAR_FUNC(tetrahedral);
//             break;
//         default:
//             assert(0);
//     }
//     interp_func(lut3d, indata_g, indata_b, indata_r,
//                 indata_a, outdata_g, outdata_b, outdata_r,
//                 outdata_a, width, height, linesize);
    return (0);
}


typedef int (action_func)(LUT3DContext *lut3d, const uint8_t *indata, uint8_t *outdata, int width, int height,
                          int linesize);


static int
apply_lut(char *filename, const uint8_t *indata, uint8_t *outdata, int width, int height, int linesize, int depth,
          int interpolation, int is16bit) {
    action_func *interp_func = 0;
    LUT3DContext *lut3d = lut3d_load(filename);

    if (lut3d == NULL)
        return (-1);
    lut3d->rgba_map[0] = 0;
    lut3d->rgba_map[1] = 1;
    lut3d->rgba_map[2] = 2;
    lut3d->rgba_map[3] = 3;
    lut3d->step = depth;

    printf("interp_func\n");
    printf("width:%d height:%d lineSize:%d\n", width, height, linesize);
    interp_tetrahedral_uint8(lut3d, indata, outdata, width, height, linesize);
    free(lut3d);
    return (0);
}


char saveFile[1024];

unsigned char *loadImage(const char *filename, int *Width, int *Height, int *Channels) {
    return (stbi_load(filename, Width, Height, Channels, 0));
}


void saveImage(const char *filename, int Width, int Height, int Channels, unsigned char *Output) {
    memcpy(saveFile + strlen(saveFile), filename, strlen(filename));
    *(saveFile + strlen(saveFile) + 1) = 0;
    if (!stbi_write_jpg(saveFile, Width, Height, Channels, Output, 100)) {
        fprintf(stderr, "save JPEG fail.\n");
        return;
    }

#ifdef USE_SHELL_OPEN
    browse(saveFile);
#endif
}


void splitpath(const char *path, char *drv, char *dir, char *name, char *ext) {
    static int i4 = 0;
    if(i4 == 0)
    {
        printf("in func:%s line:%d\n", __FILE__,__LINE__); 
        i4++;
    }
    const char *end;
    const char *p;
    const char *s;
    if (path[0] && path[1] == ':') {
        if (drv) {
            *drv++ = *path++;
            *drv++ = *path++;
            *drv = '\0';
        }
    } else if (drv)
        *drv = '\0';
    for (end = path; *end && *end != ':';)
        end++;
    for (p = end; p > path && *--p != '\\' && *p != '/';)
        if (*p == '.') {
            end = p;
            break;
        }
    if (ext)
        for (s = end; (*ext = *s++);)
            ext++;
    for (p = end; p > path;)
        if (*--p == '\\' || *p == '/') {
            p++;
            break;
        }
    if (name) {
        for (s = p; s < end;)
            *name++ = *s++;
        *name = '\0';
    }
    if (dir) {
        for (s = path; s < p;)
            *dir++ = *s++;
        *dir = '\0';
    }
}

void getCurrentFilePath(const char *filePath, char *saveFile) {
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname[_MAX_FNAME];
    char ext[_MAX_EXT];
    splitpath(filePath, drive, dir, fname, ext);
    size_t n = strlen(filePath);
    memcpy(saveFile, filePath, n);
    char *cur_saveFile = saveFile + (n - strlen(ext));
    cur_saveFile[0] = '_';
    cur_saveFile[1] = 0;
}


int main(int argc, char **argv) {
    printf("lut 3d demo\n ");
    printf("blog:http://cpuimage.cnblogs.com/ \n ");

    if (argc < 3) {
        printf("usage: %s 3dlut image \n ", argv[0]);
        printf("eg: %s 3DLUT d:\\image.jpg \n ", argv[0]);

        return (0);
    }
    char *lutfile = argv[1];
    char *szfile = argv[2];

    getCurrentFilePath(szfile, saveFile);

    int Width = 0;
    int Height = 0;
    int Channels = 0;
    unsigned char *inputImage = NULL;

    double startTime = now();
    inputImage = loadImage(szfile, &Width, &Height, &Channels);

    double nLoadTime = calcElapsed(startTime, now());
    printf("load time: %d ms.\n ", (int) (nLoadTime * 1000));
    if ((Channels != 0) && (Width != 0) && (Height != 0)) {
        unsigned char *outputImg = (unsigned char *) stbi__malloc(Width * Channels * Height * sizeof(unsigned char));
        if (inputImage) {
            memcpy(outputImg, inputImage, Width * Channels * Height);
        } else {
            printf("load: %s fail!\n ", szfile);
        }
        startTime = now();

        int is16bit = 0;
        //  INTERPOLATE_NEAREST
        //	INTERPOLATE_TRILINEAR
        //	INTERPOLATE_TETRAHEDRAL
        int interp_mode = INTERPOLATE_TETRAHEDRAL;
        apply_lut(lutfile, inputImage, outputImg, Width, Height, Width * Channels, Channels, interp_mode,
                  is16bit);
        double nProcessTime = calcElapsed(startTime, now());

        printf("process time: %d ms.\n ", (int) (nProcessTime * 1000));

        startTime = now();

        saveImage("_done.jpg", Width, Height, Channels, outputImg);
        double nSaveTime = calcElapsed(startTime, now());

        printf("save time: %d ms.\n ", (int) (nSaveTime * 1000));

        if (outputImg) {
            stbi_image_free(outputImg);
        }

        if (inputImage) {
            stbi_image_free(inputImage);
        }
    } else {
        printf("load: %s fail!\n", szfile);
    }

    getchar();
    printf("press any key to exit. \n");

    return (EXIT_SUCCESS);
}