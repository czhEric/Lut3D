#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lut3d.h"

int _isspace(int c) {
    static int i0 = 0;
    if(i0 == 0)
    {
        printf("in func:%s line:%d\n", __FILE__,__LINE__); 
        i0++;
    }
    return (c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' ||
            c == '\v');
}

static int skip_line(const char *p) {
    
    while (*p && _isspace(*p))
        p++;
    return (!*p || *p == '#');
}

LUT3DContext *parse_cube(char *filename) {
    FILE *f = fopen(filename, "r");
    if (f == NULL) return NULL;
    char line[MAX_LINE_SIZE];
    float min[3] = {0.0, 0.0, 0.0};
    float max[3] = {1.0, 1.0, 1.0};
    int lutsize = 0;
    LUT3DContext *lut3d = NULL;
    while (fgets(line, sizeof(line), f)) {
        if (!strncmp(line, "LUT_3D_SIZE ", 12)) {
            int i, j, k;
            const int size = strtol(line + 12, NULL, 0);
            if (size < 2 || size > MAX_LEVEL) {
                printf("Too large or invalid 3D LUT size\n");
                fclose(f);
                return (NULL);
            }
            printf("size:%d\n",size);
            lutsize = size;
            if (size != 0 && lut3d == NULL) {
                lut3d = (LUT3DContext *) calloc(1, sizeof(LUT3DContext));
            }
            lut3d->lutsize = lutsize;
            for (k = 0; k < size; k++) {
                for (j = 0; j < size; j++) {
                    for (i = 0; i < size; i++) {
                        struct rgbvec *vec = &lut3d->lut[i][j][k];

                        do {
                            try_again:
                            NEXT_LINE(0);
                            if (!strncmp(line, "DOMAIN_", 7)) {
                                float *vals = NULL;
                                if (!strncmp(line + 7, "MIN ", 4))
                                    vals = min;
                                else if (!strncmp(line + 7, "MAX ", 4))
                                    vals = max;
                                if (!vals) {
                                    fclose(f);
                                    free(lut3d);
                                    return (NULL);
                                }

                                sscanf(line + 11, "%f %f %f", vals, vals + 1, vals + 2);
                                //printf("min: %f %f %f | max: %f %f %f\n", 	min[0], min[1], min[2], max[0], max[1], max[2]);
                                goto try_again;
                            }
                        } while (skip_line(line));
                        if (sscanf(line, "%f %f %f", &vec->r, &vec->g, &vec->b) != 3) {
                            fclose(f);
                            free(lut3d);
                            return (NULL);
                        }
                        vec->r *= max[0] - min[0];
                        vec->g *= max[1] - min[1];
                        vec->b *= max[2] - min[2];
                    }
                }
            }
            break;
        }
    }
    fclose(f);
    return (lut3d);
}


LUT3DContext *lut3d_load(char *filename) {
    static int i2 = 0;
    if( 0 == i2 )
    {
        printf("in func:%s line:%d\n", __FILE__,__LINE__); 
        i2++;
    }
    int ret = 0;
    const char *ext;
    if (!filename) {
        return (0);
    }
    LUT3DContext *lut3d = NULL;

    ext = strrchr(filename, '.');
    if (!ext) {
        printf("Unable to guess the format from the extension\n");
        goto end;
    }
    ext++;

    if (!strcasecmp(ext, "cube")) {
        printf("cube file\n");
        lut3d = parse_cube(filename);
    }
    if (!ret && !lut3d->lutsize) {
        printf("3D LUT is empty\n");
    }
    end:
    return (lut3d);
}




