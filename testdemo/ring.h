#ifndef _RING_BUFFER_
#define _RING_BUFFER_

# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <string.h> 
#include <stdint.h>
# include <windows.h>
#define  MAX_UNSIGNED_SIZE   ((uint32_t)-1)
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
@ring_buf ���о��
@buffer Ŀ��
@ size �������С
@return ʵ�ʻ�ȡ�Ĵ�С
*/
uint32_t ring_buffer_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);

/*
\����������
@ring_buf ���о��
@buffer Ŀ��
@ size �������С
@return ʵ�ʷ���Ĵ�С
*/
uint32_t ring_buffer_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);



/*
\����������ȡ��
@ring_buf ���о��
@buffer Ŀ��
@ size �������С
@time_out ��ʱʱ�䣬�����-1�����õȴ�
@return ���趨�˳�ʱʱ�䣬һ��Ҫע��鿴����ֵ������0��ζ�ų�ʱ�����û�г�ʱ����ô����ֵһ���Ƿ����С�����򲻻᷵�أ�һֱ������
*/
uint32_t ring_buf_get_block(struct ring_buffer * ring_buf, char *buffer, uint32_t size, uint32_t  time_out);



/*
\��������������
@ring_buf ���о��
@buffer Ŀ��
@ size ��ȡ����С
@time_out ��ʱʱ�䣬�����-1�����õȴ�
@return ���趨�˳�ʱʱ�䣬һ��Ҫע��鿴����ֵ������0��ζ�ų�ʱ��
*/

uint32_t ring_buf_put_block(struct ring_buffer *ring_buf, char *buffer, uint32_t size, uint32_t  time_out);


/*
\����������������
@ring_buf ���о��
@buffer Ŀ��
@ size ��ȡ����С
@return ����0 ʧ�ܣ����򷵻�size����0 һ����size������������ֵ
*/

uint32_t ring_buf_try_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size);


/*
\������������ȡ��
@ring_buf ���о��
@buffer Ŀ��
@ size ��ȡ����С
@return  ����0ʧ�ܣ����򷵻�size
*/
uint32_t ring_buf_try_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size);


#ifdef __cplusplus

}

#endif


#endif 