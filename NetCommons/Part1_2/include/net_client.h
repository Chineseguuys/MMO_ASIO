#ifndef __NET_CLIENT_H__
#define __NET_CLIENT_H__

#include "net_common.h"
#include "net_connection.h"
#include "net_tsqueue.h"


namespace olc 
{
	namespace net 
	{
		template <typename T>
		class client_interface
		{
		public:
			client_interface() : m_socket(m_context)
			{

			}

			virtual ~client_interface()
			{
				Disconnect();
			}

		public:
			bool Connect(const std::string &host, const uint16_t port)
			{
				try
				{
					asio::ip::tcp::resolver resolver(this->m_context);
					asio::ip::tcp::resolver::results_type endpoints = 	\
						resolver.resolve(host, std::to_string(port));


					this->m_connection = std::make_unique<connection<T> >(
						connection<T>::owner::client,	// 类型
						this->m_context,	// 上下文
						asio::ip::tcp::socket(this->m_context), // 与这个连接相关的 socket 
						this->m_qMessagesIn		// 保存接收到的数据的队列
					);

					this->m_connection->ConnectToServer(endpoints);

					this->thrContext = std::thread( [this]() { this->m_context.run(); } );

					return true;

				}
				catch (std::exception &e)
				{
					std::cerr << "Client Exception: " << e.what() << '\n';
					return false;
				}
			}

			void Disconnect()
			{
				if (IsConnected())
				{
					m_connection->Disconnect();

				}

				m_context.stop();

				if (thrContext.joinable())
					thrContext.join();

				m_connection.release();
			}

			bool IsConnected()
			{
				if (m_connection)
					return  m_connection->IsConnected();
				else
					return false;
			}

		public:
			void Send(const message<T> &msg)
			{
				if (this->IsConnected())
					this->m_connection->Send(msg);
			}

			tsqueue<owned_message<T> >& Incoming()
			{
				return  m_qMessagesIn;
			}	

		protected:
			asio::io_context m_context;
			std::thread thrContext;
			asio::ip::tcp::socket m_socket;
			std::unique_ptr<connection<T> > m_connection;

		private:
			tsqueue<owned_message<T> > m_qMessagesIn;
		};
	}
}

#endif