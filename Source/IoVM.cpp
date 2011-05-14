// LikeMagic C++ Binding Library
// Copyright 2008-2010 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#include "LikeMagic/Backends/Io/API_Io_Impl.hpp"
#include "LikeMagic/Backends/Io/IoVM.hpp"
#include "LikeMagic/Backends/Io/IoBlock.hpp"
#include "LikeMagic/Backends/Io/IoObjectExpr.hpp"
#include "LikeMagic/Backends/Io/ToIoObjectExpr.hpp"
#include "LikeMagic/Backends/Io/FromIoTypeInfo.hpp"
#include "LikeMagic/TypeConv/NoChangeConv.hpp"

#include "LikeMagic/SFMO/CppObjProxy.hpp"

#include "LikeMagic/Utility/TypeDescr.hpp"
#include "LikeMagic/Utility/BetterTypeInfo.hpp"
#include "LikeMagic/Utility/FuncPtrTraits.hpp"
#include "LikeMagic/Utility/UserMacros.hpp"

#include "boost/lexical_cast.hpp"

#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace LikeMagic::Marshaling;
using namespace LikeMagic::SFMO;
using namespace LikeMagic::Backends::Io;
using namespace LikeMagic::Utility;

extern "C"
{
    void collector_free(void* ptr)
    {
        IoObject* io_obj = reinterpret_cast<IoObject*>(ptr);
        IoState* state = IoObject_state(io_obj);
        IoVM* iovm = reinterpret_cast<IoVM*>(state->callbackContext);
        iovm->on_collector_free(io_obj);
    }
}

/*
struct SequenceToToIoTypeInfoConv : public LikeMagic::TypeConv::AbstractTypeConverter
{
    AbstractTypeSystem const& type_sys;
    SequenceToToIoTypeInfoConv(AbstractTypeSystem const& type_sys_) : type_sys(type_sys_) {}

    virtual ExprPtr wrap_expr(ExprPtr expr) const
    {
        auto str_expr = type_sys.try_conv<std::string>(expr);
        return Term<ToIoTypeInfo, true>::create(str_expr->eval());
    }

    virtual std::string describe() const { return "SequenceToToIoTypeInfoConv"; }
};
*/

// The difference between this and a no-change or implicit conv is this evals in context to return IoObject* directly.
struct PtrToIoObjectConv : public LikeMagic::TypeConv::AbstractTypeConverter
{
    static IoObject* eval_in_context(IoObject *self, IoObject *locals, IoMessage *m, IoObject* value)
    {
        return value;
    }

    virtual ExprPtr wrap_expr(ExprPtr expr) const
    {
        return ToIoObjectExpr<IoObject*, PtrToIoObjectConv>::create(expr);
    }

    virtual std::string describe() const { return "PtrToIoObjectConv"; }
};

