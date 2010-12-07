// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

// Defines a collection of macros to aid in binding or protos.

#include "LikeMagic/RuntimeTypeSystem.hpp"

#include "boost/preprocessor/seq/for_each.hpp"
#include "boost/preprocessor/stringize.hpp"

#define LM_ENUM_PROTOS_IMPL(r, data, elem) data.add_proto(BOOST_PP_STRINGIZE(elem), elem);
#define LM_ENUM_PROTOS(vm_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_ENUM_PROTOS_IMPL, vm_name, SEQ)

#define LM_ADD_PROTO(vm_name, class_name, ...) vm_name.add_proto(#class_name, class_name(__VA_ARGS__));

// Does a to-script conversion on the proto, turning doubles into IoNumber and strings into IoSeq, etc.
#define LM_ADD_NATIVE_PROTO(vm_name, class_name, ...) vm_name.add_proto(#class_name, class_name(__VA_ARGS__), true);

#define LM_ADD_PROTOS_IMPL(r, data, elem) LM_ADD_PROTO(data, elem)
#define LM_ADD_PROTOS(vm_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_ADD_PROTOS_IMPL, vm_name, SEQ)

// Like LM_ADD_PROTO but doesn't add parens to make it a constructor call
#define LM_ADD_VALUE(vm_name, value_name, ...) vm_name.add_proto(#value_name, value_name);

#define LM_ADD_VALUES_IMPL(r, data, elem) LM_ADD_VALUE(data, elem)
#define LM_ADD_VALUES(vm_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_ADD_VALUES_IMPL, vm_name, SEQ)

// Add vector<class_name> to vm with name "vector_of_class_name"
#include <vector>
#define LM_ADD_VECTOR(vm_name, class_name) vm_name.add_proto("vector_of_" #class_name, std::vector<class_name>());

// Add vectors for several classes.
#define LM_ADD_VECTORS_IMPL(r, data, elem) LM_ADD_VECTOR(data, elem)
#define LM_ADD_VECTORS(vm_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_ADD_VECTORS_IMPL, vm_name, SEQ)

#define LM_CLASS(type_sys, class_name) auto& class_name##_LM = (type_sys).register_class<class_name>(#class_name);

#define LM_CLASS_NO_COPY(type_sys, class_name) auto& class_name##_LM = (type_sys).register_class<class_name, false>(#class_name);

#define LM_ENUM(type_sys, class_name) auto& class_name##_LM = (type_sys).register_enum<class_name>(#class_name);

// Your LikeMagic Class object must be named with the class name followed by "_LM" (do not provide the _LM to the macro)
#define LM_FUNC_IMPL(r, data, elem) data##_LM.bind_method(BOOST_PP_STRINGIZE(elem), &data::elem);
#define LM_FUNC(class_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_FUNC_IMPL, class_name, SEQ)

#define LM_FUNC_OVERLOAD(class_name, given_func_name, actual_func, ret_type, ...) class_name##_LM.bind_method<ret_type (class_name::*)(__VA_ARGS__)>(given_func_name, &class_name::actual_func);

// Use this for const member functions.
#define LM_FUNC_OVERLOAD_CONST(class_name, given_func_name, actual_func, ret_type, ...) class_name##_LM.bind_method<ret_type (class_name::*)(__VA_ARGS__) const>(given_func_name, &class_name::actual_func);

// For LM_FUNC_OVERLOAD_BOTH
template <typename T> struct LM_InsertConst { typedef T const type; };
template <typename T> struct LM_InsertConst<T*> { typedef T const* type; };
template <typename T> struct LM_InsertConst<T&> { typedef T const& type; };

#define LM_FUNC_OVERLOAD_BOTH(class_name, actual_func, ret_type, ...) LM_FUNC_OVERLOAD_CONST(class_name, #actual_func "_c", actual_func, LM_InsertConst<ret_type>::type, __VA_ARGS__) LM_FUNC_OVERLOAD(class_name, #actual_func "_nc", actual_func, ret_type, __VA_ARGS__)

#define LM_OP_OVERLOAD(class_name, CONST, op, ret_type, ...) class_name##_LM.bind_method<ret_type (class_name::*)(__VA_ARGS__) CONST>(#op, &class_name::operator op);

#define LM_OP_IMPL(r, data, elem) data##_LM.bind_method(BOOST_PP_STRINGIZE(elem), &data::operator elem);
#define LM_OP(class_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_OP_IMPL, class_name, SEQ)

#define LM_CONSTR(class_name, constructor_name, ...)  class_name##_LM.bind_constructor<__VA_ARGS__>(constructor_name);

// Your LikeMagic Class objects must be named with the class name followed by "_LM" (do not provide the _LM to the macro)
#define LM_BASE(class_name, base_name) class_name##_LM.add_base(base_name##_LM);


#define LM_FIELD_IMPL(r, data, elem) data##_LM.bind_field(BOOST_PP_STRINGIZE(elem), &data::elem);
#define LM_FIELD(class_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_FIELD_IMPL, class_name, SEQ)

#define LM_ARRAY_FIELD_IMPL(r, data, elem) data##_LM.bind_array_field(BOOST_PP_STRINGIZE(elem), &data::elem);
#define LM_ARRAY_FIELD(class_name, SEQ) BOOST_PP_SEQ_FOR_EACH(LM_ARRAY_FIELD_IMPL, class_name, SEQ)

#define LM_STATIC_FUNC(type_sys, class_name, func_name) type_sys.register_functions().bind_method(#func_name, class_name::func_name);
#define LM_STATIC_FUNC_OVERLOAD(type_sys, class_name, given_func_name, actual_func, ret_type, ...) type_sys.register_functions().bind_method<ret_type (*)(__VA_ARGS__)>(given_func_name, &class_name::actual_func);

#include "../Marshaling/ICustomField.hpp"

#define LM_BIT_FIELD(class_name, field_name) \
{ \
    typedef class_name* objp; \
    typedef decltype(objp(0)->field_name) FieldType;\
    \
    class BitField \
        : public ICustomField<class_name, FieldType> \
    { \
    public: \
    \
        virtual void set(class_name& obj, FieldType value) const \
        { \
            obj.field_name = value; \
        } \
        \
        virtual FieldType get(class_name const& obj) const \
        { \
            return obj.field_name; \
        } \
    }; \
    \
    typedef ICustomField<class_name, FieldType>* FieldAccessor; \
    FieldAccessor f(new BitField); \
    class_name##_LM.bind_custom_field<FieldAccessor>(#field_name, f); \
}

// This needs to be done once in every DLL to set the type info cache singleton to a shared value.
#define LM_SET_TYPE_INFO(type_sys) \
LikeMagic::Utility::TypeInfoCache::set_instance(type_sys.get_typeinfo_cache());
