#include "BmpImage.h"

BmpImage::BmpImage(std::string fileName) :
	bmp(NULL),
	imgData(NULL),
	extData(NULL),
	offset(NULL)
{
	this->fileName = fileName;
}

BmpImage::~BmpImage()
{
	if (this->offset != NULL)
	{
		delete []offset;
	}
	if(bmp != NULL)
	{ 
		fclose(this->bmp);
	}
	if (imgData != NULL)
	{
		delete []this->imgData;
	}
	if (extData != NULL)
	{
		delete[]this->extData;
	}
}

bool BmpImage::LoadFile(void)
{
	if (bmp != NULL)
	{
		fclose(bmp);
	}
	if (fopen_s(&this->bmp, this->fileName.c_str(), "rb") != 0)
	{
		return false;
	}
	else
	{
		return ReadFileHeader() && ReadInfoHeader() && ReadImageData();
	}
	return false;
}


bool BmpImage::ReadFileHeader(void)
{
	if (fread(fh.data, sizeof(unsigned char), FILE_HEADER_SIZE, this->bmp) < FILE_HEADER_SIZE)
	{
		return false;
	}
	else
	{
		this->fh.bfType[0] = this->fh.data[0];
		this->fh.bfType[1] = this->fh.data[1];
		if (this->fh.bfType[0] == 'B' || this->fh.bfType[1] == 'M')
		{
			memcpy(&this->fh.bfSize, this->fh.data + 2, sizeof(this->fh.bfSize));
			memcpy(&this->fh.offSet, this->fh.data + 10, sizeof(this->fh.offSet));
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
}

bool BmpImage::ReadInfoHeader(void)
{
	if (fread(this->ih.data, sizeof(unsigned char), INFO_HEADER_SIZE, this->bmp) < INFO_HEADER_SIZE)
	{
		return false;
	}
	else
	{
		memcpy(&this->ih.biWidth, this->ih.data+4, sizeof(this->ih.biWidth));
		memcpy(&this->ih.biHeight, this->ih.data+8, sizeof(this->ih.biHeight));
		memcpy(&this->ih.biBitCount, this->ih.data + 14, sizeof(this->ih.biBitCount));
		memcpy(&this->ih.biSizeImage, this->ih.data+20, sizeof(this->ih.biSizeImage));
		return true;
	}
	return false;
}

bool BmpImage::ReadImageData(void)
{
	
	if (imgData != NULL)
	{
		delete []this->imgData;
	}
	if (this->extData != NULL)
	{
		delete []this->extData;
	}
	if (this->offset != NULL)
	{
		delete []this->offset;
	}
	this->imgData = new unsigned char[this->ih.biSizeImage];
	this->extData = new unsigned char[this->ih.biSizeImage];
	if (this->fh.offSet > 54) {
		this->offset = new unsigned char[this->fh.offSet - 54];
		fread(this->offset, sizeof(unsigned char), this->fh.offSet - 54, this->bmp);
	}
	size_t size = fread(this->imgData, sizeof(unsigned char), this->ih.biSizeImage, this->bmp);
	if (size < ih.biSizeImage)
	{
		return false;
	}
	else
	{
		memcpy(this->extData, this->imgData, this->ih.biSizeImage);
		return true;
	}

	return false;
}

bool BmpImage::SaveAsBmp(std::string saveName)
{
	FILE *out;
	if(fopen_s(&out, saveName.c_str(), "wb") != 0)
	{
		return false;
	}
	else
	{
		fwrite(this->fh.data, sizeof(unsigned char), FILE_HEADER_SIZE, out);
		fwrite(this->ih.data, sizeof(unsigned char), INFO_HEADER_SIZE, out);
		if (this->fh.offSet > 54) {
			fwrite(this->offset, sizeof(unsigned char), this->fh.offSet - 54, out);
		}
		fwrite(this->extData, sizeof(unsigned char), this->ih.biSizeImage, out);
		fclose(out);
		return true;
	}
	return false;
}

Rgba BmpImage::GetRgba(int row, int col)
{
	Rgba color;
	int realWidth;
	int bPos, gPos, rPos, aPos;
	if (this->ih.biBitCount == 32)
	{
		realWidth = this->ih.biWidth * 4 + this->ih.biWidth % 4;
		bPos  = gPos = rPos = aPos = row * realWidth + 4 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
		aPos = bPos + 3;
		color.a = this->imgData[aPos];
	}
	if (this->ih.biBitCount == 8)
	{
		realWidth = this->ih.biWidth  + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 1 * col;
	}
	else if (this->ih.biBitCount == 24)
	{
		realWidth = this->ih.biWidth * 3 + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 3 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
	}
	color.r = this->imgData[rPos];
	color.g = this->imgData[gPos];
	color.b = this->imgData[bPos];
	
	return color;
}

Rgba BmpImage::GetExtRgba(int row, int col)
{
	Rgba color;
	int realWidth;
	int bPos, gPos, rPos, aPos;
	if (this->ih.biBitCount == 32)
	{
		realWidth = this->ih.biWidth * 4 + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 4 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
		aPos = bPos + 3;
		color.a = this->imgData[aPos];
	}
	if (this->ih.biBitCount == 8)
	{
		realWidth = this->ih.biWidth + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 1 * col;
	}
	else if (this->ih.biBitCount == 24)
	{
		realWidth = this->ih.biWidth * 3 + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 3 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
	}
	color.r = this->extData[rPos];
	color.g = this->extData[gPos];
	color.b = this->extData[bPos];

	return color;
}

void BmpImage::SetRgba(int row, int col, Rgba color)
{
	int realWidth;
	int bPos, gPos, rPos, aPos;
	if (this->ih.biBitCount == 32)
	{
		realWidth = this->ih.biWidth * 4 + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 4 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
		aPos = bPos + 3;
		color.a = this->imgData[aPos];
	}
	if (this->ih.biBitCount == 8)
	{
		realWidth = this->ih.biWidth + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 1 * col;
	}
	else if (this->ih.biBitCount == 24)
	{
		realWidth = this->ih.biWidth * 3 + this->ih.biWidth % 4;
		bPos = gPos = rPos = aPos = row * realWidth + 3 * col;
		gPos = bPos + 1;
		rPos = bPos + 2;
	}
	memcpy(this->extData+ rPos, &color.r, sizeof(unsigned char));
	memcpy(this->extData + gPos, &color.g, sizeof(unsigned char));
	memcpy(this->extData+ bPos, &color.b, sizeof(unsigned char));
}

void BmpImage::GrayScale(void)
{
	Rgba color;
	int i, j;

	for (i = 0; i < this->ih.biHeight; i++)
	{
		for (j = 0; j < this->ih.biWidth; j++)
		{
			color = GetRgba(i, j);
			color.r = color.g = color.b = (color.r * 11 + color.g * 16 + color.b * 5) / 32;
			this->SetRgba(i, j, color);
		}
	}
}


void BmpImage::RestoreImg(void)
{
	memcpy(this->extData, this->imgData, this->ih.biSizeImage);
}
