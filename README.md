![Subprocessing in C++]

## Design Goals
The only goal was to develop something that is as close as python2.7 subprocess module in dealing with processes.
Could not find anything similar done for C++ and here we are.

This library had these design goals:
- **Intuitive Interface**. Using modern C++ features, made the API usage as simple as that in python in most cases and even better in some (See pipiing example).

- **Correctness**. Dealing with processes is not a simple job. It has many low level details that needs to be taken care of. This library takes care of dealing with those low level details making it easier to launch processes.

- **Error Handling**. Currently the error handling is achieved via the use of exceptions. It is important that no errors are ignored silently. I am working towards achieving this goal, but most of the error conditions have been taken care of.


## Supported Platforms
Unlike python2.7 subprocess module, this library currently only supports MAC OS and Linux.
It has no support for Windows in its current state.

## Integration
Subprocess library has just a single source `subprocess.hpp` present at the top directory of this repository. All you need to do is add

```cpp
#inlcude "subprocess.hpp"

using namespace subprocess;
// OR
// namespace sp = subprocess; 
// Or give any other alias you like.
```
to the files where you want to make use of subprocessing. Make sure to add necessary switches to add C++11 support (-std=c++11 in g++ and clang).

Checkout http://templated-thoughts.blogspot.in/2016/03/sub-processing-with-modern-c.html as well.

## Compiler Support
Linux - g++ 4.8 and above
Mac OS - Clang 3.4 and later 

## Examples
Here are few examples to show how to get started:

1) Executing simple unix commands
The easy way:
```cpp
auto obuf = check_output({"ls", "-l"});
std::cout << "Data : " << obuf.buf.data() << std::endl;
std::cout << "Data len: " << obuf.length << std::endl;
```

More detailed way:
```cpp
auto p = Popen({"ls", "-l"});
auto obuf = p.communicate().first;
std::cout << "Data : " << obuf.buf.data() << std::endl;
std::cout << "Data len: " << obuf.length << std::endl;
```

2) Output redirection
Redirecting a message input to `cat` command to a file.

```cpp
auto p = Popen({"cat", "-"}, input{PIPE}, output{"cat_fredirect.txt"});
auto msg = "through stdin to stdout";
p.send(msg, strlen(msg));
p.wait();
```
OR
```cpp
auto p = Popen({"cat", "-"}, input{PIPE}, output{"cat_fredirect.txt"});
auto msg = "through stdin to stdout";
p.communicate(msg, strlen(msg))
```

OR Reading redirected output from stdout
```cpp
auto p = Popen({"grep", "f"}, output{PIPE}, input{PIPE});
auto msg = "one\ntwo\nthree\nfour\nfive\n";
p.send(msg, strlen(msg));
auto res = p.communicate();

std::cout << res.first.buf.data() << std::endl;
```

3) Piping Support
Your regular unix command piping

Ex: cat subprocess.hpp | grep template | cut -d, -f 1

```cpp
auto cat = Popen({"cat", "../subprocess.hpp"}, output{PIPE});
auto grep = Popen({"grep", "template"}, input{cat.output()}, output{PIPE});
auto cut = Popen({"cut", "-d,", "-f", "1"}, input{grep.output()}, output{PIPE});
auto res = cut.communicate().first;
std::cout << res.buf.data() << std::endl;
```

4) Easy Piping
There is another way to do piping for simple commands like above:

```cpp
auto res = pipeline("cat ../subprocess.hpp", "grep Args", "grep template");
std::cout << res.buf.data() << std::endl;
```

The commands provided to the `pipeline` function is in the order that would have appeared in your regular unix command.

5) Environment variables

For example, if a shell script requires some new environment variables to be defined, you can provide it with the below easy to use syntax.

```cpp
int st= Popen("./env_script.sh", environment{{
                                   {"NEW_ENV1", "VALUE-1"},
                                   {"NEW_ENV2", "VALUE-2"},
                                   {"NEW_ENV3", "VALUE-3"}
             }}).wait();
assert (st == 0);
```


5) Other examples
There are lots of other examples in the tests with lot more overloaded API's to support most of the functionalities supported by python2.7.

## License

<img align="right" src="http://opensource.org/trademarks/opensource/OSI-Approved-License-100x137.png">

The class is licensed under the [MIT License](http://opensource.org/licenses/MIT):

Copyright &copy; 2016-2018 [Arun Muralidharan]

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
