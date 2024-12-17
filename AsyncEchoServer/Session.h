#pragma once
#include <iostream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

//session属于服务器
class Session{
public:
    Session(boost::asio::io_context& ioc) : _socket(ioc){

    }

    tcp::socket& Socket(){
        return this->_socket;
    }
    //监听对客户端的读与写
    void Start();


private:
    //回调函数
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_write(const boost::system::error_code& error);
    tcp::socket _socket;
    enum {max_length = 1024};
    char _data[max_length];
};

class Server{
public:
    Server(boost::asio::io_context& ioc, short port);
private:
    void start_accept();
    void handle_accept(Session* new_session, const boost::system::error_code& error);
    boost::asio::io_context& _ioc; //io_context 不允许被拷贝
    tcp::acceptor _acceptor;
};