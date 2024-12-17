#include <iostream>
#include <boost/asio.hpp>
#include "Session.h"

int main(){
    try{
        boost::asio::io_context ioc;
        Server s(ioc, 10086);
        ioc.run();
    }
    catch(std::exception& e){
        std::cout << "Error" << e.what() << std::endl;
    }
    return 0;
}