/*
UTF-8으로, LF에서 CLRF로 해라ㅏㅏㅏ
리눅스라 Windows.h 없어서 별도 헤더파일 정의
cpp header로 다 교체
GCC말고 G++쓰기!!!
*/

// #pragma warning(disable:4996)
// #include <Windows.h>
// #include <stdio.h>
// #include <stdlib.h>

#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cmath>
#include "bmp.h"

void InverseImage (BYTE* Img, BYTE *Out, int W, int H)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = 255 - Img[i];
	}
}

void BrightnessAdj(BYTE* Img, BYTE* Out, int W, int H, int Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] + Val > 255)
		{
			Out[i] = 255;
		}
		else if (Img[i] + Val < 0)
		{
			Out[i] = 0;
		}
		else 	Out[i] =Img[i] + Val;
	}
}

void ContrastAdj(BYTE* Img, BYTE* Out, int W, int H, double Val)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++)
	{
		if (Img[i] * Val > 255.0)
		{
			Out[i] = 255;
		}
		else 	Out[i] = (BYTE)(Img[i] * Val);
	}
}

void ObtainHistogram(BYTE* Img, int* Histo, int W, int H)
{
	int ImgSize = W * H;
	double Temp[256] = { 0 };
	for (int i = 0; i < ImgSize; i++) {
		Histo[Img[i]]++;
	}
	/*for (int i = 0; i < 256; i++) {
		Temp[i] = (double)Histo[i] / ImgSize;
	}
	FILE* fp = fopen("histogram.txt", "wt");
	for(int i=0; i<256; i++) fprintf(fp, "%lf\n", Temp[i]);
	fclose(fp);*/
}

void ObtainAHistogram(int* Histo, int* AHisto)
{
	for (int i = 0; i < 256; i++) {
		for (int j = 0; j <= i; j++) {
			AHisto[i] = AHisto[i] + Histo[j];
		}
	}
	/*FILE* fp = fopen("Ahistogram.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", AHisto[i]);
	fclose(fp);*/
}

void HistogramStretching(BYTE * Img, BYTE * Out, int * Histo, int W, int H)
{
	int ImgSize = W * H;
	BYTE Low, High;
	for (int i = 0; i < 256; i++) {
		if (Histo[i] != 0) {
			Low = i;
			break;
		}
	}
	for (int i = 255; i >= 0; i--) {
		if (Histo[i] != 0) {
			High = i;
			break;
		}
	}
	for (int i = 0; i < ImgSize; i++) {
		Out[i] = (BYTE)((Img[i] - Low) / (double)(High - Low) * 255.0);
	}
}
void HistogramEqualization(BYTE* Img, BYTE* Out, int* AHisto, int W, int H)
{
	int ImgSize = W * H;
	int Nt = W * H, Gmax = 255;
	double Ratio = Gmax / (double)Nt;
	BYTE NormSum[256];
	for (int i = 0; i < 256; i++) {
		NormSum[i] = (BYTE)(Ratio * AHisto[i]);
	}
	/*FILE* fp = fopen("NormSum.txt", "wt");
	for (int i = 0; i < 256; i++) fprintf(fp, "%d\n", NormSum[i]);
	fclose(fp);*/
	for (int i = 0; i < ImgSize; i++)
	{
		Out[i] = NormSum[Img[i]];
	}
}

void Binarization(BYTE * Img, BYTE * Out, int W, int H, BYTE Threshold)
{
	int ImgSize = W * H;
	for (int i = 0; i < ImgSize; i++) {
		if (Img[i] < Threshold) Out[i] = 0;
		else Out[i] = 255;
	}
}

int GozalezBinThresh(BYTE * Img, int W, int H, float epsilon)
{
	int optimal_thresh = 0;

	int ImgSize = W * H;
	int hist[256] = {0}; // don't forget to init. to 0!!!!!
	int hist_size = sizeof(hist) / sizeof(hist[0]);

	// (BYTE* Img, int* Histo, int W, int H)
	ObtainHistogram(Img, hist, W, H);

	float max_ob = .0;
	int max_brightness;
	int min_brightness;

	for (int bw = 0; bw < hist_size; bw++) {
		if (hist[bw] > 0) {
			max_brightness = bw;
			break;
		}
	}
	for (int bw = hist_size - 1; bw > 0; bw--) {
		if (hist[bw] > 0) {
			min_brightness = bw;
			break;
		}
	}

	float T = (max_brightness + min_brightness) / 2;

	// main caculation
	for (int thresh = 0; thresh < hist_size; thresh++) {

		float np_f = .0; // no. pixels foreground
		float np_b = .0; // no. pixels background
		float in_f = .0; // intensity foreground
		float in_b = .0; // intensity backgorund

		for (int bw_lvl = 0; bw_lvl < hist_size; bw_lvl++) {
			if (bw_lvl < thresh) {
				// left (darker) background
				np_b += hist[bw_lvl];
				in_b += hist[bw_lvl] * bw_lvl;
			}
			else {
				// right (brighter) foreground
				np_f += hist[bw_lvl];
				in_f += hist[bw_lvl] * bw_lvl;
			}
		}
		
		// float Wb = np_b / ImgSize; // % of no. bg px
		// float Wf = np_f / ImgSize; // % of no. fg px
		float Ub; // mean intensity bg
		float Uf; // mean intensity fg

		// 0 division guard
		if ( not (np_b <= 0 or np_f <= 0) ) {
			Ub = in_b / np_b;
			Uf = in_f / np_f;
		}
		else { // if 0 div set to 0
			Ub = .0;
			Uf = .0;
		}

		float new_T = (Ub + Uf) / 2.0;
		float t_delta = std::fabs(new_T - T);

		// end condition, using T delta
		std::cout << t_delta << std::endl;
		if (t_delta < epsilon) return thresh;

		/* otsu's method...?
		float ob = Wb * Wf * pow(Ub - Uf, 2);
		if (ob > max_ob) {
			max_ob = ob;
			optimal_thresh = thresh;

			std::cout << "=============" << std::endl;
			std::cout << "max_ob: " << ob << std::endl;
			std::cout << "optimal_thresh: " << optimal_thresh << std::endl;
		}
		*/
	}

	// left it just in case, also used when otsu's method is used
	return optimal_thresh;
}

int main()
{
	BITMAPFILEHEADER hf; // 14바이트
	BITMAPINFOHEADER hInfo; // 40바이트
	RGBQUAD hRGB[256]; // 1024바이트
	FILE* fp;
	fp = fopen("../inp_img/coin.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	// (BYTE * Img, int W, int H)
	float eps = 2.0;
	int Thres = GozalezBinThresh(Image, hInfo.biWidth, hInfo.biHeight, eps);
	std::cout << "Epsilon: " << eps << std::endl;
	std::cout << "Threshold: " << Thres << std::endl;
	Binarization(Image, Output, hInfo.biWidth, hInfo.biHeight, Thres);

	fp = fopen("./out/gonz_bin.bmp", "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), ImgSize, fp);
	fclose(fp);
	free(Image);
	free(Output);
	return 0;
}