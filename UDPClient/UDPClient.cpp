#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/ip/udp.hpp>
#include <cstdlib>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <iostream>

using boost::asio::deadline_timer;
using boost::asio::ip::udp;

class UDPClient
{
public:
	UDPClient(
		boost::asio::io_service& io_service,
		const std::string& host,
		const std::string& port
	) : io_service_(io_service), 
		socket_(io_service, udp::endpoint(udp::v4(), 0)),
		deadline_(io_service_) {
		udp::resolver resolver(io_service_);
		udp::resolver::query query(udp::v4(), host, port);
		udp::resolver::iterator iter = resolver.resolve(query);
		endpoint_ = *iter;
		recvBuffer_.fill(0);
		deadline_.expires_at(boost::posix_time::pos_infin);
		check_deadline();
	}

	~UDPClient()
	{
		socket_.close();
	}

	void send(const std::string& msg) {
		socket_.send_to(boost::asio::buffer(msg, msg.size()), endpoint_);
	}

	std::string receive(boost::posix_time::time_duration timeout, boost::system::error_code& ec) {
		deadline_.expires_from_now(timeout);
		ec = boost::asio::error::would_block;
		std::size_t length = 0;
		
		socket_.async_receive(boost::asio::buffer(recvBuffer_),
							boost::bind(&UDPClient::handle_receive, _1, _2, &ec, &length));
							
		do io_service_.run_one(); while (ec == boost::asio::error::would_block);

		data_ = std::string(std::begin(recvBuffer_), std::end(recvBuffer_));
		recvBuffer_.fill(0);
		return data_;
	}

private:
	void check_deadline()
	{
		if (deadline_.expires_at() <= deadline_timer::traits_type::now())
		{
			socket_.cancel();
			deadline_.expires_at(boost::posix_time::pos_infin);
		}

		deadline_.async_wait(boost::bind(&UDPClient::check_deadline, this));
	}

	static void handle_receive(const boost::system::error_code& ec, std::size_t length,
								boost::system::error_code* out_ec, std::size_t* out_length)
	{
		*out_ec = ec;
		*out_length = length;
	}


	boost::asio::io_service& io_service_;
	udp::socket socket_;
	udp::endpoint endpoint_;
	deadline_timer deadline_;
	std::array<char, 1024> recvBuffer_;
	std::string data_;
};

int main()
{
	boost::asio::io_service io_service;
	boost::system::error_code ec;
	UDPClient client(io_service, "localhost", "1111");

	client.send("Hello, World! (Client)");
	std::string data = client.receive(boost::posix_time::seconds(10), ec);
	if (ec)
	{
		std::cout << "Receive error: " << ec.message() << "\n"; 
	}
	else
	{
		std::cout << "Received: ";
		std::cout << data;
		std::cout << "\n";
	}
}