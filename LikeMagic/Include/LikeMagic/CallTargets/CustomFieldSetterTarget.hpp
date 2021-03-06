// LikeMagic C++ Binding Library
// Copyright 2008-2011 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#pragma once

#include "LikeMagic/Utility/StripModifiers.hpp"

#include "LikeMagic/CallTargets/AbstractCallTargetSelector.hpp"

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_same.hpp"
#include "boost/type_traits/is_void.hpp"

#include "LikeMagic/SFMO/MethodCall.hpp"
#include "LikeMagic/SFMO/ExprProxy.hpp"

#include "LikeMagic/CallTargets/ICustomField.hpp"

#include "boost/shared_ptr.hpp"

namespace LikeMagic { namespace CallTargets {

using namespace LikeMagic::Utility;
using namespace LikeMagic::SFMO;

template <typename CallAs, typename FieldAccessor>
class CustomFieldSetterTarget : public AbstractCallTargetSelector
{
private:
    typedef StripModifiers<CallAs> stripped;
    typedef typename stripped::type ObjType;
    FieldAccessor f;
    typedef typename boost::remove_pointer<FieldAccessor>::type::FieldType FieldType;

public:

    static bool const is_const_func = false;

    CustomFieldSetterTarget(FieldAccessor f_, AbstractTypeSystem const& type_system_) : AbstractCallTargetSelector(type_system_), f(f_) {}

    // Both the set and get target have a pointer to the same field accessor object;
    // only one of them needs to delete the shared accessor.
    ~CustomFieldSetterTarget() { delete f; }

    virtual AbstractCppObjProxy* call(AbstractCppObjProxy* proxy, ArgList args) const
    {
        f->set(
            type_system.try_conv<CallAs>(proxy->get_expr())->eval(),
            type_system.try_conv<FieldType>(args[0])->eval()
        );
        return 0;
    }

    virtual TypeInfoList get_arg_types() const
    {
        return make_arg_list(TypePack<FieldType>());
    }

};

}}
