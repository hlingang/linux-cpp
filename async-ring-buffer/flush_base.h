#ifndef __FLUSH_BASE_H
#define __FLUSH_BASE_H

struct FlushBase
{
    int         status;
    virtual int flush()                  = 0;
    virtual int overflow( int wait = 0 ) = 0;
};
#endif