/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#ifndef DROP_TAIL_PACKET_QUEUE_HH
#define DROP_TAIL_PACKET_QUEUE_HH

#include "dropping_packet_queue.hh"
#include <iostream>

#include <cassert>
#include <memory>
#include <limits>
#include <fstream>
#include "util.hh"
#include "ezio.hh"

const std::string debug_log = "debug_log.log";

class DropTailPacketQueue : public DroppingPacketQueue
{

private:

    std::unique_ptr<std::ofstream> drop_log_;
    virtual const std::string & type( void ) const override
    {
        static const std::string type_ { "droptail" };
        return type_;
    }

public:
    using DroppingPacketQueue::DroppingPacketQueue;

    DropTailPacketQueue(const std::string & args ) : DroppingPacketQueue(args), drop_log_() {
      drop_log_.reset(new std::ofstream(debug_log));
      if (not drop_log_->good()) {
        throw std::runtime_error( debug_log + ": error opening for writing" );
      }

      *drop_log_ << "Drop Log" << std::endl;
    }


    void enqueue( QueuedPacket && p ) override
    {
        if ( good_with( size_bytes() + p.contents.size(),
                        size_packets() + 1 ) ) {
            accept( std::move( p ) );
        } else {
          *drop_log_ << p.contents << std::endl;

        }

        assert( good() );
    }
};

#endif /* DROP_TAIL_PACKET_QUEUE_HH */ 
