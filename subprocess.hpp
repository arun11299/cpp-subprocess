#include <iostream>
#include <map>
#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include <vector>
#include <sstream>
#include <type_traits>
#include <initializer_list>
#include <exception>

extern "C" {
  #include <unistd.h>
  #include <fcntl.h>
  #include <sys/types.h>
  #include <sys/wait.h>    
}

namespace subprocess {

// Max buffer size allocated on stack for read error
// from pipe
static const size_t SP_MAX_ERR_BUF_SIZ = 1024;

// Exception Classes
class CalledProcessError: public std::runtime_error
{
public:
  CalledProcessError(const std::string& error_msg):
    std::runtime_error(error_msg)
  {}
};



class OSError: public std::runtime_error
{
public:
  OSError(const std::string& err_msg, int err_code):
    std::runtime_error( err_msg + " : " + std::strerror(err_code) )
  {}
};

//--------------------------------------------------------------------

namespace util
{
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

  std::string join(const std::vector<std::string>& vec,
		   const std::string& sep = " ")
  {
    std::string res;
    for (auto& elem : vec) res.append(elem + sep);
    res.erase(--res.end());
    return res;
  }

  void set_clo_on_exec(int fd, bool set = true)
  {
    int flags = fcntl(fd, F_GETFL, 0);
    if (set) flags |= FD_CLOEXEC;
    else flags &= ~FD_CLOEXEC;
    //TODO: should check for errors
    fcntl(fd, F_SETFL, flags);
  }

  std::pair<int, int> pipe_cloexec()
  {
    int pipe_fds[2];
    int res = pipe(pipe_fds);
    if (res) {
      throw OSError("pipe failure", errno);
    }

    set_clo_on_exec(pipe_fds[0]);
    set_clo_on_exec(pipe_fds[1]);

    return std::make_pair(pipe_fds[0], pipe_fds[1]);
  }


  int write_n(int fd, const char* buf, size_t length)
  {
    int nwritten = 0;
    while (nwritten < length) {
      int written = write(fd, buf + nwritten, length - nwritten);
      if (written == -1) return -1;
      nwritten += written;
    }
    return nwritten;
  }

  int read_atmost_n(int fd, char* buf, size_t read_upto)
  {
    int rbytes = 0;
    int eintr_cnter = 0;

    while (1) {
      int read_bytes = read(fd, buf, read_upto);
      if (read_bytes == -1) {
      	if (errno == EINTR) {
      	  if (eintr_cnter >= 50) return -1;
      	  eintr_cnter++;
      	  continue;
	}
	return -1;
      }
      if (read_bytes == 0) return rbytes;

      rbytes += read_bytes;
    }
    return rbytes;
  }

  int wait_for_child_exit(int pid)
  {
    int status;
    int ret = -1;
    while (1) {
      ret = waitpid(pid, &status, WNOHANG); 
      if (ret == -1) break;
      if (ret == 0) continue;
      return pid;
    }

    return ret;
  }


}; // end namespace util


// Popen Arguments
struct bufsiz      { int  bufsiz = 0; };
struct defer_spawn { bool defer  = false; };
struct close_fds   { bool close_all = false; };

struct string_arg
{
  string_arg(const char* arg): arg_value(arg) {}
  string_arg(std::string&& arg): arg_value(std::move(arg)) {}
  string_arg(std::string arg): arg_value(std::move(arg)) {}
  std::string arg_value;
};

struct executable: string_arg
{
  template <typename T>
  executable(T&& arg): string_arg(std::forward<T>(arg)) {}
};

struct cwd: string_arg
{
  template <typename T>
  cwd(T&& arg): string_arg(std::forward<T>(arg)) {}
};

struct environment
{
  environment(std::map<std::string, std::string>&& env):
    env_(std::move(env)) {}
  environment(const std::map<std::string, std::string>& env):
    env_(env) {}
  std::map<std::string, std::string> env_;
};

enum IOTYPE { 
  STDIN = 0,
  STDOUT,
  STDERR,
  PIPE,
};

struct input
{
  input(int fd): rd_ch_(fd) {}

