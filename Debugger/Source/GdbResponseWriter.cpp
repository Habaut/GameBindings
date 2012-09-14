#include "GdbResponseWriter.hpp"
#include "Exception.hpp"
#include "SharedTypesPrinter.hpp"

#if (defined(__MINGW32__) || defined(__MINGW64__)) && (__GNUC__ == 4)
#include <stddef.h>
// workaround a mingw bug, http://sourceforge.net/tracker/index.php?func=detail&aid=2373234&group_id=2435&atid=102435
int swprintf (wchar_t *, size_t, const wchar_t *, ...);
#endif

#include "WriteGrammars.hpp"

using namespace Iocaste::Debugger;

#include <string>
using namespace std;

#include "StringEscapeWriter.hpp"

using namespace Iocaste::Debugger::GdbResponses;

namespace Iocaste { namespace Debugger {

struct GdbResponsePrinter : public SharedTypesPrinter
{
    void operator()(const ProgramExited& t) const
    {
        cerr << "Program exited, how: " << t.how << endl;
    }

    void operator()(const ValueHistory& t) const
    {
        cerr << "Value history is $" << t.number << endl;
    }

    void operator()(const RawStr& t) const
    {
        cerr << "RawStr is " << t.value << endl;
    }

    void operator()(const SquareBracketMsg& t) const
    {
        cerr << "SquareBracketMsg is " << t.msg << endl;
    }

    void operator()(const SignalReceived& t) const
    {
        cerr << "SignalReceived is " << t.msg << endl;
    }

    void operator()(const UninitializedVariant& t) const
    {
        cerr << "got uninitialized variant" << endl;
        //raiseError(GeneratorException("Generator was passed uninitialized variant in GdbResponse object."));
    }

    void operator()(const TestStr1& t) const
    {
        cerr << "got test string1 ->" << t.value << "<-" << endl;
    }

    void operator()(const TestStr2& t) const
    {
        cerr << "got test string2 ->" << t.value << "<-" << endl;
    }

    void operator()(const Banner& t) const
    {
        cerr << "banner is version->" << t.version << "< message->" << t.msg << "<" << endl;
    }

    void operator()(const ReadingSymbols& t) const
    {
        cerr << "reading libs is " << t.message << endl;
    }

    void operator()(const LocalsInfo& t) const
    {
        cerr << "LocalsInfo is ";
        boost::apply_visitor(SharedTypesPrinter(), t.value);
        cerr << endl;
    }

    void operator()(const BreakpointSet& t) const
    {
        cerr << "breakpoint set is "
        << t.breakpoint_number << " "
        << t.address.hex_value << " "
        << t.file_name << " "
        << t.line_number
        << endl;
    }

    void operator()(const CursorPos& t) const
    {
        cerr << "cursor pos is"
        << " " << t.file_name
        << " " << t.line_number
        << " " << t.char_number
        << " " << t.unknown
        << " " << t.address.hex_value
        << endl;
    }

    void operator()(const BreakpointHit& t) const
    {
        cerr << "breakpoint hit is"
        << " " << t.breakpoint_number
        << " " << t.function.name << " (";

        for (auto arg : t.function.args)
        {
            cerr << arg.name << arg.equals << "GdbValue";
            if (arg.value.value_as_string)
                cerr << " \"" << arg.value.value_as_string->text << "\"";
            cerr << ", ";
        }

        cerr << ") " << t.file_name
        << " " << t.line_number
        << endl;
    }

    template <typename T>
    void printOpt(std::string msg, boost::optional<T> value) const
    {
        if (value)
        {
            cerr << msg;
            SharedTypesPrinter()(*value);
        }
    }

    void operator()(const BacktraceLine& t) const
    {
        cerr << "backtrace line is"
        << " #" << t.backtrace_number;
        printOpt(" at addr:", t.address_in);
        cerr << " in func:" << t.function.name;
        cerr << " with args:";
        SharedTypesPrinter()(t.function.args);
        printOpt(" from module:", t.module);
        printOpt(" in file:", t.file_name);
        printOpt(" at line:", t.line_number);
        cerr << endl;
    }

    void operator()(const AddressInFunction& t) const
    {
        cerr << "address in function is"
        << " " << t.address_in.address.hex_value
        << " " << t.function.name
        << " " << t.function.args.size()
        << endl;
    }

    void operator()(const Empty& t) const
    {
        cerr << "empty is (no members)" << endl;
    }
};

string GdbResponseWriter::Write(vector<GdbResponseType> const& response) const
{
    namespace karma = boost::spirit::karma;
    typedef std::back_insert_iterator<std::string> sink_type;

    // For debugging
    for (auto element : response)
        boost::apply_visitor(GdbResponsePrinter(), element);

    std::string result;
    sink_type sink(result);

    std::unique_ptr<
        boost::spirit::karma::grammar<std::back_insert_iterator<std::string>, vector<GdbResponseType>()>
    > g(response_write_grammar());

    if(!karma::generate(sink, *g, response))
        raiseError(GeneratorException("Error writing user response."));

    return result;
}


GdbResponseWriter::GdbResponseWriter(AbstractOutput<StringWithPrompt>& sink_)
    : sink(sink_) {}

void GdbResponseWriter::WriteData(GdbResponse const& input)
{
    string result = Write(input.values);
    sink.WriteData({result, input.prompt});
}

}}
