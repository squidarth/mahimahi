#include "red_packet_queue.hh"

using namespace std;

REDPacketQueue::REDPacketQueue( const string & args)
  : DroppingPacketQueue(args), drop_log_()
{
    drop_log_.reset(new std::ofstream(red_debug_log));
    if (not drop_log_->good()) {
      throw std::runtime_error( red_debug_log + ": error opening for writing" );
    }

    *drop_log_ << "Drop Log" << std::endl;
}

unsigned int REDPacketQueue::limit_packets (void ) const {
  if (packet_limit_) {
    return packet_limit_;
  } else if (byte_limit_ ) {
    return byte_limit_ / PACKET_SIZE;
  } else {
      throw runtime_error( "No queue limit provided");
  }
}

void REDPacketQueue::enqueue( QueuedPacket && p )
{
    auto instanteous_queue_size = size_packets();

    auto ratio = (instanteous_queue_size * 1.0)/limit_packets();
    std::default_random_engine generator (0);
    std::uniform_real_distribution<double> distribution (0.0,1.0);
    double threshold = distribution(generator);

    if ( (threshold > ratio) && good_with( size_bytes() + p.contents.size(),
                    size_packets() + 1 ) ) {
        accept( std::move( p ) );
    } else {
        *drop_log_ << p.contents << endl;
    }

    assert( good() );
}
