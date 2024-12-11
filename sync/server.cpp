#include <iostream>
#include <boost/asio.hpp>
#include <set>
#include <memory>

using boost::asio::ip::tcp;
const int max_length = 1024;
typedef std::shared_ptr<tcp::socket> socket_ptr;
std::set<std::shared_ptr<std::thread> > thread_set;
using namespace std;

//专门处理一个客户端连接上来收发数据
void session(socket_ptr sock){
    try{
        for(;;){
            char data[max_length]; //存放收到的内容
            memset(data, '\0', max_length);
            boost::system::error_code error;
            //size_t length = boost::asio::read(sock, boost::asio::buffer(data, max_length), error);
            size_t length = sock -> read_some(boost::asio::buffer(data, max_length), error);
            if(error == boost::asio::error::eof){
                std::cout << "connection closed by peer" << std::endl;
                break;
            }
            else if(error){
                throw boost::system::system_error(error);
            }
            std::cout << "receive from " << sock->remote_endpoint().address().to_string() << endl;
            std::cout << "receive message:" << data << endl;
            //回传给客户端
            boost::asio::write(*sock, boost::asio::buffer(data, length));
        }
    }
    catch(exception& e){
        std::cerr << "Exception in thread " << e.what() << "\n" << std::endl;
    }
}

void server(boost::asio::io_context& io_context, unsigned short port){
    tcp::acceptor a(io_context, tcp::endpoint(tcp::v4(), port)); //参数：上下文，绑定服务器本地的地址
    for(;;){
        socket_ptr socket(new tcp::socket(io_context));
        a.accept(*socket);
        auto t = std::make_shared<std::thread>(session, socket);
        thread_set.insert(t); //智能指针引用计数

    } 
}
int main(){
    try{
        boost::asio::io_context ioc;
        server(ioc, 10086);
        for(auto& t : thread_set){
            t->join(); //保证子线程退出后主线程才会退出
        }
    }
    catch(std::exception& e){
        std::cerr << "Exception" << e.what() << "\n";
    }
    return 0;
}