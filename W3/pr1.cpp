
// UTF-8, LF에서 CLRF로 해라ㅏㅏㅏ
// 리눅스라 Windows.h 없어서 별도 헤더파일 정의

/*
=============== <헤더파일 정의 ("./bmp.h")> ===============

// The original Windows headers define BITMAPINFOHEADER and RGBQUAD 
// in a way that matches the BMP file layout (often with internal packing already handled).

// When you create your own definitions on Linux, 
// you must ensure that these structures are packed 
// (using #pragma pack(push, 1) / #pragma pack(pop) or equivalent attributes) 
// so that no extra padding is added. 

// Otherwise, the layout might not match the BMP file, 
// and reading the file directly into the structure would result in incorrect data.

#ifdef BMP_H
    // 호옥시나 하니
    #pragma message("Warning: BMP.h 이미 존재함!")
#endif

#ifndef BMP_H
#define BMP_H

#include <stdint.h>

// 파일헤더니까 패딩없이 struct 바이트 우겨넣기
// 프래그마 사용필
#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;
} BITMAPFILEHEADER;
#pragma pack(pop)

typedef struct {
    uint32_t biSize;
    int32_t  biWidth;
    int32_t  biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    int32_t  biXPelsPerMeter;
    int32_t  biYPelsPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
} BITMAPINFOHEADER;

typedef struct {
    uint8_t rgbBlue;
    uint8_t rgbGreen;
    uint8_t rgbRed;
    uint8_t rgbReserved;
} RGBQUAD;

typedef unsigned char BYTE;

#endif

=============== <헤더파일 정의 끝> ===============
*/

// 리눅스라 안씀
// #include <Windows.h>
// #pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "bmp.h" // 따로 헤더파일 제작 (일단 여기 쓴 struct들이라도)

// 별도 추가
#include <string>


void save_bmp (
	std::string fname,
	BITMAPFILEHEADER hf,
	BITMAPINFOHEADER hInfo,
	RGBQUAD* hRGB,
	BYTE* img
	) {
	// fopen이 c_string(const char)만 받아서 std::string에서 char arr로 바꿔줘야함
	// [별도 data 디렉터리 놔둔 가정 하에 작동]
	std::string path = std::string("./data/") + fname + ".bmp";
	const char* PATH = path.c_str();

	printf("%s 저장 중...", PATH);

	/* file output/saving
	전에 쓴 fp재정의하니 리셋된거임
	Byte IOstream형태가 FIFO인 듯? header부터 넣네
	*/
	FILE* fp = fopen(PATH, "wb");
	fwrite(&hf, sizeof(BYTE), sizeof(BITMAPFILEHEADER), fp);
	fwrite(&hInfo, sizeof(BYTE), sizeof(BITMAPINFOHEADER), fp);
	fwrite(hRGB, sizeof(RGBQUAD), 256, fp);
	fwrite(img, sizeof(BYTE), hInfo.biWidth * hInfo.biHeight, fp);
	fclose(fp);

	printf("완료!\n");
}


int main()
{
	// bmp.h에서 온 struct로 정의!
	BITMAPFILEHEADER hf;	// 14 바이트
	BITMAPINFOHEADER hInfo;	// 40 바이트
	RGBQUAD hRGB[256];		// 4(채널) * 256(color depth) => 1024 바이트 팔레트

	// file handling
	FILE* fp;
	fp = fopen("./data/lenna.bmp", "rb");
	if(fp == NULL) return -1;

	/*
	"Because the BMP file is stored as a continuous stream of bytes, 
	you can read the whole structure at once without manually reading each member"
	
	fread (	void *__restrict__ __ptr, 
			size_t __size, 
			size_t __n, 
			FILE *__restrict__ __stream)
	
	file pointer는 계속 이동하니 그냥 계속 써주면 됨
	옮겨주거나 바꿀필요 없이
	Header정보 로딩 해주기...
	*/
	fread(&hf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&hInfo, sizeof(BITMAPINFOHEADER), 1, fp);
	fread(hRGB, sizeof(RGBQUAD), 256, fp);  // arr는 이미 주소이니 &가 없

	int ImgSize = hInfo.biWidth * hInfo.biHeight;  // 화소수
	printf("화소: %d\n", ImgSize);

	/*
	동적할당 필: ImgSize만큼 allocate, 위에서 만든거
	malloc이 (C는 선택인데) Cpp에서는 void* 에서 
	implicit 변환이 불가/경고해서
	따로 BYTE로 casting후에 allocate해줘야함.
	*/
	BYTE* Image		= (BYTE *)malloc(ImgSize);	// 원본
	BYTE* Output	= (BYTE *)malloc(ImgSize);	// edit 후

	// Image로 헤더 정보얻은 후의 나머지 화소 데이타 모두 dump
	// 앞의 malloc이 pointer return이므로 걍 & 없이...
	fread(Image, sizeof(BYTE), ImgSize, fp);

	// end loading
	fclose(fp);

	/*======== 효과랑 저장 ========*/
	
	// 효과 없음
	save_bmp("output1", hf, hInfo, hRGB, Image);

	// 밝기 50증가
	for(int i=0; i<ImgSize; i++)
		Output[i] = 50 + Image[i];
	save_bmp("output2", hf, hInfo, hRGB, Output);

	// 역상처리 (네거티브/반전 효과)
	for(int i=0; i<ImgSize; i++)
		Output[i] = 255 - Image[i];
	save_bmp("output3", hf, hInfo, hRGB, Output);

	// cleanup (동적할당(malloc)은 따로 free 필요)
	free(Image);
	free(Output);

	return 0;
}