/*
* @see https://www.filmlight.ltd.uk/pdf/whitepapers/FL-TL-TN-0057-SoftwareLib.pdf
*/
struct rgbvec interp_tetrahedral(const LUT3DContext *lut3d,
                                const struct rgbvec *s) {

    static int i3 = 0;
    if(i3 == 0)
    {
        printf("in func:%s line:%d\n", __FILE__,__LINE__); 
        i3++;
    }
    //printf("interp_tetrahedral\n");
    const int prev[] = {PREV(s->r), PREV(s->g), PREV(s->b)};
    const int next[] = {NEXT(s->r), NEXT(s->g), NEXT(s->b)};
    const struct rgbvec d = {s->r - prev[0], s->g - prev[1], s->b - prev[2]};
    const struct rgbvec c000 = lut3d->lut[prev[0]][prev[1]][prev[2]];
    const struct rgbvec c111 = lut3d->lut[next[0]][next[1]][next[2]];
    struct rgbvec c;
    if (d.r > d.g) {
        if (d.g > d.b) {
            const struct rgbvec c100 = lut3d->lut[next[0]][prev[1]][prev[2]];
            const struct rgbvec c110 = lut3d->lut[next[0]][next[1]][prev[2]];
            c.r = (1 - d.r) * c000.r + (d.r - d.g) * c100.r + (d.g - d.b) * c110.r + (d.b) * c111.r;
            c.g = (1 - d.r) * c000.g + (d.r - d.g) * c100.g + (d.g - d.b) * c110.g + (d.b) * c111.g;
            c.b = (1 - d.r) * c000.b + (d.r - d.g) * c100.b + (d.g - d.b) * c110.b + (d.b) * c111.b;
        } 
        else if (d.r > d.b) {
            const struct rgbvec c100 = lut3d->lut[next[0]][prev[1]][prev[2]];
            const struct rgbvec c101 = lut3d->lut[next[0]][prev[1]][next[2]];
            c.r = (1 - d.r) * c000.r + (d.r - d.b) * c100.r + (d.b - d.g) * c101.r + (d.g) * c111.r;
            c.g = (1 - d.r) * c000.g + (d.r - d.b) * c100.g + (d.b - d.g) * c101.g + (d.g) * c111.g;
            c.b = (1 - d.r) * c000.b + (d.r - d.b) * c100.b + (d.b - d.g) * c101.b + (d.g) * c111.b;
        }  
        else {
            const struct rgbvec c001 = lut3d->lut[prev[0]][prev[1]][next[2]];
            const struct rgbvec c101 = lut3d->lut[next[0]][prev[1]][next[2]];
            c.r = (1 - d.b) * c000.r + (d.b - d.r) * c001.r + (d.r - d.g) * c101.r + (d.g) * c111.r;
            c.g = (1 - d.b) * c000.g + (d.b - d.r) * c001.g + (d.r - d.g) * c101.g + (d.g) * c111.g;
            c.b = (1 - d.b) * c000.b + (d.b - d.r) * c001.b + (d.r - d.g) * c101.b + (d.g) * c111.b;
        }
    } 
    else {
        if (d.b > d.g) {
            const struct rgbvec c001 = lut3d->lut[prev[0]][prev[1]][next[2]];
            const struct rgbvec c011 = lut3d->lut[prev[0]][next[1]][next[2]];
            c.r = (1 - d.b) * c000.r + (d.b - d.g) * c001.r + (d.g - d.r) * c011.r + (d.r) * c111.r;
            c.g = (1 - d.b) * c000.g + (d.b - d.g) * c001.g + (d.g - d.r) * c011.g + (d.r) * c111.g;
            c.b = (1 - d.b) * c000.b + (d.b - d.g) * c001.b + (d.g - d.r) * c011.b + (d.r) * c111.b;
        } 
        else if (d.b > d.r) {
            const struct rgbvec c010 = lut3d->lut[prev[0]][next[1]][prev[2]];
            const struct rgbvec c011 = lut3d->lut[prev[0]][next[1]][next[2]];
            c.r = (1 - d.g) * c000.r + (d.g - d.b) * c010.r + (d.b - d.r) * c011.r + (d.r) * c111.r;
            c.g = (1 - d.g) * c000.g + (d.g - d.b) * c010.g + (d.b - d.r) * c011.g + (d.r) * c111.g;
            c.b = (1 - d.g) * c000.b + (d.g - d.b) * c010.b + (d.b - d.r) * c011.b + (d.r) * c111.b;
        } 
        else {
            const struct rgbvec c010 = lut3d->lut[prev[0]][next[1]][prev[2]];
            const struct rgbvec c110 = lut3d->lut[next[0]][next[1]][prev[2]];
            c.r = (1 - d.g) * c000.r + (d.g - d.r) * c010.r + (d.r - d.b) * c110.r + (d.b) * c111.r;
            c.g = (1 - d.g) * c000.g + (d.g - d.r) * c010.g + (d.r - d.b) * c110.g + (d.b) * c111.g;
            c.b = (1 - d.g) * c000.b + (d.g - d.r) * c010.b + (d.r - d.b) * c110.b + (d.b) * c111.b;
        }
    }
    return (c);
}


/**
 * Clip a signed integer value into the 0-255 range.
 * @param a value to clip
 * @return clipped value
 */
unsigned char clip_uint8(int a) {
    if (a & (~0xFF))
        return ((~a) >> 31);
    else return (a);
}


int interp_tetrahedral_uint8( LUT3DContext * lut3d, const unsigned char * indata, unsigned char * outdata, int width, int height, int linesize)
{            
    int nbits = 8;                                        
    int        x, y;                                                 
    const int    direct    = outdata == indata;                                       
    const int    step    = lut3d->step;                                     
    const unsigned char    r    = lut3d->rgba_map[R];                                
    const unsigned char    g    = lut3d->rgba_map[G];                                
    const unsigned char    b    = lut3d->rgba_map[B];                                
    const unsigned char    a    = lut3d->rgba_map[A];  
    printf("r:%d g:%d b:%d a:%d\n", r, g, b, a);                              
    unsigned char         *dstrow = outdata;                
    const unsigned char   *srcrow = indata;     
    const unsigned char   uint8_max = (1 << 8) - 1;          
    const float    scale    = (1.f /  uint8_max ) * (lut3d->lutsize - 1);              
    printf("scale:%f lutsize:%d\n", scale, lut3d->lutsize); 
    printf("width:%d height:%d\n", width, height);   
    
    for ( y = 0; y < height; y++ ) {                             
        unsigned char * dst = (unsigned char *)dstrow;                         
        const unsigned char *src = (const unsigned char *)srcrow;                    
        for ( x = 0; x < width * step; x += step ) {                          
            const struct rgbvec    scaled_rgb = { src[x + r] * scale,                     
                                    src[x + g] * scale,                     
                                    src[x + b] * scale };                      
            struct rgbvec        vec = interp_tetrahedral( lut3d, &scaled_rgb );                      
            dst[x + r]    = clip_uint8( vec.r * (float) uint8_max );              
            dst[x + g]    = clip_uint8( vec.g * (float) uint8_max );              
            dst[x + b]    = clip_uint8( vec.b * (float) uint8_max );              
            if ( !direct && step == 4 )                                  
                dst[x + a] = src[x + a];                                
        }                                                
        dstrow    += linesize;                                     
        srcrow    += linesize;                                     
    }                                              
    return 0;                                            
}



