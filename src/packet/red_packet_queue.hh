/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RED_PACKET_QUEUE_HH
#define RED_PACKET_QUEUE_HH

#include <random>
#include "dropping_packet_queue.hh"

/*
   Random Early Detection (RED) AQM Implementation
*/
class REDPacketQueue : public DroppingPacketQueue
{
private:
    //Configuration parameters
    virtual const std::string & type( void ) const override
    {
        static const std::string type_ { "red" };
        return type_;
    }
public:
    using DroppingPacketQueue::DroppingPacketQueue;

    //REDPacketQueue( const std::string & args );
    void enqueue( QueuedPacket && p ) override;
//    void enqueue( QueuedPacket && p ) override
//    {
//        if ( good_with( size_bytes() + p.contents.size(),
//                        size_packets() + 1 ) ) {
//            accept( std::move( p ) );
//        }
//
//        assert( good() );
//    }

};

#endif
