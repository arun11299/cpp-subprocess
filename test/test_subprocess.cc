#include <iostream>
#include "../subprocess.hpp"

using namespace subprocess;

void test_input()
{
  auto p = Popen({"grep", "f"}, output{PIPE}, input{PIPE});
  const char* msg = "one\ntwo\nthree\nfour\nfive\n";
  p.send(msg, strlen(msg));
  auto res = p.communicate(nullptr, 0);
  std::cout << res.first.buf.data() << std::endl;
}

void test_piping()
{
  auto cat = Popen({"cat", "../subprocess.hpp"}, output{PIPE});
  auto grep = Popen({"grep", "template"}, input{cat.output()}, output{PIPE});
  auto cut = Popen({"cut", "-d,", "-f", "1"}, input{grep.output()}, output{PIPE});
  auto res = cut.communicate().first;
  std::cout << res.buf.data() << std::endl;
}

void test_easy_piping()
{
  auto res = pipeline("cat ../subprocess.hpp", "grep Args", "grep template");
  std::cout << res.buf.data() << std::endl;
}

void test_shell()
{
  auto obuf = check_output({"ls", "-l"}, shell{false});
  std::cout << obuf.buf.data() << std::endl;
}

int main() {
  test_input();
  test_piping();
  test_easy_piping();
  test_shell();
  return 0;
}
