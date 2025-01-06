#include "Session.h"
#include <iostream>

using namespace std;

std::string& Session::GetUuid(){
    return _uuid;
}

void Session::Start(){
    memset(_data, 0, max_length);
    _socket.async_read_some(boost::asio::buffer(_data, max_length),
        std::bind(&Session::handle_read, this, std::placeholders::_1, std::placeholders::_2, shared_from_this()));
}

void Session::handle_read(const boost::system::error_code& error, size_t bytes_transferred, shared_ptr<Session> _self_shared){
    if(!error){
        cout << "server receive data is " << _data << endl;
        //echo 服务器，将收到的发送回去
        boost::asio::async_write(_socket, boost::asio::buffer(_data, bytes_transferred),
            std::bind(&Session::handle_write, this, std::placeholders::_1, _self_shared));
    }
    else{
        cout << "Read Error" << endl;
        _server->ClearSession(_uuid);
    }
}

void Session::handle_write(const boost::system::error_code& error, shared_ptr<Session> _self_shared){
    if(!error){
        memset(_data, 0, max_length);
        _socket.async_read_some(boost::asio::buffer(_data, max_length),
            std::bind(&Session::handle_read, this, std::placeholders::_1, std::placeholders::_2, _self_shared));
    }
    else{
        cout << "Write Error" << endl;
        _server->ClearSession(_uuid);
    }
}

Server::Server(boost::asio::io_context& ioc, short port) : _ioc(ioc), _acceptor(ioc, tcp::endpoint(tcp::v4(), port)){
    std::cout << "Server start success, on port: " << port << std::endl; 
    start_accept();
}

void Server::ClearSession(std::string uuid){
    _sessions.erase(uuid);
}

//当这个函数执行完以后，new_session不会被释放
//bind绑定new_session的时候按值传递，所以其引用计数会加一
//所以new_session的生命周期就会与新生成的函数对象一致


void Server::start_accept() {
    shared_ptr<Session> new_session = make_shared<Session>(_ioc, this);
    // 绑定回调函数时，确保传入正确的参数顺序
    _acceptor.async_accept(new_session->Socket(),
        std::bind(&Server::handle_accept, this, new_session, std::placeholders::_1));
}

void Server::handle_accept(shared_ptr<Session> new_session, const boost::system::error_code& error) {
    if (!error) {
        new_session->Start(); 
        _sessions.insert(make_pair(new_session->GetUuid(), new_session));       
    } else {
        // 错误处理
    }
    start_accept();
}