#include "endpoint.h"
#include <boost/asio.hpp>
#include <iostream>
#include <system_error>

int client_end_point(){
    //对端地址
    std::string raw_ip_address = "127.4.8.1";
    //对端端口号
    unsigned short port_num = 3333;

    //定义错误码
    boost::system::error_code ec;
    //从string转化为ip地址
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(raw_ip_address, ec);
    if(ec.value() != 0){
        std::cout << "Fail to phrase the IP address.Error code = " << ec.value() << ".Message is " << ec.message();
        return ec.value();
    }

    //生成端点，以后客户段可以通过ep这个端点进行连接
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}

int server_end_point(){
    //服务器自己是知道自己的ip地址的，所以只需要端口号
    unsigned short port_num = 3333;
    boost::asio::ip::address ip_address = boost::asio::ip::address_v6::any();

    //生成端点
    boost::asio::ip::tcp::endpoint ep(ip_address, port_num);
    return 0;
}

int create_tcp_socket(){
    //要想使用socket，必须要有上下文
    //目的告诉asio，该socket属于哪一个上下文
    boost::asio::io_context ioc;
    
    //创建协议，判断打开是否成功，但是新版本已经不需要
    boost::asio::ip::tcp protocol = boost::asio::ip::tcp::v4();
    boost::asio::ip::tcp::socket sock(ioc);
    boost::system::error_code ec;
    sock.open(protocol, ec);
    if(ec.value() != 0){
        std::cout << "Fail to open the socket!Error code = " << ec.value() << ".Message: " << ec.message();
        return ec.value();
    }
    return 0;
}

int create_acceptor_socket(){
    boost::asio::io_context ios;
    //创建acceptor并且直接绑定
    boost::asio::ip::tcp::acceptor acceptor(ios, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 3333));
}

int bind_acceptor_socket(){
    unsigned short port_num = 3333;
    //创建端点，接收任何地址的连接
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
    boost::asio::io_context ios;
    //生成acceptor，并手动进行绑定
    boost::asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
    boost::system::error_code ec;
    acceptor.bind(ep, ec);
    if(ec.value() != 0){
        std::cout << "Fail to bind the acceptor socket!Error code = " << ec.value() << ".Message: " << ec.message();
        return ec.value();
    }
    return 0;
}

int connect_to_end(){
    std::string raw_ip_address = "192.168.1.124";
    unsigned short port_num = 3333;
    try{
        //服务器是绑定端点，客户端是连接端点
        boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address::from_string(raw_ip_address), port_num);
        boost::asio::io_context ios;
        boost::asio::ip::tcp::socket sock(ios, ep.protocol());
        sock.connect(ep);
    }
    catch (std::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }
}

int dns_connect_to_end(){
    std::string host = "llfc.club";
    std::string port_num = "3333";
    boost::asio::io_context ios;
    //生成域名解析器
    boost::asio::ip::tcp::resolver::query resolver_query(host, port_num, boost::asio::ip::tcp::resolver::query::numeric_service);
    boost::asio::ip::tcp::resolver resolver(ios);
    try{
        //返回该域名对应的所有ip地址的一个迭代器
        boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(resolver_query);
        boost::asio::ip::tcp::socket sock(ios);
        boost::asio::connect(sock, it);
    }
    catch(std::system_error& e){
        std::cout << "Error occured! Error code = " << e.code() << ".Message: " << e.what();
        return e.code().value();
    }
}

int accept_new_connection(){
    //监听队列
    const int BACKLOG_SIZE = 30;
    unsigned short port_num = 3333;
    boost::asio::ip::tcp::endpoint ep(boost::asio::ip::address_v4::any(), port_num);
    boost::asio::io_context ios;
    try{
        boost::asio::ip::tcp::acceptor acceptor(ios, ep.protocol());
        acceptor.bind(ep);
        acceptor.listen(BACKLOG_SIZE);
        boost::asio::ip::tcp::socket sock(ios);
        acceptor.accept(sock);
    }
    catch (std::system_error& e) {
        std::cout << "Error occured! Error code = " << e.code()
            << ". Message: " << e.what();

        return e.code().value();
    }
}