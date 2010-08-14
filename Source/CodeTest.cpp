


#include "LikeMagic/SFMO/Expression.hpp"

#include "LikeMagic/Utility/TypeDescr.hpp"
#include "LikeMagic/Utility/StripModifiers.hpp"
#include "LikeMagic/Utility/TypePack.hpp"

#include "boost/utility/enable_if.hpp"
#include "boost/type_traits/is_base_of.hpp"

#include <iostream>

using namespace LikeMagic;
using namespace LikeMagic::SFMO;
using namespace LikeMagic::Utility;

/*

namespace CodeTest
{

class TestClass1 : public LikeMagic::IMarkable
{
public:
    virtual void mark() const
    {
    }
};


// The compiler seems to treat numeric temporaries as "extra temporary",
// too aggressively deciding the temporary isn't needed and dropping it off the stack.
// So don't allow a const& to be made to any numeric; just store the value.
template <typename T> struct TermStoreAs { typedef T type; };
template <> struct TermStoreAs<float const&> { typedef float type; };
template <> struct TermStoreAs<double const&> { typedef double type; };
template <> struct TermStoreAs<signed char const&> { typedef signed char type; };
template <> struct TermStoreAs<int const&> { typedef int type; };
template <> struct TermStoreAs<long const&> { typedef long type; };
template <> struct TermStoreAs<long long const&> { typedef long long type; };
template <> struct TermStoreAs<unsigned char const&> { typedef unsigned char type; };
template <> struct TermStoreAs<unsigned int const&> { typedef unsigned int type; };
template <> struct TermStoreAs<unsigned long const&> { typedef unsigned long type; };
template <> struct TermStoreAs<unsigned long long const&> { typedef unsigned long long type; };

template <typename T, bool IsCopyable>
class Term :
    public Expression<T&>
{
private:
    typename TermStoreAs<T>::type value;

    static void mark(IMarkable const* obj)
    {
        obj->mark();
    }

    static void mark(IMarkable const& obj)
    {
        obj.mark();
    }

    template <typename MarkType>
    typename boost::enable_if<boost::is_base_of<IMarkable, MarkType>
        >::type mark_if_possible(TypePack<MarkType>) const
    {
        static_assert(sizeof(MarkType) && false, "Marking copyable type");
        mark(value);
    }


    template <typename MarkType>
    typename boost::disable_if<boost::is_base_of<IMarkable, MarkType>
        >::type mark_if_possible(TypePack<MarkType>) const
    {
        static_assert(sizeof(MarkType) && false, "Not marking copyable type");
    }


    Term() : value()
    {
    }

    template <typename... Args>
    Term(Args && ... args) : value(std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    Term(Args const& ... args) : value(args...)
    {
    }

public:

    static boost::intrusive_ptr<Expression<T&>> create() { return new Term(); }

    template <typename... Args>
    static boost::intrusive_ptr<Expression<T&>> create(Args && ... args) { return new Term(std::forward<Args>(args)...); }

    template <typename... Args>
    static boost::intrusive_ptr<Expression<T&>> create(Args const& ... args) { return new Term(args...); }

    inline virtual T& eval() { return value; }

    virtual boost::intrusive_ptr<Expression<T&>> clone() const { return new Term<T, IsCopyable>(value); }

    virtual std::set<AbstractObjectSet*> get_objsets() const { return std::set<AbstractObjectSet*>(); }
    virtual bool is_terminal() const { return true; }
    virtual bool is_lazy() const { return false; }

    virtual std::string description() const
    {
        return std::string("Term<" + LikeMagic::Utility::TypeDescr<T>::text() + ">");
    }

    virtual void mark() const
    {
        typedef typename StripModifiers<T>::strip::type MarkType;
        mark_if_possible(TypePack<MarkType>());
        //mark_if_possible(TypePack<TestClass1>());
    }

};


template <typename T>
class Term<T, false> :
    public Expression<T&>
{
private:
    T value;

    static void mark(IMarkable const* obj)
    {
        obj->mark();
    }

    static void mark(IMarkable const& obj)
    {
        obj.mark();
    }

    template <typename MarkType>
    typename boost::enable_if<boost::is_base_of<IMarkable, MarkType>
        >::type mark_if_possible(TypePack<MarkType>) const
    {
        static_assert(sizeof(MarkType) && false, "Marking noncopyable type");
        mark(value);
    }

    template <typename MarkType>
    typename boost::disable_if<boost::is_base_of<IMarkable, MarkType>
        >::type mark_if_possible(TypePack<MarkType>) const
    {
        static_assert(sizeof(MarkType) && false, "Not marking noncopyable type");
    }

    Term() : value()
    {
    }

    template <typename... Args>
    Term(Args && ... args) : value(std::forward<Args>(args)...)
    {
    }

    template <typename... Args>
    Term(Args const& ... args) : value(args...)
    {
    }

public:

    static boost::intrusive_ptr<Expression<T&>> create() { return new Term(); }

    template <typename... Args>
    static boost::intrusive_ptr<Expression<T&>> create(Args && ... args) { return new Term(std::forward<Args>(args)...); }

    template <typename... Args>
    static boost::intrusive_ptr<Expression<T&>> create(Args const& ... args) { return new Term(args...); }

    inline virtual T& eval() { return value; }

    virtual boost::intrusive_ptr<Expression<T&>> clone() const
    {
        throw std::logic_error("Cannot clone " + description() + " because the class is registered as not having a copy constructor.");
    }

    virtual std::set<AbstractObjectSet*> get_objsets() const { return std::set<AbstractObjectSet*>(); }
    virtual bool is_terminal() const { return true; }
    virtual bool is_lazy() const { return false; }

    virtual std::string description() const
    {
        return std::string("Term<" + LikeMagic::Utility::TypeDescr<T>::text() + ">");
    }

    virtual void mark() const
    {
        typedef typename StripModifiers<T>::strip MarkType;
        mark_if_possible(TypePack<MarkType>());
    }

};


static_assert(boost::is_base_of<IMarkable, TestClass1>::value, "is_base_of failed");

ExprPtr TestTerm  = Term<TestClass1, true>::create();



}

*/

