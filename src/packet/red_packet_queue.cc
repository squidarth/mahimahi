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
    return size_packets();
  } else if (byte_limit_ ) {
    return size_bytes() / PACKET_SIZE;
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

    if ( threshold > ratio && good_with( size_bytes() + p.contents.size(),
                    size_packets() + 1 ) ) {
        accept( std::move( p ) );
    } else {
      if (threshold > ratio) {
        //*drop_log_ << "QUEUE FULL: ";
        *drop_log_ << p.contents << endl;
      } else {
        //*drop_log_ << "PROBABILISTIC DROP: ";
        *drop_log_ << p.contents << endl;
      }
    }

    assert( good() );
}
