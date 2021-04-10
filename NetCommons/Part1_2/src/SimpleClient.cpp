#include <iostream>
#include "net_message.h"
#include "net_common.h"
#include "net_client.h"


enum class CustomMsgTypes : uint32_t
{
	ServerAccept,
	ServerDeny,
	ServerPing,
	MessageAll,
	ServerMessage,
};

class CustomClient : public olc::net::client_interface<CustomMsgTypes>
{
public:
	void PingServer()
	{
		std::cout << "Ping Server\n";
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::ServerPing;

		std::chrono::system_clock::time_point timenow = \
			std::chrono::system_clock::now();

		msg << timenow;

		#ifdef __DEBUG_OUT__
			std::cout << "msg body size = " << msg.header.size << '\n';
		#endif

		Send(msg);
	}


	void MessageAll()
	{
		olc::net::message<CustomMsgTypes> msg;
		msg.header.id = CustomMsgTypes::MessageAll;

		Send(msg);
	}
};



int main(int argc, char *argv[]) 
{
	char ch;
	bool bQuit = false;
	CustomClient c;
	c.Connect("127.0.0.1", 6000);

	while (!bQuit)
	{
		std::cin >> ch;
		if (ch == 'p') c.PingServer();
		if (ch == 'a') c.MessageAll();

		if (c.IsConnected())
		{
			if (!c.Incoming().empty())
			{
				auto msg = c.Incoming().pop_front().msg;

				switch (msg.header.id)
				{
					case CustomMsgTypes::ServerAccept:
					{
						std::cout << "Server Acceptd Connection\n";
					}
					break;

					case CustomMsgTypes::ServerPing:
					{
						std::chrono::system_clock::time_point timenow = \
							std::chrono::system_clock::now();

						std::chrono::system_clock::time_point timethen;
						msg >> timethen;

						std::cout << "Ping: " << std::chrono::duration<double>(timenow - timethen).count() << '\n'; 
					}
					break;

					case CustomMsgTypes::ServerMessage:
					{
						uint32_t clientID;
						msg >> clientID;
						std::cout << "Hello from [" << clientID << "]\n";
					}
					break;
				}
			}
		}
		else 
		{
			std::cout << "Server Down\n";
			bQuit  = true;
		}
	}


	return  0;
}