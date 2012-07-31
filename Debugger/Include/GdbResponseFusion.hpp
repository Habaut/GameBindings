#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/fusion/include/adapt_struct.hpp>

#include "GdbResponse.hpp"

// Ok because this header should only ever be included in implementation files:
using namespace Iocaste::Debugger;


BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::Banner,
    (std::string, version)
    (std::string, msg)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::Empty,
    (boost::optional<std::string>, dummy)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::ReadingLibs,
    (std::string, periods)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::BreakpointSet,
    (int, breakpoint_number)
    (std::string, address)
    (std::string, file_name)
    (int, line_number)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::BreakpointHit,
    (int, breakpoint_number)
    (std::string, function)
    (std::string, args)
    (std::string, file_name)
    (int, line_number)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::AddressInFunction,
    (std::string, address)
    (std::string, function)
    (std::string, args)
)


BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::BacktraceLine,
    (int, backtrace_number)
    (boost::optional<std::string>, address)
    (std::string, function)
    (std::string, args)
    (boost::optional<std::string>, module)
    (boost::optional<std::string>, file_name)
    (boost::optional<int>, line_number)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::CursorPos,
    (std::string, file_name)
    (int, line_number)
    (int, char_number)
    (std::string, unknown)
    (std::string, address)
)

BOOST_FUSION_ADAPT_STRUCT(
    GdbResponses::LocalsInfo,
    (std::string, msg)
)
