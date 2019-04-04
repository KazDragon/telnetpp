#include <telnetpp/parser.hpp>
#include <telnetpp/element.hpp>
#include <gtest/gtest.h>
#include <iterator>
#include <tuple>
#include <vector>

using testing::ValuesIn;

namespace {

class parser_test : public testing::Test
{
protected:
    void parse(telnetpp::bytes data)
    {
        remainder_ = telnetpp::parse(
            data,
            [this](telnetpp::element const &elem)
            {
                result_.push_back(elem);
            }
        );
    }

    template <typename F>
    void parse_with_check(telnetpp::bytes data, F&& check)
    {
        remainder_ = telnetpp::parse(
            data,
            [this, &check](telnetpp::element const &elem)
            {
                check(elem);
                result_.push_back(elem);
            }
        );
    }

    std::vector<telnetpp::element> result_;
    telnetpp::bytes remainder_;
};

TEST_F(parser_test, empty_range_parses_to_nothing)
{
    std::vector<telnetpp::byte> data;

    parse({});

    ASSERT_EQ(size_t{0}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());
}

TEST_F(parser_test, normal_character_parses_to_span_of_character)
{
    static constexpr telnetpp::byte const data[] = {'x'};
    
    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    auto const actual = boost::get<telnetpp::bytes>(result_[0]);
    ASSERT_EQ(size_t{1}, actual.size());
    ASSERT_EQ('x', actual[0]);
}

TEST_F(parser_test, two_normal_characters_parse_to_string)
{
    static constexpr telnetpp::byte const data[] = { 'x', 'y' };
    static constexpr telnetpp::byte const expected_values[] = { 'x', 'y' };
    static constexpr telnetpp::bytes const expected{expected_values};

    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    auto const actual = boost::get<telnetpp::bytes>(result_[0]);

    ASSERT_EQ(expected, actual);
}

TEST_F(parser_test, lone_iac_parses_to_nothing)
{
    static constexpr telnetpp::byte const data[] = { 0xFF };

    parse(data);

    ASSERT_EQ(size_t{0}, result_.size());
    ASSERT_EQ(size_t{1}, remainder_.size());
}

TEST_F(parser_test, byte_then_iac_emits_byte_only)
{
    static constexpr telnetpp::byte const data[] = { 'x', 0xFF };
    static constexpr telnetpp::byte const expected_values[] = { 'x' };
    static constexpr telnetpp::bytes const expected{expected_values};

    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{1}, remainder_.size());

    auto const actual = boost::get<telnetpp::bytes>(result_[0]);

    ASSERT_EQ(expected, actual);
}

TEST_F(parser_test, double_iac_parses_to_iac)
{
    static constexpr telnetpp::byte const data[] = { 0xFF, 0xFF };
    static constexpr telnetpp::byte const expected_values[] = { 0xFF };
    static constexpr telnetpp::bytes const expected{expected_values};

    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    auto const actual = boost::get<telnetpp::bytes>(result_[0]);

    ASSERT_EQ(expected, actual);
}

using command_test_data = std::tuple<
    telnetpp::bytes,  // bytes for command
    telnetpp::command // resultant command
>;

class for_non_negotiating_commands 
  : public testing::TestWithParam<command_test_data>
{
};

