#pragma once

#ifndef WORLD_H
#define WORLD_H

#include "WorldSession.h"
#include "WorldSocketMgr.h"

class World {
public: // public interface
	static World* Instance();

	void Update();

	void AddSession(std::shared_ptr<WorldSession> session);
	void RemoveSession(uint16_t);

	WorldSocketMgr* GetSocketManager();
private: // constructors
	World();
	~World();
private: // services
	std::unique_ptr<WorldSocketMgr> socket_mgr_;

private: // world internal
	std::unordered_map<uint16_t, std::shared_ptr<WorldSession>> sessions_;
	uint16_t session_counter_;
}; // class World

#endif // WORLD_H