#pragma once
#include <memory>
#include <boost/asio.hpp>
#include <iostream>
#include <queue>

const int RECVSIZE = 1024;
class MsgNode{
public:
    //发送时的构造函数, 构造写节点
    MsgNode(char const * msg, int total_len) : _total_len(total_len), _cur_len(0){
        _msg = new char[total_len];
        memcpy(_msg, msg, total_len);
    } 
    //接受时， 构造读节点
    MsgNode(int total_len) : _total_len(total_len), _cur_len(0){
        _msg = new char[total_len];
    }
    ~MsgNode(){
        delete[] _msg;
    } 

    int _total_len;
    int _cur_len;
    char* _msg;
};

class Session{
public:
    Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket);
    void Connect(const boost::asio::ip::tcp::endpoint& ep);
    void WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, std::shared_ptr<MsgNode>);
    void WriteToSocketErr(const std::string buf);
    void WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteToSocket(const std::string& buf);
    void WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void WriteAllToSocket(const std::string& buf);
    void ReadFromSocket();
    void ReadCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
    void ReadAllFromSocket();
    void ReadAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred);
private:
    std::shared_ptr<boost::asio::ip::tcp::socket> _socket;
    std::shared_ptr<MsgNode> _send_node;
    std::queue<std::shared_ptr<MsgNode> > _send_queue;
    bool _send_pending;

    std::shared_ptr<MsgNode> _recv_node;
    bool _recv_pending;
};
