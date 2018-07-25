/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef RED_PACKET_QUEUE_HH
#define RED_PACKET_QUEUE_HH

#include <string>
#include "util.hh"
#include <iostream>

#include <fstream>
#include <memory>
#include <deque>
#include <random>
#include "dropping_packet_queue.hh"

const std::string red_debug_log = "debug_log.log";
const int QUEUE_DEPTH_WINDOW = 2048;
/*
   Random Early Detection (RED) AQM Implementation
*/
class REDPacketQueue : public DroppingPacketQueue
{
private:
    //Configuration parameters
    std::unique_ptr<std::ofstream> drop_log_;
    const static unsigned int PACKET_SIZE = 1504;
    std::deque<int> queue_depths_;

    const std::string & type( void ) const override
    {
        static const std::string type_ { "red" };
        return type_;
    }

    unsigned int average_queue_depth();
    unsigned int max_queue_depth_packets() const;

public:
//    using DroppingPacketQueue::DroppingPacketQueue;

    REDPacketQueue( const std::string & args );
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
