
#pragma once

#include "Expression.hpp"

#include "LikeMagic/Utility/TypeDescr.hpp"

#include <boost/type_traits/is_base_of.hpp>

#include <iostream>

namespace LikeMagic { namespace SFMO {

using namespace LikeMagic::Utility;

template <typename T, bool is_markable=boost::is_base_of<LikeMagic::IMarkable, T>::value>
struct IfCanMark
{
    static void mark(IMarkable const& obj)
    {
        //std::cout << "Marking " << BetterTypeInfo::create<T>().describe() << std::endl;
        obj.mark();
    }
};

template <typename T>
struct IfCanMark<T, false>
{
    static void mark(T const& obj)
    {
        //std::cout << BetterTypeInfo::create<T>().describe() << " is not markable." << std::endl;
    }
};

template <typename T>
class Term : 
    public Expression<T&>
{
private:
    T value;

    template <typename... Args>
    Term(Args... args) : value(args...)
    {
    }

    template <typename T_>
    typename boost::disable_if<boost::is_base_of<LikeMagic::IMarkable, T_>>::type
    mark(T_ const&) const
    {
        std::cout << "Not marking: " << description() << std::endl;
    }

    template <typename T_>
    typename boost::enable_if<boost::is_base_of<LikeMagic::IMarkable, T_>>::type
    mark(LikeMagic::IMarkable const& obj) const
    {
        std::cout << "Marking: " << description() << std::endl;
        obj.mark();
    }

public:

    template <typename... Args>
    static boost::intrusive_ptr<Expression<T&>> create(Args... args) { return new Term(args...); }

    inline virtual T& eval() { return value; }
    virtual boost::intrusive_ptr<Expression<T&>> clone() const { return new Term<T>(value); }
    virtual std::set<AbstractObjectSet*> get_objsets() const { return std::set<AbstractObjectSet*>(); }
    virtual bool is_terminal() const { return true; }
    virtual bool is_lazy() const { return false; }

    virtual std::string description() const
    {
        return std::string("Term<" + LikeMagic::Utility::TypeDescr<T>::text() + ">");
    }

    virtual void mark() const
    {
        IfCanMark<T>::mark(value);
    }

};

template <>
class Term<void> : public Expression<void>
{
private:
    Term()
    {
        std::cout << "warning:  void term exists." << std::endl;
    }

public:
    //static boost::intrusive_ptr<Expression<void>> create() { return new Term(); }

    template <typename... Args>
    static boost::intrusive_ptr<Expression<void>> create(Args... args) { return new Term(); }

    // A term never has any object sets, even if it's a container, because it represents one object by itself so it doesn't
    // have anything to iterate against.  There is another class called ContainerSet that holds a Term
    // but represents an element from the set in the held Term; it's target does change when the set is iterated.
    virtual std::set<AbstractObjectSet*> get_objsets() const { return std::set<AbstractObjectSet*>(); }

    inline virtual void eval() { }
    virtual boost::intrusive_ptr<Expression<void>> clone() const { return new Term(); }
    virtual bool is_terminal() const { return true; }
    virtual bool is_lazy() const { return false; }

    virtual std::string description() const
    {
        return std::string("Term<void>");
    }

    virtual void mark() const {}

};

}}
