#include "log.hpp"

void ini_log(const std::string &app_name)
{
    const std::string fmt = "[%TimeStamp%][%Severity%]: (App:" + app_name + ") %Message%";
    boost::log::register_simple_formatter_factory< boost::log::trivial::severity_level, char >("Severity");

    boost::log::add_file_log
    (
        boost::log::keywords::file_name = "/tmp/client_server_%N.log",
        boost::log::keywords::rotation_size = 20 * 1204 * 1024,
        boost::log::keywords::time_based_rotation = boost::log::sinks::file::rotation_at_time_point(0, 0, 0),
        boost::log::keywords::format = fmt,
        boost::log::keywords::auto_flush = true,
        boost::log::keywords::open_mode = std::ios::app
    );

    boost::log::add_console_log(
        std::cout,
        boost::log::keywords::format = fmt,
        boost::log::keywords::auto_flush = true
    );
    
    boost::log::add_common_attributes();

}
