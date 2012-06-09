//
// Boost.Process
// ~~~~~~~~~~~~~
//
// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#define BOOST_FILESYSTEM_VERSION 2
#include <boost/process.hpp>

#include <string>
#include <vector>
#include <iostream>

namespace bp = ::boost::process;


bp::child start_child()
{
    std::string exec = "/bin/cat";

    std::vector<std::string> args;
    args.push_back("/bin/cat");

    bp::context ctx;
    ctx.stdout_behavior = bp::capture_stream();
    ctx.stdin_behavior = bp::capture_stream();

    return bp::launch(exec, args, ctx);
}

int main()
{
    bp::child c = start_child();

    bp::postream &os = c.get_stdin();
    os << "line 1" << std::endl << "line 2" << std::endl << "line 3" << std::endl;
    os.close();

    bp::pistream &is = c.get_stdout();
    std::string line;
    while (std::getline(is, line))
        std::cout << line << std::endl;

    return 0;
}
