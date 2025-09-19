#include "laggy_packet.h"

#include "callable_utils.h"

void LaggyPacketChannel::drop_out_of_order_packets() {
	while (packets.size() > 0 && packets[0].mode == MultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED && packets[0].sequence <= last_handled_ordered) {
		packets.remove_at(0);
	}
}

void LaggyPacketChannel::generate_sequence(LaggyPacket &p_packet) {
	ERR_FAIL_COND(p_packet.sequence != 0);
	switch (p_packet.mode) {
		case MultiplayerPeer::TRANSFER_MODE_UNRELIABLE:
			break;
		case MultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED:
			p_packet.sequence = ordered_sequence++;
			break;
		case MultiplayerPeer::TRANSFER_MODE_RELIABLE:
			p_packet.sequence = reliable_sequence++;
			break;
	}
}

void LaggyPacketChannel::push(const LaggyPacket &p_packet) {
	ERR_FAIL_COND(p_packet.mode != MultiplayerPeer::TRANSFER_MODE_UNRELIABLE && p_packet.sequence == 0);
	packets.push_back(p_packet);
}

std::optional<LaggyPacket> LaggyPacketChannel::take_next(double p_time) {
	drop_out_of_order_packets();
	for (Size i = 0; i < packets.size(); i++) {
		if (const LaggyPacket &packet = packets[i]; packet.time_of_delivery <= p_time) {
			bool consume = true;
			switch (packet.mode) {
				case MultiplayerPeer::TRANSFER_MODE_UNRELIABLE:
					break;
				case MultiplayerPeer::TRANSFER_MODE_UNRELIABLE_ORDERED:
					last_handled_ordered = packet.sequence;
					break;
				case MultiplayerPeer::TRANSFER_MODE_RELIABLE:
					if (packet.sequence == last_handled_reliable + 1) {
						last_handled_reliable = packet.sequence;
					} else {
						consume = false;
					}
					break;
			}
			if (consume) {
				std::optional result = { packet };
				packets.remove_at(i);
				return result;
			}
		}
	}
	return {};
}
