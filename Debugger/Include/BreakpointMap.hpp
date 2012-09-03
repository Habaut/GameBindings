#pragma once

#include "Exception.hpp"
#include "boost/variant.hpp"

#include <iostream>

namespace Iocaste { namespace Debugger {

struct GdbBreakpoint
{
    int number;
};

struct IoBreakpoint
{
    int number;
    std::string file_name;
    int line_number;
};

struct UserBreakpoint
{
    int number;
};

struct OurBreakpoint
{
    int number;

    bool operator ==(OurBreakpoint const& that)
    {
        return this->number == that.number;
    }
};

class BreakpointMap
{
private:
    typedef boost::variant<UserBreakpoint, OurBreakpoint> breakpoint_user;
    typedef boost::variant<GdbBreakpoint, IoBreakpoint> breakpoint_provider;
    typedef std::pair<breakpoint_user, breakpoint_provider> row_t;
    std::vector<row_t> tbl;

    template <typename Result>
    Result next_user_breakpoint()
    {
        // TODO:  right now this returns max, perhaps it should return first free?
        // (To handle the case of deleting a breakpoint.)

        int max = 0;

        try
        {
            for (row_t row : tbl)
                if (Result* m = boost::get<Result>(&row.first))
                    if (m->number > max)
                        max = m->number;
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error getting next_user_breakpoint exception was " << e.what() << std::endl;
            throw;
        }

        return {max+1};
    }

public:

    template <typename Result, typename Source>
    Result get_user_breakpoint(Source t)
    {
        try
        {
            for (row_t row : tbl)
                if (Result* m = boost::get<Result>(&row.first))
                    if (m->number == t.number)
                        return *m;
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error in get_user_breakpoint " << t.number << " exception was " << e.what() << std::endl;
            raiseError(e);
        }

        // If we didn't find an existing user breakpoint, just pull the next available number.
        Result r = next_user_breakpoint<Result>();
        tbl.push_back( { r, t } );
        return r;
    }

    template <typename Result, typename Source>
    bool has_user_breakpoint(Source t)
    {
        try
        {
            for (row_t row : tbl)
                if (Result* m = boost::get<Result>(&row.first))
                    if (m->number == t.number)
                        return true;
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error in has_user_breakpoint " << t.number << " exception was " << e.what() << std::endl;
            raiseError(e);
        }

        return false;
    }

    template <typename Result, typename Source>
    Result get_provider_breakpoint(Source t)
    {
        try
        {
            for (row_t row : tbl)
                if (Result* m = boost::get<Result>(&row.second))
                    if (m->number == t.number)
                        return *m;
        }
        catch (std::exception const& e)
        {
            std::cerr << "Error in get_provider_breakpoint " << t.number << " exception was " << e.what() << std::endl;
            raiseError(e);
        }

        // If we don't find a provider breakpoint, that's an error.
        raiseError(LogicError("No provider breakpoint for this user breakpoint number."));
    }

};

}}
