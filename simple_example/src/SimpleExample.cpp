#include <iostream>
#include <chrono>
#include <asio.hpp>
#include <asio/ts/buffer.hpp>
#include <asio/ts/internet.hpp>

using namespace std::chrono_literals;	// using for ms, s, us

std::vector<char> vBuffer(1 * 1024);

void GrabSomeData(asio::ip::tcp::socket &socket) 
{
	// async_read_some 在调用之后都会马上返回，但是它会在上下文当中注册一个读事件，当 socket
	// 可读的时候，会调用回调函数，然后事件从上下文当中移除；这也是为什么下面的代码在回调函数当中
	// 再次调用 GrabSomeData() 函数的原因；如果要读取完整的数据，就需要将读事件不停地注册到上
	// 文当中。
	socket.async_read_some(asio::buffer(vBuffer.data(), vBuffer.size()),
		[&](std::error_code ec, std::size_t length){
		if (!ec) 
		{
			std::cout << "\n\nread " << length << " bytes\n\n";

			for (int i = 0; i < length; ++i)
				std::cout << vBuffer[i];

			GrabSomeData(socket);
		}
	}
	);
}

int main(int argc, char *argv[]) 
{
	asio::error_code ec;
	
	// io 上下文，asio 的任何的 io 操作都需要一个 io 上下文
	asio::io_context context;

	// give some fake tasks to asio so the context doesnt finish
	// 一般情况下，io 上下文在 run() 之后，如果事件循环当中没有事件发生了，将会自动的退出
	// 如果要防止其退出，需要使用一个假的任务，防止其退出
	asio::io_context::work idlework(context);

	//  start the context
	// io 上下文，事件循环在新的线程下进行
	std::thread thrcontext = std::thread([&]() { context.run(); });

	//给定我们想要连接的 ip 地址以及端口号
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.216.34", ec), 80);

	asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

	// clash 使用的 socket 端口为本地的 2341 端口，因此可以连接成功
	//asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 4000);

	// 创建一个 socket 
	asio::ip::tcp::socket socket(context);
	// 尝试连接这个端口
	socket.connect(endpoint, ec);

	if (!ec)
	{
		std::cout << "Connetced!" << std::endl;
	}
	else 
	{
		std::cout << "Failed to connect to address:\n" << ec.message() << std::endl;
	}


	if (socket.is_open()) 
	{
		GrabSomeData(socket);

		std::string sRequest = 
			"GET /index.html HTTP/1.1\r\n"
			"Host: david-barr.co.uk\r\n"
			"Connection: Close\r\n\r\n";

		// 阻塞式写入过程，只有写入成功之后才会返回。不会添加事件到 io 上下文
		socket.write_some(asio::buffer(sRequest.data(), sRequest.size()), ec);

		/**
		socket.wait(socket.wait_read);

		size_t bytes = socket.available(); // 非阻塞，在发出请求后立刻执行读取，一般都会返回 0 
		std::cout << "Bytes Avaliable : " << bytes << std::endl;

		if (bytes > 0) 
		{
			std::vector<char> vBuffer(bytes);
			socket.read_some(asio::buffer(vBuffer.data(), vBuffer.size()), ec);
			for (auto c : vBuffer)
				std::cout << c;
		}
		*/
		std::this_thread::sleep_for(5000ms);

		// stop() 会删除 context io 上下文当中的所有的事件任务，并停止事件循环
		context.stop();

		if (thrcontext.joinable()) thrcontext.join();
	}

	return 0;
}