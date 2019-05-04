#include <iostream>
#include <utility>

struct bufsize { int siz_; };
struct executable { std::string exe_; };

class Ex {
public:
    template <typename... Args>
    Ex(Args&&... args) {
        set_options(std::forward<Args>(args)...);
    }

    template <typename T>
    void set_options(T&& arg) {
        set_option(std::forward<T>(arg));
    }

    template <typename T, typename... Args>
    void set_options(T&& first, Args&&... rem_args) {
        set_option(std::forward<T>(first));
        set_options(std::forward<Args>(rem_args)...);
    }

    void set_option(bufsize&& bufs) {
        std::cout << "bufsize opt" << std::endl;
        bufsiz_ = bufs.siz_;
    }
    void set_option(executable&& exe) {
        std::cout << "exe opt" << std::endl;
        exe_name_ = std::move(exe.exe_);
    }

private:
    int bufsiz_ = 0;
    std::string exe_name_;
};

int main() {
    Ex e(bufsize{1}, executable{"finger"});
    return 0;
}
