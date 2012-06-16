
namespace Iocaste {
    namespace Debugger {

template <typename T>
class AbstractInput
{
public:
    virtual bool HasData() const = 0;
    virtual T ReadData() = 0;
};

    }
}
