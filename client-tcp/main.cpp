#include <thread>
#include <chrono>
#include <iostream>
#include <fstream>
#include <boost/asio.hpp>

#include "../log.hpp"

using namespace boost::asio;
using namespace std::chrono_literals;


int main (int argc, char *argv[]) 
{
    try {
        ini_log("Client");
        log(boost::log::trivial::info, "Start client");

        //check file exist
        std::ifstream stream;
        stream.open(argv[1], std::ios_base::in);
        if (!stream.is_open()) {
            throw std::invalid_argument("File d'not open!");
        }

        //get file size
        stream.seekg(0, std::ios::end);
        const u_int64_t file_size = stream.tellg();
        stream.seekg(0, std::ios::beg);
        log(boost::log::trivial::info, "Get file size: %d", file_size);

        boost::asio::io_service io_service;
        ip::tcp::socket socket(io_service);
        socket.connect(ip::tcp::endpoint(boost::asio::ip::address::from_string("127.0.0.1"), 1234));
        log(boost::log::trivial::info, "Connect to server");
        boost::system::error_code error;

        //send file size
        auto sended = boost::asio::write(socket, boost::asio::buffer(&file_size, sizeof(file_size)), error);
        if (error) {
            throw std::runtime_error(error.message());
        } else {
            log(boost::log::trivial::info, "Send file size: %d", file_size);
        }

        //send file data
        std::array<char, 65536> buf;
        u_int64_t data_send = 0;
        while(stream.peek() != EOF) {
            const std::streamsize readed = stream.rdbuf()->sgetn(buf.data(), buf.size());
            sended = boost::asio::write(socket, boost::asio::buffer(buf.data(), readed), error);
            if (error) {
                throw std::runtime_error(error.message());
            }
            data_send += sended;
        }
        log(boost::log::trivial::info, "Send file data: %d", data_send);

        //recive result
        log(boost::log::trivial::info, "Waiting result from server..."); 
        u_int64_t count_line = 0;
        auto rec = boost::asio::read(socket, boost::asio::buffer(&count_line, sizeof(count_line)), error);
        log(boost::log::trivial::info, "Recive result from server: count line: %d", count_line);
    } catch (std::exception &ex) {
        log(boost::log::trivial::error, "Runtime std::exception %s", ex.what());
    } catch (...) {
        log(boost::log::trivial::error, "Runtime undefined exception");
    }
    
}