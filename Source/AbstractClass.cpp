#include "LikeMagic/Marshaling/AbstractClass.hpp"
#include "LikeMagic/Marshaling/AbstractCallTargetSelector.hpp"
#include "LikeMagic/SFMO/AbstractExpression.hpp"

#include "boost/lexical_cast.hpp"

using namespace LikeMagic::Marshaling;

void AbstractClass::add_method(std::string method_name, AbstractCallTargetSelector* method)
{
    int num_args = method->get_arg_types().size();

    if (has_method(method_name, num_args))
    {
        std::cout <<
                class_name + "::" + method_name + " taking " + boost::lexical_cast<std::string>(num_args) + " arguments"
                + " has previously been registered."
                + " (Method names can be overloaded, but only if they have different arg counts. You will have to give one"
                + " of the methods a different name.)" << std::endl;
    }
    
    methods[method_name][num_args] = method;
}

AbstractCallTargetSelector* AbstractClass::get_method(std::string method_name, int num_args) const
{
    if (has_method(method_name, num_args))
        return methods.find(method_name)->second.find(num_args)->second;
    else
        for (auto it=bases.begin(); it != bases.end(); it++)
            if (it->second->has_method(method_name, num_args))
                return it->second->get_method(method_name, num_args);

    auto candidates = methods.find(method_name);

    if (candidates == methods.end())
        throw std::logic_error("Class " + get_class_name() + " does not have any method named " + method_name);
    else
    {
        std::string arg_nums_list = "";

        int count = 0;
        for (auto it=candidates->second.begin(); it!=candidates->second.end(); ++it, ++count)
        {
            if (count > 0)
                arg_nums_list += ", ";

            arg_nums_list += boost::lexical_cast<std::string>(it->first);
        }

        std::string msg = "Class " + get_class_name() + " does have a method called " + method_name
                + " but that method does not have any overload that takes " + boost::lexical_cast<std::string>(num_args)
                + " arguments.  There is/are " + boost::lexical_cast<std::string>(candidates->second.size())
                + " version(s) of that method taking " + boost::lexical_cast<std::string>(arg_nums_list) + " argument(s).";
        throw std::invalid_argument(msg);
    }
}


std::vector<BetterTypeInfo> AbstractClass::get_arg_types(std::string method_name, int num_args) const
{
    return get_method(method_name, num_args)->get_arg_types();
}

std::vector<std::string> AbstractClass::get_method_names() const
{
    std::vector<std::string> result;

    for (auto it=methods.begin(); it != methods.end(); it++)
        result.push_back(it->first);

    return result;
}

bool AbstractClass::has_method(std::string method_name, int num_args) const
{
    auto candidates = methods.find(method_name);
    
    return 
        candidates != methods.end()
    &&
        candidates->second.find(num_args) != candidates->second.end();
}

void AbstractClass::add_base_abstr(AbstractClass const* base)
{
    bases[base->get_class_name()] = base;
}

std::vector<std::string> AbstractClass::get_base_names() const
{
    std::vector<std::string> result;

    for (auto it=bases.begin(); it != bases.end(); it++)
        result.push_back(it->first);

    return result;
}

std::string AbstractClass::get_class_name() const
{
    return class_name;
}

AbstractCppObjProxy* AbstractClass::call(AbstractCppObjProxy* target, std::string method_name, std::vector<boost::intrusive_ptr<AbstractExpression>> args) const
{
    return get_method(method_name, args.size())->call(target, args);
}
