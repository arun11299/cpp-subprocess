#include <iostream>
#include "../subprocess.hpp"

using namespace subprocess;

void test_input()
{
  auto p = Popen({"grep", "f"}, output{PIPE}, input{PIPE});
  const char* msg = "one\ntwo\nthree\nfour\nfive\n";
  p.send(msg, strlen(msg));
  auto res = p.communicate(nullptr, 0);
  std::cout << res.first.data() << std::endl;
}

int main() {
  test_input();
  return 0;
}
