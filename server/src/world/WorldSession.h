#pragma once

#ifndef WORLD_SESSION_H
#define WORLD_SESSION_H

#include "../network/websocket.h"

struct WorldSocket {
	uint16_t id;
	std::shared_ptr<websocket> handle;
};

class WorldSession {
public:
	WorldSession(uint16_t socket_id, std::shared_ptr<websocket> socket);
	~WorldSession();

	void Send(std::vector<uint8_t> packet);
	void Update();

	uint16_t GetId();
private:
	uint16_t id_;
	
	WorldSocket socket_;

}; // class WorldSession

#endif // WORLD_SESSION_H