IoVM::IoVM(RuntimeTypeSystem& type_sys) : type_system(type_sys),
    record_freed_flag(false), free_watch_flag(false), last_exception(0)
{
    state = IoState_new();
    IoState_init(state);
    state->callbackContext = reinterpret_cast<void*>(this);
    original_free_func = state->collector->freeFunc;
    Collector_setFreeFunc_(state->collector, &collector_free);

    IoState_exceptionCallback_(state, &io_exception);

    IoObject* bootstrap = IoObject_new(state);

    IoObject_setSlot_to_(state->lobby,
        IoState_symbolWithCString_(state, "bootstrap"),
        bootstrap);

    LM_Proxy = API_io_proto(state);
    IoObject_setSlot_to_(bootstrap, IoState_symbolWithCString_(state, "LikeMagicProxy"),
        LM_Proxy);

    // Bind the forward method so all methods will work on it.
    bind_method(LM_Proxy, "forward");

    LM_Protos = IoObject_new(state);
    IoObject_setSlot_to_(bootstrap, IoState_symbolWithCString_(state, "LM_Protos"),
        LM_Protos);

    /*
    string ns_code = "CppNamespace := LikeMagic clone do(type=\"C++Namespace\"); CppNamespace global := LikeMagic CppNamespace clone";
    IoObject* ns_proto = do_string(ns_code);

    string io_code = "LikeMagic classes := Object clone do(type=\"C++Classes\")";
    IoObject* classes = do_string(io_code);

    if (last_exception)
    {
        last_exception = 0;
        throw std::logic_error("Caught Io exception while creating LikeMagic classes object.");
    }

    if (ISERROR(classes))
        throw std::logic_error("Cannot create LikeMagic classes object.");

    if (!classes || classes == state->ioNil)
    {
        throw std::logic_error("Cannot create LikeMagic classes object.");
    }
    */

    // Can only add type system observer after creating LikeMagic classes object.
    //type_system.add_type_system_observer(this);


    // IoObjectExpr expression holds unconverted Io objects; it has type of struct IoObjectExprTag.
    //type_sys.add_type(BetterTypeInfo::create_index<IoObjectExprTag>());

    add_convs_from_script(type_sys, this);
    add_convs_to_script(type_sys, this);

    LM_CLASS_NO_COPY(type_sys, ITypeSystemObserver)

    // Register this vm
    LM_CLASS_NO_COPY(type_sys, IoVM)
    LM_BASE(IoVM, ITypeSystemObserver)
    LM_FUNC(IoVM, (run_cli)(do_string)(castToIoObjectPointer)(record_freed_objects)(set_record_freed_objects)
            (watch_freed_objects)(set_watch_freed_objects)(add_watch_for_freed_object)(check_if_freed))
    LM_FIELD(IoVM, (onRegisterMethod)(onRegisterClass)(onRegisterBase))

    /*
    LM_CLASS(runtime_type_sys, AbstractTypeInfo)

    LM_CLASS(runtime_type_sys, ToIoTypeInfo)
    LM_BASE(ToIoTypeInfo, AbstractTypeInfo)
    LM_CONSTR(ToIoTypeInfo,,)
    LM_CONSTR(ToIoTypeInfo,, std::string)


    // Make the ToIoTypeInfo constructible from a string so that
    // the proxy as_script_type() can be called using a string and receive a ToIoTypeInfo.
    // Using Sequence instead of string for the from type so as not to conflict with other languages' type conversions.
    //type_sys.add_converter_simple(FromIoTypeInfo::create_index("Sequence"), BetterTypeInfo::create_index<ToIoTypeInfo>(), new SequenceToToIoTypeInfoConv(type_sys));


    LM_CLASS(runtime_type_sys, FromIoTypeInfo)
    LM_BASE(FromIoTypeInfo, AbstractTypeInfo)
    LM_CONSTR(FromIoTypeInfo,, std::string)

    */

    LM_CLASS_NO_COPY(type_sys, AbstractTypeSystem)
    LM_CLASS_NO_COPY(type_sys, RuntimeTypeSystem)
    LM_BASE(RuntimeTypeSystem, AbstractTypeSystem)

    LM_CLASS(type_sys, IoObject)

    LM_CLASS_NO_COPY(type_sys, AbstractCppObjProxy)
    LM_EXTENSION_METHOD(AbstractCppObjProxy, (proxy_to_io_obj))

    // To convert an Io object to a void*
    type_sys.add_converter_simple(FromIoTypeInfo::create_index("Object"), BetterTypeInfo::create_index<void*>(), new LikeMagic::TypeConv::ImplicitConv<IoObject*, void*>);

    // Make general Io objects convertible with IoObject*.
    type_sys.add_converter_simple(FromIoTypeInfo::create_index("Object"), BetterTypeInfo::create_index<IoObject*>(), new LikeMagic::TypeConv::NoChangeConv);
    type_sys.add_converter_simple(BetterTypeInfo::create_index<IoObject*>(), ToIoTypeInfo::create_index("Object"), new PtrToIoObjectConv);
    type_sys.add_converter_simple(ToIoTypeInfo::create_index("Object"), ToIoTypeInfo::create_index(), new LikeMagic::TypeConv::NoChangeConv);

    // Allow LikeMagic proxy objects to be converted to the C/C++ type IoObject*
    type_sys.add_converter_simple(FromIoTypeInfo::create_index("LikeMagic"), BetterTypeInfo::create_index<IoObject*>(), new LikeMagic::TypeConv::NoChangeConv);

    // Allow conversion of Io blocks to IoObject*
    type_sys.add_converter_simple(FromIoTypeInfo::create_index("Block"), BetterTypeInfo::create_index<IoObject*>(), new LikeMagic::TypeConv::NoChangeConv);

    // Allow reference/value conversions for IoBlock.
    type_sys.add_conv<LikeMagic::Backends::Io::IoBlock&, LikeMagic::Backends::Io::IoBlock>();
    type_sys.add_conv<LikeMagic::Backends::Io::IoBlock&, LikeMagic::Backends::Io::IoBlock const&>();

    // You have to have registered the types before you can add protos for them.

    // Add a proto to support all the static class members and namespace level C++ functions.
    //add_proto<StaticMethod>("CppFunc");

    // Make this vm accessible in the bootstrap environment
    add_proto<IoVM&>("IoVM", *this);

    // Also make the abstract type system available by pointer.
    add_proto<RuntimeTypeSystem&>("type_system", type_sys);

    // The object that represents the global namespace.
    add_proto("namespace", Namespace::global(type_sys).register_functions().create_class_proxy(), false);
}

