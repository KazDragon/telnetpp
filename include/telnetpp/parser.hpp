#pragma once

#include "telnetpp/command.hpp"
#include "telnetpp/core.hpp"
#include "telnetpp/negotiation.hpp"
#include "telnetpp/subnegotiation.hpp"

#include <algorithm>

namespace telnetpp {

class parser final
{
public:
    template <typename Continuation>
    constexpr void operator()(telnetpp::bytes data, Continuation &&c)
    {
        std::ranges::for_each(
            data, [&](telnetpp::byte by) { parse_byte(by, c); });

        emit_plain_data(c);
    }

private:
    enum class parsing_state : std::uint8_t
    {
        state_idle,
        state_iac,
        state_negotiation,
        state_subnegotiation,
        state_subnegotiation_content,
        state_subnegotiation_content_iac
    };

    parsing_state state_{parsing_state::state_idle};

    telnetpp::byte_storage plain_data_;
    telnetpp::byte_storage subnegotiation_content_;
    telnetpp::negotiation_type negotiation_type_;
    telnetpp::option_type subnegotiation_option_;

    template <typename Continuation>
    constexpr void parse_byte(telnetpp::byte by, Continuation &&c)
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

    template <typename Continuation>
    constexpr void parse_idle(telnetpp::byte by, Continuation &&c)
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
    constexpr void parse_iac(telnetpp::byte by, Continuation &&c)
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
    constexpr void parse_negotiation(telnetpp::byte by, Continuation &&c)
    {
        c(telnetpp::negotiation{negotiation_type_, by});
        state_ = parsing_state::state_idle;
    }

    template <typename Continuation>
    constexpr void parse_subnegotiation(telnetpp::byte by, Continuation &&c)
    {
        subnegotiation_option_ = by;
        subnegotiation_content_.clear();
        state_ = parsing_state::state_subnegotiation_content;
    }

    template <typename Continuation>
    constexpr void parse_subnegotiation_content(
        telnetpp::byte by, Continuation &&c)
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
    constexpr void parse_subnegotiation_content_iac(
        telnetpp::byte by, Continuation &&c)
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
    constexpr void emit_plain_data(Continuation &&c)
    {
        if (!plain_data_.empty())
        {
            c(plain_data_);
            plain_data_.clear();
        }
    }
};

}  // namespace telnetpp
