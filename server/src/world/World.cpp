
#include "World.h"


World::World() 
	: sessions_(), socket_mgr_(std::make_unique<WorldSocketMgr>())
{
	socket_mgr_->SetOnAddSocketCallback([this](uint16_t socket_id, std::shared_ptr<websocket> socket)
	{
		auto session = std::make_shared<WorldSession>(socket_id, socket);
		this->AddSession(session);
	});
	socket_mgr_->SetOnCloseSocketCallback([this](uint16_t socket_id, std::shared_ptr<websocket> socket) {
		this->RemoveSession(socket_id);
	});
}

World::~World()
{

}

World* World::Instance()
{
	static World instance;
	return &instance;
}

void World::Update()
{
	// accept new connections, clear old ones
	socket_mgr_->Update();

	for (auto session : sessions_) {
		session.second->Send({});
	}

	// TODO: update entities, scripts, ...
	spdlog::info("World::update, sessions: {}", sessions_.size());
}

void World::AddSession(std::shared_ptr<WorldSession> session)
{
	sessions_.insert(std::make_pair(session_counter_, session));
}

void World::RemoveSession(uint16_t session_id)
{
	sessions_.erase(session_id);
	
}

WorldSocketMgr* World::GetSocketManager()
{
	return socket_mgr_.get();
}
