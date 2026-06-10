#ifndef _RING_BUFFER_MANAGER_H
#define _RING_BUFFER_MANAGER_H

#include <list>
#include <mutex>
#include <ring-buffer.h>
struct RingBufferManager
{
    std::list< FlushBase* > ring_buffer_list;
    std::mutex              m_mtx;
    void                    overflow();
    void                    __register_ring_buff( FlushBase* ring_buff );
};
RingBufferManager* get_ring_buffer_manager();
void               register_ring_buffer( FlushBase* ring_buff );
void               overflow_ring_buff();

#endif