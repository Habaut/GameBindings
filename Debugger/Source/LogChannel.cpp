#include <iostream>
#include <string>
#include <complex>
using namespace std;

#include "ActivityLog.hpp"
#include "LogChannel.hpp"
#include "ActivityLogLine.hpp"
using namespace Iocaste::Debugger;


namespace Iocaste {
    namespace Debugger {

AbstractOutput<std::string>& no_call_copy_constructor(LogChannel& value)
{
    return static_cast<AbstractOutput<std::string>&>(value);
}


LogChannel::LogChannel(
  AbstractOutput<std::string>& sink_,
  ActivityLog& log_,
  std::string label_
)
    : sink(sink_), log(log_), label(label_)
{
    log_.AddChannel(label_, *this);
}

void LogChannel::WriteData(std::string const& data)
{
    ActivityLogLine entry;
    entry.label = label;
    entry.content = data;
    log.WriteData(entry);
    sink.WriteData(data);
}

void LogChannel::WriteData(ActivityLogLine const& data)
{
    sink.WriteData(data.content);
}

    }
}