  input(const char* filename) {
    int fd = open(filename, O_RDONLY);
    if (fd == -1) throw OSError("File not found: ", errno);
    rd_ch_ = fd;
  }
  input(IOTYPE typ) {
    assert (typ == PIPE);
    std::tie(rd_ch_, wr_ch_) = util::pipe_cloexec();
  }

  int rd_ch_ = -1;
  int wr_ch_ = -1;
};

struct output
{
  output(int fd): wr_ch_(fd) {}

  output(const char* filename) {
    int fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0640);
    if (fd == -1) throw OSError("File not found: ", errno);
    wr_ch_ = fd;
  }
  output(IOTYPE typ) {
    assert (typ == PIPE);
    std::tie(rd_ch_, wr_ch_) = util::pipe_cloexec();
  }

  int rd_ch_ = -1;
  int wr_ch_ = -1;
};

struct error {
  error(int fd): wr_ch_(fd) {}

  error(const char* filename) {
    int fd = open(filename, O_APPEND | O_CREAT | O_RDWR, 0640); 
    if (fd == -1) throw OSError("File not found: ", errno);
    wr_ch_ = fd;
  }
  error(IOTYPE typ) {
    assert (typ == PIPE);
    std::tie(rd_ch_, wr_ch_) = util::pipe_cloexec();
  }

  int rd_ch_ = -1;
  int wr_ch_ = -1;
};

// ~~~~ End Popen Args ~~~~

// Fwd Decl.
class Popen;

namespace detail {

struct ArgumentDeducer
{
  ArgumentDeducer(Popen* p): popen_(p) {}

  template <typename T>
  void set_options(T&& arg)
  {
    set_option(std::forward<T>(arg));
  }

  template <typename T, typename... Args>
  void set_options(T&& farg, Args&&... rem_args)
  {
    set_option(std::forward<T>(farg));
    set_options(std::forward<Args>(rem_args)...);
  }

  void set_option(executable&& exe);

  void set_option(cwd&& cwdir);

  void set_option(bufsiz&& bsiz);

  void set_option(environment&& env);

  void set_option(defer_spawn&& defer);

  void set_option(input&& inp);

  void set_option(output&& out);

  void set_option(error&& err);

  void set_option(close_fds&& cfds);

private:
  Popen* popen_ = nullptr;
};

}; // end namespace detail


class Popen
{
public:
  friend class detail::ArgumentDeducer;

  template <typename... Args>
  Popen(const std::string& cmd_args, Args&& ...args): 
    args_(cmd_args)
  {
    vargs_ = util::split(cmd_args);
    init_args(std::forward<Args>(args)...);

    try {
      if (!defer_process_start_) execute_process();
    } catch (const std::exception& e) {
      cleanup_fds();
      throw e;
    }

    setup_comm_channels();
  }

  template <typename... Args>
  Popen(std::initializer_list<const char*> cmd_args, Args&& ...args)
  {
    vargs_.insert(vargs_.end(), cmd_args.begin(), cmd_args.end());
    init_args(std::forward<Args>(args)...);

    try {
      if (!defer_process_start_) execute_process();
    } catch (const std::exception& e) {
      cleanup_fds();
      throw e;
    }

    setup_comm_channels();
  }

  void start_process() throw (CalledProcessError, OSError)
  {
    // The process was started/tried to be started
    // in the constructor itself.
    // For explicitly calling this API to start the
    // process, 'defer_spawn' argument must be set to
    // true in the constructor.
    if (!defer_process_start_) {
      assert (0); 
      return;
    }
    try {
      execute_process();
    } catch (const std::exception& e) {
      cleanup_fds();
      throw e;
    }

    setup_comm_channels();
  }

  FILE* input()  { return input_;  }
  FILE* output() { return output_; }
  FILE* error()  { return error_;  }

private:
  template <typename... Args>
  void init_args(Args&&... args);
  void populate_c_argv();
  void execute_process() throw (CalledProcessError, OSError);
  void cleanup_fds();
  void setup_comm_channels();

private:
  bool defer_process_start_ = false;
  bool close_fds_ = false;
  int bufsiz_ = 0;
  std::string exe_name_;
  std::string cwd_;
  std::map<std::string, std::string> env_;

