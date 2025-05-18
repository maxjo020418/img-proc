#include "bmp.h"
#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>

void SaveBMP(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, RGBQUAD* hRGB, BYTE* output, int W, int H, const char* filename) {
	FILE* fp = fopen(filename, "wb");
	int imgSize = W * H;

	// True Color RGB
	if (hInfo.biBitCount == 24) {
		std::cout << "saving as color" << std::endl;
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(output, sizeof(BYTE), imgSize * 3, fp);
	}
	// GreyScale
	else {
		std::cout << "saving as greyscale" << std::endl;
		fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
		fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
		fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
		fwrite(output, sizeof(BYTE), imgSize, fp);
	}
}

void setPixel(BYTE* image, BITMAPINFOHEADER hInfo, int x, int y, BYTE r, BYTE g, BYTE b) {
	int h = hInfo.biWidth;
	image[y * h * 3 + x * 3 + 0] = b;
	image[y * h * 3 + x * 3 + 1] = g;
	image[y * h * 3 + x * 3 + 2] = r;
}

int main()
{
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hInfo;
	RGBQUAD hRGB[256];
	FILE* fp;

	fp = fopen("inp/test.bmp", "rb");

	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}

	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);	// header file
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);	// header info (img)

	int ImgSize = hInfo.biWidth * hInfo.biHeight;

	BYTE* Image;
	BYTE* Output; // unused

	// True Color RGB
	if (hInfo.biBitCount == 24) {
		std::cout << "color inp: ";
		Image	= (BYTE*) malloc(ImgSize * 3);
		Output	= (BYTE*) malloc(ImgSize * 3);
		fread(Image, sizeof(BYTE), ImgSize * 3, fp);
	}
	// GreyScale
	else {
		std::cout << "greyscale inp: ";
		fread(hRGB, sizeof(RGBQUAD), 256, fp);
		Image	= (BYTE*) malloc(ImgSize);
		Output	= (BYTE*) malloc(ImgSize);
		fread(Image, sizeof(BYTE), ImgSize, fp);
	}
	std::cout << ImgSize << " " << hInfo.biWidth << "x" << hInfo.biHeight << std::endl;

	fclose(fp);

	/*
	================================
	Editing part starting from below
	================================
	NOTES:
	1. BGR사용하니 주의할것
	2. Image는 1dim arr.이니 참고
	3. [[B,G,R], [B,G,R], [B,G,R], [B,G,R]] 이런식으로 있음

	Image[40 * W * 3 + 50 * 3 + 0] => Blue @ (50, 40)
	Image[40 * W * 3 + 50 * 3 + 1] => Green @ (50, 40)
	Image[40 * W * 3 + 50 * 3 + 2] => Red @ (50, 40)
	*/

	int xStart = 100;
	int xEnd = xStart + 800;

	int yStart = 100;
	int rect_height = 200;
	int yEnd = yStart + rect_height;

	/*
	BMP가 y축에 대하여 4bit padding/buffer있으니 어긋나보이는거 참고
	왼쪽 아래부터 origin으로 오른쪽/위로 증가하는 좌표계

	scale/weight 진행도(?)로 만든 다음 255곱해서
	선형 증가 형태로 만듦, min/max 스타일로
	x에 대한 선형 변화 일어나야 하니 x루프에 넣음
	*/
	double scale;
	int delta;

	// blue(0, 0, 255) -> yellow(255, 255, 0)
	for (int x = xStart; x < xEnd; x++) {
		scale = (double) x / xEnd;
		for (int y = yStart; y < yEnd; y++) {
			delta = scale * 255;
			setPixel(Image, hInfo, x, y, delta, delta, 255 - delta);
		}
	}

	yStart = yEnd;
	yEnd = yStart + rect_height;
	// green(0, 255, 0) -> magenta(255, 0, 255)
	for (int x = xStart; x < xEnd; x++) {
		scale = (double) x / xEnd;
		for (int y = yStart; y < yEnd; y++) {
			delta = scale * 255;
			setPixel(Image, hInfo, x, y, delta, 255 - delta, delta);
		}
	}
	
	yStart = yEnd;
	yEnd = yStart + rect_height;
	// red(255, 0, 0) -> cyan(0, 255, 255)
	for (int x = xStart; x < xEnd; x++) {
		scale = (double) x / xEnd;
		for (int y = yStart; y < yEnd; y++) {
			delta = scale * 255;
			setPixel(Image, hInfo, x, y, 255 - delta, delta, delta);
		}
	}

	/*================ SAVE ================*/

	SaveBMP(hf, hInfo, hRGB, Image, hInfo.biWidth, hInfo.biHeight, "./out/test.bmp");
}
