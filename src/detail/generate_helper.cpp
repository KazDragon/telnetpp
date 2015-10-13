#include "telnetpp/detail/generate_helper.hpp"
#include "telnetpp/protocol.hpp"

namespace telnetpp { namespace detail {
    
namespace {

struct generator_token_visitor : boost::static_visitor<>
{
    generator_token_visitor(std::vector<telnetpp::u8> &result)
      : result_(result)
    {
    }
    
    void operator()(std::string const &text)
    {
        append_escaped(text.begin(), text.end());
    }
    
    void operator()(telnetpp::command const &cmd)
    {
        result_.push_back(telnetpp::iac);
        result_.push_back(cmd.value());
    }

    void operator()(telnetpp::negotiation const &neg)
    {
        result_.push_back(telnetpp::iac);
        result_.push_back(neg.request());
        result_.push_back(neg.option());
    }

    void operator()(telnetpp::subnegotiation const &sub)
    {
        result_.push_back(telnetpp::iac);
        result_.push_back(telnetpp::sb);
        result_.push_back(sub.option());
        append_escaped(sub.content().begin(), sub.content().end());
        result_.push_back(telnetpp::iac);
        result_.push_back(telnetpp::se);
    }
    
    std::vector<telnetpp::u8> &result_;
    
private :
    template <class InputIterator1, class InputIterator2>
    void append_escaped(InputIterator1 begin, InputIterator2 end)
    {
        while (begin != end)
        {
            auto &&value = telnetpp::u8(*begin++);
            
            if (value == telnetpp::iac)
            {
                result_.push_back(value);
            }
            
            result_.push_back(value);
        }
    }
};

}

void generate_helper(
    std::vector<telnetpp::u8> &result, 
    telnetpp::element const &token)
{
    generator_token_visitor visitor(result);
    boost::apply_visitor(visitor, token);
}

}}

