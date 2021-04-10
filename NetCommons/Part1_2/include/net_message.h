#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__


#include "net_common.h"

namespace olc 
{
	namespace net 
	{
		// 每一次传递的数据的头部结构，需要指明这个报文的 id, 以及整个报文的长度信息
		template <typename T>
		struct message_header
		{
			// 报文的类型
			T id {};
			// 存储报文的主体部分的大小，不包含报文头部的大小
			uint32_t size = 0;
		};


		template <typename T>
		struct message 
		{
			// T 只是数据头部的类型，数据的主体部分为字符串类型 uint8_t
			message_header<T> header {};
			// 报文的主体
			std::vector<uint8_t> body {};

			// 返回这个报文的长度，不包含报文的头部
			size_t size() const 
			{
				return  this->body.size();
			}

			// 为了在调试的过程中方便的查看这个报文的内容，我们重载这个报文的标准输出
			friend std::ostream& operator << (std::ostream &os, const message<T> &msg)
			{
				os << "ID: " << static_cast<int>(msg.header.id) << " size: " << msg.header.size;
				return os; 
			}

			// 将一些 POD-like 类型的数据直接输入到 message 的 buffer 当中来
			// POD-like 的数据在内存上的存储是连续的，可以通过内存的拷贝就实现数据的拷贝。
			// 例如，一个非 POD-like 的数据结构中包含了指针对象，那么直接内存的拷贝无法实现对该数据的拷贝（浅拷贝）
			// 普通数据类型(int float char) 都是 POD-like 类型的数据
			template <typename DataType>
			friend message<T>& operator << (message<T> &msg, const DataType &data)
			{
				static_assert(std::is_standard_layout<DataType>::value, 
						"Data is too complex to be pushed into vector");

				size_t i = msg.body.size();

				// 重新设定 buffer 的大小
				msg.body.resize(msg.body.size() + sizeof(DataType));

				// 将新的数据放在 buffer 的最后面
				std::memcpy(msg.body.data() + i, static_cast<const void*>(&data), sizeof(DataType));

				// 更新报头中的大小数据
				msg.header.size = msg.size();

				return msg;
			}

			template <typename DataType>
			friend message<T>& operator >> (message<T> &msg, DataType &data)
			{
				static_assert(std::is_standard_layout<DataType>::value, 
						"Data is too complex to getting from vector");

				size_t i = msg.body.size() - sizeof(DataType);

				std::memcpy(static_cast<void*>(&data), msg.body.data() + i, sizeof(DataType));

				msg.body.resize(i);

				msg.header.size = msg.size();

				return msg;
			}
		};


		template <typename T>
		class connection;

		template <typename T>
		struct owned_message
		{
			// 给出了这个消息所属的连接
			std::shared_ptr< connection<T> > remote = nullptr;
			message<T> msg;

			friend std::ostream& operator << (std::ostream &os, const owned_message<T> &msg)
			{
				os << msg.msg;
				return os;
			}
		};
	}
}

#endif