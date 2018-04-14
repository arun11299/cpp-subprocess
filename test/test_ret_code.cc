#include <iostream>
#include "../subprocess.hpp"

namespace sp = subprocess;

void test_ret_code()
{
  std::cout << "Test::test_poll_ret_code" << std::endl;
  auto p = sp::Popen({"/usr/bin/false"});
  while (p.poll() == -1) {
    usleep(1000 * 100);
  }
  assert (p.retcode() == 1);
}

void test_ret_code_comm()
{
  using namespace sp;
  auto cat = Popen({"cat", "../subprocess.hpp"}, output{PIPE});
  auto grep = Popen({"grep", "template"}, input{cat.output()}, output{PIPE});
  auto cut = Popen({"cut", "-d,", "-f", "1"}, input{grep.output()}, output{PIPE});
  auto res = cut.communicate().first;
  std::cout << res.buf.data() << std::endl;

  std::cout << "retcode: " << cut.retcode() << std::endl;
}

int main() {
  test_ret_code();
  test_ret_code_comm();

  return 0;
}
