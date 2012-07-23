#include "LineInput.hpp"

using namespace std;
using namespace Iocaste::Debugger;

LineInput::LineInput(istream& stream_) :
    stream(stream_)
{
}

bool LineInput::HasData() const
{
    return stream.good();
}

string LineInput::ReadData()
{
    string result;
    getline(stream, result);
    //result.push_back('\n');
    return result;
}


