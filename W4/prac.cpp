#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "bmp.h"

void inverseImage (BYTE* img, BYTE *out, int W, int H) {
    int img_size = W * H;
    for (int i = 0; i < img_size; i++) {
        out[i] = 255 - img[i];
    }
}

void brightnessAdj (BYTE* img, BYTE* out, int W, int H, int val) {
    int img_size = W * H;
    for (int i = 0; i < img_size; i++) {
        int respx = img[i] + val;

        if (respx > 255) out[i] = 255;
        else if (respx < 0) out[i] = 0;        
        else out[i] = respx;
    }
}

void contrastAdj (BYTE* img, BYTE* out, int W, int H, double val) {
    int img_size = W * H;
    for (int i = 0; i < img_size; i++) {
        int respx = img[i] * val;

        if (respx > 255) out[i] = 255;
        else if (respx < 0) out[i] = 0;        
        else out[i] = respx;
    }
}

void obtainHist (BYTE* img, int* hist, int W, int H) {
    int img_size = W * H;
    for (int i = 0; i < img_size; i++) {
        hist[img[i]]++;
    }
}

void obtainAHist (BYTE* hist, BYTE* aHist) {
    int csp = 256;
    for (int a = 0; a < csp; a++) {
        for (int h = 0; h < csp; h++) {
            aHist[a] += hist[h];
        }
    }    
}

void histStretch (BYTE* img, BYTE* out, int* hist, int W, int H) {
    int img_size = W * H;
    int min = 255;
    int max = 0;
    int hist_size = 256;
    int i = 0;

    // hist. 안쓸 경우
    for (i = 0; i < img_size; i++) {
        if (img[i] > max) max = img[i];
        if (img[i] < min) min = img[i];
    }

    // hist. 쓸 경우
    for (i = 0; i < hist_size; i++) {
        if (hist[i] != 0) {
            min = i;
            break;
        }
    }
    for (i = hist_size - 1; i >= 0; i--) {
        if (hist[i] != 0) {
            max = i;
            break;
        }
    }

    for (i = 0; i < img_size; i++) {
        out[i] = (BYTE) ( (img[i] - min) / (max - min) ) * 255.0;
    }
}

void histEQ (BYTE* img, BYTE* out, int* aHist, int W, int H) {
    int img_size = W * H, pMax = 255, hist_size = 256;
    double ratio = (double) pMax / img_size;
    BYTE normSum[256]; // Tis' important

    for (int i = 0; i < hist_size; i++) {
        normSum[i] = (BYTE) aHist[i] * ratio;
    }
    
    for (int i = 0; i < img_size; i++) {
        out[i] = normSum[img[i]]; // important!!!!
    }
}

void binarization (BYTE* img, BYTE* out, int W, int H, BYTE thresh) {
    int img_size = W * H;
    for (int i = 0; i < img_size; i++) {
        if (img[i] >= thresh) out[i] = 255;
        else out[i] = 0;
    }
}

void gonzalez (BYTE* img, int W, int H, float eps) {
    
}

int main() {



    return 0;
}

