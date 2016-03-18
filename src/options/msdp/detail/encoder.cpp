#include "telnetpp/options/msdp/detail/encoder.hpp"
#include <algorithm>
#include <iterator>

namespace telnetpp { namespace options { namespace msdp { namespace detail {
    
namespace {

static void append_name(telnetpp::u8stream &stream, std::string const &name);
static void append_value(telnetpp::u8stream &stream, value_type const &value);
static telnetpp::u8stream &append_variable(
    telnetpp::u8stream &stream,
    telnetpp::options::msdp::variable const &variable);

class value_appender : public boost::static_visitor<>
{
public :
    value_appender(telnetpp::u8stream &stream)
      : stream_(stream)
    {
    }

    void operator()(std::string const &string_value)
    {
        using std::begin;
        using std::end;
        
        stream_.push_back(telnetpp::options::msdp::val);
        stream_.insert(end(stream_), begin(string_value), end(string_value));
    }
    
    void operator()(std::vector<std::string> const &array_value)
    {
        stream_.push_back(telnetpp::options::msdp::val);
        stream_.push_back(telnetpp::options::msdp::array_open);
        std::for_each(array_value.begin(), array_value.end(), 
            [this](auto const &value){append_value(stream_, value);});
        stream_.push_back(telnetpp::options::msdp::array_close);
    }
    
    void operator()(
        std::vector<telnetpp::options::msdp::variable> const &table)
    {
        stream_.push_back(telnetpp::options::msdp::val);
        stream_.push_back(telnetpp::options::msdp::table_open);
        std::for_each(table.begin(), table.end(), 
            [this](auto const &variable)
            {
                append_variable(stream_, variable);
            });
        stream_.push_back(telnetpp::options::msdp::table_close);
    }
    
private :
    telnetpp::u8stream &stream_;
};

static void append_name(telnetpp::u8stream &stream, std::string const &name)
{
    using std::begin;
    using std::end;

    stream.push_back(telnetpp::options::msdp::var);
    stream.insert(end(stream), begin(name), end(name));
}

static void append_value(telnetpp::u8stream &stream, value_type const &value)
{
    value_appender appender(stream);
    boost::apply_visitor(appender, value);
}

static telnetpp::u8stream &append_variable(
    telnetpp::u8stream &stream,
    telnetpp::options::msdp::variable const &variable)
{
    append_name(stream, variable.name);
    append_value(stream, variable.value);

    return stream;
}

}

telnetpp::u8stream encode(
    std::vector<telnetpp::options::msdp::variable> const &variables)
{
    return std::accumulate(
        variables.begin(),
        variables.end(),
        telnetpp::u8stream{},
        append_variable);
}

}}}}
