
/**************************************************

Implement a thread-safe circular queue

2018年11月2日11:21:56

BY：zpf

***************************************************/

#include "ring.h"

#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))  
#define WAIT_TIME_OUT   5
#define LIB_TOSTRING(s) #s    
#define LIB_STRINGIFY(s)         LIB_TOSTRING(s)
#define __CHECK_IN_VALUE(cond) do {                                               \
if (!(cond)) {\
printf("Assertion %s failed at %s:%d\n", \
LIB_STRINGIFY(cond), __FILE__, __LINE__);                 \
abort();                                                        \
}                                                                   \
} while (0)
#define   __MIN_FIND(A, B)  (((A) < (B)) ? (A) : (B))
#define  __MIN_RET( __ARG_A_SOURCE ,__ARG_B_SOURCE , __ARG_A_DST , __ARG_B_DST, __RESULT)   do \
{               \
	__ARG_A_DST=__ARG_A_SOURCE;\
    __ARG_B_DST=__ARG_B_SOURCE;\
	__RESULT = __MIN_FIND(__ARG_A_DST, __ARG_B_DST); \
}while (0)


#define  __SAFE_GET( _NO_ )   do\
{\
EnterCriticalSection(&ring_buf->cs_safe); \
ret = __ring_buffer_get(ring_buf, buffer, size); \
LeaveCriticalSection(&ring_buf->cs_safe); \
}while (0);

#define  __SAFE_PUT(_NO_)   do\
{\
	EnterCriticalSection(&ring_buf->cs_safe); \
	ret = __ring_buffer_put(ring_buf, buffer, size); \
	LeaveCriticalSection(&ring_buf->cs_safe); \
}while (0);


/*

init a buffer

*/ 
struct ring_buffer* ring_buffer_init(uint32_t size)
{
    char * buffer;
	struct ring_buffer *ring_buf = NULL;

	if (!is_power_of_2(size))
	{
		fprintf(stderr, "Size must be power of 2.\n");
		goto end;
	}

	if (buffer = (char *)malloc(size),!buffer)
	{
		fprintf(stderr, "Failed to malloc ring memory\n");
		goto end;
	}
	
	if (ring_buf = (struct ring_buffer *)malloc(sizeof(struct ring_buffer)),!ring_buf)
	{
		fprintf(stderr, "Failed to malloc memory\n");
		goto malloc_ring;
	}
	

	/*lazy*/
	memset(ring_buf, 0, sizeof(struct ring_buffer));
	ring_buf->in = 0;
	ring_buf->out = 0;
	ring_buf->size = size;
	ring_buf->buffer = buffer;
	InitializeCriticalSection(&ring_buf->cs_safe);
	InitializeCriticalSection(&ring_buf->cs_multit_put);
	InitializeCriticalSection(&ring_buf->cs_multit_get);
    ring_buf->wake_put_event  = CreateEvent(NULL, FALSE, FALSE, NULL);
	ring_buf->wake_get_event = CreateEvent(NULL, FALSE, FALSE, NULL);

return 	ring_buf;
/*free memory*/
malloc_ring:
free(buffer);
end :	
	return NULL;
}

 
void ring_buffer_free(struct ring_buffer *ring_buf)
{
	if (ring_buf)
	{
		if (ring_buf->buffer)
		{
			free(ring_buf->buffer);
			ring_buf->buffer = NULL;
		    CloseHandle(ring_buf->wake_put_event);
			CloseHandle(ring_buf->wake_get_event);
			DeleteCriticalSection(&ring_buf->cs_safe);
			DeleteCriticalSection(&ring_buf->cs_multit_put);
			DeleteCriticalSection(&ring_buf->cs_multit_get);
		}
		free(ring_buf);
		ring_buf = NULL;
	}
	
	 
}



uint32_t __ring_buffer_use_len( struct ring_buffer *ring_buf)
{
	__CHECK_IN_VALUE(ring_buf);
	return (ring_buf->in - ring_buf->out);
}



uint32_t __ring_buffer_get(struct ring_buffer *  ring_buf, char * buffer, uint32_t size)
{
	__CHECK_IN_VALUE(ring_buf&&buffer);
	uint32_t  len = 0, tmp_arga, tmp_argb;
    __MIN_RET(size, ring_buf->in - ring_buf->out, tmp_arga, tmp_argb, size);
    __MIN_RET(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)) ,tmp_arga,tmp_argb,len);
	memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);
	memcpy(buffer + len, ring_buf->buffer, size - len);
	ring_buf->out += size;
	return size;
}


 
uint32_t __ring_buffer_put( struct ring_buffer *ring_buf, char *buffer, uint32_t size)
{
	__CHECK_IN_VALUE(ring_buf&&buffer);
	uint32_t len = 0, tmp_arga, tmp_argb;
    __MIN_RET(size, ring_buf->size - ring_buf->in + ring_buf->out, tmp_arga, tmp_argb, size);
	__MIN_RET(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)), tmp_arga, tmp_argb, len);
	memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
	memcpy(ring_buf->buffer, buffer + len, size - len);
	ring_buf->in += size;
	return size;
}


