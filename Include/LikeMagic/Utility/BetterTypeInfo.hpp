// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#pragma once

#include "boost/type_traits.hpp"

#include <typeinfo>
#include <stdexcept>

#include "TypeDescr.hpp"
#include "StripModifiers.hpp"

#include "LikeMagic/Utility/AbstractTypeInfo.hpp"

namespace LikeMagic { namespace SFMO {
    template <typename T> class Expression;
}}

namespace LikeMagic { namespace Utility {

class BetterTypeInfo : public AbstractTypeInfo
{
private:
    std::type_info const* info;
    bool is_const;  // when type is pointer, this means the pointed-at object is const; to find if the bits of the pointer are const see is_const_ptr
    bool is_ref;
    bool is_ptr;
    bool is_const_ptr;  // this indicates the address stored in the pointer is unchangeable; the pointed at object may or may not be const!

    BetterTypeInfo(std::type_info const* info_, bool is_const_, bool is_ref_, bool is_ptr_, bool is_const_ptr_);
    BetterTypeInfo();
    BetterTypeInfo(BetterTypeInfo const& that);

protected:

    virtual std::string get_system() const
    {
        return "C++";
    }

    virtual bool less(const AbstractTypeInfo& other) const;
    virtual bool equals(const AbstractTypeInfo& other) const;

    virtual std::size_t calc_hash() const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, is_const);
        boost::hash_combine(seed, is_ref);
        boost::hash_combine(seed, is_ptr);
        boost::hash_combine(seed, is_const_ptr);
        boost::hash_combine(seed, std::string(info->name()));
        return seed;
    }

public:

    template <typename T>
    static TypeInfoPtr create()
    {
        typedef typename StripModifiers<T>::strip stripped;

        static TypeInfoPtr cached(
            new BetterTypeInfo(
                &typeid(typename stripped::type),
                stripped::is_const,
                stripped::is_ref,
                stripped::is_ptr,
                stripped::is_const_ptr
            )
        );

        return cached;
    }

    template <typename T>
    static TypeToken create_token()
    {
        typedef typename StripModifiers<T>::strip stripped;

        static TypeToken cached(
            TypeInfoCache::get_instance()->get_token(
                std::auto_ptr<AbstractTypeInfo const>(
                    new BetterTypeInfo(
                        &typeid(typename stripped::type),
                        stripped::is_const,
                        stripped::is_ref,
                        stripped::is_ptr,
                        stripped::is_const_ptr
                    )
                )
            )
        );

        return cached;
    }

    bool get_is_const() const;
    bool get_is_ptr() const;
    bool get_is_ref() const;
    bool get_is_const_ptr() const;

    TypeInfoPtr bare_type() const;
    TypeInfoPtr as_const_type() const;
    TypeInfoPtr as_nonconst_type() const;
    TypeInfoPtr remove_reference() const;
    TypeInfoPtr as_ptr_to_const_type() const;
    TypeInfoPtr as_ptr_to_nonconst_type() const;
    TypeInfoPtr remove_all_const() const;

    template <typename T>
    bool is_type()
    {
        return *this == BetterTypeInfo::create<T>();
    }

    virtual std::string describe() const;
};


}}
