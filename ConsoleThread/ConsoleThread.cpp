// ConsoleThread.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#pragma warning(disable:4996)

#ifdef _DEBUG
//for memory leak check
#define _CRTDBG_MAP_ALLOC //使生成的内存dump包含内存块分配的具体代码为止
#include <stdlib.h> 
#include <crtdbg.h>
#define CheckMemoryLeak _CrtSetDbgFlag( _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG )| _CRTDBG_LEAK_CHECK_DF)
#endif

#include <iostream>
#include <thread>

void fun1(int &n)
{
	n++;
	printf("now i is %d\n", n);
}

void fun2(char *cstr)
{
	printf("now cstr is %s\n", cstr);
	strcpy(cstr, "now change");
	printf("now change cstr is %s\n", cstr);

	delete[] cstr;
}

void fun3(std::string &sstr)
{
	printf("now string is %s\n", sstr.data());
	sstr = "Bstring";
	printf("now change sstr is %s\n", sstr.data());
}

struct A
{
	int i;
};
//引用
void fun4(A &stru)
{
	++stru.i;
	printf("now i is %d\n", stru.i);
}
//指针
void fun5(A *stru)
{
	++(stru->i);
	printf("now i is %d\n", stru->i);
}

int main()
{
#ifdef _DEBUG
	CheckMemoryLeak;
#endif

	int n = 1;
	printf("n is %d\n", n);

	std::thread t1(fun1, std::ref(n));
	t1.join();

	printf("n after thread: %d\n\n", n);


	char *cstr = new char[20];
	strcpy(cstr, "hello world");
	printf("cstr is %s\n", cstr);
	
	std::thread t2(fun2, std::ref(cstr));
	t2.join();
	
	printf("cstr after thread: %s\n\n", cstr);


	std::string sstr = "Astring";
	printf("sstr is %s\n", sstr.data());

	std::thread t3(fun3, std::ref(sstr));
	t3.join();

	printf("sstr after thread: %s\n\n", sstr.data());


	A struA{ 9 };
	printf("i is %d\n", struA.i);

	std::thread t4(fun4, std::ref(struA));
	t4.join();

	printf("i after thread: %d\n\n", struA.i);

	A *struB = &struA;
	printf("i is %d\n", struA.i);

	std::thread t5(fun5, struB);
	t5.join();

	printf("i after thread: %d\n", struA.i);

	return 0;
}


