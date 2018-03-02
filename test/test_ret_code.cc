#include <iostream>
#include "../subprocess.hpp"

namespace sp = subprocess;

void test_ret_code()
{
  std::cout << "Test::test_poll_ret_code" << std::endl;
  auto p = sp::Popen({"/bin/false"});
  while (p.poll() == -1) {
    usleep(1000 * 100);
  }
  assert (p.retcode() == 1);
}

int main() {
  test_ret_code();
}