  FILE* input_  = nullptr;
  FILE* output_ = nullptr;
  FILE* error_  = nullptr;

  // Pipes for communicating with child

  // Emulates stdin
  int write_to_child_   = -1; // Parent owned descriptor
  int read_from_parent_ = -1; // Child owned descriptor

  // Emulates stdout
  int write_to_parent_ = -1; // Child owned descriptor
  int read_from_child_ = -1; // Parent owned descriptor

  // Emulates stderr
  int err_write_ = -1; // Write error to parent (Child owned)
  int err_read_  = -1; // Read error from child (Parent owned)

  // Command in string format
  std::string args_;
  // Comamnd provided as sequence
  std::vector<std::string> vargs_;
  std::vector<char*> cargv_;

  // Pid of the child process
  int child_pid_ = -1;
};

template <>
void Popen::init_args() {
  populate_c_argv();
}

template <typename... Args>
void Popen::init_args(Args&&... args)
{
  detail::ArgumentDeducer argd(this);
  argd.set_options(std::forward<Args>(args)...);
}

void Popen::populate_c_argv()
{
  cargv_.reserve(vargs_.size());
  for (auto& arg : vargs_) cargv_.push_back(&arg[0]);
}

void Popen::cleanup_fds()
{
  if (write_to_child_ != -1 && read_from_parent_ != -1) {
    close(write_to_child_);
    close(read_from_parent_);
  }

  if (write_to_parent_ != -1 && read_from_child_ != -1) {
    close(write_to_parent_);
    close(read_from_child_);
  }

  if (err_write_ != -1 && err_read_ != -1) {
    close(err_write_);
    close(err_read_);
  }
}

void Popen::setup_comm_channels()
{
  if (write_to_child_ != -1) {
    input_ = fdopen(write_to_child_, "wb");
  }
  if (write_to_parent_ != -1) {
    output_ = fdopen(write_to_parent_, "rb");
  }
  if (err_read_ != -1) {
    error_ = fdopen(err_read_, "rb");
  }

  auto handles = {input_, output_, error_};
  for (auto& h : handles) {
    //TODO: error checking
    if (h == nullptr) continue;
    if (bufsiz_ == 0)
      setvbuf(h, nullptr, _IONBF, BUFSIZ);
    else if (bufsiz_ == 1)
      setvbuf(h, nullptr, _IONBF, BUFSIZ);
    else
      setvbuf(h, nullptr, _IOFBF, bufsiz_);
  }
}


