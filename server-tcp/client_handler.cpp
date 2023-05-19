
#include "client_handler.hpp"

std::tuple<int,int,int,int,int,int> field::create_tuple() {
    return std::make_tuple(date.tm_year, date.tm_mon, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec);
}

bool operator < (field &l, field &r)
{
    return l.create_tuple() < r.create_tuple();
}

client_handler::client_handler(boost::asio::io_context &io, unsigned long long id) :
    _sock(io), _id(id)
{
    
}

field client_handler::create_field(const std::string &str)
{
    field f;
    if (auto *ptr = strptime(str.c_str(), "%d.%m.%Y %H:%M:%S", &f.date); ptr) {
        if (ptr != "\0") {
            auto pos = str.find(ptr, format_size + separator_size);
            f.v1 = str.substr(format_size + separator_size, pos - separator_size);
            f.v2 = str.substr(pos + separator_size);
        }
    }
    return f;
}

boost::asio::ip::tcp::socket& client_handler::sock() 
{
    return _sock;
}

void client_handler::handler()
{
    try {
        boost::system::error_code error;

        //recive file size
        u_int64_t file_size = 0;
        auto rec = boost::asio::read(_sock, boost::asio::buffer(&file_size, sizeof(file_size)), error);
        if (error) {
             throw std::runtime_error(error.message());
        } else {
            log(boost::log::trivial::info, "Recive file size: %d", file_size);
        }

        //recive file data
        u_int64_t data_read = 0;
        u_int64_t count_line = 0;
        std::array<char,65536> buf;
        std::string incomplite_str("");
        while (file_size > 0) {
            const auto size = std::min(buf.size(), file_size);
            auto read = boost::asio::read(_sock, boost::asio::buffer(buf.data(), size), error);
            if (error) {
                throw std::runtime_error(error.message());
            } else {
                file_size -= read;
                data_read += read;
            }

            //parsing line
            auto start = 0;
            auto end = 0;
            for (end = start; end < read; ++end) {
                if (buf[end] == '\n' || (file_size == 0 && end == read -1)) {
                    std::string str;
                    if (!incomplite_str.empty()) {
                        str = std::move(incomplite_str);
                    }
                    str += std::string(buf.data() + start, end - start);

                    //check field                   
                    auto field = create_field(str);
                    if (max_field < field) {
                        max_field = std::move(field);
                    }
                    start = end + 1;
                    ++count_line;
                }
            }
            if (start != end) {
                incomplite_str = std::string(buf.data() + start, end - start);
            }
        }
        log(boost::log::trivial::info, "Recive file data: %d", data_read);

        //send count line
        auto sended = boost::asio::write(_sock, boost::asio::buffer(&count_line, sizeof(count_line)), error);
        if (error) {
            throw std::runtime_error(error.message());
        } else {
            log(boost::log::trivial::info, "Send count line: %d", data_read);
        }

        try {  
            char buf[64];
            if (std::strftime(buf, sizeof(buf), "%c", &max_field.date)) {
                log(boost::log::trivial::info, "Max date: %s value:%f", buf, std::stod(max_field.v1 ) / std::stod(max_field.v2));
            }
        } catch (std::exception &ex) {
            log(boost::log::trivial::error, "Error convert to double: %s ", ex.what());
        }
    } catch (std::exception &ex) {
        log(boost::log::trivial::error, "Runtime std::exception %s", ex.what());
    }
    
}

client_handler::client_handler_ptr client_handler::ptr()
{
    return shared_from_this();
}

unsigned long long client_handler::id() const 
{
    return _id;
}

client_handler::client_handler_ptr client_handler::create(boost::asio::io_context &io, unsigned long long id)
{
    return boost::shared_ptr<client_handler>(new client_handler(io, id));
}
