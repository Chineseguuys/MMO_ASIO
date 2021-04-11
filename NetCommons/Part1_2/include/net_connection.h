#ifndef __NET_CONNECTION_H__
#define __NET_CONNECTION_H__

#include "net_common.h"
#include "net_message.h"
#include "net_tsqueue.h"


namespace olc 
{
	namespace net 
	{
		template <typename T>
		class connection : public std::enable_shared_from_this<connection<T> >
		{

		public:
			// 当前的连接要么属于客户端要么属于服务器端
			enum class owner 
			{
				server,
				client,
			};

		public:
			// 我们需要给出：
			// 1. 这个连接属于服务器还是客户端
			// 2. 这个连接需要一个上下文来执行相应的收发操作
			// 3. 每一个连接都和一个唯一的 socket 对应
			// 4. 这个连接接收到的数据需要缓冲区存储起来
			connection(owner parent, 
				asio::io_context& asioContext, 
				asio::ip::tcp::socket socket, 
				tsqueue<owned_message<T> >& qIn)
				: m_asioContext(asioContext), m_socket( std::move(socket) ), m_qMessagesIn(qIn)
			{
				this->m_nOwnerType = parent;
			}

			virtual ~connection() {}

			uint32_t GetID() const 
			{
				return  this->id;
			}

		public:
			void ConnectToClient(uint32_t uid = 0)
			{
				// 只有服务器可以调用这个函数
				/*
				服务器调用这个函数的时候，实际上连接已经建立起来了，这里只是为了给这个连接
				设定一个 id，当然你也可以在构造函数当中设定 id
				*/
				if (this->m_nOwnerType == owner::server)
				{
					if (this->m_socket.is_open()) 
					{
						this->id = uid;
						#ifdef __DEBUG_OUT__
							std::cout <<"add read header into context\n";
						#endif
						this->ReadHeader();
					}
				}
			}

			void ConnectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
			{
				if (this->m_nOwnerType == owner::client) 
				{
					// 尝试去连接远端
					asio::async_connect(this->m_socket, endpoints,
						[this](std::error_code ec, asio::ip::tcp::endpoint endpoint)
						{
							if (!ec)
							{	
								/*
									一旦连接成功，这个回调函数就会被执行，注册读报文头事件到上下文中
								*/
								this->ReadHeader();
							}
						}
					);
				}
			}

			void Disconnect() 
			{
				//关闭 socket 的操作也是异步进行的
				if (this->IsConnected())
				{
					asio::post(this->m_asioContext,
						[this]()
						{
							this->m_socket.close();
						}
					);
				}
			}

			bool IsConnected() const
			{
				return  this->m_socket.is_open();
			}
			void StartListening() { }

		public:
			/*
			接收数据的时刻是由操作系统决定的，socket 可以读的时候就是需要接收的时候，所有的接收的过程都
			是通过事件自动触发的，但是发送就不一样了，这是由服务器或者用户主动进行的过程
			*/
			void Send(const message<T>& msg)
			{
				// 我们通过 Post 将一个写任务加入到上下文当中去，至于这个消息到底是什么时候发送出去的，
				// 则是上下文所决定的
				asio::post(
					this->m_asioContext,
					[this, msg]()
					{
						/*
						我们知道，当发送队列为空的时候，就不再执行发送事件了。如果我们这一次要发送的消息是
						消息队列中的第一个消息，那么我们需要执行添加写任务到上下文当中去。保证上下文开始监
						听可写事件

						??? 存在可能的 bug: m_qMessagesOut 当前读出来不是空，但是随后马上其中的数据被
						发送出去了，并且变成了空
						*/
						bool bWritingMessage = !this->m_qMessagesOut.empty();
						#ifdef __DEBUG_OUT__
							std::cout << "Push msg into outqueue\n";
						#endif
						this->m_qMessagesOut.push_back(msg);

						if (!bWritingMessage)
						{
							#ifdef __DEBUG_OUT__
								std::cout << "push write header into context\n";
							#endif
							this->WriteHeader();
						}
					}
				);
			}

