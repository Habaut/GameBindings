#pragma once

#include "LikeMagic/Backends/Io/IoVM.hpp"
#include "LikeMagic/Backends/Io/API_Io.hpp"

#include "LikeMagic/AbstractTypeSystem.hpp"
#include "LikeMagic/SFMO/CppObjProxy.hpp"

#include "LikeMagic/IMarkable.hpp"

#include <tuple>

namespace LikeMagic { namespace Backends { namespace Io {

using LikeMagic::AbstractTypeSystem;
using namespace LikeMagic::SFMO;

class IoBlock : public LikeMagic::IMarkable
{
private:
    AbstractTypeSystem const* type_sys;
    
    // The block to activate
    IoObject* io_block;
    
    // The target on which to activate the block.
    IoObject* io_target;

    template <typename T>
    AbstractCppObjProxy* make_proxy(T t) const
    {
        return CppObjProxy<T&>::create(Term<T>::create(t), *type_sys);
    }

    void add_arg(IoMessage* m, AbstractCppObjProxy* proxy) const;

    void add_args(IoMessage* m) const {}

    template <typename Arg0, typename... Args>
    void add_args(IoMessage* m, Arg0 arg0, Args... args) const
    {
        add_arg(m, make_proxy(arg0));
        add_args(m, args...);
    }
    
    IoObject* activate(IoMessage* m) const;

public:
    IoBlock();
    IoBlock(AbstractTypeSystem const* type_sys_, IoObject* io_block_, IoObject* io_target_);

    template <typename... Args>
    void operator()(Args... args) const
    {
        if (type_sys && io_block && io_target)
        {
            IoMessage* m = new_message(io_target, "foo");
            add_args(m, args...);
            activate(m);
        }
    }

    template <typename R, typename... Args>
    R eval(Args... args) const
    {
        if (!empty())
        {
            IoMessage* m = new_message(io_target, "foo");
            add_args(m, args...);
            IoObject* result = activate(m);
            ExprPtr expr = from_script(io_target, result, BetterTypeInfo::create<R>(), *type_sys);
            return type_sys->try_conv<R>(expr)->eval();
        }
        else
        {
            throw std::logic_error("Tried to eval an empty block.");
        }
    }

    bool empty() const;

    virtual void mark() const;
};


}}}