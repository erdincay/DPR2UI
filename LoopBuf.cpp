#include "StdAfx.h"
#include "LoopBuf.h"

CLoopBuf::CLoopBuf(void)
{
}

CLoopBuf::~CLoopBuf(void)
{
}

//data out
int CLoopBuf::put_buf(byte* src, loop_buf * dist, int count)
{
	if ((dist->save_ptr+count)>=BUFFER_LENGTH)
	{
		int firstcopy_num = BUFFER_LENGTH-dist->save_ptr;
		memcpy(dist->buffer+dist->save_ptr,src,firstcopy_num);
		int secondcopy_num = count-firstcopy_num ;
		memcpy(dist->buffer,(src+firstcopy_num),secondcopy_num);
		dist->save_ptr=secondcopy_num;
	}
	else
	{
		memcpy(dist->buffer+dist->save_ptr,src,count);
		dist->save_ptr=dist->save_ptr+count;
	}
	return count;
}

// copy and change loop ptr
int CLoopBuf::get_buf(byte* dest, struct loop_buf * src, int count)
{
	int max_copy_count;
	int copyed_count=0;

	if ((int)(src->save_ptr-src->load_ptr) >= 0)
	{
		max_copy_count= src->save_ptr - src->load_ptr;
		if (count <max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,count);
			copyed_count=count;
			src->load_ptr=src->load_ptr+count;
		}
		if (count>=max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,max_copy_count);
			copyed_count=max_copy_count;
			src->load_ptr=src->load_ptr+max_copy_count;
		}
		return copyed_count;
	}
	else if((int)(src->save_ptr-src->load_ptr) < 0)
	{	
		max_copy_count=BUFFER_LENGTH + src->save_ptr - src->load_ptr;
		if (count <= (int)(BUFFER_LENGTH - src->load_ptr))
		{
			memcpy(dest,src->buffer+src->load_ptr,count);
			copyed_count=count;
			src->load_ptr=src->load_ptr+count;
		}
		else  if ( count <max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,BUFFER_LENGTH-src->load_ptr);
			memcpy(dest+BUFFER_LENGTH-src->load_ptr,src->buffer,count-(BUFFER_LENGTH-src->load_ptr));
			copyed_count=count;
			src->load_ptr=count-(BUFFER_LENGTH-src->load_ptr);
		}
		else
		{
			memcpy(dest,src->buffer+src->load_ptr,BUFFER_LENGTH-src->load_ptr);
			memcpy(dest+BUFFER_LENGTH-src->load_ptr,src->buffer,src->save_ptr);
			copyed_count=max_copy_count;
			src->load_ptr=src->save_ptr;
		}
		return copyed_count;
	}

	return copyed_count;
}

// get a char and change loop ptr
int CLoopBuf::pop_char(byte * poped_char, struct loop_buf * src)
{
	if (src->save_ptr!=src->load_ptr)
	{
		*poped_char=src->buffer[src->load_ptr];
		src->load_ptr=(src->load_ptr+1)%BUFFER_LENGTH;
		return 0;
	}
	else
	{
		return -1;
	}
}

int CLoopBuf::char_num(struct loop_buf * src)
{
	int ret = 0;
	if ((int)(src->save_ptr-src->load_ptr) >= 0)
	{
		ret= src->save_ptr - src->load_ptr;
		return ret;
	}
	else if((int)(src->save_ptr-src->load_ptr) < 0)
	{	
		ret=BUFFER_LENGTH + src->save_ptr - src->load_ptr;
		return ret;
	}
	return ret;
}

// just copy but not change loop ptr
int CLoopBuf::copyfrom_buf(byte * dest, struct loop_buf * src, int count)
{
	int max_copy_count;
	int copyed_count=0;

	if ((int)(src->save_ptr-src->load_ptr)>0)
	{
		max_copy_count= src->save_ptr - src->load_ptr;
		if (count <max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,count);
			copyed_count=count;
		}
		if (count>=max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,max_copy_count);
			copyed_count=max_copy_count;
		}
		return copyed_count;
	}

	if((int)(src->save_ptr-src->load_ptr)<=0)
	{	
		max_copy_count=BUFFER_LENGTH + src->save_ptr - src->load_ptr;
		if (count <= (int)(BUFFER_LENGTH-src->load_ptr))
		{
			memcpy(dest,src->buffer+src->load_ptr,count);
			copyed_count=count;
		}
		else  if ( count <max_copy_count)
		{
			memcpy(dest,src->buffer+src->load_ptr,BUFFER_LENGTH-src->load_ptr);
			memcpy(dest+BUFFER_LENGTH-src->load_ptr,src->buffer,count-(BUFFER_LENGTH-src->load_ptr));
			copyed_count=count;
		}
		else
		{
			memcpy(dest,src->buffer+src->load_ptr,BUFFER_LENGTH-src->load_ptr);
			memcpy(dest+BUFFER_LENGTH-src->load_ptr,src->buffer,src->save_ptr);
			copyed_count=max_copy_count;

		}
		return copyed_count;
	}

	return copyed_count;
}
