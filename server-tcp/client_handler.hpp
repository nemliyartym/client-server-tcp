
#pragma once

#include <iostream>
#include <string_view>

#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/make_shared.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../log.hpp"

using namespace boost::asio;
using namespace std::chrono_literals;


constexpr std::string_view format = "dd.mm.yyyy hh:mm:ss";
constexpr size_t format_size = format.size();
constexpr size_t separator_size = 1;

struct field
{
    std::tm date;
    std::string v1;
    std::string v2;

    std::tuple<int,int,int,int,int,int> create_tuple();
    
    friend bool operator < (field &l, field &r);
};

class client_handler : public boost::enable_shared_from_this<client_handler> {
public:
    using client_handler_ptr = boost::shared_ptr<client_handler>;

    [[nodiscard]] static client_handler_ptr create(boost::asio::io_context &io, unsigned long long id);

    client_handler_ptr ptr();

    unsigned long long id() const;

    boost::asio::ip::tcp::socket& sock();   

    void handler();

private:
    client_handler(boost::asio::io_context &io, unsigned long long id);
    
    field create_field(const std::string &str);

    boost::asio::ip::tcp::socket _sock;
    unsigned long long _id;
    field max_field;
};
