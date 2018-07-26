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
/*
   Random Early Detection (RED) AQM Implementation
*/
class REDPacketQueue : public DroppingPacketQueue
{
private:
    //Configuration parameters
    std::unique_ptr<std::ofstream> drop_log_;
    const static unsigned int PACKET_SIZE = 1504;
    double wq_, min_thresh_, max_thresh_;

    const std::string & type( void ) const override
    {
        static const std::string type_ { "red" };
        return type_;
    }

    double weighted_average_ = 0;
    unsigned int max_queue_depth_packets() const;

public:
    REDPacketQueue( const std::string & args );
    void enqueue( QueuedPacket && p ) override;
};

#endif
