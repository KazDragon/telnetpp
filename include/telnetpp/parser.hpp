#pragma once

#include "telnetpp/command.hpp"
#include "telnetpp/core.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"

#include <boost/range/algorithm_ext/insert.hpp>
#include <boost/range/iterator_range_core.hpp>
#include <boost/scope_exit.hpp>

#include <vector>

namespace telnetpp {

class parser final
{
public:
    template <typename Continuation>
    void operator()(telnetpp::bytes data, Continuation &&c)
    {
        boost::range::insert(
            unparsed_data_,
            unparsed_data_.end(),
            boost::make_iterator_range(data.begin(), data.end()));

        if (!std::exchange(currently_parsing_, true))
        {
            BOOST_SCOPE_EXIT(&unparsed_data_, &currently_parsing_)
            {
                unparsed_data_.clear();
                currently_parsing_ = false;
            }
            BOOST_SCOPE_EXIT_END;

            // Note: this is an indexed loop because it is plausible that
            // a response to a Telnet message may cause extra data to be
            // received, which would change the length of the unparsed data.
            for (unsigned char by : unparsed_data_)
            {
                switch (state_)
                {
                    case parsing_state::state_idle:
                        parse_idle(by, c);
                        break;

                    case parsing_state::state_iac:
                        parse_iac(by, c);
                        break;

                    case parsing_state::state_negotiation:
                        parse_negotiation(by, c);
                        break;

                    case parsing_state::state_subnegotiation:
                        parse_subnegotiation(by, c);
                        break;

                    case parsing_state::state_subnegotiation_content:
                        parse_subnegotiation_content(by, c);
                        break;

                    case parsing_state::state_subnegotiation_content_iac:
                        parse_subnegotiation_content_iac(by, c);
                        break;

                    default:
                        break;
                }
            }

            emit_plain_data(c);
        }
    }

private:
    enum class parsing_state
    {
        state_idle,
        state_iac,
        state_negotiation,
        state_subnegotiation,
        state_subnegotiation_content,
        state_subnegotiation_content_iac
    };

    parsing_state state_{parsing_state::state_idle};
    bool currently_parsing_{false};
    std::vector<telnetpp::byte> unparsed_data_;

    telnetpp::byte_storage plain_data_;
    telnetpp::byte_storage subnegotiation_content_;
    telnetpp::negotiation_type negotiation_type_;
    telnetpp::option_type subnegotiation_option_;

    template <typename Continuation>
    void parse_idle(telnetpp::byte by, Continuation &&c)
    {
        switch (by)
        {
            case telnetpp::iac:
                state_ = parsing_state::state_iac;
                break;

            default:
                plain_data_.push_back(by);
                break;
        }
    }

    template <typename Continuation>
    void parse_iac(telnetpp::byte by, Continuation &&c)
    {
        switch (by)
        {
            case telnetpp::iac:
                plain_data_.push_back(by);
                state_ = parsing_state::state_idle;
                break;

            case telnetpp::will:  // fall-through
            case telnetpp::wont:  // fall-through
            case telnetpp::do_:   // fall-through
            case telnetpp::dont:
                emit_plain_data(c);
                negotiation_type_ = by;
                state_ = parsing_state::state_negotiation;
                break;

            case telnetpp::sb:
                emit_plain_data(c);
                state_ = parsing_state::state_subnegotiation;
                break;

            default:
                emit_plain_data(c);
                c(telnetpp::command{by});
                state_ = parsing_state::state_idle;
                break;
        }
    }

    template <typename Continuation>
    void parse_negotiation(telnetpp::byte by, Continuation &&c)
    {
        c(telnetpp::negotiation{negotiation_type_, by});
        state_ = parsing_state::state_idle;
    }

    template <typename Continuation>
    void parse_subnegotiation(telnetpp::byte by, Continuation &&c)
    {
        subnegotiation_option_ = by;
        subnegotiation_content_.clear();
        state_ = parsing_state::state_subnegotiation_content;
    }

    template <typename Continuation>
    void parse_subnegotiation_content(telnetpp::byte by, Continuation &&c)
    {
        switch (by)
        {
            case telnetpp::iac:
                state_ = parsing_state::state_subnegotiation_content_iac;
                break;

            default:
                subnegotiation_content_.push_back(by);
                break;
        }
    }

    template <typename Continuation>
    void parse_subnegotiation_content_iac(telnetpp::byte by, Continuation &&c)
    {
        switch (by)
        {
            case telnetpp::se:
                c(telnetpp::subnegotiation{
                    subnegotiation_option_, subnegotiation_content_});
                state_ = parsing_state::state_idle;
                break;

            default:
                subnegotiation_content_.push_back(by);
                state_ = parsing_state::state_subnegotiation_content;
                break;
        }
    }

    template <typename Continuation>
    void emit_plain_data(Continuation &&c)
    {
        if (!plain_data_.empty())
        {
            c(plain_data_);
            plain_data_.clear();
        }
    }
};

}  // namespace telnetpp
