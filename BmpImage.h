#pragma once
#include <stdint.h>
#include <iostream>
#include <string>

#define FILE_HEADER_SIZE	14
#define INFO_HEADER_SIZE	40

typedef struct Color{
	unsigned char r;
	unsigned char  g;
	unsigned char b;
	unsigned char  a;
}Rgba;

typedef struct FileHeader {
	unsigned char data[FILE_HEADER_SIZE];
	unsigned char bfType[2];
	unsigned long bfSize;
	unsigned long offSet;
}FileHeader;

typedef struct InfoHeader {
	unsigned char data[INFO_HEADER_SIZE];
	long biWidth;
	long biHeight;
	unsigned int biBitCount;
	unsigned long biSizeImage;
}InfoHeader;

class BmpImage {
public:
	BmpImage(std::string);
	~BmpImage();

	bool LoadFile(void);
	bool ReadFileHeader(void);
	bool ReadInfoHeader(void);
	bool ReadImageData(void);
	bool SaveAsBmp(std::string);

	Rgba GetRgba(int, int );
	Rgba GetExtRgba(int, int);
	void SetRgba(int, int, Rgba);
	void GrayScale(void);

	void RestoreImg(void);

private:
	FILE *bmp;
	FileHeader fh;
	InfoHeader ih;
	std::string fileName;
	unsigned char *imgData;
	unsigned char *extData;
	unsigned char *offset;
};