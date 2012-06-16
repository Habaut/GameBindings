#define BOOST_FILESYSTEM_VERSION 2
#include <boost/process.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
using namespace std;
namespace bp = ::boost::process;

//#include "Worker.hpp"

#include "poll.h"

bp::child start_child(std::vector<string> args)
{
    std::string exec = args[0];

    bp::context ctx;
    ctx.stdout_behavior = bp::capture_stream();
    ctx.stdin_behavior = bp::capture_stream();

    return bp::launch(exec, args, ctx);
}

int main(int argc, char* argv[])
{
    std::vector<std::string> args;
    args.push_back("/usr/bin/gdb");

    for (int i=1; i<argc; ++i)
        args.push_back(argv[i]);

    bp::child c = start_child(args);

    bp::postream& os = c.get_stdin();
    bp::pistream& is = c.get_stdout();

    int is_fd = is.handle().get();

    struct pollfd fds[1];
    fds[0].fd = is_fd;

    while (1)
    {
        std::string s;
        int failcount = 0;
        while (true)
        {
            bool is_eof = is.eof();
            int ret = poll(fds, 1, 0);
            bool has_data = (ret > 0);

            std::cerr << " is_eof = " << " ret = " << ret << " has_data = " << has_data << std::endl;

            if (is_eof || !has_data)
            {
                ++failcount;
                if (failcount > 3)
                    break;
                else
                {
                    usleep(1000000L);
                }
            }
            else
            {
                char c;
                ssize_t bytes_read = read(is_fd, &c, 1);
                //is.get(c);
                std::cout << c << std::flush;
                std::cerr << "bytes_read = " << bytes_read << std::endl;
            }
        }
        std::cout << std::flush;

        //std::cerr << "wait on input" << std::endl;
        string line;
        if (std::getline(std::cin, line))
        {
            //std::cerr << "getline" << std::endl;
            os << line << std::endl << std::flush;
        }
    }

    /*

    std::ofstream debug_log("debug.log", ofstream::out);

    Worker worker1(std::cin, os, true, "from cin to child stdin", debug_log);
    Worker worker2(is, std::cout, false, "from child stdout to cout", debug_log);

    while (!worker2.is_stopped())
    {
        pthread_yield();
        usleep(100);
    }

    worker1.stop_thread();

    c.wait();

    */

    return 0;
}

