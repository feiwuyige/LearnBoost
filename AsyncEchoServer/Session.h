#pragma once
#include <iostream>
#include <boost/asio.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <map>

using namespace std;
using boost::asio::ip::tcp;

//session属于服务器
class Server;
class Session : public std::enable_shared_from_this<Session> {
public:
    Session(boost::asio::io_context& ioc, Server* server) : _socket(ioc), _server(server){
        //生成唯一id(雪花算法)
        boost::uuids::uuid a_uuid = boost::uuids::random_generator()(); //random_generator是一个函数对象（）1变成一个函数，（）2调用这个函数
        _uuid = boost::uuids::to_string(a_uuid);
    }

    tcp::socket& Socket(){
        return this->_socket;
    }
    //监听对客户端的读与写
    void Start();
    //访问uuid
    std::string& GetUuid();

private:
    //回调函数
    void handle_read(const boost::system::error_code& error, size_t bytes_transferred, std::shared_ptr<Session> _self_shared);
    void handle_write(const boost::system::error_code& error, std::shared_ptr<Session> _self_shared);
    tcp::socket _socket;
    enum {max_length = 1024};
    char _data[max_length];
    Server* _server;
    std::string _uuid;
};

class Server{
public:
    Server(boost::asio::io_context& ioc, short port);
    void ClearSession(std::string uuid);
private:
    void start_accept();
    void handle_accept(shared_ptr<Session> new_session, const boost::system::error_code& error);
    boost::asio::io_context& _ioc; //io_context 不允许被拷贝
    tcp::acceptor _acceptor;
    std::map<std::string, std::shared_ptr<Session> > _sessions;
};