#ifndef _RING_BUFFER_
#define _RING_BUFFER_

# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <string.h> 
#include <stdint.h>
# include <windows.h>

struct ring_buffer
{
	char   * buffer;            
	uint32_t     size;          
	uint32_t     in;          
	uint32_t     out;          
	uint32_t     is_block;   
	HANDLE wake_put_event;    
	HANDLE wake_get_event;   
	CRITICAL_SECTION  cs_safe;  
	CRITICAL_SECTION cs_multit_put;
	CRITICAL_SECTION cs_multit_get;
};
#ifdef __cplusplus           

extern "C"{

#endif

/*
\申请环形缓冲区
*/
struct ring_buffer* ring_buffer_init(uint32_t size);


/*
\释放环形缓冲区
*/
void ring_buffer_free(struct ring_buffer *ring_buf);


/*
\缓冲区剩余
*/
uint32_t ring_buffer_sur_len( struct ring_buffer *ring_buf);


/*
\缓冲区已经使用
*/
uint32_t ring_buffer_use_len( struct ring_buffer *ring_buf);


/*
\缓冲区取出
*/
uint32_t ring_buffer_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);


/*
\缓冲区放入
*/
uint32_t ring_buffer_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);




/*
\阻塞缓冲区取出
*/
uint32_t ring_buf_get_block(struct ring_buffer * ring_buf, char *buffer, uint32_t size);



/*
\阻塞缓冲区放入
*/

uint32_t ring_buf_put_block(struct ring_buffer *ring_buf, char *buffer, uint32_t size);



/*
\非阻塞缓冲区放入
*/
uint32_t ring_buf_try_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);


/*
\非阻塞缓冲区放入
*/
uint32_t ring_buf_try_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);


#ifdef __cplusplus

}

#endif


#endif 