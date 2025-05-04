#include <cpp-subprocess/subprocess.hpp>

#include <cassert>
#include <string>

namespace sp = subprocess;

// JSON requires the use of double quotes (see: https://json.org/).
// This test verifies proper handling of them.
void test_double_quotes()
{
    // A simple JSON object.
    const std::string expected{"{\"name\": \"value\"}"};
#ifdef __USING_WINDOWS__
    const std::string command{"cmd.exe /c echo "};
#else
    const std::string command{"echo "};
#endif
    auto p = sp::Popen(command + expected, sp::output{sp::PIPE});
    const auto out = p.communicate().first;
    std::string result{out.buf.begin(), out.buf.end()};
    // The `echo` command appends a newline.
    result.erase(result.find_last_not_of(" \n\r\t") + 1);
    assert(result == expected);
}

int main() {
  test_double_quotes();
  return 0;
}
