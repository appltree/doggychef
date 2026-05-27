
#ifndef _SINT_H_
#define _SINT_H_


class SInt
{
public:
    
    SInt();
    SInt(int val);
    SInt(unsigned char* buff);
    
    int get();
    void set(int val);
    void add(int arg);
    void sub(int arg);
    
    void store(unsigned char* buff);
	
private:
    unsigned char m_buff[4];
};

#endif // _SINT_H_


