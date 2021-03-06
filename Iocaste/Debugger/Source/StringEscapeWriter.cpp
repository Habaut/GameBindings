

#include "StringEscapeWriter.hpp"
#include "WriteGrammars.hpp"

namespace Iocaste { namespace Debugger {

boost::spirit::karma::grammar<std::back_insert_iterator<std::string>, std::string()>* string_escape_write_grammar()
{
    return new StringEscapeWriter<std::back_insert_iterator<std::string>>();
}

}}
