#include "SInt.h"
#include <string.h>


unsigned char g_sckey = 156;

SInt::SInt()
{
}


SInt::SInt(int val)
{
    set(val);
}


SInt::SInt(unsigned char* buff)
{
    memcpy(m_buff, buff, 4);
}


int SInt::get()
{
    unsigned int val;
    unsigned char temp[4];
    
    for(int i = 0; i < 4; i++)
    {
        temp[i] = (unsigned char)(m_buff[i] ^ g_sckey);
    }
    
    val = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + (temp[3] << 0);
    
    return val;
}


void SInt::set(int val)
{
    m_buff[0] = (unsigned char)((val >> 24) & 0xFF);
    m_buff[1] = (unsigned char)((val >> 16) & 0xFF);
    m_buff[2] = (unsigned char)((val >> 8) & 0xFF);
    m_buff[3] = (unsigned char)(val & 0xFF);
    
    for (int i = 0; i < 4; i++)
    {
        m_buff[i] ^= g_sckey;
    }
}

void SInt::add(int arg)
{
    int val;
    unsigned char temp[4];
    
    for (int i = 0; i < 4; i++)
    {
        temp[i] = (unsigned char)(m_buff[i] ^ g_sckey);
    }
    
    val = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + (temp[3] << 0);
    val += arg;
    
    m_buff[0] = (unsigned char)((val >> 24) & 0xFF);
    m_buff[1] = (unsigned char)((val >> 16) & 0xFF);
    m_buff[2] = (unsigned char)((val >> 8) & 0xFF);
    m_buff[3] = (unsigned char)(val & 0xFF);
    
    for (int i = 0; i < 4; i++)
    {
        m_buff[i] ^= g_sckey;
    }
}


void SInt::sub(int arg)
{
    int val;
    unsigned char temp[4];
    
    for (int i = 0; i < 4; i++)
    {
        temp[i] =(unsigned char)(m_buff[i] ^ g_sckey);
    }
    
    val = (temp[0] << 24) + (temp[1] << 16) + (temp[2] << 8) + (temp[3] << 0);
    val -= arg;
    if (val < 0) val = 0;
    
    m_buff[0] = (unsigned char)((val >> 24) & 0xFF);
    m_buff[1] = (unsigned char)((val >> 16) & 0xFF);
    m_buff[2] = (unsigned char)((val >> 8) & 0xFF);
    m_buff[3] = (unsigned char)(val & 0xFF);
    
    for (int i = 0; i < 4; i++)
    {
        m_buff[i] ^= g_sckey;
    }
    
}

void SInt::store(unsigned char* buff)
{
    memcpy(buff, m_buff, 4);
    
}


