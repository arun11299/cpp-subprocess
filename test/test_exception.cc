#include <cassert>
#include <cstring>
#include <cpp-subprocess/subprocess.hpp>

namespace sp = subprocess;

void test_exception()
{
  bool caught = false;
  try {
    auto p = sp::Popen("invalid_command");
    assert(false); // Expected to throw
  } catch (sp::CalledProcessError& e) {
#ifdef __USING_WINDOWS__
    assert(std::strstr(e.what(), "CreateProcess failed: The system cannot find the file specified."));
#else
    assert(std::strstr(e.what(), "execve failed: No such file or directory"));
#endif
    caught = true;
  }
  assert(caught);
}

int main() {
  test_exception();
  return 0;
}
