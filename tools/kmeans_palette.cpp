#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <time.h>

#include "tk_alloc.h"
#include "tk_array.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// simple image palettizer built for the LDJAM game jam
struct PaletteEntry {
    
    // Warning these are RGB float in range 0..255 not 0..1 as usual
    float r;
    float g;
    float b;

    // extra stuff for kmeans
    float mr;
    float mg;
    float mb;
    int c;
};


struct SimpleImage {
    int width;
    int height;
    uint8_t* data;
};

int cmpPaletteEntry(const void * va, const void * vb) {
    PaletteEntry* a = (PaletteEntry*)va;
    PaletteEntry* b = (PaletteEntry*)vb;

    float lumaA = 0.2126f * a->r + 0.7152f * a->g + 0.0722f * a->b;
    float lumaB = 0.2126f * b->r + 0.7152f * b->g + 0.0722f * b->b;
    if (lumaA > lumaB) {
        return 1.0f;
    }
    else {
        return -1.0f;
    }

}


PaletteEntry palettize_color(tk::Array<PaletteEntry>& palette, float r, float g, float b)
{
    float bestDist = FLT_MAX;
    int bestNdx = 0;
    // find the closest color
    for (int j = 0; j < palette.Length(); j++) {
        PaletteEntry& p = palette[j];
        float dr = (r - p.r);
        float dg = (g - p.g);
        float db = (b - p.b);
        float d = dr * dr + dg * dg + db * db;
        if (d < bestDist) {
            bestNdx = j;
            bestDist = d;
        }
    }
    return palette[bestNdx];
}

tk::Array<PaletteEntry> make_palette(SimpleImage img, int targetNum, tk::IAllocator *alloc )
{
    tk::Array<PaletteEntry> palette;

    palette.SetLength(targetNum, alloc);


    // Initialize the palette randomly
    for (int i = 0; i < targetNum; i++) {
        PaletteEntry& p = palette[i];
        p.r = ((float)rand() / (float)RAND_MAX) * 255.0f;
        p.g = ((float)rand() / (float)RAND_MAX) * 255.0f;
        p.b = ((float)rand() / (float)RAND_MAX) * 255.0f;        
        p.c = 0;
    }
    

    int iter = 0;
    int imgsz = img.width * img.height;
    int* lastbucket = (int*)alloc->alloc(sizeof(int) * imgsz, 0L, __FILE__, __LINE__); // todo need alloc macros
    while (iter < 1000) {
        

        // Clear the palette
        for (int i = 0; i < targetNum; i++) {
            PaletteEntry& p = palette[i];
            p.mr = 0.0f;
            p.mg = 0.0f;
            p.mb = 0.0f;
            p.c = 0;
        }

        int changeCount = 0;
        for (int i = 0; i < imgsz; i++) {
            float r = img.data[i * 3 + 0];
            float g = img.data[i * 3 + 1];
            float b = img.data[i * 3 + 2];

            // first pass, choose randomly
            int bestNdx = 0;
            float bestDist = FLT_MAX;
            //if (iter == 0) {
              //  bestNdx = ((float)rand() / (float)RAND_MAX) * targetNum;                
            //}
            //else 
            {
                // find the closest color
                for (int j = 0; j < targetNum; j++) {
                    PaletteEntry& p = palette[j];
                    float dr = (r - p.r);
                    float dg = (g - p.g);
                    float db = (b - p.b);
                    float d = dr * dr + dg * dg + db * db;
                    if (d < bestDist) {
                        bestNdx = j;
                        bestDist = d;
                    }
                }
            }

            
            // Add the color to the average
            // Ignore exact matches since those are solved for the palette
            PaletteEntry& p = palette[bestNdx];            
            p.mr += r;
            p.mg += g;
            p.mb += b;
            p.c += 1;


            if ((iter == 0) || (lastbucket[i] != bestNdx)) {
                changeCount++;
            }
            lastbucket[i] = bestNdx;
        }

        // Average each cluster
        for (int i = 0; i < targetNum; i++) {
            PaletteEntry& p = palette[i];
            if (p.c > 0) {
                p.r = p.mr / p.c;
                p.g = p.mg / p.c;
                p.b = p.mb / p.c;
            }
            else {
                // No colors landed in this bucket, so re-randomize it
                p.r = ((float)rand() / (float)RAND_MAX) * 255.0f;
                p.g = ((float)rand() / (float)RAND_MAX) * 255.0f;
                p.b = ((float)rand() / (float)RAND_MAX) * 255.0f;
                changeCount++;
            }
        }

        // Now check that the colors are not too close to prevent the palette from collapsing
        for (int i = 0; i < targetNum; i++) {
            PaletteEntry& p = palette[i];
            for (int j = i+1; j < targetNum; j++) {                

                PaletteEntry& p1 = palette[i];
                PaletteEntry& p2 = palette[j];
                float dr = (p1.r - p2.r);
                float dg = (p1.g - p2.g);
                float db = (p1.b - p2.b);
                float d = dr * dr + dg * dg + db * db;
                if (d < 1.0f) {
                    // Duplicate of another color in the palette, randomize
                    p2.r = ((float)rand() / (float)RAND_MAX) * 255.0f;
                    p2.g = ((float)rand() / (float)RAND_MAX) * 255.0f;
                    p2.b = ((float)rand() / (float)RAND_MAX) * 255.0f;
                    changeCount++;
                }
            }
        }
        
        iter++;
        printf("Iteration %d, %d items changed\n", iter, changeCount );
        if (changeCount == 0) {
            // Palette has converged
            break;
        }        
    }

    alloc->free(lastbucket, __FILE__, __LINE__);

    return palette;
}

