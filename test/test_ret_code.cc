#include <iostream>
#include <subprocess.hpp>

namespace sp = subprocess;

void test_ret_code()
{
  std::cout << "Test::test_poll_ret_code" << std::endl;
#ifdef __USING_WINDOWS__
  auto p = sp::Popen({"cmd.exe", "/c", "exit", "1"});
#else
  auto p = sp::Popen({"/usr/bin/false"});
#endif
  while (p.poll() == -1) {
#ifdef __USING_WINDOWS__
    Sleep(100);
#else
    usleep(1000 * 100);
#endif
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

void test_ret_code_check_output()
{
  using namespace sp;
  bool caught = false;
  try {
      auto obuf = check_output({"/bin/false"}, shell{false});
      assert(false); // Expected to throw
  } catch (CalledProcessError &e) {
      std::cout << "retcode: " << e.retcode << std::endl;
      assert (e.retcode == 1);
      caught = true;
  }
  assert(caught);
}

int main() {
  test_ret_code();
#ifndef __USING_WINDOWS__
  test_ret_code_comm();
  test_ret_code_check_output();
#endif

  return 0;
}
