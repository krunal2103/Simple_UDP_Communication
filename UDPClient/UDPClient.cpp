#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

class UDPClient
{
public:
	UDPClient(
		boost::asio::io_service& io_service,
		const std::string& host,
		const std::string& port
	) : io_service_(io_service), socket_(io_service, udp::endpoint(udp::v4(), 0)) {
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;
		recvBuffer_.fill(0);
	}

	~UDPClient()
	{
		socket_.close();
	}

	void send(const std::string& msg) {
		socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
	}

	std::string receive() {
		socket_.receive_from(boost::asio::buffer(recvBuffer_), endpoint_);
		data_ = std::string(std::begin(recvBuffer_), std::end(recvBuffer_));
		recvBuffer_.fill(0);
		return data_;
	}

private:
	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint endpoint_;
	std::array<char, 1024> recvBuffer_;
	std::string data_;
};

int main()
{
	boost::asio::io_service io_service;
	UDPClient client(io_service, "localhost", "1111");

	client.send("Hello, World! (Client)");
	std::cout << client.receive() << std::endl;
}
