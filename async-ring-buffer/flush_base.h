#ifndef __FLUSH_BASE_H
#define __FLUSH_BASE_H

struct FlushBase
{
    int         m_type;
    int         m_buffer_id;
    int         status;
    virtual int flush()                  = 0;
    virtual int overflow( int wait = 0 ) = 0;
    int         set_buff_type( int type )
    {
        return m_type = type;
    }
    int get_buff_type()
    {
        return m_type;
    }
    virtual int set_buff_id( int __id )
    {
        return m_buffer_id = __id;
    }
    int get_buff_id()
    {
        return m_buffer_id;
    }
};
#endif