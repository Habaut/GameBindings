#include "UserCmdParser.hpp"
using namespace Iocaste::Debugger;

#if (defined(__MINGW32__) || defined(__MINGW64__)) && (__GNUC__ == 4)
#include <stddef.h>
// workaround a mingw bug, http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
int swprintf (wchar_t *, size_t, const wchar_t *, ...);
#endif

#include <boost/spirit/include/qi.hpp>
#include <boost/fusion/include/io.hpp>
#include <boost/spirit/include/karma.hpp>

#include <iostream>
#include <string>
#include <complex>
using namespace std;

#include "Exception.hpp"

using namespace Iocaste::Debugger;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include "UserCmdFusion.hpp"


// Uncomment to pass otherwise unknown commands un-parsed
//#define PARSE_RAW_STRING

namespace Iocaste {
    namespace Debugger {

template <typename Iterator>
struct UserCmdParseGrammar : qi::grammar<Iterator, UserCmd(), ascii::space_type>
{
    UserCmdParseGrammar() : UserCmdParseGrammar::base_type(start)
    {
        raw_str = +qi::print;
        ident = +(qi::alpha | qi::char_('-'));
        file_name = +(qi::print -  qi::char_(':'));
        device_name = +(qi::print); // for tty
        value = +qi::char_;
        set_option = qi::lit("set") >> set_option_no_modifier | set_option_with_modifier;
        set_option_no_modifier = ident >> value >> qi::eoi;
        set_option_with_modifier = ident >> ident >> value >> qi::eoi;
        show_option = qi::lit("show") >> ident >> -ident;
        set_breakpoint = qi::lit("break") >> "\"" >> file_name >> ":" >> qi::int_ >> "\"";
        source = qi::lit("source") >> file_name;
        directory = qi::lit("directory") >> file_name;
        tty = qi::lit("tty") >> device_name;
        run = qi::lit("run") >> -value;
        start = set_option | show_option | set_breakpoint | source | directory | tty | run;
        #ifdef PARSE_RAW_STRING
            | raw_str
        #endif
        ;
    }

    qi::rule<Iterator, std::string()> raw_str;
    qi::rule<Iterator, std::string()> ident;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, std::string()> file_name;
    qi::rule<Iterator, std::string()> device_name;
    qi::rule<Iterator, UserCmd::SetOption(), ascii::space_type> set_option_with_modifier;
    qi::rule<Iterator, UserCmd::SetOption::Pair(), ascii::space_type> set_option_no_modifier;
    qi::rule<Iterator, UserCmd::SetOption(), ascii::space_type> set_option;
    qi::rule<Iterator, UserCmd::ShowOption(), ascii::space_type> show_option;
    qi::rule<Iterator, UserCmd::SetBreakpoint(), ascii::space_type> set_breakpoint;
    qi::rule<Iterator, UserCmd::Source(), ascii::space_type> source;
    qi::rule<Iterator, UserCmd::Directory(), ascii::space_type> directory;
    qi::rule<Iterator, UserCmd::TTY(), ascii::space_type> tty;
    qi::rule<Iterator, UserCmd::Run(), ascii::space_type> run;
    qi::rule<Iterator, UserCmd(), ascii::space_type> start;
};


void UserCmd::Parse(std::string str)
{
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;
    typedef UserCmdParseGrammar<iterator_type> UserCmdGrammar;

    UserCmdGrammar g; // Our grammar

    std::string::const_iterator iter = str.begin();
    std::string::const_iterator end = str.end();
    bool success = phrase_parse(iter, end, g, space, *this);

    if (!success)
    {
        stringstream ss;
        ss << "Failed to parse: " << str << std::endl;
        throw boost::enable_current_exception(ParseException(ss.str()));
    }
    else
    {
        if (iter != end)
        {
            stringstream ss;
            ss << "Not all of the line was parsed: " << std::string(iter, end) << std::endl;
            throw boost::enable_current_exception(ParseException(ss.str()));
        }
    }
}


UserCmdParser::UserCmdParser(AbstractOutput<UserCmd>& sink_)
    : sink(sink_)
{
}

void UserCmdParser::WriteData(std::string const& input)
{
    UserCmd cmd;
    //cmd.raw_string = input;
    cmd.Parse(input);
    sink.WriteData(cmd);
}


    }
}
