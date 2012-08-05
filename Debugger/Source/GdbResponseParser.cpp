#include "GdbResponseParser.hpp"
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

#include <boost/algorithm/string.hpp>

#include <boost/spirit/home/phoenix/bind/bind_member_variable.hpp>
#include <boost/spirit/home/phoenix/scope/let.hpp>
#include <boost/spirit/home/phoenix/scope/local_variable.hpp>
#include <boost/spirit/home/phoenix/statement/if.hpp>
using namespace boost::phoenix::local_names;
namespace phx = boost::phoenix;

#include "Exception.hpp"

using namespace Iocaste::Debugger;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

#include "GdbResponseFusion.hpp"


// Uncomment to pass otherwise unknown commands un-parsed
//#define PARSE_RAW_STRING

namespace Iocaste {
    namespace Debugger {

/*
template <typename Iterator>
struct GdbResponseGrammar : qi::grammar<Iterator, GdbResponses::TestStr2()>
{
    GdbResponseGrammar() : GdbResponseGrammar::base_type(test_str2)
    {
        raw_str = qi::char_('R') >> qi::char_('e');
        test_str1 = raw_str; //qi::string("Reading symbols for shared libraries .... done");
        test_str2 = raw_str; // *(qi::char_('R') >> qi::char_('e') >> qi::char_('a') >> qi::char_);
        uninit = qi::double_;
        //start = test_str1 | test_str2 | uninit;
        start = test_str1; // | test_str2 | uninit;
    }

    qi::rule<Iterator, std::string()> raw_str;
    qi::rule<Iterator, GdbResponses::TestStr1()> test_str1;
    qi::rule<Iterator, GdbResponses::TestStr2()> test_str2;
    qi::rule<Iterator, GdbResponses::AddressInFunction> uninit;
    qi::rule<Iterator, GdbResponseType()> start;
};
*/

template <typename Iterator>
struct GdbResponseGrammar : qi::grammar<Iterator, GdbResponseType()>
{
    GdbResponseGrammar() : GdbResponseGrammar::base_type(start)
    {
        ident = +(qi::alpha | qi::char_('-'));
        file_name = +(qi::print -  qi::char_(':') - qi::char_(','));
        device_name = +(qi::print); // for tty
        dummy = +qi::char_('\xFF');
        function_name = +(qi::char_ - qi::char_(' '));
        function_args = *(qi::char_ - qi::char_(')'));
        version_number = +(qi::digit | qi::char_('.') | qi::char_('-'));
        reading_libs = qi::lit("Reading symbols for shared libraries ") >> *(qi::char_('.') | qi::char_('+')) >> " done";

        address = qi::string("0x") >> +qi::alnum;

        breakpoint_set = qi::lit("Breakpoint ") >> qi::int_ >> " at " >> address >> ": file " >> file_name >> "," >> " line " >> qi::int_ >> ".";

        //\z\z/Users/dennisferron/code/LikeMagic-All/Iocaste/Debugger/TestProject/main.cpp:7:62:beg:0x100000e46
        cursor_pos = qi::lit("\x1A\x1A") >> file_name >> ":" >> qi::int_ >> ":" >> qi::int_ >> ":" >> *qi::alpha >> ":" >> address;

        // Breakpoint 1, main () at /Users/dennisferron/code/LikeMagic-All/Iocaste/Debugger/TestProject/main.cpp:7
        breakpoint_hit = qi::lit("Breakpoint ") >> qi::int_ >> ", " >> function_name >> " (" >> function_args >> ") at " >> file_name >> ":" >> qi::int_;

        // No locals.
        // No symbol table info available.
        no_locals = qi::string("No locals.") | qi::string("No arguments.") | qi::string("No symbol table info available.");
        locals_info = no_locals;

        // 0x0000000100000e20 in start ()
        address_in_function = address >> " in " >> function_name >> " (" >> function_args >> ")";

        // #6  0xb7f42f47 in operator new () from /usr/lib/libstdc++.so.6
        // #7  0x0805bd20 in Image<Color>::fft (this=0xb467640) at ../image_processing/image.cpp:545
        // #0  main () at /Users/dennisferron/code/LikeMagic-All/Iocaste/Debugger/TestProject/main.cpp:7
        // #0  0x0000000100000e20 in start ()
        // -(address in) (function|?? (args)) ((at file:line)(from module))
        backtrace_line = qi::lit("#") >> qi::int_ >> "  "
            >> -(address >> " in ")
            >> function_name >> " (" >> function_args >> ")"
            >> -(" from " >> file_name)
            >> -(" at " >> file_name)
            >> -(":" >> qi::int_);

        raw_str = *qi::char_;
        //test_str1 = qi::lit("#0  ") >> raw_str;

        start = reading_libs | breakpoint_set | cursor_pos | breakpoint_hit | locals_info | address_in_function | backtrace_line;
    }

