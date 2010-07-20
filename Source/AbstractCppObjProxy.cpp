// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#include "LikeMagic/SFMO/AbstractCppObjProxy.hpp"

using namespace LikeMagic::Marshaling;
using namespace LikeMagic::SFMO;
using namespace LikeMagic::Utility;

std::string AbstractCppObjProxy::get_class_name() const
{
    return type_system.get_class_name(get_type());
}

AbstractCppObjProxy* AbstractCppObjProxy::call(std::string method_name, ArgList args)
{
    auto proxy1 = type_system.call(get_type(), method_name, this, args);

    if (!proxy1)
    {
        return NULL;
    }
    else if (proxy1->is_lazy() || proxy1->is_terminal())
    {
        return proxy1;
    }
    else
    {
        // eagerly evaluate the first result, delete that proxy, and return the new result instead.
        auto proxy2 = type_system.call(proxy1->get_type(), "eval", proxy1, ArgList());
        delete proxy1;
        return proxy2;
    }
}

TypeInfoList AbstractCppObjProxy::get_arg_types(std::string method_name, int num_args) const
{
    return type_system.get_arg_types(get_type(), method_name, num_args);
}

void AbstractCppObjProxy::check_magic()
{
    if (!this)
    {
        std::cout << "AbstractCppObjProxy:  'this' was null" << std::endl;
        throw std::logic_error("AbstractCppObjProxy:  'this' was null");
    }

    long magic = this->magic_number;
    if (magic == 0xFFFFFFFF)
    {
        std::cout << "check_magic: passed a CppObjProxy that has been deleted " << std::setbase(16) << magic
            << " this = " << this;
        throw std::logic_error("Deleted CppObjProxy used.");
    }
    else if (magic != 0xCAFEBABE)
    {
        std::cout << "Object was supposed to be an CppObjProxy, but is not.  Magic number was " << std::setbase(16) << magic
            << " this = " << this;
        throw std::logic_error("Object was supposed to be an CppObjProxy, but is not.");
    }
}
