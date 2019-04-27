#include <string>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>

using boost::asio::ip::udp;

class UDPServer {
public:
  UDPServer(boost::asio::io_service& io_service,
            const std::string& port
            ) : socket_(io_service, udp::endpoint(udp::v4(), std::stoi(port)))
  {
    startReceive();
    recvBuffer_.fill(0);
  }

private:
  void startReceive() {
    socket_.async_receive_from(
      boost::asio::buffer(recvBuffer_), remoteEndpoint_,
      boost::bind(&UDPServer::handleReceive, this,
                  boost::asio::placeholders::error,
                  boost::asio::placeholders::bytes_transferred));
    data_ = std::string(std::begin(recvBuffer_), std::end(recvBuffer_));
    recvBuffer_.fill(0);
    std::cout << data_ << std::endl;
  }

  void handleReceive(const boost::system::error_code& error,
                     std::size_t bytes_transferred) {
    if (!error || error == boost::asio::error::message_size) {

      auto message = std::make_shared<std::string>("Hello, World! (Server)\n");

      socket_.send_to(boost::asio::buffer(*message), remoteEndpoint_);
      startReceive();
    }
  }

  udp::socket socket_;
  udp::endpoint remoteEndpoint_;
  std::array<char, 1024> recvBuffer_{ {0} };
  std::string data_;
};

int main() {
  try {
    boost::asio::io_service io_service;
    UDPServer server(io_service, "1111");
    io_service.run();
  } catch (const std::exception& ex) {
    std::cerr << ex.what() << std::endl;
  }
  return 0;
}
