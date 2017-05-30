#define STB_RECT_PACK_IMPLEMENTATION
#include "stb_rect_pack.h" // For better packing overall

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h" // for saving png

/*
// this holds all the per-glyph data
typedef struct
{
   unsigned short x0,y0,x1,y1; // coordinates of bbox in bitmap
   float xoff,yoff,xadvance;
   float xoff2,yoff2;
} stbtt_packedchar;
*/

/*
// this is used to specify which ranges of glyphs to pack
typedef struct
{
   float font_size;
   int first_unicode_codepoint_in_range;  // if non-zero, then the chars are continuous, and this is the first codepoint
   int *array_of_unicode_codepoints;       // if non-zero, then this is an array of unicode codepoints
   int num_chars;
   stbtt_packedchar *chardata_for_range; // output
   unsigned char h_oversample, v_oversample; // don't set these, they're used internally
} stbtt_pack_range;
*/

int main(int argc, char const *argv[])
{
    FILE *fp = fopen("FiraMono-Regular.ttf", "rb");

    int ttf_size_max = 1e6; // most likely large enough, 1MB
    unsigned char *ttf_buffer = (unsigned char*)malloc(ttf_size_max);
    fread(ttf_buffer, 1, ttf_size_max, fp);
    
    fclose(fp);

    // probably large enough
    int width = 1024;
    int height = 1024;
    unsigned char *bitmap = (unsigned char*)malloc(height*width);
    
    stbtt_pack_context pc;
    stbtt_packedchar cdatas[7][95];

    stbtt_PackBegin(&pc, bitmap, width, height, 0, 1, NULL);   
    stbtt_PackSetOversampling(&pc, 1, 1);
    
    if (0) {
        // sequential packing, fairly good quality. Packing the big ones first is probably for the best
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 96, 32, 95, cdatas[0]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 72, 32, 95, cdatas[1]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 48, 32, 95, cdatas[2]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 36, 32, 95, cdatas[3]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 24, 32, 95, cdatas[4]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 18, 32, 95, cdatas[5]);
        stbtt_PackFontRange(&pc, ttf_buffer, 0, 12, 32, 95, cdatas[6]);
    } else {
        // probably better packing, since it takes into account all the ranges at once
        stbtt_pack_range ranges[7] = {{96, 32, NULL, 95, cdatas[0], 0, 0},
                                      {72, 32, NULL, 95, cdatas[1], 0, 0},
                                      {48, 32, NULL, 95, cdatas[2], 0, 0},
                                      {36, 32, NULL, 95, cdatas[3], 0, 0},
                                      {24, 32, NULL, 95, cdatas[4], 0, 0},
                                      {18, 32, NULL, 95, cdatas[5], 0, 0},
                                      {12, 32, NULL, 95, cdatas[6], 0, 0}};
        stbtt_PackFontRanges(&pc, ttf_buffer, 0, ranges, 7);
    }


    stbtt_PackEnd(&pc);

    int max_y = 0;
    for (int j = 0; j < 7; j++) {
        for (int i = 0; i < 95; i++) {
            printf("%d %d  %d %d  %d %d  %f %f  %f %f  %f\n", j, i, cdatas[j][i].x0,   cdatas[j][i].y0,   cdatas[j][i].x1,    cdatas[j][i].y1,
                                                                    cdatas[j][i].xoff, cdatas[j][i].yoff, cdatas[j][i].xoff2, cdatas[j][i].yoff2,
                                                                    cdatas[j][i].xadvance);

            if (cdatas[j][i].y1 > max_y) {
                max_y = cdatas[j][i].y1;
            }
        }
    }

    printf("max_y = %d\n", max_y);

    stbi_write_png("font1.png", width, max_y, 1, bitmap, 0);

    free(bitmap);

    return 0;
}