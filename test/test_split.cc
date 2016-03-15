#include <iostream>
#include <string>
#include <vector>

std::vector<std::string>
split(const std::string& str, const std::string& delims=" \t")
{
    std::vector<std::string> res;
    size_t init = 0;

    while (true) {
      auto pos = str.find_first_of(delims, init);
      if (pos == std::string::npos) {
        res.emplace_back(str.substr(init, str.length()));
        break;
      }
      res.emplace_back(str.substr(init, pos - init));
      pos++;
      init = pos;
    }

    return res;
}

std::string join(const std::vector<std::string>& vec)
{
  std::string res;
  for (auto& elem : vec) {
  res.append(elem + " ");
  }
  res.erase(--res.end());
  return res;
}

int main() {

  auto vec = split ("a b c");
  for (auto elem : vec) { std::cout << elem << std::endl; }

  std::cout << join(vec).length() << std::endl;
  return 0;
}