void IoVM::bind_method(IoObject* obj, std::string method_name)
{
    vector<string> method_names;
    method_names.push_back(method_name);
    auto mtbl = make_io_method_table(method_names);
    IoObject_addMethodTable_(obj, mtbl);
    delete[] mtbl;
}


void IoVM::register_base(LikeMagic::Marshaling::AbstractClass const* class_, LikeMagic::Marshaling::AbstractClass const* base)
{
    //cout << "Register base " << class_->get_class_name() << " : " << base->get_class_name() << endl << flush;
    //string proto_code = code_to_get_class_proto(class_);
    //do_string(proto_code + " appendProto(" + code_to_get_class_proto(base) + ")");

    if (!onRegisterBase.empty())
        onRegisterBase(class_, base);
}

void IoVM::register_method(LikeMagic::Marshaling::AbstractClass const* class_, std::string method_name, LikeMagic::Marshaling::AbstractCallTargetSelector* method)
{
    if (!onRegisterMethod.empty())
        onRegisterMethod(class_, method_name, method);

/*
    bool debug_test = (method_name == "add_bindings_irr");

    cout << "Register method " << class_->get_class_name() << "::" << method_name << endl << flush;
    std::string class_name = class_->get_class_name();
    std::string code = "LikeMagic classes " + class_name;
    IoObject* mset_proto = do_string(code);

    if (last_exception)
    {
        last_exception = 0;
        throw std::logic_error("Caught Io exception while getting mset_proto object.");
    }

    if (ISERROR(mset_proto))
        throw std::logic_error("Cannot get LikeMagic mset_proto object.");

    vector<string> method_names;
    method_names.push_back(method_name);
    auto mtbl = make_io_method_table(method_names);
    IoObject_addMethodTable_(mset_proto, mtbl);
    delete[] mtbl;
*/

}

/*

IoObject* IoVM::create_namespace(NamespacePath const ns)
{
    if (!ns->is_root())
    {
        create_namespace(ns->get_parent());

        string test_code = "CppNamespace " + ns->get_parent()->to_string() + " hasSlot(\"" + ns->get_name() + "\")";
        bool has_slot = get_expr<bool>(test_code);

        if (!has_slot)
        {
            string create_ns_code = "CppNamespace " + ns->to_string() + " := LikeMagic clone";
            do_string(create_ns_code);
        }
    }

    return do_string("CppNamespace " + ns->to_string());
}

string IoVM::code_to_get_class_proto(LikeMagic::Marshaling::AbstractClass const* class_)
{
    std::string name = class_->get_class_name();
    NamespacePath const ns = class_->get_namespace();

    if (ns != NULL)
        return "CppNamespace " + ns->to_string() + " " + name;
    else
        return "LikeMagic classes " + name;
}

*/

void IoVM::register_class(LikeMagic::Marshaling::AbstractClass const* class_)
{
    if (!onRegisterClass.empty())
        onRegisterClass(class_);

/*

    IoObject* mset_proto;
    std::string name = class_->get_class_name();
    NamespacePath const ns = class_->get_namespace();

    if (ns != NULL)
        create_namespace(ns);

    string proto_code = code_to_get_class_proto(class_);
    do_string(proto_code + " := LikeMagic clone do(type = \"" + name + "\")");
    mset_proto = do_string(proto_code);

    if (last_exception)
    {
        last_exception = 0;
        throw std::logic_error("Caught Io exception while getting mset_proto object.");
    }

    cpp_protos[type_index] = mset_proto;

    auto mtbl = make_io_method_table(type_system.get_method_names(type_index));
    IoObject_addMethodTable_(mset_proto, mtbl);
    delete[] mtbl;

    // Give the proto a C++ proxy object so constructors and proxy methods can be called on it.
    AbstractCppObjProxy* proxy = type_system.create_class_proxy(type_index);
    IoObject_setDataPointer_(mset_proto, proxy);

    // Then hook up all the bases as protos
    auto bases = class_->get_base_classes();
    for (auto base=bases.begin(); base != bases.end(); base++)
        register_base(class_, *base);

*/

}

IoVM::~IoVM()
{
    IoState_free(state);
}

IoObject* IoVM::castToIoObjectPointer(void* p)
{
    return reinterpret_cast<IoObject*>(p);
}

