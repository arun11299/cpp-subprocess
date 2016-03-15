#include "subprocess.hpp"

using namespace subprocess;

int main() {
  auto p = Popen({"./script.sh"},
		 output{"out.txt"});
  return 0;
}
