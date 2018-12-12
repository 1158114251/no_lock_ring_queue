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
\���뻷�λ�����
*/
struct ring_buffer* ring_buffer_init(uint32_t size);


/*
\�ͷŻ��λ�����
*/
void ring_buffer_free(struct ring_buffer *ring_buf);


/*
\������ʣ��
*/
uint32_t ring_buffer_sur_len( struct ring_buffer *ring_buf);


/*
\�������Ѿ�ʹ��
*/
uint32_t ring_buffer_use_len( struct ring_buffer *ring_buf);


/*
\������ȡ��
*/
uint32_t ring_buffer_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);


/*
\����������
*/
uint32_t ring_buffer_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);




/*
\����������ȡ��
*/
uint32_t ring_buf_get_block(struct ring_buffer * ring_buf, char *buffer, uint32_t size);



/*
\��������������
*/

uint32_t ring_buf_put_block(struct ring_buffer *ring_buf, char *buffer, uint32_t size);



/*
\����������������
*/
uint32_t ring_buf_try_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);


/*
\����������������
*/
uint32_t ring_buf_try_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);


#ifdef __cplusplus

}

#endif


#endif 