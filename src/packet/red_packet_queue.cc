#include "red_packet_queue.hh"
#include <algorithm>

using namespace std;

REDPacketQueue::REDPacketQueue( const string & args)
  : DroppingPacketQueue(args),
    wq_(get_float_arg(args, "wq")),
    min_thresh_(get_float_arg(args, "minthresh")),
    max_thresh_(get_float_arg(args, "maxthresh"))
{
  if ( wq_ == 0.0 || min_thresh_ == 0.0 || max_thresh_ == 0.0 ) {
    throw runtime_error( "RED queue must have wq, minthresh, and maxthresh arguments." );
  }

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

void REDPacketQueue::enqueue( QueuedPacket && p )
{
    auto instantaneous_queue_size = size_packets();
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
    }

    weighted_average_ = (instantaneous_queue_size * wq_ ) + (1- wq_) * weighted_average_;

    assert( good() );
}
