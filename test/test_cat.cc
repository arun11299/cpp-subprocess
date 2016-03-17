#include <iostream>
#include "../subprocess.hpp"

using namespace subprocess;

int main() {
  auto p = Popen({"cat", "-"},
		 input{PIPE},
		 output{PIPE});
  const char* msg = "through stdin to stdout";
  auto res = p.communicate(msg, strlen(msg)).first;
  std::cout << res.buf.data() << std::endl;

  return 0;
}
