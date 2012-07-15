#pragma once

#include "AbstractAdapter.hpp"
#include "UserCmd.hpp"
#include <string>

namespace Iocaste { namespace Debugger {

class UserCmdParser : public AbstractAdapter<std::string>
{
private:
    AbstractOutput<UserCmd>& sink;

public:
    UserCmdParser(AbstractOutput<UserCmd>& sink_);
    virtual void WriteData(std::string const& input);
};

}}
