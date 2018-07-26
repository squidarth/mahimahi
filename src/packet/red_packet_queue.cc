#include "timestamp.hh"
#include "red_packet_queue.hh"
#include <algorithm>
#include <cmath> 

using namespace std;

REDPacketQueue::REDPacketQueue( const string & args)
  : DroppingPacketQueue(args),
    drop_log_(),
    wq_(get_float_arg(args, "wq")),
    min_thresh_(get_float_arg(args, "minthresh")),
    max_thresh_(get_float_arg(args, "maxthresh")),
    time_at_zero_q_(0)
{
  if ( wq_ == 0.0 || min_thresh_ == 0.0 || max_thresh_ == 0.0 ) {
    throw runtime_error( "RED queue must have wq, minthresh, and maxthresh arguments." );
  }

  drop_log_.reset(new std::ofstream(debug_log_red));
  if (not drop_log_->good()) {
    throw std::runtime_error( debug_log_red + ": error opening for writing" );
  }

      *drop_log_ << "Drop Log" << std::endl;

}

unsigned int REDPacketQueue::max_queue_depth_packets (void ) const {
  if (packet_limit_) {
    return packet_limit_;
  } else if (byte_limit_ ) {
    return byte_limit_ / 100;
  } else {
      throw runtime_error( "No queue limit provided");
  }
}

QueuedPacket REDPacketQueue::dequeue( void )
{

    auto packet = DroppingPacketQueue::dequeue();
    if (size_packets() == 0) {
      time_at_zero_q_ = timestamp();
    }

    return packet;
}

void REDPacketQueue::enqueue( QueuedPacket && p )
{
    int s = 4;
    auto instantaneous_queue_size = size_packets();

    if (size_packets() == 0) {
        weighted_average_ = powf((1 - wq_), (timestamp() - time_at_zero_q_)/s) * weighted_average_;
    } else {
        weighted_average_ = (instantaneous_queue_size * wq_ ) + (1- wq_) * weighted_average_;
    }
    auto ratio = (weighted_average_)/max_queue_depth_packets();
    std::default_random_engine generator (0);
    std::uniform_real_distribution<double> distribution (min_thresh_, max_thresh_);
    double threshold = distribution(generator);
    if (ratio > max_thresh_) {
      ratio = 1;
    }
    if (ratio < min_thresh_) {
      ratio = 0;
    }

    if ( (threshold > ratio) && good_with( size_bytes() + p.contents.size(),
                    size_packets() + 1 ) ) {
        accept( std::move( p ) );
    } else {
      *drop_log_ << p.contents << std::endl;
    }


    assert( good() );
}
