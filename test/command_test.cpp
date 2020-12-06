#include <telnetpp/command.hpp>
#include <gtest/gtest.h>
#include <tuple>
#include <sstream>

using testing::ValuesIn;

using command_string = std::tuple<
    telnetpp::command,
    std::string
>;

class commands_with_strings
  : public testing::TestWithParam<command_string>
{
};

TEST_P(commands_with_strings, can_be_streamed_to_an_ostream)
{
    using std::get;

    auto const &param = GetParam();
    auto const &command = get<0>(param);
    auto const &expected_string = get<1>(param);

    std::stringstream stream;
    std::ostream &out = stream;

    out << command;
    ASSERT_EQ(expected_string, stream.str());
}

static command_string const command_strings[] = {
    command_string { telnetpp::command{ telnetpp::nop }, "command[NOP]" },
    command_string { telnetpp::command{ telnetpp::dm  }, "command[DM]"  },
    command_string { telnetpp::command{ telnetpp::brk }, "command[BRK]" },
    command_string { telnetpp::command{ telnetpp::ip  }, "command[IP]"  },
    command_string { telnetpp::command{ telnetpp::ao  }, "command[AO]"  },
    command_string { telnetpp::command{ telnetpp::ayt }, "command[AYT]" },
    command_string { telnetpp::command{ telnetpp::ec  }, "command[EC]"  },
    command_string { telnetpp::command{ telnetpp::el  }, "command[EL]"  },
    command_string { telnetpp::command{ telnetpp::ga  }, "command[GA]"  },

    // It's plausible (but highly unlikely) that new commands could be added
    // to lower values.  These are output as hex codes.
    command_string { telnetpp::command{ 0x01          }, "command[0x01]"},
    command_string { telnetpp::command{ 0x1B          }, "command[0x1B]"},
};

INSTANTIATE_TEST_CASE_P(
    commands_can_be_streamed_to_an_ostream,
    commands_with_strings,
    ValuesIn(command_strings)
);
