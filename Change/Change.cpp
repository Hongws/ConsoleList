// Change.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//		//尺寸和质量两种压缩算法;支持JPEG、bmp、PNG等格式。
// 宽窄字节转换
#pragma warning(disable:4996)

#ifdef _DEBUG
//for memory leak check
#define _CRTDBG_MAP_ALLOC //使生成的内存dump包含内存块分配的具体代码为止
#include <stdlib.h> 
#include <crtdbg.h>
#define CheckMemoryLeak _CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG )| _CRTDBG_LEAK_CHECK_DF)
#endif

#include <string>

#include <iostream>
using namespace std;

#include<Windows.h>
//GDI++
#include <GdiPlus.h>
#pragma comment( lib, "GdiPlus.lib" )
using namespace Gdiplus;


char* W2M(LPWSTR wchar, char* mchar);
LPWSTR M2W(char* mchar, LPWSTR wchar);

char* W2M(LPWSTR wchar, char* mchar)
{
	DWORD dwFlags = 0;
	// Do the conversion.
	int len = WideCharToMultiByte(CP_OEMCP, dwFlags, wchar, -1, mchar, 0, NULL, NULL);
	if (len > 0)
	{
		int cchWide = wcslen(wchar) + 1;
		len = WideCharToMultiByte(CP_OEMCP, dwFlags, wchar, cchWide, mchar, len, NULL, NULL);
		if (len < 0)
		{
			memset(mchar, 0, 1);
			return NULL;
		}
		else {
			return mchar;
		}
	}
	else
		return NULL;
}

LPWSTR M2W(char* mchar, LPWSTR wchar)
{
	int   iLength;
	iLength = MultiByteToWideChar(CP_OEMCP, 0, mchar, -1, NULL, 0);
	if (iLength <= 0)return NULL;
	MultiByteToWideChar(CP_OEMCP, 0, mchar, -1, wchar, iLength + 1);
	return wchar;
}




//jpg尺寸压缩			原文件路径					新文件路径					新文件大小
bool CompressImagePixel(const WCHAR* pszOriFilePath, const WCHAR* pszDestFilePah, UINT ulNewHeigth, UINT ulNewWidth);
//jpg质量压缩			原文件路径					新文件路径					压缩比率
bool CompressImageQuality(const WCHAR* pszOriFilePath, const WCHAR* pszDestFilePah, ULONG quality);
bool GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid);

//jpg尺寸压缩
bool CompressImagePixel(
	const WCHAR* pszOriFilePath,
	const WCHAR* pszDestFilePah,
	UINT ulNewHeigth,
	UINT ulNewWidth)
{
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Status stat = GenericError;
	stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (Ok != stat) {
		return false;
	}
	// 重置状态
	stat = GenericError;

	// Get an image from the disk.
	Image* pImage = new Image(pszOriFilePath);

	do {
		if (NULL == pImage) {
			break;
		}

		// 获取长宽
		UINT unOriHeight = pImage->GetHeight();
		UINT unOriWidth = pImage->GetWidth();

		do {
			CLSID encoderClsid;
			if (unOriWidth < 1 || unOriHeight < 1) {
				break;
			}

			// Get the CLSID of the JPEG encoder.
			if (!GetEncoderClsid(L"image/jpeg", &encoderClsid)) {
				break;
			}

			REAL fSrcX = 0.0f;
			REAL fSrcY = 0.0f;
			REAL fSrcWidth = (REAL)unOriWidth;
			REAL fSrcHeight = (REAL)unOriHeight;
			RectF RectDest(0.0f, 0.0f, (REAL)ulNewWidth, (REAL)ulNewHeigth);

			Bitmap* pTempBitmap = new Bitmap(ulNewWidth, ulNewHeigth);
			Graphics* graphics = NULL;

			do {
				if (!pTempBitmap) {
					break;
				}

				graphics = Graphics::FromImage(pTempBitmap);
				if (!graphics) {
					break;
				}

				stat = graphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
				if (Ok != stat) {
					break;
				}

				stat = graphics->SetSmoothingMode(Gdiplus::SmoothingModeHighQuality);
				if (Ok != stat) {
					break;
				}

				stat = graphics->DrawImage(pImage, RectDest, fSrcX, fSrcY, fSrcWidth, fSrcHeight,
					UnitPixel, NULL, NULL, NULL);
				if (Ok != stat) {
					break;
				}

				stat = pTempBitmap->Save(pszDestFilePah, &encoderClsid, NULL);
				if (Ok != stat) {
					break;
				}

			} while (0);

			if (NULL != graphics) {
				delete graphics;
				graphics = NULL;
			}

			if (NULL != pTempBitmap) {
				delete pTempBitmap;
				pTempBitmap = NULL;
			}
		} while (0);
	} while (0);

	if (pImage) {
		delete pImage;
		pImage = NULL;
	}

	GdiplusShutdown(gdiplusToken);

	return ((Ok == stat) ? true : false);
}