void IoVM::add_proto(std::string name, AbstractCppObjProxy* proxy, bool conv_to_script) const
{
    IoObject* clone;
    if (conv_to_script)
    {
        clone = to_script(state->lobby, state->lobby, NULL, proxy);
    }
    else
    {
        //string io_code = "LikeMagic classes " + proxy->get_class_name() + " clone";
        //clone = do_string(io_code);

        //clone = API_io_rawClone(LM_Proxy);

        IoObject* proto = 0;

        TypeIndex type = proxy->get_type();

        //if (cpp_protos.find(type) == cpp_protos.end())
        //{
        //    cout << "No proto for type " << type.describe() << " using default LikeMagic proto instead." << endl;
            proto = LM_Proxy;
        //}
        //else
        //{
        //    proto = cpp_protos.find(type)->second;
        //}

        clone = API_io_rawClone(proto);

        /*
        if (last_exception)
        {
            last_exception = 0;
            throw std::logic_error("Caught Io exception while cloning mset_proto object for add proto.");
        }
        */

        IoObject_setDataPointer_(clone, proxy);
    }

    IoObject_setSlot_to_(LM_Protos, IoState_symbolWithCString_(state, name.c_str()), clone);


}

IoObject* IoVM::do_string(std::string io_code) const
{
    //std::cout << "IoVM::do_string(" << io_code << ")" << std::endl;
    std::cout << io_code << std::endl;

    IoObject* result = IoState_doCString_(state, io_code.c_str());

    // If we throw the exception here, can't check it in other IoVM code.
    // If we do not throw the exception here, could miss it in user code.
    // TODO:  Use try-catch instead of last_exception check in other IoVM functions.
    //if (last_exception)
    //{
    //    last_exception = 0;
    //    throw std::logic_error("Caught Io exception while running Io code.");
    //}

    return result;
}

void IoVM::run_cli() const
{
    IoState_runCLI(state);
}

ExprPtr IoVM::get_abs_expr(std::string io_code) const
{
    auto io_obj = do_string(io_code);
    static TypeIndex unspec_type = TypeIndex();  // id = -1, type not specified
    return from_script(state->lobby, io_obj, type_system, unspec_type);
}

void IoVM::io_exception(void* context, IoObject* coroutine)
{
    IoCoroutine_rawPrintBackTrace(coroutine);

    IoVM* vm = reinterpret_cast<IoVM*>(context);
	IoObject* e = IoCoroutine_rawException(coroutine);

    cout << "Caught Io exception: ";

    IoObject* self = coroutine;
    IoSymbol* error = IoObject_rawGetSlot_(e, IOSYMBOL("error"));

    cout << CSTRING(error) << endl;

    vm->last_exception = e;
}

void IoVM::mark() const
{
    IoObject_shouldMarkIfNonNull(last_exception);
    IoObject_shouldMarkIfNonNull(LM_Proxy);

    // Since we're using MarkableObjGraph and these are LM_FIELD's,
    // the child objects will be automatically marked.  However,
    // we do still need to mark the IoObjects above.
    //onRegisterBase.mark();
    //onRegisterClass.mark();
    //onRegisterMethod.mark();
}

// Had to accept IoObject here instead of Proxy object because we need a pointer
// to the IoVM which isn't part of Proxy object.
IoObject* IoVM::proxy_to_io_obj(IoObject* self)
{
    auto iovm = reinterpret_cast<IoVM*>(IOSTATE->callbackContext);
    IoObject* proto = iovm->LM_Proxy;
    IoObject* clone = IOCLONE(proto);

    auto proxy = reinterpret_cast<AbstractCppObjProxy*>(IoObject_dataPointer(self));
    IoObject_setDataPointer_(clone, proxy);

    return clone;
}


