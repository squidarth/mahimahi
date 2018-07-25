#include "red_packet_queue.hh"
#include <algorithm>

using namespace std;

REDPacketQueue::REDPacketQueue( const string & args)
  : DroppingPacketQueue(args), drop_log_(), queue_depths_()
{
    drop_log_.reset(new std::ofstream(red_debug_log));
    if (not drop_log_->good()) {
      throw std::runtime_error( red_debug_log + ": error opening for writing" );
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

unsigned int REDPacketQueue::average_queue_depth(  ) {
  unsigned int sum = 0;
  std::for_each(queue_depths_.begin(), queue_depths_.end(), [&sum](int i) {  sum += i; });

  if (queue_depths_.size() > 0) {
    return sum/queue_depths_.size();
  } else {
    return 0;
  }
}

void REDPacketQueue::enqueue( QueuedPacket && p )
{
    auto instantaneous_queue_size = size_packets();
    auto ratio = (average_queue_depth() * 1.0)/max_queue_depth_packets();
    std::default_random_engine generator (0);
    std::uniform_real_distribution<double> distribution (0.0,1.0);
    double threshold = distribution(generator);

    if ( (threshold > ratio) && good_with( size_bytes() + p.contents.size(),
                    size_packets() + 1 ) ) {
        accept( std::move( p ) );
    } else {
        *drop_log_ << p.contents << endl;
    }

    queue_depths_.push_back(instantaneous_queue_size);
    if (queue_depths_.size() > QUEUE_DEPTH_WINDOW) {
      queue_depths_.pop_front();
    }
    assert( good() );
}
