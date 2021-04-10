#include <iostream>
#include "net_server.h"
#include "net_tsqueue.h"
#include "net_common.h"
#include "net_connection.h"


using namespace std::chrono_literals;

enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

class CustomServer : public olc::net::server_interface<CustomMsgTypes>
{
public:
	CustomServer(uint16_t port) 
		: olc::net::server_interface<CustomMsgTypes> (port)
	{

	}

protected:
	// 是否接受一个客户端的连接
	virtual bool OnClientConnect(std::shared_ptr<olc::net::connection<CustomMsgTypes> > client)
	{
		return true;
	}


		// 如果发现一个客户端已经断开了连接，那么需要执行下面的处理过程
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes> > client)
	{

	}

			// 如果有信息到达的话，需要执行下面的处理过程
	virtual void OnMessage(std::shared_ptr<olc::net::connection<CustomMsgTypes> > client, 
			olc::net::message<CustomMsgTypes> &msg) 
	{
		switch(msg.header.id)
		{
			case CustomMsgTypes::ServerPing:
			{
				std::cout << "[" << client->GetID() << "]: Server Ping\n";

				client->Send(msg);
			}
			break;
		}
	} 
};


int main(int argc, char *argv[]) 
{
	CustomServer server(6000);

	server.Start();

	while(true) 
	{
		std::this_thread::sleep_for(2000ms);

		server.Update();
	}
	return  0;
}