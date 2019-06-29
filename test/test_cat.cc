#include <iostream>
#include "../subprocess.hpp"

namespace sp = subprocess;

void test_cat_pipe_redirection()
{
  std::cout << "Test::test_cat_pipe_redirection" << std::endl;
  auto p = sp::Popen({"cat", "-"}, sp::input{sp::PIPE}, sp::output{sp::PIPE});
  auto msg = "through stdin to stdout";
  auto res_buf = p.communicate(msg, strlen(msg)).first;
  assert(res_buf.length == strlen(msg));
  std::cout << "END_TEST" << std::endl;
}

void test_cat_file_redirection()
{
  std::cout << "Test::test_cat_file_redirection" << std::endl;
  auto p = sp::Popen({"cat", "-"}, sp::input{sp::PIPE}, sp::output{"cat_fredirect.txt"});
  auto msg = "through stdin to stdout";
  int wr_bytes = p.send(msg, strlen(msg));
  assert (wr_bytes == (int)strlen(msg));
  std::cout << "END_TEST" << std::endl;
}

void test_cat_send_terminate()
{
  std::cout << "Test::test_cat_send_terminate" << std::endl;
  std::vector<sp::Popen> pops;

  for (int i=0; i < 5; i++) {
    pops.emplace_back(sp::Popen({"cat", "-"}, sp::input{sp::PIPE}));
    pops[i].send("3 5\n", 5);
    pops[i].close_input();
  }

  for (int i=0; i < 5; i++) {
    pops[i].wait();
  }

  std::cout << "END_TEST" << std::endl;
}

void test_buffer_growth()
{
  auto obuf = sp::check_output({"cat", "../subprocess.hpp"});
  std::cout << obuf.length << std::endl;
  assert (obuf.length > sp::DEFAULT_BUF_CAP_BYTES);
}

void test_buffer_growth_threaded_comm()
{
  std::cout << "Test::test_buffer_growth_threaded_comm" << std::endl;
  auto buf = sp::check_output("cat ../subprocess.hpp", sp::error{sp::PIPE});
  std::cout << buf.length << std::endl;
  assert (buf.length > sp::DEFAULT_BUF_CAP_BYTES);
  std::cout << "END_TEST" << std::endl;
}

int main() {
  // test_cat_pipe_redirection();
  test_cat_send_terminate();
  /*
  test_cat_file_redirection();
  test_buffer_growth();
  test_buffer_growth_threaded_comm();
  */
  return 0;
}
