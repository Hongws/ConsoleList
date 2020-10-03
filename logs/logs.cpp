// logs.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#pragma warning(disable:4996)

#ifdef _DEBUG
//for memory leak check
#define _CRTDBG_MAP_ALLOC //使生成的内存dump包含内存块分配的具体代码为止
#include <stdlib.h> 
#include <crtdbg.h>
#define CheckMemoryLeak _CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG )| _CRTDBG_LEAK_CHECK_DF)
#endif

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <Windows.h>

void	g_log(const char *fmt, ...);
void	g_logf(FILE *l_file, const char *fmt, ...);
long GetCurrentYMD();


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

void	g_log(const char *fmt, ...)
{
	FILE	*l_file;
	l_file = nullptr;

	char  szLogFile[256] = { 0 };

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

	long nDate = GetCurrentYMD();
	sprintf(szLogFile, "%s\\%ld.log", szTemp, nDate / 100);

	//写死目录
	//sprintf(szLogFile, "D:\\%ld.log", /*szOperLogPath,*/ nDate / 100);
	l_file = fopen(szLogFile, "a");

	va_list ap;
	time_t ti;
	struct tm *tim;
	char tbuf[50];

	if (!l_file)
		return;

	va_start(ap, fmt);
	ti = time(0);
	tim = localtime(&ti);
	strftime(tbuf, 50, "%H:%M:%S-%d/%m", tim);
	fprintf(l_file, "%s: ", tbuf);
	vfprintf(l_file, fmt, ap);
	fprintf(l_file, "\n");
	fflush(l_file);

	if (l_file != nullptr)
	{
		fclose(l_file);
		l_file = nullptr;
	}
}


void	g_logf(FILE *l_file, const char *fmt, ...)
{
	va_list ap;
	time_t ti;
	struct tm *tim;
	char tbuf[50];

	if (!l_file)
		return;

	va_start(ap, fmt);
	ti = time(0);
	tim = localtime(&ti);
	strftime(tbuf, 50, "%H:%M:%S-%d/%m", tim);
	fprintf(l_file, "%s: ", tbuf);
	vfprintf(l_file, fmt, ap);
	fprintf(l_file, "\n");
	fflush(l_file);
}

long GetCurrentYMD()
{
	struct tm *cvtm;
	time_t	tip;
	long	nDate;

	time(&tip);
	cvtm = localtime(&tip);
	if (tip < 0)
		return -1;
	nDate = (cvtm->tm_year + 1900) * 10000 + (cvtm->tm_mon + 1) * 100 + cvtm->tm_mday;
	return nDate;
}

int main()
{
#ifdef _DEBUG
	CheckMemoryLeak;
#endif
	//g_logf用法
	//FILE	*logsfile;
	//logsfile = nullptr;

	//char  szLogFile[256] = { 0 };

	//long nDate = GetCurrentYMD();
	//sprintf(szLogFile, "D:\\%ld.log", /*szOperLogPath,*/ nDate / 100);
	//logsfile = fopen(szLogFile, "a");

	//g_logf(logsfile, "Start Logs!");
	//char p[10] = "test";
	//g_logf(logsfile, "....%s(%s)----", p, "10");

	//g_logf(logsfile, "....%d(%d)----", 10, 100);

	//printf("Hello World!\n");


	//if (logsfile != nullptr)
	//{
	//	fclose(logsfile);
	//	logsfile = nullptr;
	//}

	//g_log用法啊
	g_log("Start Log hello!");
	char p[10] = "test log";
	g_log( "....%s(%s)----", p, "20");

	g_log( "....%d(%d)----", 100, 1000);

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