int main( int argc, const char **argv )
{
    tk::DefaultAllocator defaultAlloc;
    tk::IAllocator *alloc = &defaultAlloc;

    int numColors = 16;
    const char* srcImage = NULL;        

    srand(time(0));

    int ndx = 1;
    while (ndx < argc) {

    	if (!strcmp(argv[ndx],"-n")) {
    		ndx++;
    		if (ndx==argc) {
    			printf("ERROR: Expected -n <num> number of colors.\n");
    			exit(1);
    		} 

    		numColors = atoi( argv[ndx] );
    		if ((numColors <= 0) || (numColors > 512)) {
    			printf("ERROR: Expected numColors between 1..512\n");
    			exit(1);
    		}
    	}
        else {
            if (argv[ndx][0] == '-') {
                printf("Unknown option %s\n", argv[ndx]);
            }
            else {
                if (srcImage == NULL) {
                    srcImage = argv[ndx];
                }
            }
        }

    	ndx++;
    }

    // DBG
    //srcImage = "c:\\stuff\\sunset.png";
    //srcImage = "c:\\stuff\\colorwheel.png";
    //numColors = 32;
    //srcImage = "c:\\stuff\\brite.png";

    // Assumes image ends in ".png"
    char* lutImage = (char*)malloc(strlen(srcImage) + 4);
    strcpy(lutImage, srcImage);
    strcpy(lutImage + strlen(lutImage) - 4, "_lut.png");

    // Palette image has a fancy palette bar
    char* palImage = (char*)malloc(strlen(srcImage) + 4);
    strcpy(palImage, srcImage);
    strcpy(palImage + strlen(palImage) - 4, "_pal.png");


    int n;
    SimpleImage img;
    img.data = stbi_load( srcImage, &(img.width), &(img.height), &n, 3);
    
    tk::Array<PaletteEntry> pally = make_palette(img, numColors, alloc);

    for (int i = 0; i < pally.Length(); i++) {
        PaletteEntry& p = pally[i];
        printf("Palette %d: %3.2f %3.2f %3.2f\n", i, p.r, p.g, p.b);
    }

    // Make LUT image 
    int lutDim = 16;
    float step = 255.0f / (float)lutDim;
    SimpleImage imgLut;
    imgLut.width = lutDim * lutDim;
    imgLut.height = lutDim;
    imgLut.data = (uint8_t*)malloc(imgLut.width * imgLut.height * 3);

    for (int gg = 0; gg < lutDim; gg++) {
        for (int bb = 0; bb < lutDim; bb++) {
            for (int rr = 0; rr < lutDim; rr++) {
                int ndx = (gg * lutDim * lutDim) + (bb * lutDim) + rr;

                float r = step * rr;
                float g = step * gg;
                float b = step * bb;

                PaletteEntry p = palettize_color(pally, r, g, b);
                
                imgLut.data[ndx * 3 + 0] = (uint8_t)(p.r);
                imgLut.data[ndx * 3 + 1] = (uint8_t)(p.g);
                imgLut.data[ndx * 3 + 2] = (uint8_t)(p.b);
            }
        }
    }

    stbi_write_png( lutImage, imgLut.width, imgLut.height, 3, imgLut.data, imgLut.width * 3);


    // Sort the palette by luminance
    qsort(pally._elems, pally.Length(), sizeof(PaletteEntry), cmpPaletteEntry);


    // Make palette image
    int barHeight = 32;
    SimpleImage imgPal;
    imgPal.width = img.width;
    imgPal.height = img.height + barHeight;
    imgPal.data = (uint8_t*)malloc(imgPal.width * imgPal.height * 3);
    
    // Palettize the regular image
    for (int j = 0; j < img.height; j++) {
        for (int i = 0; i < img.width; i++) {
            int ndx = j * img.width + i;

            float r = img.data[ndx * 3 + 0];
            float g = img.data[ndx * 3 + 1];
            float b = img.data[ndx * 3 + 2];

            PaletteEntry p = palettize_color(pally, r, g, b);

            imgPal.data[ndx * 3 + 0] = (uint8_t)(p.r);
            imgPal.data[ndx * 3 + 1] = (uint8_t)(p.g);
            imgPal.data[ndx * 3 + 2] = (uint8_t)(p.b);
        }
    }
    

    int blockSz = img.width / numColors;
    for (int j = 0; j < barHeight; j++) {
        for (int i = 0; i < img.width; i++) {
            int ndx = imgPal.width * (img.height + j) + i;

            PaletteEntry& p = pally[i / blockSz];
            imgPal.data[ndx * 3 + 0] = (uint8_t)(p.r);
            imgPal.data[ndx * 3 + 1] = (uint8_t)(p.g);
            imgPal.data[ndx * 3 + 2] = (uint8_t)(p.b);
        }
    }
    
    stbi_write_png( palImage, imgPal.width, imgPal.height, 3, imgPal.data, imgPal.width*3);

    stbi_image_free(img.data);
    free(imgPal.data);

    printf("Image size is %d x %d\n", img.width, img.height );
    printf("numColors is %d\n", numColors );
    printf("lutImage is %s\n", lutImage);


}