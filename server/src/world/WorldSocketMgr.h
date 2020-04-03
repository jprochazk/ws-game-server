#pragma once

#ifndef WORLD_SOCKET_MGR_H
#define WORLD_SOCKET_MGR_H

#include "../network/socket_manager.h"

class WorldSocketMgr : public socket_manager {
public:
	typedef std::function<void(uint16_t, std::shared_ptr<websocket>)> SocketCallback;
public:
	WorldSocketMgr();
	~WorldSocketMgr();

	void Update();
	void SetOnAddSocketCallback(SocketCallback callback);
	void SetOnCloseSocketCallback(SocketCallback callback);
private:
	SocketCallback on_add_socket_callback;
	SocketCallback on_close_socket_callback;

protected:
	void on_add_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) override;
	void on_close_socket(uint16_t socket_id, std::shared_ptr<websocket> socket) override;
};


#endif // WORLD_SOCKET_MGR_H