void Popen::execute_process() throw (CalledProcessError, OSError)
{
  int err_rd_pipe, err_wr_pipe;
  int sys_ret = -1;

  std::tie(err_rd_pipe, err_wr_pipe) = util::pipe_cloexec();

  if (!exe_name_.length()) {
    exe_name_ = vargs_[0];
  }

  child_pid_ = fork();

  if (child_pid_ < 0) {
    close (err_rd_pipe);
    close (err_wr_pipe);
    throw OSError("fork failed", errno);
  }

  if (child_pid_ == 0) {/* Child Part of Code */
    try {
      // Close descriptors belonging to parent
      if (write_to_child_ != -1) close(write_to_child_);
      if (read_from_child_ != -1) close(read_from_child_);
      if (err_read_ != -1) close(err_read_);
      close(err_rd_pipe);

      // Make the child owned descriptors as the
      // stdin, stdout and stderr for the child process
      auto _dup2_ = [](int fd, int to_fd) {
	if (fd == to_fd) {
	  // dup2 syscall does not reset the 
	  // CLOEXEC flag if the descriptors
	  // provided to it are same.
	  // But, we need to reset the CLOEXEC
	  // flag as the provided descriptors
	  // are now going to be the standard 
	  // input, output and error
	  util::set_clo_on_exec(fd, false);
	} else if(fd != -1) {
	  int res = dup2(fd, to_fd);
	  if (res == -1) throw OSError("dup2 failed", errno);
	}
      };

      // Create the standard streams
      _dup2_(read_from_parent_, 0); // Input stream
      _dup2_(write_to_parent_,  1); // Output stream
      _dup2_(err_write_,        2); // Error stream

      // Close the extra sockets
      if (read_from_parent_ != -1 && read_from_parent_ > 2) close(read_from_parent_);
      if (write_to_parent_ != -1 && write_to_parent_ > 2) close(write_to_parent_);
      if (err_write_ != -1 && err_write_ > 2) close(err_write_);

      // Close all the inherited fd's except the error write pipe
      if (close_fds_) {
	int max_fd = sysconf(_SC_OPEN_MAX);

	if (max_fd == -1) throw OSError("sysconf failed", errno);

	for (int i = 3; i < max_fd; i++) {
	  if (i == err_wr_pipe) continue;
	  close(i);
	}
      }

      // Change the working directory if provided
      if (cwd_.length()) {
	sys_ret = chdir(cwd_.c_str());
	if (sys_ret == -1) throw OSError("chdir failed", errno);
      }

      // Replace the current image with the executable
      if (env_.size()) {
	for (auto& kv : env_) setenv(kv.first.c_str(), kv.second.c_str(), 1);
	sys_ret = execvp(exe_name_.c_str(), cargv_.data());
      } else {
	sys_ret = execvp(exe_name_.c_str(), cargv_.data());
      }
      if (sys_ret == -1) throw OSError("execve failed", errno);

    } catch (const OSError& exp) {
      // Just write the exception message
      // TODO: Give back stack trace ?
      std::string err_msg(exp.what());
      //ATTN: Can we do something on error here ?
      util::write_n(err_wr_pipe, err_msg.c_str(), err_msg.length());
      close(err_wr_pipe);
    }

    // Calling application would not get this
    // exit failure
    exit (EXIT_FAILURE);
  } else { // Parent code
    // Close the err_wr_pipe
    // Else get stuck forever!!
    close (err_wr_pipe);
    // Read the error from child if at all
    char err_buf[SP_MAX_ERR_BUF_SIZ] = {0,};
    int read_bytes = util::read_atmost_n(err_rd_pipe, err_buf, SP_MAX_ERR_BUF_SIZ);

    close(err_rd_pipe);

    if (read_bytes || strlen(err_buf)) {
      // Call waitpid to reap the child process
      // waitpid suspends the calling process until the 
      // child terminates.
      sys_ret = util::wait_for_child_exit(child_pid_);
      // If the child could not be reaped successfully
      // raise that error instead of child error
      if (sys_ret == -1) throw OSError("child exit", errno);

      // Throw whatever information we have about child failure
      throw CalledProcessError(err_buf);
    }
  }
}


namespace detail {

  void ArgumentDeducer::set_option(executable&& exe) {
    popen_->exe_name_ = std::move(exe.arg_value);
  }

  void ArgumentDeducer::set_option(cwd&& cwdir) {
    popen_->cwd_ = std::move(cwdir.arg_value);
  }

  void ArgumentDeducer::set_option(bufsiz&& bsiz) {
    popen_->bufsiz_ = bsiz.bufsiz;
  }

  void ArgumentDeducer::set_option(environment&& env) {
    popen_->env_ = std::move(env.env_);
  }

  void ArgumentDeducer::set_option(defer_spawn&& defer) {
    popen_->defer_process_start_ = defer.defer;
  }

  void ArgumentDeducer::set_option(input&& inp) {
    popen_->read_from_parent_ = inp.rd_ch_;
    if (inp.wr_ch_ != -1) popen_->write_to_child_ = inp.wr_ch_;
  }

  void ArgumentDeducer::set_option(output&& out) {
    popen_->write_to_parent_ = out.wr_ch_;
    if (out.rd_ch_ != -1) popen_->read_from_child_ = out.rd_ch_;
  }

  void ArgumentDeducer::set_option(error&& err) {
    popen_->err_write_ = err.wr_ch_;
    if (err.rd_ch_ != -1) popen_->err_read_ = err.rd_ch_;
  }

  void ArgumentDeducer::set_option(close_fds&& cfds) {
    popen_->close_fds_ = cfds.close_all;
  }


}; // end namespace detail


};
