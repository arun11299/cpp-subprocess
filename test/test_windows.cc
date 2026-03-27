#include <iostream>
#include <thread>
#include <cpp-subprocess/subprocess.hpp>

int main()
{
  subprocess::Popen p({ "cmd.exe" }, subprocess::input(subprocess::PIPE));
  std::thread t = std::thread{ [&]() {
      while (true)
      {
        std::string ret;
        char buf[1024];
        while (fgets(buf, sizeof(buf), p.output()))
        {
          ret += buf;
          if (ret.back() == '\n')
            break;
        }
        // eof or error
        if (ret.empty())
          return ret;
        else
          std::cout << ret;
      }
    } };
  t.detach();


  int n = p.send("ping\n");
  fflush(p.input());

  n = p.send("ping 127.0.0.1\n");
  fflush(p.input());

  while (true)
  {

  }

  return 0;
}