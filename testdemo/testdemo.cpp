/************************************************************************
*测试demo，需注意读写参数说明
*************************************************************************/

#include <stdio.h>
#include "testdef.h"
#include "ring.h"
#include <time.h>

#define  MAX_BUFFER_SIZE  4 * 1024 * 1024
DWORD WINAPI pthread_function(LPVOID lpParam);
uint32_t InBuffer[MAX_BUFFER_SIZE] = { 0 };
uint32_t OutBuffer[MAX_BUFFER_SIZE] = { 0 };

struct ring_buffer*  handle;
int main(void)
{
	DWORD  threadId;
	uint32_t  ret;
	if (handle = ring_buffer_init(64 * 1024 * 1024), !handle)
	{
		fprintf(stderr, "%s","Error of malloc mem");
		goto exit1;
	}

	for (uint32_t i = 0; i < sizeof(InBuffer) / sizeof(uint32_t); i++)
	{
		InBuffer[i] = i;
	}

	CreateThread(NULL, 0, pthread_function, 0, 0, &threadId); // 创建线程

	while (1)
	{
		  ret = ring_buf_put_block(handle,(char *) InBuffer, sizeof(InBuffer));
		if (ret != sizeof(InBuffer))
		{
			abort();
		}
		Sleep(10);
	}

	getchar();
	ring_buffer_free(handle);

exit1:
	return 0;
}


DWORD WINAPI pthread_function(LPVOID lpParam)
{
	LARGE_INTEGER litmp;
	LONGLONG QPart1, QPart2;
	double dfMinus, dfFreq, dfTim;

	QueryPerformanceFrequency(&litmp);
	dfFreq = (double)litmp.QuadPart;
	uint32_t ret;
	while (1)
	{


		memset(OutBuffer, 0, sizeof(OutBuffer));
		QueryPerformanceCounter(&litmp);
		QPart1 = litmp.QuadPart;
		ret = ring_buf_get_block(handle, (char*)OutBuffer, sizeof(OutBuffer));
		if (ret != sizeof(OutBuffer))
		{
			abort();
		}
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;

		dfMinus = (double)(QPart2 - QPart1);
		dfTim = dfMinus / dfFreq;
#if  1
		for (uint32_t i = 0; i < sizeof(OutBuffer) / sizeof(uint32_t); i++)
		{

			if (OutBuffer[i] != i)
			{
				printf("%d  %d\n", OutBuffer[i], i);
				abort();
			}

		}
#endif
		printf("发送时间: %lf GB\n", sizeof(OutBuffer) / dfTim/1024.0/1024.0/1024.0);
	}
	return 0;
}






