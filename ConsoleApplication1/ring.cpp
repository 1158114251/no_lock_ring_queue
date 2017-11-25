# include <stdio.h>
# include <stdlib.h>
# include <stdio.h>
# include <assert.h>
# include <string.h>
typedef  unsigned int uint32_t;
#define  pthread_mutex_t int
//判断x是否是2的次方  
#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))  
//取a和b中最小值  
#define min(a, b) (((a) < (b)) ? (a) : (b))  

typedef struct ring_buffer
{
	char   * buffer;     //缓冲区  
	uint32_t     size;       //大小  
	uint32_t     in;         //入口位置  
	uint32_t     out;        //出口位置  
	pthread_mutex_t *f_lock;    //互斥锁  
}* ring_point;

//初始化缓冲区  
struct ring_buffer* ring_buffer_init(uint32_t size, pthread_mutex_t *f_lock)
{
	struct ring_buffer *ring_buf = NULL;
	char * buffer;
	if (!is_power_of_2(size))
	{
		fprintf(stderr, "size must be power of 2.\n");
		return ring_buf;
	}
	buffer = (char *)malloc(size);
	if (!buffer)
	{
		fprintf(stderr, "Failed to malloc ring memory\n");
		return ring_buf;
	}
	ring_buf = (struct ring_buffer *)malloc(sizeof(struct ring_buffer));
	if (!ring_buf)
	{
		fprintf(stderr, "Failed to malloc memory\n");
		return ring_buf;
	}
	memset(ring_buf, 0, sizeof(struct ring_buffer));
	ring_buf->buffer = buffer;
	ring_buf->size = size;
	ring_buf->in = 0;
	ring_buf->out = 0;
	ring_buf->f_lock = f_lock;
	return ring_buf;
}

//释放缓冲区  
void ring_buffer_free(struct ring_buffer *ring_buf)
{
	if (ring_buf)
	{
		if (ring_buf->buffer)
		{
			free(ring_buf->buffer);
			ring_buf->buffer = NULL;
		}
		free(ring_buf);
		ring_buf = NULL;
	}
}


//缓冲区的长度  
uint32_t __ring_buffer_len(const struct ring_buffer *ring_buf)
{
	return (ring_buf->in - ring_buf->out);
}


//从缓冲区中取数据  
uint32_t __ring_buffer_get(struct ring_buffer *ring_buf, char * buffer,volatile uint32_t size)
{
	uint32_t len = 0;
	if (!(ring_buf && buffer))
	{
		return 0;
	}

	size = min(size, ring_buf->in - ring_buf->out);
	
	// 内存屏障

	len = min(size, ring_buf->size - (ring_buf->out & (ring_buf->size - 1)));
	
	memcpy(buffer, ring_buf->buffer + (ring_buf->out & (ring_buf->size - 1)), len);
	
	
	memcpy(buffer + len, ring_buf->buffer, size - len);
	ring_buf->out += size;
	return size;
}


//向缓冲区中存放数据  
uint32_t __ring_buffer_put(struct ring_buffer *ring_buf, char *buffer,volatile uint32_t size)
{
	assert(ring_buf || buffer);
	uint32_t len = 0;
	size = min(size, ring_buf->size - ring_buf->in + ring_buf->out);
	/* first put the data starting from fifo->in to buffer end */
	len = min(size, ring_buf->size - (ring_buf->in & (ring_buf->size - 1)));
	memcpy(ring_buf->buffer + (ring_buf->in & (ring_buf->size - 1)), buffer, len);
	/* then put the rest (if any) at the beginning of the buffer */
	memcpy(ring_buf->buffer, buffer + len, size - len);
	ring_buf->in += size;
	return size;
}

uint32_t ring_buffer_len(const struct ring_buffer *ring_buf)
{
	uint32_t len = 0;
//	pthread_mutex_lock(ring_buf->f_lock);
	len = __ring_buffer_len(ring_buf);
//	pthread_mutex_unlock(ring_buf->f_lock);
	return len;
}

uint32_t ring_buffer_get(struct ring_buffer * ring_buf, char *buffer, uint32_t size)
{
	uint32_t ret;
	//pthread_mutex_lock(ring_buf->f_lock);
	ret = __ring_buffer_get(ring_buf, buffer, size);
	//buffer中没有数据  
	if (ring_buf->in == ring_buf->out)
		ring_buf->in = ring_buf->out = 0;
	//pthread_mutex_unlock(ring_buf->f_lock);
	return ret;
}

uint32_t ring_buffer_put(struct ring_buffer *ring_buf, char *buffer, uint32_t size)
{
	uint32_t ret;
	//pthread_mutex_lock(ring_buf->f_lock);
	ret = __ring_buffer_put(ring_buf, buffer, size);
	//pthread_mutex_unlock(ring_buf->f_lock);
	return ret;
}


int main(void)
{
	char data[] = "data";
	char rdata[5] = { 0 };
	// 分配缓冲区
	ring_point ring_oo;
	if ( ring_oo = ring_buffer_init(16, 0) ,!ring_oo)
	{
		return 0;
	}

	
	__ring_buffer_put(ring_oo, data, sizeof(data));


	int count_recv_size = ring_buffer_get(ring_oo, rdata, 5);
	printf("%d ___ %s ", count_recv_size, rdata);
	getchar();
	return 0;

}