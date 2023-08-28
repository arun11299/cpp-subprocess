#include <iostream>
#include <subprocess.hpp>

using namespace subprocess;

#ifndef __USING_WINDOWS__

void test_env()
{
  int st= Popen("./env_script.sh", environment{{
					{"NEW_ENV1", "VALUE-1"},
					{"NEW_ENV2", "VALUE-2"},
					{"NEW_ENV3", "VALUE-3"}
                                    }}).wait();
  assert (st == 0);
}

#endif

int main() {
#ifndef __USING_WINDOWS__
  test_env();
#endif
  return 0;
}
