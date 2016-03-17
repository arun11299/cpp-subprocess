#include <iostream>
#include "../subprocess.hpp"

namespace sp = subprocess;

void test_cat_pipe_redirection()
{
  std::cout << "Test::test_cat_pipe_redirection" << std::endl;
  auto p = sp::Popen({"cat", "-"}, sp::input{sp::PIPE}, sp::output{sp::PIPE});
  const char* msg = "through stdin to stdout";
  auto res_buf = p.communicate(msg, strlen(msg)).first;
  assert(res_buf.length == strlen(msg));
  std::cout << "END_TEST" << std::endl;
}

void test_cat_file_redirection()
{
  std::cout << "Test::test_cat_file_redirection" << std::endl;
  auto p = sp::Popen({"cat", "-"}, sp::input{sp::PIPE}, sp::output{"cat_fredirect.txt"});
  const char* msg = "through stdin to stdout";
  int wr_bytes = p.send(msg, strlen(msg));
  assert (wr_bytes == strlen(msg));
  std::cout << "END_TEST" << std::endl;
}

int main() {
  test_cat_pipe_redirection();
  test_cat_file_redirection();
  return 0;
}
