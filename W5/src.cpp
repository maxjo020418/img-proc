#include <cstdio>
#include <cstdlib>
#include <string>
#include <iostream>

#include "../bmp.h"

void SaveBMPFile(BITMAPFILEHEADER hf, BITMAPINFOHEADER hInfo, 
	RGBQUAD* hRGB, BYTE* Output, int W, int H, const char* FileName)
{
	FILE * fp = fopen(FileName, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(Output, sizeof(BYTE), W*H, fp);
	fclose(fp);
}

void swap(BYTE* a, BYTE* b) 
{
	BYTE temp = *a;
	*a = *b;
	*b = temp;
}

BYTE Median(BYTE* arr, int size)
{
	// �������� ����
	const int S = size;
	for (int i = 0; i < size - 1; i++) // pivot index
	{
		for (int j = i + 1; j < size; j++) // �񱳴�� index
		{
			if (arr[i] > arr[j]) 	swap(&arr[i], &arr[j]);
		}
	}
	return arr[S/2];
}

int main()
{
	BITMAPFILEHEADER hf;
	BITMAPINFOHEADER hInfo;
	RGBQUAD hRGB[256];
	FILE* fp;
	fp = fopen("inp/lenna_impulse.bmp", "rb");
	if (fp == NULL) {
		printf("File not found!\n");
		return -1;
	}
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);
	int ImgSize = hInfo.biWidth * hInfo.biHeight;
	BYTE * Image = (BYTE *)malloc(ImgSize);
	BYTE * Temp = (BYTE*)malloc(ImgSize);
	BYTE* Output = (BYTE*)malloc(ImgSize);
	fread(Image, sizeof(BYTE), ImgSize, fp);
	fclose(fp);

	/* Median filtering */

	// 마스크/커널의 한 변의 길이
	int Length = 9;

	std::string filename = "./out/median_" + std::to_string(Length) + ".bmp";
    std::cout << filename << " 처리중..." << std::endl;
	
	// Margin -> aka. Padding 
	// 커널이 원본 이미지에서 "삐져나온"파트 매꿔주는것
	// 중심점 기준이라 `(한변 길이-1) / 2`로 계tks하면 됨, 
	// Cpp에서는 알아서 casting되어서(default: floor) 그냥 바로 나눠버림
	int Margin = Length / 2;

	// 커널 크기(픽셀 수), 정사각형이니 ^2
	// 메모리 할당(크기)과 median찾을때 사용
	int WSize = Length * Length;

	// 커널크기(Wsize)의 바이트(픽셀) 정보 저장 (typedef unsigned char BYTE)
	// temp는 (크기 sizeof(BYTE) * WSize의) 동작할당된 heap주소
	// 각 stride당 kernel계산 위한 buffer
	BYTE* temp = (BYTE*) malloc(sizeof(BYTE) * WSize);

	// 앞서 가져온 가공대상 이미지 넓이, 높이
	int W = hInfo.biWidth, H = hInfo.biHeight;

	int i, j, m, n;
	// ~ 커널 중심 움직임 (stride 과정정) ~
	// 옆으로 움직이는 형태(scanline), i++, j++이므로 stride도 1
	// i: 원본 row
	// j: 원본 col
	for (i = Margin; i < H - Margin; i++) {
		for (j = Margin; j < W - Margin; j++) {
			// ~ 커널 내부 iteration ~
			// 커널 중심점 기준으로 iterate하는 중, (-/+)
			// Image에 대하여 절대적이지 않은 상대적
			for (m = -Margin; m <= Margin; m++) {
				for (n = -Margin; n <= Margin; n++) {
					// temp: 각 stride마다 덮어씌움, kernel과 Image가 겹치는 부분
					// Image[(i+m)*W + j+n]: i, j 중심점에 상대적으로 이동, 
					// Image도 1차원으로 flatten했으니 다음과같이 계산
					temp[(m + Margin) * Length + (n + Margin)] = Image[(i+m)*W + j+n];
				}
			}
			// Median은 temp받아서 중심점에 있는 픽셀값(BYTE) 반환, Output에 저장
			Output[i * W + j] = Median(temp, WSize);
		}
	}

	// memleak 방지
	free(temp);

	/* Median filtering */
		
	SaveBMPFile(hf, hInfo, hRGB, Output, hInfo.biWidth, hInfo.biHeight, filename.c_str());
	std::cout << "완료" << std::endl;


	free(Image);
	free(Output);
	free(Temp);
	return 0;
}