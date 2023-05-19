#include <boost/thread.hpp>
#include <boost/bind/bind.hpp>

#include "../log.hpp"
#include "client_handler.hpp"

int main () 
{
    ini_log("Server");
    log(boost::log::trivial::info, "Start server");
    boost::asio::io_context io;
    ip::tcp::acceptor acceptor(io, ip::tcp::endpoint(ip::tcp::v4(), 1234));

    static unsigned long clinet_count = 0;
    while (true) {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        try{
            auto cli = client_handler::create(io, ++clinet_count);
            acceptor.accept(cli->sock());
            log(boost::log::trivial::info, "Client accept11 %d", cli->id());
            boost::thread(boost::bind(&client_handler::handler, cli)).detach();            
        } catch (std::exception &ex) {
            log(boost::log::trivial::error, "Runtime std::exception %s", ex.what());
        } catch (...) {
            log(boost::log::trivial::error, "Runtime undefined exception");
        }
    }
    
    return 0;
}