#include "Session.h"
#include <iostream>

using namespace std;

void Session::Start(){
    memset(_data, 0, max_length);
    _socket.async_read_some(boost::asio::buffer(_data, max_length),
        std::bind(&Session::handle_read, this, std::placeholders::_1, std::placeholders::_2));
}

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred){
    if(!error){
        cout << "server receive data is " << _data << endl;
        //echo 服务器，将收到的发送回去
        boost::asio::async_write(_socket, boost::asio::buffer(_data, bytes_transferred),
            std::bind(&Session::handle_write, this, std::placeholders::_1));
    }
    else{
        cout << "Read Error" << endl;
        delete this; //把这个session销毁掉，连接断开
    }
}

void Session::handle_write(const boost::system::error_code& error){
    if(!error){
        memset(_data, 0, max_length);
        _socket.async_read_some(boost::asio::buffer(_data, max_length),
            std::bind(&Session::handle_read, this, std::placeholders::_1, std::placeholders::_2));
    }
    else{
        cout << "Write Error" << endl;
        delete this; //把这个session销毁掉，连接断开
    }
}

Server::Server(boost::asio::io_context& ioc, short port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)){
    std::cout << "Server start success, on port: " << port << std::endl; 
    start_accept();
}

void Server::start_accept(){
    Session* new_session = new Session(_ioc);
    _acceptor.async_accept(new_session->Socket(), std::bind(&Server::handle_accept, this, new_session, std::placeholders::_1));

}

void Server::handle_accept(Session* new_session, const boost::system::error_code& error){
    if(!error){
        new_session->Start();        
    }
    else{
        delete new_session;
    }
    start_accept();
}