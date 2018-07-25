#include "red_packet_queue.hh"

using namespace std;

//REDPacketQueue::REDPacketQueue( const string & args)
//  : DroppingPacketQueue(args)
//{
//
//
//}

void REDPacketQueue::enqueue( QueuedPacket && p )
{
    if ( good_with( size_bytes() + p.contents.size(),
                    size_packets() + 1 ) ) {
        accept( std::move( p ) );
    }

    assert( good() );
}
