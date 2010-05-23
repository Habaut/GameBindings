
#include "LikeMagic/AbstractTypeSystem.hpp"
#include "LikeMagic/Marshaling/AbstractClass.hpp"

#include <iostream>
using namespace std;

using namespace LikeMagic;

AbstractCppObjProxy* AbstractTypeSystem::create_class_proxy(BetterTypeInfo type) const
{
    return get_class(type)->create_class_proxy();
}

bool AbstractTypeSystem::is_ref_to_value_conv(BetterTypeInfo from_type, BetterTypeInfo to_type) const
{
    return from_type.bare_type() == to_type.bare_type() && from_type.is_ptr == to_type.is_ptr && from_type.is_ref && !to_type.is_ref;
}

bool AbstractTypeSystem::is_value_to_ref_conv(BetterTypeInfo from_type, BetterTypeInfo to_type) const
{
    return from_type.bare_type() == to_type.bare_type() && from_type.is_ptr == to_type.is_ptr && !from_type.is_ref && to_type.is_ref && to_type.is_const;
}

bool AbstractTypeSystem::is_nullptr_conv(ExprPtr from, BetterTypeInfo to_type) const
{
    return to_type.is_ptr && dynamic_cast<NullExpr<void*>*>(from.get());
}

bool AbstractTypeSystem::is_const_adding_conv(BetterTypeInfo from_type, BetterTypeInfo to_type) const
{
    return
            from_type.as_const_type() == to_type
        ||
            (
                from_type.is_ptr
            &&
                (
                    from_type.as_ptr_to_const_type() == to_type
                ||
                    from_type.as_const_type().as_ptr_to_const_type() == to_type
                )
            )
        ;
}


ExprPtr AbstractTypeSystem::search_for_conv(ExprPtr from, BetterTypeInfo from_type, BetterTypeInfo to_type) const
{
    AbstractTypeConverter const* abstract_converter;

    if (has_converter(from_type, to_type))
        abstract_converter = get_converter(from_type, to_type);
    else if (has_converter(from_type.as_const_type(), to_type))
        abstract_converter = get_converter(from_type.as_const_type(), to_type);
    else if (has_converter(from_type, to_type.as_const_type()))
        abstract_converter = get_converter(from_type, to_type.as_const_type());
    else if (has_converter(from_type, to_type.as_nonconst_type()))
        abstract_converter = get_converter(from_type, to_type.as_nonconst_type());
    else if (has_converter(from_type.as_const_type(), to_type.as_nonconst_type()))
        abstract_converter = get_converter(from_type.as_const_type(), to_type.as_nonconst_type());
    else
        throw std::logic_error("No type converter from " + from_type.describe() + " to " + to_type.describe());

    return abstract_converter->wrap_expr(from);
}

AbstractCppObjProxy* AbstractTypeSystem::call
(
    BetterTypeInfo type,
    std::string method_name,
    AbstractCppObjProxy* proxy,
    std::vector<ExprPtr> args
) const
{
    return get_class(type)->call(proxy, method_name, args);
}

std::vector<std::string> const& AbstractTypeSystem::get_method_names(BetterTypeInfo type) const
{
    return get_class(type)->get_method_names();
}

std::vector<BetterTypeInfo> AbstractTypeSystem::get_arg_types(
    BetterTypeInfo type,
    std::string method_name,
    int num_args
) const
{
    return get_class(type)->get_arg_types(method_name, num_args);
}

bool AbstractTypeSystem::has_class(BetterTypeInfo type) const
{
    return classes.find(type.bare_type()) != classes.end();
}

AbstractClass const* AbstractTypeSystem::get_class(BetterTypeInfo type) const
{
    if (has_class(type))
        return classes.find(type.bare_type())->second;
    else
        return unknown_class;
}

std::vector<BetterTypeInfo>
    AbstractTypeSystem::get_registered_types() const
{
    std::vector<BetterTypeInfo> list;

    for (auto it=classes.begin(); it != classes.end(); it++)
        list.push_back(it->first);

    return list;
}

std::vector<std::string>
    AbstractTypeSystem::get_base_names(BetterTypeInfo type) const
{
    return get_class(type)->get_base_names();
}

std::string AbstractTypeSystem::get_class_name(BetterTypeInfo type) const
{
    std::string name = "";

    if (has_class(type))
        name = get_class(type)->get_class_name();
    else
    {
        name = std::string("Unknown_CppObj");
        std::cout << "warning: nothing registered for type "
            << type.describe() << std::endl;
    }

    return name;
}

bool AbstractTypeSystem::has_converter(BetterTypeInfo from, BetterTypeInfo to) const
{
    auto from_it = converters.find(from);

    return from_it != converters.end()
        && from_it->second.find(to) != from_it->second.end();
}

AbstractTypeConverter const* AbstractTypeSystem::get_converter(BetterTypeInfo from, BetterTypeInfo to) const
{
    if (!has_converter(from, to))
        throw std::logic_error(std::string("No type converter registered from ") + from.describe() + " to " + to.describe());

    auto from_it = converters.find(from);
    auto to_it = from_it->second.find(to);

    return to_it->second;
}

void AbstractTypeSystem::add_converter(BetterTypeInfo from, BetterTypeInfo to, AbstractTypeConverter const* conv)
{
    if (has_converter(from, to))
        delete converters[from][to];

    converters[from][to] = conv;
}



