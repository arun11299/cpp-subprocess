#include <iostream>
#include "../subprocess.hpp"

using namespace subprocess;

void test_input()
{
  auto p = Popen({"grep", "f"}, output{PIPE}, input{PIPE});
  const char* msg = "one\ntwo\nfour\n";
  std::fwrite(msg, 1, strlen(msg), p.input());
  fclose (p.input());

  std::vector<uint8_t> rbuf(128);
  std::fread(rbuf.data(), 1, 128, p.output());
}

int main() {
  test_input();
  return 0;
}