uint32_t ring_buffer_sur_len( struct ring_buffer *ring_buf)
{
	uint32_t ret ;
	EnterCriticalSection(&ring_buf->cs_safe);
	ret = ring_buf->size - __ring_buffer_use_len(ring_buf);
	LeaveCriticalSection(&ring_buf->cs_safe);
	return ret;
}

uint32_t ring_buffer_use_len( struct ring_buffer *ring_buf)
{
	uint32_t ret ;
	EnterCriticalSection(&ring_buf->cs_safe);
	ret = __ring_buffer_use_len(ring_buf);
	LeaveCriticalSection(&ring_buf->cs_safe);
	return ret;
}





uint32_t ring_buffer_get( struct ring_buffer * ring_buf, char *buffer, uint32_t size)
{
	uint32_t ret;
	__SAFE_GET(ret);
	return ret;
}




uint32_t ring_buffer_put( struct ring_buffer *ring_buf, char *buffer, uint32_t size)
{
	uint32_t ret; 
	__SAFE_PUT(ret);
	return ret;
}



/*

一个阻塞的队列,条件变量的引入意味着，需要注意查看条件，而非单纯相信通知唤醒。
唤醒的bug Too many，在这个业务场景。
*/
uint32_t ring_buf_put_block(struct ring_buffer *ring_buf, char *buffer, uint32_t size, uint32_t  time_out)
{
	EnterCriticalSection(&ring_buf->cs_multit_put);
	uint32_t ret=0, cnt_max_loop = time_out / WAIT_TIME_OUT + 1;;
	while (ring_buffer_sur_len(ring_buf)<size && cnt_max_loop)
	 {
	
		 /*wait,it is good*/
		 WaitForSingleObject(ring_buf->wake_put_event, WAIT_TIME_OUT);
		 if (time_out != MAX_UNSIGNED_SIZE)
			 cnt_max_loop--;
	     
	 }
	if (ring_buffer_sur_len(ring_buf) < size)
		goto end;
	 __SAFE_PUT(ret);
	SetEvent(ring_buf->wake_get_event);
end:
	LeaveCriticalSection(&ring_buf->cs_multit_put);
	return ret;
}



uint32_t ring_buf_get_block(struct ring_buffer * ring_buf, char *buffer, uint32_t size, uint32_t  time_out)
{
	EnterCriticalSection(&ring_buf->cs_multit_get);
	uint32_t ret=0,cnt_max_loop= time_out / WAIT_TIME_OUT + 1 ;
	while (ring_buffer_use_len(ring_buf)<size && cnt_max_loop)
	{
		/*wait*/
		WaitForSingleObject(ring_buf->wake_get_event, WAIT_TIME_OUT);
		if (time_out != MAX_UNSIGNED_SIZE)
			cnt_max_loop--;
	}
	if (ring_buffer_use_len(ring_buf) < size)
		goto end;
	__SAFE_GET(ret);
	SetEvent(ring_buf->wake_put_event);
end:
LeaveCriticalSection(&ring_buf->cs_multit_get);
return ret;
}


/*Multiple threads need to be locked for in/out*/
uint32_t ring_buf_try_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size)
{
	EnterCriticalSection(&ring_buf->cs_multit_put);
	uint32_t ret=0;
	if(ring_buffer_sur_len(ring_buf)<size)
	{
		goto end;
	}
	__SAFE_PUT(ret);
	SetEvent(ring_buf->wake_get_event);
end:
	LeaveCriticalSection(&ring_buf->cs_multit_put);
	return ret;
}




uint32_t ring_buf_try_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size)
{
	EnterCriticalSection(&ring_buf->cs_multit_get);
	uint32_t ret=0;
	if(ring_buffer_use_len(ring_buf)<size)
	{
		
		goto end;
	}
	__SAFE_GET(ret);
	SetEvent(ring_buf->wake_put_event);
end:
	LeaveCriticalSection(&ring_buf->cs_multit_get);
	return ret;
}





uint32_t  ring_buf_rm(struct ring_buffer * ring_buf, char *buffer, uint32_t size)
{

	uint32_t ret = 0;
	if (ring_buffer_use_len(ring_buf) < size)
		goto end;

	EnterCriticalSection(&ring_buf->cs_safe); 
		ring_buf->out += size;
	LeaveCriticalSection(&ring_buf->cs_safe); 

	SetEvent(ring_buf->wake_put_event);
end:
	return ret;

}
