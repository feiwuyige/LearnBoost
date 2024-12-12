#include "Session.h"

Session::Session(std::shared_ptr<boost::asio::ip::tcp::socket> socket) : _socket(socket), _send_pending(false),
_recv_pending(false) {

}

void Session::Connect(const boost::asio::ip::tcp::endpoint& ep){
    _socket->connect(ep);
}

void Session::WriteCallBackErr(const boost::system::error_code& ec, std::size_t bytes_transferred, 
        std::shared_ptr<MsgNode> msg_node){
            if(bytes_transferred + msg_node->_cur_len < msg_node->_total_len){
                _send_node->_cur_len += bytes_transferred;
                this->_socket->async_write_some(
                    boost::asio::buffer(_send_node->_msg+ _send_node->_cur_len, _send_node->_total_len - _send_node->_cur_len),
                    std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2, _send_node));
            }
        }
//这里的实现有问题，因为异步的方式没有办法保证顺序
//比如用户第一次发送 "hello,world"
//但是由于tcp缓冲区太小，只发送了"hello"，然后在回调函数WriteCallBackErr的if中继续发送
//但是这个时候用户又一次发送了"hello,world"
//由于不保证顺序
//最后可能就是 "hellohello,world,world"
//通过队列来保证发送顺序
void Session::WriteToSocketErr(const std::string buf){
    _send_node = std::make_shared<MsgNode> (buf.c_str(), buf.length());
    this->_socket->async_write_some(boost::asio::buffer(_send_node->_msg, _send_node->_total_len), 
        std::bind(&Session::WriteCallBackErr, this, std::placeholders::_1, std::placeholders::_2,_send_node));
}

void Session::WriteCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if(ec.value() != 0){
        std::cout << "Error code is " << ec.value() << ".message is " << ec.message();
        return;
    }
    auto & send_data = _send_queue.front();
    send_data->_cur_len += bytes_transferred;
    if(send_data->_cur_len < send_data->_total_len){
        this->_socket->async_write_some(boost::asio::buffer(send_data->_msg + send_data->_cur_len, 
        send_data->_total_len - send_data->_cur_len), 
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }
    _send_queue.pop();
    if(_send_queue.empty()){
        _send_pending = false;
    }
    else{
        auto & send_data = _send_queue.front();
        this->_socket->async_write_some(boost::asio::buffer(send_data->_msg + send_data->_cur_len,
        send_data->_total_len - send_data->_cur_len),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));

    }
}
void Session::WriteToSocket(const std::string& buf){
    _send_queue.emplace(new MsgNode(buf.c_str(), buf.length()));
    //如果还有数据没有发完，就不要去调用回调函数
    if(_send_pending){
        return;
    }
    this->_socket->async_write_some(boost::asio::buffer(buf),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2));
    _send_pending = true;
}

void Session::WriteAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){
    if(ec.value() != 0){
        std::cout << "Error code is " << ec.value() << ".message is " << ec.message();
        return;
    }
    _send_queue.pop();
    if(_send_queue.empty()){
        _send_pending = false;
    }
    if(!_send_queue.empty()){
        auto & send_data = _send_queue.front();
        this->_socket->async_send(boost::asio::buffer(send_data->_msg + send_data->_cur_len,
        send_data->_total_len - send_data->_cur_len),
        std::bind(&Session::WriteCallBack, this, std::placeholders::_1, std::placeholders::_2)); 
    }
}

void Session::WriteAllToSocket(const std::string& buf){
    _send_queue.emplace(new MsgNode(buf.c_str(), buf.length()));
    if(_send_pending){
        return;
    }
    //async_send内部多次调用async_write_some，从而保证一定发完
    this->_socket->async_send(boost::asio::buffer(buf), 
        std::bind(&Session::WriteAllCallBack, this, std::placeholders::_1, std::placeholders::_2));
    _send_pending = true;
}

void Session::ReadFromSocket(){
    if(_recv_pending){
        return;
    }
    _recv_node = std::make_shared<MsgNode> (RECVSIZE);
    _socket->async_read_some(boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
    std::bind(&Session::ReadCallBack, this, std::placeholders::_1, std::placeholders::_2));
    _recv_pending = true;
}

void Session::ReadCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){
    _recv_node->_cur_len += bytes_transferred;
    if(_recv_node->_cur_len < _recv_node->_total_len){
        _socket->async_read_some(boost::asio::buffer(_recv_node->_msg + _recv_node->_cur_len,
         _recv_node->_total_len - _recv_node->_cur_len),
        std::bind(&Session::ReadCallBack, this, std::placeholders::_1, std::placeholders::_2));
        return;
    }
    _recv_pending = false;
    _recv_node = nullptr;
}

void Session::ReadAllFromSocket(){
    if(_recv_pending){
        return;
    }
    _recv_node = std::make_shared<MsgNode>(RECVSIZE);
    _socket->async_receive(boost::asio::buffer(_recv_node->_msg, _recv_node->_total_len),
        std::bind(&Session::ReadAllCallBack, this, std::placeholders::_1, std::placeholders::_2));
    _recv_pending = true;
}
void Session::ReadAllCallBack(const boost::system::error_code& ec, std::size_t bytes_transferred){
    _recv_node->_cur_len += bytes_transferred;
    _recv_node = nullptr;
    _recv_pending = false;
}