		private:
			// 异步  在上下文准备好读取一个报文的头的时候
			void ReadHeader()
			{
				asio::async_read(this->m_socket, 
					asio::buffer(&this->m_msgTemporaryIn.header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{	
							#ifdef __DEBUG_OUT__
								std::cout << "read msg header from socket:" << "size= " << this->m_msgTemporaryIn.header.size << '\n';
							#endif
							if (this->m_msgTemporaryIn.header.size > 0)
							{
								// 从读取到的数据头中可以得到这个数据包的主体部分的长度，因此我们需要初始化暂存区域的大小
								// 以及向 上下文 注册读取主体部分。
								this->m_msgTemporaryIn.body.resize(this->m_msgTemporaryIn.header.size);
								#ifdef __DEBUG_OUT__
									std::cout << "add read body into context\n";
								#endif
								this->ReadBody();
							}
							else 
							{
								// 如果数据头给出的主体部分的长度为 0， 那么这个数据包就只有头部。此时直接将其提交给 deque 
								// 当中
								this->AddToIncomingMessageQueue();
							}
						}
						else 
						{
							std::cout << "[" << this->id << "] Read Header Fail.\n";
							this->m_socket.close();
						}
					}
				);
			}

			void WriteBody()
			{
				asio::async_write(
					this->m_socket,
					asio::buffer(this->m_qMessagesOut.front().body.data(), this->m_qMessagesOut.front().body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							#ifdef __DEBUG_OUT__
								std::cout << "msg body have write to socket\n";
							#endif
							this->m_qMessagesOut.pop_front();
							if (!m_qMessagesOut.empty())
							{
								/*
									发送完一个消息，查看发送队列中是否还有数据包发送，如果没有了，就没有必要再注册 WriteHeader了
									如果还有数据包要发送，那么就需要注册 WriteHeader
								*/
								this->WriteHeader();
							}
						}
						else 
						{
							std::cout << "["  << this->id << "] Write Body Fail.\n";
							this->m_socket.close();
						}
					}
				);
			}

			void WriteHeader() 
			{
				asio::async_write(
					this->m_socket, 
					asio::buffer(&this->m_qMessagesOut.front().header, sizeof(message_header<T>)),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							#ifdef __DEBUG_OUT__
								std::cout << "write msg header into socket\n";
							#endif
							if (this->m_qMessagesOut.front().body.size() > 0)
							{
								// 数据包的 body 长度不为 0 ，就需要发送数据 body。
								#ifdef __DEBUG_OUT__
									std::cout << "add write body into context\n";
								#endif
								this->WriteBody();
							}
							else // 这个数据包没有 body，只有一个数据头，发送完数据头，这个数据包就完成了发送
							{
								this->m_qMessagesOut.pop_front();
								/*
									发送完一个消息，查看发送队列中是否还有数据包发送，如果没有了，就没有必要再注册 WriteHeader了
									如果还有数据包要发送，那么就需要注册 WriteHeader
								*/
								if (!m_qMessagesOut.empty())
								{
									this->WriteHeader();
								}
							}
						}
						else 
						{
							std::cout << "[" << this->id << "] Write Header Fail.\n";
							this->m_socket.close();
						}
					}
				);
			}

			// 读取一个数据包的主体部分
			void ReadBody()
			{
				asio::async_read(
					this->m_socket, 
					asio::buffer(this->m_msgTemporaryIn.body.data(), this->m_msgTemporaryIn.body.size()),
					[this](std::error_code ec, std::size_t length)
					{
						if (!ec)
						{
							// 读取成功，将数据存储到 deque 当中
							std::cout << "have read body into m_msgTemporaryIn\n";
							this->AddToIncomingMessageQueue();
						}
						else 
						{
							std::cout << "["  << this->id << "] Read Body Fail.\n";
							this->m_socket.close();
						}
					}
				);
			}

			void AddToIncomingMessageQueue()
			{
				#ifdef __DEBUG_OUT__
					std::cout << "send message into m_qMessagesIn\n";
				#endif
				if (this->m_nOwnerType == owner::server)
					this->m_qMessagesIn.push_back( { this->shared_from_this(), m_msgTemporaryIn } );
				else 
					this->m_qMessagesIn.push_back( { nullptr, m_msgTemporaryIn } );
				// 完成了一个完整的数据包的接收过程，此时重新将读取报文事件注册到 上下文(context) 当中
				this->ReadHeader();
			}


		protected:
			asio::ip::tcp::socket m_socket;

			asio::io_context& m_asioContext;

			tsqueue<message<T> > m_qMessagesOut;

			tsqueue<owned_message<T> >& m_qMessagesIn;

			message<T> m_msgTemporaryIn;

			owner m_nOwnerType = owner::server;

			// 每一个连接拥有一个自己的唯一的标识符
			uint32_t id = 0;
		};
	}
}



#endif