// ConsoleList.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
//线程	结构体	链表	同步锁

#pragma warning(disable:4996)

#ifdef _DEBUG
//for memory leak check
#define _CRTDBG_MAP_ALLOC //使生成的内存dump包含内存块分配的具体代码为止
#include <stdlib.h> 
#include <crtdbg.h>
#define CheckMemoryLeak _CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG )| _CRTDBG_LEAK_CHECK_DF)
#endif


#include "glist.h"
#include "SyncLocker.h"

#include <iostream>
using namespace std;


struct struTestChar{
	int nthread;
	int index;
	//char data[10];
	char *data;
};

struct struTest {
	int nthread;
	int index;
	char data[10];
	//char *data;
};

struct struTestData {
	int nthread;
	int index;
	char data[10];
	//char *data;

	struTestData()		//1种初始化
	{
		nthread = 0;
		index = 0;
		memset(data, 0, sizeof(data));
	}
};


unsigned long __stdcall thread_one(void* pParam);
unsigned long __stdcall thread_two(void* pParam);
unsigned long __stdcall thread_three(void* pParam);

GList *g_list;
SyncLocker *g_syncLocker;

//CRITICAL_SECTION cs; // 全局变量

int main()
{
#ifdef _DEBUG
	CheckMemoryLeak;
#endif

	cout << "---------------------start------------------------\n";

	//InitializeCriticalSection(&cs);
	g_list = nullptr;
	g_syncLocker = SyncLocker::createNew();

	HANDLE hThread, hThread_one, hThread_three;
	hThread = 0;
	hThread_one = 0;
	hThread_three = 0;
	DWORD dwThreaID = 0L;
	hThread_one = CreateThread(nullptr, 0, thread_one, nullptr, 0, nullptr);		//第4个参数	 this 	struThreadInfo* thd = new(std::nothrow) struThreadInfo;
	hThread = CreateThread(nullptr, 0, thread_two, nullptr, 0, &dwThreaID);
	hThread_three = CreateThread(nullptr, 0, thread_three, nullptr, 0, nullptr);

	/*--------------------------------*/
	GList *gList = nullptr;
	//链表加入
	for (int i = 0; i < 10; i++)
	{
		char *p = new char[10];
		memset(p, 0, 10);

		memcpy(p, "abcdefg", strlen("abcdefg"));
		gList = g_list_append(gList, p);	//尾部插入
	}

	//首位插入
	for (int i = 0; i < 2; i++)
	{
		char *p = new char[10];
		memset(p, 0, 10);

		memcpy(p, "zzzzz", strlen("zzzzz"));
		gList = g_list_prepend(gList, p);
	}

	//中间插入
	char *kk = new char[10];
	memset(kk, 0, 10);

	memcpy(kk, "kkkkkkk", strlen("kkkkkkk"));
	gList = g_list_insert(gList, kk, 2);

		
	//遍历链表
	GList* list = nullptr;
	for (list = gList; list; list = list->next)
	{
		char *p = (char *)list->data;
		cout << "--" << p << endl;
	}

	//释放内存
	list = nullptr;
	for (list = gList; list; list = list->next)
	{
		char *p = (char *)list->data;
		if (p)
		{
			delete[] p;
		}
	}
	g_list_free(gList);
	gList = nullptr;
	
	/*--------------------------------*/
	//线程结束   	  
	if (hThread_one)
	{
		//WaitForSingleObject(hThread_one, 500);
		WaitForSingleObject(hThread_one, INFINITE);
		CloseHandle(hThread_one);
		hThread_one = 0;
	}

	if (hThread)
	{
		WaitForSingleObject(hThread, INFINITE);
		CloseHandle(hThread);
		hThread = 0;
	}

	if (hThread_three)
	{
		WaitForSingleObject(hThread_three, INFINITE);
		CloseHandle(hThread_three);
		hThread_three = 0;
	}

	//内存释放
	list = nullptr;
	for (list = g_list; list; list = list->next)
	{
		struTestData *testdata = (struTestData *)list->data;
		if (testdata)
		{
			delete testdata;
		}
	}
	g_list_free(g_list);
	g_list = nullptr;

	if (g_syncLocker)
	{
		g_syncLocker->reclaim();
		g_syncLocker = nullptr;
	}
	//DeleteCriticalSection(&cs);

	//Sleep(2000);
	cout << "---------------------end------------------------\n";


	cout << "--------------------test char*------------------\n";
	struTestChar* task = new(std::nothrow) struTestChar;
	if (!task)
	{
		return -1;
	}
	memset(task, 0, sizeof(*task));
	
	struTest *test = new struTest;
	memset(test, 0, sizeof(*test));

	
	delete task;
	delete test;
	//struTest test;
	//memset(&test, 0, sizeof(test));

	cout << "--------------------test char* endl-------------\n";

	//system("pause");

	return 0;

}

unsigned long __stdcall thread_one(void* pParam)
{
	//struThreadInfo* thd = (struThreadInfo*)pParam;

	//while(true)
	//{
		if (g_syncLocker)	g_syncLocker->Lock();
		//EnterCriticalSection(&cs);
		for (int i = 0; i < 200; i++)
		{
			struTestData* testdata = new struTestData;
			//memset(testdata, 0, sizeof(*testdata));	//1种初始化
			testdata->index = i;
			testdata->nthread = 1;
			memcpy(testdata->data, "xc111", strlen("xc111"));

			g_list = g_list_append(g_list, testdata);	//尾部插入
			//char *p = new char[10];
			//memset(p, 0, 10);

			//memcpy(p, "xc111", strlen("xc111"));
			//g_list = g_list_append(g_list, p);	//尾部插入
		}
		//LeaveCriticalSection(&cs);
		if (g_syncLocker)	g_syncLocker->unLock(); 
	//}

	return 0L;
}

unsigned long __stdcall thread_two(void* pParam)
{
	//while (true)
	//{
		if (g_syncLocker)	g_syncLocker->Lock();
		//EnterCriticalSection(&cs);
		for (int i = 0; i < 200; i++)
		{
			struTestData* testdata = new struTestData;
			//memset(testdata, 0, sizeof(*testdata));	1种初始化
			testdata->index = i;
			testdata->nthread = 2;
			memcpy(testdata->data, "xc222", strlen("xc222"));

			g_list = g_list_append(g_list, testdata);	//尾部插入
			//char *p = new char[10];
			//memset(p, 0, 10);

			//memcpy(p, "xc222", strlen("xc222"));
			//g_list = g_list_append(g_list, p);	//尾部插入
		}
		//LeaveCriticalSection(&cs);
		if (g_syncLocker)	g_syncLocker->unLock(); 
	//}

	return 0L;
}

unsigned long __stdcall thread_three(void* pParam)
{
	Sleep(10);
	/*while (true)
	{*/
		//遍历链表
		if (g_syncLocker)	g_syncLocker->Lock();
		//EnterCriticalSection(&cs);
		GList* list = nullptr;
		for (list = g_list; list; list = list->next)
		{
			struTestData* testdata = (struTestData *)list->data;

			//cout << "线程：" << testdata->nthread << "	第" << testdata->index << "个	" << "数据：" << testdata->data << endl;
			char stemp[30] = { 0 };
			sprintf(stemp, "线程：%d	第%d个	数据：%s", testdata->nthread, testdata->index, testdata->data);
			cout << stemp << endl;
			//char *p = (char *)list->data;
			//cout << "--" << p << endl;
		}
		//LeaveCriticalSection(&cs);
		if (g_syncLocker)	g_syncLocker->unLock();
	//}

	return 0L;
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