IoObject* IoVM::io_userfunc(IoObject *self, IoObject *locals, IoMessage *m)
{
    IOASSERT(IoObject_dataPointer(self), "No C++ object");
    try
    {
        std::string method_name = CSTRING(IoMessage_name(m));

        //std::cout << "Method called is: "  << method_name << std::endl;

        auto proxy = reinterpret_cast<AbstractCppObjProxy*>(IoObject_dataPointer(self));
        proxy->check_magic();

        auto& type_sys = proxy->get_type_system();

        int arg_count = IoMessage_argCount(m);
        auto* method = proxy->get_method(method_name, arg_count);

        std::vector<ExprPtr> args;
        TypeInfoList arg_types = method->get_arg_types();

        for (size_t i=0; i<arg_types.size(); i++)
        {
            try
            {
                ExprPtr expr = get_expr_arg_at(self, locals, m, i, type_sys, arg_types[i]);
                //std::cout << "arg " << i << " expects " << arg_types[i].describe() << " got " << expr->get_type().describe() << std::endl;
                args.push_back(expr);
            }
            catch (std::logic_error le)
            {
                throw std::logic_error(std::string() + "Error converting argument " + boost::lexical_cast<std::string>(i) + ": " + le.what());
            }
            catch (std::exception e)
            {
                throw std::logic_error(std::string() + "Error converting argument " + boost::lexical_cast<std::string>(i) + ": " + e.what());
            }
        }

        if (!IOSTATE->callbackContext)
            throw std::logic_error("The IoState does not have a callbackContext (supposed to contain pointer to IoVM object).");

        IoVM* iovm = reinterpret_cast<IoVM*>(IOSTATE->callbackContext);

        if (iovm->state != IOSTATE)
            throw std::logic_error("Failed to retrieve IoVM object from IoState callback context.");

        auto result = proxy->call(method, args);
        return iovm->to_script(self, locals, m, result);
    }
    catch (std::logic_error le)
    {
        //std::cout << "Caught exception: " << le.what() << std::endl;
        IoState_error_(IOSTATE,  m, "C++ %s, %s", LikeMagic::Utility::demangle_name(typeid(le).name()).c_str(), le.what());
        return IONIL(self);
    }
    catch (std::exception e)
    {
        //std::cout << "Caught exception: " << e.what() << std::endl;
        IoState_error_(IOSTATE,  m, "C++ %s, %s", LikeMagic::Utility::demangle_name(typeid(e).name()).c_str(), e.what());
        return IONIL(self);
    }
    catch (...)
    {
        cout << "LikeMagic:  Unknown C++ exception, aborting." << endl;
        abort();
    }
}


IoObject* IoVM::to_script(IoObject *self, IoObject *locals, IoMessage *m, AbstractCppObjProxy* proxy) const
{
    static TypeIndex to_io_type = ToIoTypeInfo::create_index();

    if (!proxy)
        return IOSTATE->ioNil;
    else
        proxy->check_magic();

    AbstractTypeSystem const& type_sys = proxy->get_type_system();
    ExprPtr from_expr = proxy->get_expr();

    bool is_terminal = proxy->is_terminal();
    bool disable_to_script = proxy->disable_to_script_conv();
    bool has_conv = is_terminal && !disable_to_script && type_sys.has_conv(from_expr->get_type(), to_io_type);

    if (!disable_to_script && proxy->get_expr()->is_null())
    {
        return IOSTATE->ioNil;
    }
    if (has_conv)
    {
        ExprPtr to_expr = type_sys.try_conv(from_expr, to_io_type);
        boost::intrusive_ptr<AbstractToIoObjectExpr> io_expr = static_cast<AbstractToIoObjectExpr*>(to_expr.get());
        IoObject* io_obj = io_expr->eval_in_context(self, locals, m);
        delete proxy;
        return io_obj;
    }
    else
    {
        //auto iter = cpp_protos.find(proxy->get_type().class_type());

        //if (iter == cpp_protos.end())
        //    throw std::logic_error("No class proto for " + proxy->get_type().describe() );

        IoObject* proto;
        //proto = iter->second;
        proto = LM_Proxy;
        IoObject* clone = IOCLONE(proto);
        IoObject_setDataPointer_(clone, proxy);

        return clone;
    }
}

void IoVM::on_collector_free(IoObject* io_obj)
{
    if(record_freed_flag)
       freed_objects.insert(io_obj);

    if(free_watch_flag && watch_for_free.find(io_obj) != watch_for_free.end())
        throw std::runtime_error("Watch object freed: " + watch_for_free[io_obj]);

    // BAD THINGS happen if you don't allow the original free function run.
    // I imagine it must be doing some necessary cleanup or fixing up collector marker pointers.
    (*original_free_func)(io_obj);
}

bool IoVM::record_freed_objects() const
{
    return record_freed_flag;
}

void IoVM::set_record_freed_objects(bool value)
{
    record_freed_flag = value;
}

bool IoVM::watch_freed_objects() const
{
    return free_watch_flag;
}

void IoVM::set_watch_freed_objects(bool value)
{
    free_watch_flag = value;
}

void IoVM::add_watch_for_freed_object(IoObject* io_obj, std::string message)
{
    watch_for_free[io_obj] = message;
}

bool IoVM::check_if_freed(IoObject* io_obj)
{
    return freed_objects.find(io_obj) != freed_objects.end();
}





