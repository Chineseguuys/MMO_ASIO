#ifndef __NET_SERVER_H__
#define __NET_SERVER_H__

#include "net_common.h"
#include "net_tsqueue.h"
#include "net_message.h"
#include "net_connection.h"


namespace olc
{
	namespace net
	{
		template <typename T>
		class server_interface
		{
		public:
			// 创建一个服务器，并在特定的端口上进行监听
			server_interface(uint16_t port)
				: m_asioAcceptor(m_asioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
			{

			}

			virtual ~server_interface()
			{
				this->Stop();
			}

			bool Start()
			{
				try
				{
					// 把接受客户端连接的任务添加到上下文当中去， 然后让上下文在新的线程当中运行
					this->WaitForClientConnection();

					// 在新的线程当中执行上下文的循环过程
					// 调用了 .run() 函数，上下文才会开始事件循环
					m_threadContext = std::thread([this]() { m_asioContext.run(); });
				}
				catch (std::exception &e)
				{
					std::cerr << "[Server] Exception: " << e.what() << '\n';
					return  false;
				}

				std::cout << "[Server] Started! \n";
				return  true;
			}

			void Stop()
			{
				// 先结束上下文的循环过程
				this->m_asioContext.stop();
				// 然后就上下文所在的子线程 join 到当前的主线程当中，主线程会等待 上下文 所在的子线程
				// 执行结束
				if (m_threadContext.joinable()) m_threadContext.join();

				std::cout << "[Server] Stopped! \n";

			}

			void WaitForClientConnection()
			{
				// 这里是异步的，这个函数会立刻返回，相应的事件（接受新的连接）会添加到上下文的事件队列当中去。
				// 当事件发生的时候，回调函数(下面的 labmda 函数) 将会被执行
				this->m_asioAcceptor.async_accept(
					// 新的连接出现，就会有一个 socket 和其对应，asio 自动将这个 socket 传递给回调函数
					[this] (std::error_code ec, asio::ip::tcp::socket socket)
					{
						if (!ec)
						{
							std::cout << "[Server] New connection: " << socket.remote_endpoint() << '\n';

							std::shared_ptr<connection<T> > newconn = 
								std::make_shared<connection<T> >(connection<T>::owner::server,
									m_asioContext, std::move(socket), m_qMessageIn); 

							// 服务器通过一定的规则来选择是否拒绝这个连接
							if (this->OnClientConnect(newconn))
							{
								// 这个连接被允许，所以这个连接需要添加到连接队列当中
								this->m_deqConnections.push_back(std::move(newconn));
								this->m_deqConnections.back()->ConnectToClient(this->nIDCounter++);

								std::cout << "[" << this->m_deqConnections.back()->GetID() << "] Connection Approved\n";
							}
							else
							{
								std::cout << "[-----] Connection Denied" << '\n';
							}

						}
						else 
						{
							std::cout << "[Server] New Connection Error: " << ec.message() << '\n';
						}
						// 一次连接事件处理完成之后，这个事件就会从上下文的事件列表中删除；如果要反复的执行这个事件（接受新的连接）
						// 那么就需要在回调函数当中重新将事件注册到上下文当中去
						this->WaitForClientConnection();
					}
				);
			}

			void MessageClient(std::shared_ptr<connection<T> > client, const message<T> &msg)
			{
				if (client && client->IsConnected())
				{
					client->Send(msg);
				}
				else 
				{
					this->OnClientDisconnect(client);
					client.reset();
					this->m_deqConnections.erase(
						std::remove(m_deqConnections.begin(), m_deqConnections.end(),  client), m_deqConnections.end()
					);
				}
			}

			void MessageAllClient(const message<T> &msg, std::shared_ptr<connection<T> > pIgonreclient = nullptr)
			{
				bool bInvalidClientExists = false;

				for (auto& client :  this->m_deqConnections) 
				{
					if (client && client->IsConnected())
					{
						if (client != pIgonreclient)
								client->Send(msg);
					}
					else 
					{
						this->OnClientDisconnect(client);
						client.reset();
						bInvalidClientExists = true;
					}
				}

				if (bInvalidClientExists) 
				{
					this->m_deqConnections.erase(
						std::remove(this->m_deqConnections.begin(), m_deqConnections.end(), nullptr), 
						this->m_deqConnections.end()
					);
				}
			}

			void Update(size_t nMaxMessages = -1) 
			{
				size_t nMessageCount = 0;
				#ifdef __DEBUG_OUT__
					//std::cout << "server message in : " << m_qMessageIn.count() << '\n';
				#endif
				while (nMessageCount < nMaxMessages && !m_qMessageIn.empty())
				{
					auto msg = m_qMessageIn.pop_front();

					OnMessage(msg.remote, msg.msg);

					nMessageCount++;
				}
			}

		protected:
			// 是否接受一个客户端的连接
			// 具体的客户端类，应当重载这个方法
			virtual bool OnClientConnect(std::shared_ptr<connection<T> > client)
			{
				return false;
			}


			// 如果发现一个客户端已经断开了连接，那么需要执行下面的处理过程
			virtual void OnClientDisconnect(std::shared_ptr<connection<T> > client)
			{

			}

			// 如果有信息到达的话，需要执行下面的处理过程
			virtual void OnMessage(std::shared_ptr<connection<T> > client, message<T> &msg) 
			{

			}

		protected:
			// 一个线程安全的队列
			tsqueue<owned_message<T> > m_qMessageIn;

			std::deque<std::shared_ptr<connection<T> > > m_deqConnections;

			asio::io_context m_asioContext;
			std::thread m_threadContext;

			// 用于处理连接建立的过程
			asio::ip::tcp::acceptor m_asioAcceptor;

			// 每一个客户端需要使用一个唯一的 id 来进行区分
			uint32_t nIDCounter = 10000;
		};
	}
}


#endif