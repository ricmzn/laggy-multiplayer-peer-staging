#ifndef LAGGYMULTIPLAYERPEER_PACKET_H
#define LAGGYMULTIPLAYERPEER_PACKET_H

#include <godot_cpp/classes/multiplayer_peer.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <optional>

using namespace godot;

struct LaggyPacket {
	typedef MultiplayerPeer::TransferMode TransferMode;
	typedef uint64_t Sequence;
	typedef int32_t Channel;
	typedef int32_t Peer;

	PackedByteArray data;
	TransferMode mode;
	Sequence sequence;
	Channel channel;
	Peer peer;
	double time_of_delivery;
};

class LaggyPacketChannel {
	Vector<LaggyPacket> packets;

	LaggyPacket::Sequence reliable_sequence = 1;
	LaggyPacket::Sequence ordered_sequence = 1;

	LaggyPacket::Sequence last_handled_reliable = 0;
	LaggyPacket::Sequence last_handled_ordered = 0;

	void drop_out_of_order_packets();

public:
	void generate_sequence(LaggyPacket& p_packet);
	void push(const LaggyPacket &p_packet);
	std::optional<LaggyPacket> take_next(double p_time);
};

#endif //LAGGYMULTIPLAYERPEER_PACKET_H
