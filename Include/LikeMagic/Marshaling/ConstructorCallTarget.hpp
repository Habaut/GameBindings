// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#pragma once

#include "LikeMagic/Utility/FuncPtrTraits.hpp"

#include "AbstractCallTargetSelector.hpp"

#include "../SFMO/CppObjProxy.hpp"

namespace LikeMagic { namespace Marshaling {

using namespace LikeMagic::Utility;
using namespace LikeMagic::SFMO;

// Create as temporary value object.
template <typename ObjT, bool IsCopyable, typename... Args>
class ConstructorCallTarget : public AbstractCallTargetSelector
{
private:
    AbstractTypeSystem const& type_system;

    // This calls the Term::create function that constructs the object based on args.
    template<int... Indices>
    AbstractCppObjProxy* construct_obj(AbstractCppObjProxy* proxy, ArgList args, IndexPack<Indices...>) const
    {
        return CppObjProxy<ObjT&, IsCopyable>::create(
                Term<ObjT, IsCopyable>::create(
                    type_system.try_conv<Args>(args[Indices])->eval()...
                ), type_system
        );
    }

public:

    ConstructorCallTarget(AbstractTypeSystem const& type_system_) : type_system(type_system_) {}

    virtual AbstractCppObjProxy* call(AbstractCppObjProxy* proxy, ArgList args) const
    {
        typedef typename MakeIndexPack<sizeof...(Args)>::type IPack;
        return construct_obj(proxy, args, IPack());
    }

    virtual TypeInfoList get_arg_types() const
    {
        return make_arg_list(TypePack<Args...>());
    }

};

// Create by pointer.
template <typename ObjT, bool IsCopyable, typename... Args>
class ConstructorCallTarget<ObjT*, IsCopyable, Args...> : public AbstractCallTargetSelector
{
private:
    AbstractTypeSystem const& type_system;

    // This calls the Term::create function that constructs the object based on args.
    template<int... Indices>
    AbstractCppObjProxy* construct_obj(AbstractCppObjProxy* proxy, ArgList args, IndexPack<Indices...>) const
    {
        return CppObjProxy<ObjT*&, IsCopyable>::create(
                Term<ObjT*, IsCopyable>::create(
                    new ObjT(type_system.try_conv<Args>(args[Indices])->eval()...)
                ), type_system
        );
    }

public:

    ConstructorCallTarget(AbstractTypeSystem const& type_system_) : type_system(type_system_) {}

    virtual AbstractCppObjProxy* call(AbstractCppObjProxy* proxy, ArgList args) const
    {
        typedef typename MakeIndexPack<sizeof...(Args)>::type IPack;
        return construct_obj(proxy, args, IPack());
    }

    virtual TypeInfoList get_arg_types() const
    {
        return make_arg_list(TypePack<Args...>());
    }

};


}}
