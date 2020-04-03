#include "WorldSession.h"


WorldSession::WorldSession(uint16_t socket_id, std::shared_ptr<websocket> socket) 
	: socket_{socket_id,socket}
{

}

WorldSession::~WorldSession()
{
	socket_.handle->close();
}

void WorldSession::Send(std::vector<uint8_t> packet)
{
	socket_.handle->send(packet);
}

void WorldSession::Update()
{
	auto packets = socket_.handle->get_packets();
	for (const auto& packet : packets) {
		// handle packet
	}
}

uint16_t WorldSession::GetId()
{
	return id_;
}