bool CompressImageQuality(
	const WCHAR* pszOriFilePath,
	const WCHAR* pszDestFilePah,
	ULONG quality)
{
	// copy from http://msdn.microsoft.com/en-us/library/ms533844(v=VS.85).aspx
	// Initialize GDI+.
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Status stat = GenericError;
	stat = GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	if (Ok != stat) {
		return false;
	}

	// 重置状态
	stat = GenericError;

	// Get an image from the disk.
	Image* pImage = new Image(pszOriFilePath);

	do {
		if (NULL == pImage) {
			break;
		}

		// 获取长宽
		UINT ulHeight = pImage->GetHeight();
		UINT ulWidth = pImage->GetWidth();
		if (ulWidth < 1 || ulHeight < 1) {
			break;
		}

		// Get the CLSID of the JPEG encoder.
		CLSID encoderClsid;
		if (!GetEncoderClsid(L"image/jpeg", &encoderClsid)) {
			break;
		}

		// The one EncoderParameter object has an array of values.
		// In this case, there is only one value (of type ULONG)
		// in the array. We will let this value vary from 0 to 100.
		EncoderParameters encoderParameters;
		encoderParameters.Count = 1;
		encoderParameters.Parameter[0].Guid = EncoderQuality;
		encoderParameters.Parameter[0].Type = EncoderParameterValueTypeLong;
		encoderParameters.Parameter[0].NumberOfValues = 1;
		encoderParameters.Parameter[0].Value = &quality;		//压缩比例
		stat = pImage->Save(pszDestFilePah, &encoderClsid, &encoderParameters);
	} while (0);

	if (pImage) {
		delete pImage;
		pImage = NULL;
	}

	GdiplusShutdown(gdiplusToken);

	return ((stat == Ok) ? true : false);
}


bool GetEncoderClsid(const WCHAR* pszFormat, CLSID* pClsid)
{
	UINT  unNum = 0;          // number of image encoders
	UINT  unSize = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	// How many encoders are there?
	// How big (in bytes) is the array of all ImageCodecInfo objects?
	GetImageEncodersSize(&unNum, &unSize);
	if (0 == unSize) {
		return false;  // Failure
	}

	// Create a buffer large enough to hold the array of ImageCodecInfo
	// objects that will be returned by GetImageEncoders.
	pImageCodecInfo = (ImageCodecInfo*)(malloc(unSize));
	if (!pImageCodecInfo) {
		return false;  // Failure
	}

	// GetImageEncoders creates an array of ImageCodecInfo objects
	// and copies that array into a previously allocated buffer. 
	// The third argument, imageCodecInfos, is a pointer to that buffer. 
	GetImageEncoders(unNum, unSize, pImageCodecInfo);

	for (UINT j = 0; j < unNum; ++j) {
		if (wcscmp(pImageCodecInfo[j].MimeType, pszFormat) == 0) {
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			pImageCodecInfo = NULL;
			return true;  // Success
		}
	}

	free(pImageCodecInfo);
	pImageCodecInfo = NULL;
	return false;  // Failure
}


char * __cdecl my_strrchr(const char * string, int ch)
{
	char *start = (char *)string;

	while (*string++)                       /* find end of string */
		;
	/* search towards front */
	while (--string != start && *string != (char)ch)
		;

	if (*string == (char)ch)               /* char found ? */
		return((char *)string);

	return(NULL);
}

int main()
{
#ifdef _DEBUG
	CheckMemoryLeak;
#endif

	char oldpath[50] = { 0 };
	char newpathb[50] = { 0 };
	char newpathc[50] = { 0 };

	TCHAR tszDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, tszDir, MAX_PATH);
	char szDir[MAX_PATH] = { 0 };
	W2M(tszDir, szDir);

	char *pdest;
	int  ch = '\\';
	//pdest = my_strrchr(szDir, ch);
	pdest = strrchr(szDir, ch);
	int result = pdest - szDir + 1;
	char szTemp[MAX_PATH] = { 0 };
	memcpy(szTemp, szDir, result);
	//strcat(szTemp, "1.jpg");
	sprintf(oldpath, "%s1.jpg", szTemp);
	sprintf(newpathb, "%s2.jpg", szTemp);
	sprintf(newpathc, "%s3.jpg", szTemp);

	TCHAR toldpath[50] = { 0 };
	TCHAR tnewpathb[50] = { 0 };
	TCHAR tnewpathc[50] = { 0 };
	M2W(oldpath, toldpath);
	M2W(newpathb, tnewpathb);
	M2W(newpathc, tnewpathc);

	//wsprintf(toldpath, "%s%s", szDir, toldpath);
	
	CompressImagePixel(toldpath, tnewpathb, 500, 500);

	CompressImageQuality(toldpath, tnewpathc, 20);

	//CompressImagePixel(L"2A.png", L"2B.jpg", 100, 100);
	//CompressImageQuality(L"2A.png", L"2C.jpg", 30);

	//CompressImagePixel(L"3A.bmp", L"3B.jpg", 100, 100);
	//CompressImageQuality(L"3A.bmp", L"3C.jpg", 30);
	return 0;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
