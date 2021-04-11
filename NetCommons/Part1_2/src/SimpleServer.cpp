#include <iostream>
#include "net_server.h"
#include "net_tsqueue.h"
#include "net_common.h"
#include "net_connection.h"


using namespace std::chrono_literals;	// using for s, ms, us

// message_header 数据类型
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
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerAccept;
		client->Send(msg);

		return true;	// 接受所有的客户端的连接
	}


		// 如果发现一个客户端已经断开了连接，那么需要执行下面的处理过程
	virtual void OnClientDisconnect(std::shared_ptr<olc::net::connection<CustomMsgTypes> > client)
	{
		std::cout << "Removing client [" << client->GetID() << "]\n";
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

				//client->Send(msg);

				// msg 当中先存储着客户端发送时候的时间，服务器再存储自己发送的时间
				// 两个程序在同一台电脑上进行模拟，时钟相同
				std::chrono::system_clock::time_point timenow = std::chrono::system_clock::now();
				msg << timenow;
				client->Send(msg);
			}
			break;

			case CustomMsgTypes::MessageAll:
			{
				std::cout << "[" << client->GetID() << "]: Message All\n";

				olc::net::message<CustomMsgTypes> msg;
				msg.header.id = CustomMsgTypes::ServerMessage;
				msg << client->GetID();
				MessageAllClient(msg, client);
			}
			break;
		}
	} 
};


int main(int argc, char *argv[]) 
{
	CustomServer server(6000);

	server.Start();

	/*
		while 循环去获取是否有数据更新十分占用 CPU 
		后面需要考虑将主程序的函数注册到 asio 当中。当 asio 更新
		时自动调用注册函数，减小 CPU 占用
	*/
	while(true) 
	{
		//std::this_thread::sleep_for(2000ms);

		server.Update();
	}
	return  0;
}