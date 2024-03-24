#include <gtest/gtest.h>
#include <telnetpp/options/msdp/variable.hpp>

#include <sstream>

using namespace telnetpp::literals;  // NOLINT
using string_value = telnetpp::options::msdp::string_value;
using array_value = telnetpp::options::msdp::array_value;
using table_value = telnetpp::options::msdp::table_value;

TEST(a_default_variable, is_an_unnamed_empty_string)
{
    telnetpp::options::msdp::variable var;

    ASSERT_EQ(""_tb, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::string_value>(var.value_);
    ASSERT_EQ(""_tb, value);
}

TEST(
    a_new_variable_with_bytes_for_name_and_value,
    has_those_values_as_name_and_value)
{
    static auto const name = "name"_tb;
    static auto const val = "value"_tb;

    telnetpp::options::msdp::variable var{name, val};
    ASSERT_EQ(name, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::string_value>(var.value_);
    ASSERT_EQ(val, value);
}

TEST(
    a_new_variable_with_storage_for_name_and_value,
    has_those_values_as_name_and_value)
{
    static telnetpp::byte_storage const name = "name"_tb;
    static telnetpp::byte_storage const val = "value"_tb;

    telnetpp::options::msdp::variable var{name, val};
    ASSERT_EQ(name, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::string_value>(var.value_);
    ASSERT_EQ(val, value);
}

TEST(
    a_new_variable_with_bytes_for_name_and_array_values,
    has_those_values_as_name_and_value)
{
    static auto const name = "array_name"_tb;
    static auto const val0 = "val0"_tb;
    static auto const val1 = "val1"_tb;

    telnetpp::options::msdp::variable var{
        name, array_value{val0, val1}
    };
    ASSERT_EQ(name, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::array_value>(var.value_);
    ASSERT_EQ(2U, value.size());
    ASSERT_EQ(val0, value[0]);
    ASSERT_EQ(val1, value[1]);
}

TEST(
    a_new_variable_with_storage_for_name_and_array_values,
    has_those_values_as_name_and_value)
{
    static telnetpp::byte_storage const name = "array_name"_tb;
    static auto const val0 = "val0"_tb;
    static auto const val1 = "val1"_tb;

    telnetpp::options::msdp::variable var{
        name, array_value{val0, val1}
    };
    ASSERT_EQ(name, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::array_value>(var.value_);
    ASSERT_EQ(2U, value.size());
    ASSERT_EQ(val0, value[0]);
    ASSERT_EQ(val1, value[1]);
}

TEST(
    a_new_variable_with_bytes_for_name_and_table_values,
    has_those_values_as_name_and_value)
{
    static auto const name = "table_name"_tb;
    telnetpp::options::msdp::variable const var{
        name,
        table_value{
                    {"val0"_tb, "val00"_tb},
                    {"val1"_tb, array_value{"val010"_tb, "val011"_tb}},
                    {"val2"_tb,
             {telnetpp::options::msdp::variable{"val20"_tb, "val200"_tb}}}}
    };

    ASSERT_EQ(name, var.name_);
    auto const value =
        std::get<telnetpp::options::msdp::table_value>(var.value_);
    ASSERT_EQ(3U, value.size());
    ASSERT_EQ("val0"_tb, value[0].name_);
    ASSERT_EQ("val1"_tb, value[1].name_);
    ASSERT_EQ("val2"_tb, value[2].name_);
    auto const value0 =
        std::get<telnetpp::options::msdp::string_value>(value[0].value_);
    ASSERT_EQ("val00"_tb, value0);
    auto const value1 =
        std::get<telnetpp::options::msdp::array_value>(value[1].value_);
    ASSERT_EQ(2U, value1.size());
    ASSERT_EQ("val010"_tb, value1[0]);
    ASSERT_EQ("val011"_tb, value1[1]);
    auto const value2 =
        std::get<telnetpp::options::msdp::table_value>(value[2].value_);
    ASSERT_EQ(1U, value2.size());
    ASSERT_EQ("val20"_tb, value2[0].name_);
    auto const value20 =
        std::get<telnetpp::options::msdp::string_value>(value2[0].value_);
    ASSERT_EQ("val200"_tb, value20);
}

TEST(a_variable, can_be_streamed_to_output)
{
    telnetpp::options::msdp::variable const var{
        "table"_tb,
        table_value{
                    {"val0"_tb, "val00"_tb},
                    {"val1"_tb, array_value{"val010"_tb, "val011"_tb}},
                    {"val2"_tb,
             {telnetpp::options::msdp::variable{"val20"_tb, "val200"_tb}}}}
    };

    std::stringstream stream;
    std::ostream &out = stream;

    out << var;

    char const *expected =
        R"(table={val0="val00",val1=[val010,val011],val2={val20="val200"}})";

    ASSERT_EQ(expected, stream.str());
}

TEST(empty_variables, compare_equal)
{
    telnetpp::options::msdp::variable lhs;
    telnetpp::options::msdp::variable rhs;

    ASSERT_EQ(lhs, rhs);
    ASSERT_FALSE(lhs != rhs);
}

TEST(string_variables_with_the_same_name_and_value, compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value"_tb, "test"_tb};
    telnetpp::options::msdp::variable rhs{"value"_tb, "test"_tb};

    ASSERT_EQ(lhs, rhs);
    ASSERT_FALSE(lhs != rhs);
}

TEST(
    string_variables_with_different_names_but_the_same_value,
    do_not_compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value0"_tb, "test"_tb};
    telnetpp::options::msdp::variable rhs{"value1"_tb, "test"_tb};

    ASSERT_NE(lhs, rhs);
    ASSERT_TRUE(lhs != rhs);
}

TEST(
    string_variables_with_the_same_name_but_different_values,
    do_not_compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value"_tb, "test0"_tb};
    telnetpp::options::msdp::variable rhs{"value"_tb, "test1"_tb};

    ASSERT_NE(lhs, rhs);
    ASSERT_TRUE(lhs != rhs);
}

TEST(string_variables_with_different_names_and_values, do_not_compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value0"_tb, "test0"_tb};
    telnetpp::options::msdp::variable rhs{"value1"_tb, "test1"_tb};

    ASSERT_NE(lhs, rhs);
    ASSERT_TRUE(lhs != rhs);
}

TEST(string_variables_and_array_variables, do_not_compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value"_tb, "test"_tb};
    telnetpp::options::msdp::variable rhs{"value"_tb, array_value{"test"_tb}};

    ASSERT_NE(lhs, rhs);
    ASSERT_TRUE(lhs != rhs);

    ASSERT_NE(rhs, lhs);
    ASSERT_TRUE(rhs != lhs);
}

TEST(string_variables_and_table_variables, do_not_compare_equal)
{
    telnetpp::options::msdp::variable lhs{"value"_tb, "test"_tb};
    telnetpp::options::msdp::variable rhs{
        "value"_tb, {telnetpp::options::msdp::variable{"value"_tb, "test"_tb}}};

    ASSERT_NE(lhs, rhs);
    ASSERT_TRUE(lhs != rhs);

    ASSERT_NE(rhs, lhs);
    ASSERT_TRUE(rhs != lhs);
}
