#pragma once

const int BUFFER_LENGTH = 4096;

struct  loop_buf
{
	unsigned int   save_ptr;
	unsigned int   load_ptr;
	//unsigned char  full_flag;
	unsigned char  buffer[BUFFER_LENGTH];
};

class CLoopBuf
{
public:
	CLoopBuf(void);
public:
	~CLoopBuf(void);
	
public:
	//data out
	static int put_buf(byte* src, struct loop_buf * dist, int count);
	// copy and change loop ptr
	static int get_buf(byte* dest, struct loop_buf * src, int count);
	// get a char and change loop ptr
	static int pop_char(byte * poped_char, struct loop_buf * src);
	static int char_num(struct loop_buf * src);
	// just copy but not change loop ptr
	static int copyfrom_buf(byte * dest, struct loop_buf * src, int count);
};
