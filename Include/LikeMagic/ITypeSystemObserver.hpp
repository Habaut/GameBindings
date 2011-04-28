// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#pragma once

#include "LikeMagic/Utility/TypeInfoCache.hpp"

#include <string>

namespace LikeMagic
{

    namespace Marshaling {
        class AbstractClass;
        class AbstractCallTargetSelector;
    }

class ITypeSystemObserver
{
public:
    virtual void register_class(LikeMagic::Utility::TypeIndex type_index, LikeMagic::Marshaling::AbstractClass* class_)= 0;
    virtual void register_base(LikeMagic::Marshaling::AbstractClass* class_, LikeMagic::Marshaling::AbstractClass const* base) = 0;
    virtual void register_method(LikeMagic::Marshaling::AbstractClass* class_, std::string method_name, LikeMagic::Marshaling::AbstractCallTargetSelector* method) = 0;
};

}
