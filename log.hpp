#pragma once

#include <string>
#include <sstream>
#include <string_view>

#include "boost/log/trivial.hpp"
#include "boost/log/utility/setup.hpp"
#include <boost/interprocess/sync/scoped_lock.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>

template<typename... Args>
std::string string_format( const std::string& format, Args&& ... args )
{
    int size = std::snprintf(nullptr, 0, format.c_str(),std::forward<Args>(args)...) + 1; 
    if (size <= 0) {
         throw std::runtime_error( "Error during formatting."); 
    }
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), std::forward<Args>(args)...);
    return std::string(buf.get(), buf.get() + size - 1);
}


template<typename... Args>
void log(boost::log::trivial::severity_level lvl, const char *fmt, Args&&... args) {
    boost::interprocess::named_mutex mutex(boost::interprocess::open_or_create, "client_server_tcp");
    boost::interprocess::scoped_lock lock(mutex);
    try {
        BOOST_LOG_STREAM_WITH_PARAMS(boost::log::trivial::logger::get(),\
            (boost::log::keywords::severity = lvl)) << string_format(std::string(fmt), std::forward<Args>(args)...);
    } catch(...) {
        //this block is needed in order to correctly close the mutex
        exit(1);
    }
}

void ini_log(const std::string &app_name);




