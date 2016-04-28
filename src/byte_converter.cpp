#include "telnetpp/byte_converter.hpp"

namespace telnetpp {

namespace {

struct stream_token_appender : boost::static_visitor<>
{
    stream_token_appender(u8stream &stream)
      : stream_(stream)
    {
    }

    void operator()(u8stream const &data)
    {
        stream_.insert(stream_.end(), data.begin(), data.end());
    }

    void operator()(boost::any const &any)
    {
    }

    u8stream &stream_;
};

}

u8stream byte_converter::send(std::vector<stream_token> const &tokens)
{
    u8stream result;
    stream_token_appender appender(result);

    for (auto const &token : tokens)
    {
        boost::apply_visitor(appender, token);
    }

    return result;
}

}
