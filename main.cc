#include "subprocess.hpp"

using namespace subprocess;

int main() {
  auto p = Popen({"./script.sh"},
		 output{"out.txt"});

  auto buf = check_output("echo Hello");
  std::cout << buf.buf.data() << " :: " << buf.length << std::endl; 

  //buf = check_output("cat subprocess.hpp");
  //std::cout << buf2.buf.data() << " :: " << buf2.length << std::endl;
  //


  return 0;
}
