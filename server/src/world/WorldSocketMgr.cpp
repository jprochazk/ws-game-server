#include "WorldSocketMgr.h"

WorldSocketMgr::WorldSocketMgr()
	: socket_manager(),
	  on_add_socket_callback(nullptr),
	  on_close_socket_callback(nullptr)
{

}

WorldSocketMgr::~WorldSocketMgr()
{

}

void WorldSocketMgr::Update() {
	socket_manager::update();
}

void WorldSocketMgr::SetOnAddSocketCallback(SocketCallback callback) {
	on_add_socket_callback = callback;
}

void WorldSocketMgr::SetOnCloseSocketCallback(SocketCallback callback) {
	on_close_socket_callback = callback;
}

void WorldSocketMgr::on_add_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) {
	if(on_add_socket_callback)
		on_add_socket_callback(socket_id, socket);
}

void WorldSocketMgr::on_close_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) {
	if (on_close_socket_callback)
		on_close_socket_callback(socket_id, socket);
}