TEST_P(for_non_negotiating_commands, as_the_only_element)
{
    auto const &param            = GetParam();
    auto const &data             = std::get<0>(param);
    auto const &expected_command = std::get<1>(param);

    std::vector<telnetpp::element> result;
    auto const remainder = telnetpp::parse(
        data,
        [&result](telnetpp::element const &elem)
        {
            result.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(size_t{0}, remainder.size());

    auto const actual = boost::get<telnetpp::command>(result[0]);

    ASSERT_EQ(expected_command, actual);
}

constexpr telnetpp::byte const nop_bytes[] = { 0xFF, 0xF1 };
constexpr telnetpp::byte const dm_bytes[]  = { 0xFF, 0xF2 };
constexpr telnetpp::byte const brk_bytes[] = { 0xFF, 0xF3 };
constexpr telnetpp::byte const ip_bytes[]  = { 0xFF, 0xF4 };
constexpr telnetpp::byte const ao_bytes[]  = { 0xFF, 0xF5 };
constexpr telnetpp::byte const ayt_bytes[] = { 0xFF, 0xF6 };
constexpr telnetpp::byte const ec_bytes[]  = { 0xFF, 0xF7 };
constexpr telnetpp::byte const el_bytes[]  = { 0xFF, 0xF8 };
constexpr telnetpp::byte const ga_bytes[]  = { 0xFF, 0xF9 };

INSTANTIATE_TEST_CASE_P(
    commands_are_parsed_to_command_objects,
    for_non_negotiating_commands,
    ValuesIn(
    {
        command_test_data{nop_bytes, telnetpp::command(telnetpp::nop)},
        command_test_data{dm_bytes,  telnetpp::command(telnetpp::dm)},
        command_test_data{brk_bytes, telnetpp::command(telnetpp::brk)},
        command_test_data{ip_bytes,  telnetpp::command(telnetpp::ip)},
        command_test_data{ao_bytes,  telnetpp::command(telnetpp::ao)},
        command_test_data{ayt_bytes, telnetpp::command(telnetpp::ayt)},
        command_test_data{ec_bytes,  telnetpp::command(telnetpp::ec)},
        command_test_data{el_bytes,  telnetpp::command(telnetpp::el)},
        command_test_data{ga_bytes,  telnetpp::command(telnetpp::ga)},
    })
);

class for_incomplete_negotiations
 : public testing::TestWithParam<telnetpp::bytes>
{
};

TEST_P(for_incomplete_negotiations, as_the_only_element)
{
    auto const &data = GetParam();

    std::vector<telnetpp::element> result;
    auto const remainder = telnetpp::parse(
        data,
        [&result](telnetpp::element const &elem)
        {
            result.push_back(elem);
        });

    ASSERT_EQ(size_t{0}, result.size());
    ASSERT_EQ(data.size(), remainder.size());
}

constexpr telnetpp::byte const iac_will[] = { 0xFF, 0xFB };
constexpr telnetpp::byte const iac_wont[] = { 0xFF, 0xFC };
constexpr telnetpp::byte const iac_do[]   = { 0xFF, 0xFD };
constexpr telnetpp::byte const iac_dont[] = { 0xFF, 0xFE };

constexpr telnetpp::byte const iac_sb[]   = { 0xFF, 0xFA };
constexpr telnetpp::byte const iac_sb_option[] = { 0xFF, 0xFA, 0x00 };
constexpr telnetpp::byte const iac_sb_option_iac[] = { 0xFF, 0xFA, 0x00, 0xFF };

INSTANTIATE_TEST_CASE_P(
    negotiations_remain_unparsed,
    for_incomplete_negotiations,
    ValuesIn(
    {
        telnetpp::bytes{ iac_will },
        telnetpp::bytes{ iac_wont },
        telnetpp::bytes{ iac_do, },
        telnetpp::bytes{ iac_dont },

        telnetpp::bytes{ iac_sb },
        telnetpp::bytes{ iac_sb_option },
        telnetpp::bytes{ iac_sb_option_iac },
    })
);

TEST_F(parser_test, sb_option_iac_se_parses_to_empty_subnegotiation)
{
    static constexpr telnetpp::byte const data[] = { 
        0xFF, 0xFA, 0xAA, 0xFF, 0xF0
    };

    static constexpr telnetpp::subnegotiation const expected{0xAA, {}};

    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    auto const actual = boost::get<telnetpp::subnegotiation>(result_[0]);

    ASSERT_EQ(expected, actual);
}

TEST_F(parser_test, subnegotiation_with_content_parses_to_subnegotation)
{
    static constexpr telnetpp::byte const data[] = { 
        0xFF, 0xFA, 0xAB, 0x01, 0x02, 0x03, 0xFF, 0xF0
    };

    static constexpr telnetpp::byte const expected_content[] = {
        0x01, 0x02, 0x03
    };

    static constexpr telnetpp::subnegotiation const expected{
        0xAB, 
        expected_content
    };

    parse(data);

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    auto const actual = boost::get<telnetpp::subnegotiation>(result_[0]);

    ASSERT_EQ(expected, actual);
}

TEST_F(parser_test, an_incomplete_subnegotiation_parses_to_nothing)
{
    static constexpr telnetpp::byte const data[] = {
        0xFF, 0xFA, 0xAC, 0x01
    };
    
    parse(data);
    
    ASSERT_TRUE(result_.empty());
    ASSERT_EQ(sizeof(data), remainder_.size());
}

TEST_F(parser_test, subnegotiation_with_iac_iac_parses_to_iac)
{
    static constexpr telnetpp::byte const data[] = { 
        0xFF, 0xFA, 0xAC, 0x01, 0xFF, 0xFF, 0x02, 0x03, 0xFF, 0xF0
    };

    // It is expected that the subnegotiation contains a really
    // short-lived temporary due to it necessarily having to
    // be transformed.  So it needs to be checked in line rather
    // than being stored and checked afterwards.
    parse_with_check(
        data,
        [](telnetpp::element const &elem)
        {
            static constexpr telnetpp::byte const expected_content[] = {
                0x01, 0xFF, 0x02, 0x03
            };

            static constexpr telnetpp::subnegotiation const expected{
                0xAC, 
                expected_content
            };

            auto const actual = boost::get<telnetpp::subnegotiation>(elem);
            ASSERT_EQ(expected, actual);
        });

    ASSERT_EQ(size_t{1}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());
}

using negotiation_test_data = std::tuple<
    telnetpp::bytes,      // bytes for negotiation
    telnetpp::negotiation // expected resultant negotiation
>;

class for_complete_negotiations
  : public testing::TestWithParam<negotiation_test_data>
{
};

TEST_P(for_complete_negotiations, as_the_only_element)
{
    auto const &param                = GetParam();
    auto const &data                 = std::get<0>(param);
    auto const &expected_negotiation = std::get<1>(param);

    std::vector<telnetpp::element> result;
    auto const remainder = telnetpp::parse(
        data,
        [&result](telnetpp::element const &elem)
        {
            result.push_back(elem);
        });

    ASSERT_EQ(size_t{1}, result.size());
    ASSERT_EQ(size_t{0}, remainder.size());

    auto const actual = boost::get<telnetpp::negotiation>(result[0]);

    ASSERT_EQ(expected_negotiation, actual);
}

constexpr telnetpp::byte const will_negotiation[] = { 0xFF, 0xFB, 0xAB };
constexpr telnetpp::byte const wont_negotiation[] = { 0xFF, 0xFC, 0xBC };
constexpr telnetpp::byte const do_negotiation[]   = { 0xFF, 0xFD, 0xCD };
constexpr telnetpp::byte const dont_negotiation[] = { 0xFF, 0xFE, 0xDE };

INSTANTIATE_TEST_CASE_P(
    negotiations_are_parsed_to_negotiation_objects,
    for_complete_negotiations,
    ValuesIn(
    {
        negotiation_test_data {
            telnetpp::bytes{ will_negotiation },
            telnetpp::negotiation{ telnetpp::will, 0xAB }
        },
        negotiation_test_data {
            telnetpp::bytes{ wont_negotiation },
            telnetpp::negotiation{ telnetpp::wont, 0xBC }
        },
        negotiation_test_data {
            telnetpp::bytes{ do_negotiation },
            telnetpp::negotiation{ telnetpp::do_,  0xCD }
        },
        negotiation_test_data {
            telnetpp::bytes{ dont_negotiation },
            telnetpp::negotiation{ telnetpp::dont, 0xDE }
        },
    })
);

TEST_F(parser_test, many_elements_parses_to_many_elements)
{
    static constexpr telnetpp::byte const data[] = {
        'a', 'b', 'c',             // string: abc
        0xFF, 0xF1,                // command: NOP
        0xFF, 0xF6,                // command: AYT
        'd',                       // string: d
        0xFF, 0xFB, 0x00,          // negotiation: WILL 0
        0xFF, 0xFE, 0x01,          // negotiation: DONT 1
        0xFF, 0xFA, 0x20, 'e', 'f', 0xFF, 0xF0,
                                   // subnegotiation: 0x20[e,f]
        0xFF, 0xFA, 0xFF, 0xFF, 0xF0,
                                   // subnegotiation: 0xFF[]
        'g', 'h',                  // string: gh
        0xFF, 0xFC, 0xFF,          // negotiation: WONT 0xFF
        'i'                        // text: i
    };

    static constexpr telnetpp::byte const expected_values0[] = { 'a', 'b', 'c' };
    static constexpr telnetpp::bytes const expected0{expected_values0};
    static constexpr telnetpp::command const expected1{telnetpp::nop};
    static constexpr telnetpp::command const expected2{telnetpp::ayt};
    static constexpr telnetpp::byte const expected_values3[] = { 'd' };
    static constexpr telnetpp::bytes const expected3{expected_values3};
    static constexpr telnetpp::negotiation const expected4{telnetpp::will, 0x00};
    static constexpr telnetpp::negotiation const expected5{telnetpp::dont, 0x01};
    static constexpr telnetpp::byte const expected_content6[] = { 'e', 'f' };
    static constexpr telnetpp::subnegotiation const expected6{ 0x20, expected_content6 };
    static constexpr telnetpp::subnegotiation const expected7{ 0xFF, {}};
    static constexpr telnetpp::byte const expected_values8[] = { 'g', 'h' };
    static constexpr telnetpp::bytes const expected8{expected_values8};
    static constexpr telnetpp::negotiation const expected9 = {telnetpp::wont, 0xFF};
    static constexpr telnetpp::byte const expected_value10 = { 'i' };
    static constexpr telnetpp::bytes const expected10{expected_value10};

    parse(data);

    ASSERT_EQ(size_t{11}, result_.size());
    ASSERT_EQ(size_t{0}, remainder_.size());

    ASSERT_EQ(expected0, boost::get<decltype(expected0)>(result_[0]));
    ASSERT_EQ(expected1, boost::get<decltype(expected1)>(result_[1]));
    ASSERT_EQ(expected2, boost::get<decltype(expected2)>(result_[2]));
    ASSERT_EQ(expected3, boost::get<decltype(expected3)>(result_[3]));
    ASSERT_EQ(expected4, boost::get<decltype(expected4)>(result_[4]));
    ASSERT_EQ(expected5, boost::get<decltype(expected5)>(result_[5]));
    ASSERT_EQ(expected6, boost::get<decltype(expected6)>(result_[6]));
    ASSERT_EQ(expected7, boost::get<decltype(expected7)>(result_[7]));
    ASSERT_EQ(expected8, boost::get<decltype(expected8)>(result_[8]));
    ASSERT_EQ(expected9, boost::get<decltype(expected9)>(result_[9]));
    ASSERT_EQ(expected10, boost::get<decltype(expected10)>(result_[10]));
}

}