    qi::rule<Iterator, std::string()> raw_str;
    qi::rule<Iterator, std::string()> ident;
    qi::rule<Iterator, std::string()> value;
    qi::rule<Iterator, std::string()> dummy;
    qi::rule<Iterator, std::string()> file_name;
    qi::rule<Iterator, std::string()> function_name;
    qi::rule<Iterator, std::string()> function_args;
    qi::rule<Iterator, std::string()> device_name;
    qi::rule<Iterator, std::string()> version_number;
    qi::rule<Iterator, std::string()> address;
    qi::rule<Iterator, std::string()> no_locals;
    qi::rule<Iterator, GdbResponses::TestStr1()> test_str1;
    qi::rule<Iterator, GdbResponses::LocalsInfo()> locals_info;
    qi::rule<Iterator, GdbResponses::ReadingLibs()> reading_libs;
    qi::rule<Iterator, GdbResponses::BreakpointSet()> breakpoint_set;
    qi::rule<Iterator, GdbResponses::BreakpointHit()> breakpoint_hit;
    qi::rule<Iterator, GdbResponses::BacktraceLine()> backtrace_line;
    qi::rule<Iterator, GdbResponses::AddressInFunction()> address_in_function;
    qi::rule<Iterator, GdbResponses::CursorPos()> cursor_pos;
    qi::rule<Iterator, GdbResponseType()> start;
};

template <typename Iterator>
struct GdbBannerGrammar : qi::grammar<Iterator, GdbResponses::Banner()>
{
    GdbBannerGrammar() : GdbBannerGrammar::base_type(banner)
    {
        version_number = +(qi::digit | qi::char_('.') | qi::char_('-'));
        banner_line = +(qi::char_ - '\n');
        banner_message = banner_line >> qi::char_('\n') >> banner_line >> qi::char_('\n') >> banner_line >> qi::char_('\n') >> banner_line >> qi::char_('\n') >> banner_line >> qi::char_('\n') >> banner_line >> qi::char_('\n') >> banner_line;
        banner = qi::lit("GNU gdb ") >> version_number >> banner_message >> qi::lit("\n") >> qi::eoi;
    }

    qi::rule<Iterator, std::string()> version_number;
    qi::rule<Iterator, std::string()> banner_line;
    qi::rule<Iterator, std::string()> banner_message;
    qi::rule<Iterator, GdbResponses::Banner()> banner;
};


vector<GdbResponseType> GdbResponseParser::Parse(string const& input) const
{
    using boost::spirit::ascii::space;
    typedef std::string::const_iterator iterator_type;

    iterator_type banner_iter = input.begin();
    iterator_type banner_end = input.end();
    vector<GdbResponseType> result;

    // The banner doesn't conform to the pattern of the other GdbResponses.
    // Rather than twist the grammar to handle it I just handle it with another parser.
    GdbBannerGrammar<iterator_type> g_banner; // Banner grammar
    GdbResponses::Banner banner;
    if (parse(banner_iter, banner_end, g_banner, banner))
    {
        result.push_back(banner);
        return result;
    }
    else
    {
        // When the grammar fails to match I want to know which line failed.
        // I couldn't figure out how to get spirit expectation failure to work
        // in a spirit grammar (nothing happens) so I just handle each line individually.
        vector<string> lines;
        boost::split(lines, input, boost::is_any_of("\n"));

        // Grrr, boost split adds a suprious empty entry at the end for the last newline.
        // It also writes one vector entry for an empty input string instead of returning zero elements.
        if ( (*(lines.end()-1)).size() == 0)
            lines.erase(lines.end()-1);

        for (string line : lines)
        {
            if (line.size() == 0)
            {
                result.push_back(GdbResponses::Empty());
            }
            else
            {
                GdbResponseGrammar<iterator_type> g; // Our grammar
                iterator_type iter = line.begin();
                iterator_type end = line.end();

                GdbResponseType line_item;
                bool success = parse(iter, end, g, line_item);

                if (!success)
                {
                    stringstream ss;
                    ss << "GdbResponse failed to parse ->" << line << "<- in string ->" << input << "<-" << std::endl;
                    cerr << endl << ss.str() << endl;
                    raiseError(ParseException(ss.str()));
                }
                else if (iter != end)
                {
                    if (boost::get<GdbResponses::UninitializedVariant>(&line_item))
                        cerr << "Uninitialized after parse, success was " << success << " line was " << line << endl;
                    else if (auto* p = boost::get<GdbResponses::TestStr1>(&line_item))
                    {
                        cerr << "Got " << p->value << endl;
                    }

                    stringstream ss;
                    ss << "Not all of the line was parsed: " << std::string(iter, end) << std::endl;
                    cerr << endl << ss.str() << endl;
                    raiseError(ParseException(ss.str()));
                }
                else
                {
                    if (boost::get<GdbResponses::UninitializedVariant>(&line_item))
                        cerr << "Uninitialized after parse, success was " << success << " line was " << line << endl;
                    else if (auto* p = boost::get<GdbResponses::TestStr1>(&line_item))
                    {
                        cerr << "Got " << p->value << endl;
                    }

                    result.push_back(line_item);
                }
            }
        }
    }

    return result;
}


GdbResponseParser::GdbResponseParser(AbstractOutput<GdbResponse>& sink_)
    : sink(sink_)
{
}

void GdbResponseParser::WriteData(StringWithPrompt const& input)
{
    GdbResponse response = { Parse(input.content), input.prompt };
    sink.WriteData(response);
}

    }
}
