// LikeMagic C++ Binding Library
// Copyright 2008-2011 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#include "Bindings/Irrlicht/Bindings.hpp"

#include <irrlicht.h>

#include "LikeMagic/Utility/UserMacros.hpp"

using namespace LikeMagic;
using namespace irr::core;
using namespace irr;


namespace Bindings { namespace Irrlicht {

void add_bindings_core(RuntimeTypeSystem& type_sys)
{
    auto ns_irr_core = Namespace::global(type_sys).subspace("irr").subspace("core");

    typedef irr::core::string<fschar_t> irrFsStr;
    LM_CLASS(ns_irr_core, irrFsStr)
    LM_CONSTR(irrFsStr,,)
    LM_CONSTR(irrFsStr,, const fschar_t*)
    LM_CONSTR(irrFsStr, "newFromCStr", const fschar_t*)

    LM_CLASS(ns_irr_core, stringw)
    LM_CONSTR(stringw,,)
    LM_CONSTR(stringw,, const wchar_t*)

    LM_CLASS(ns_irr_core, dimension2du)
    LM_CONSTR(dimension2du,,)
    LM_CONSTR(dimension2du,, int, int)
    LM_FUNC(dimension2du, (set)(getArea))
    LM_FIELD(dimension2du, (Width)(Height))

    LM_CLASS(ns_irr_core, recti)
    LM_CONSTR(recti,, int, int, int, int)
    LM_CONSTR(recti, "newWithXYXY", int, int, int, int)

    LM_CLASS(ns_irr_core, vector2di)
    LM_CONSTR(vector2di,,)
    LM_CONSTR(vector2di, , int, int)
    LM_CONSTR(vector2di, "newWithXY", int, int)
    LM_FIELD(vector2di, (X)(Y))

    LM_CLASS(ns_irr_core, vector2df)
    LM_CONSTR(vector2df, "newWithXY", int, int)
    LM_FIELD(vector2df, (X)(Y))

    LM_CLASS(ns_irr_core, matrix4)
    LM_FUNC(matrix4, (getRotationDegrees))
    LM_FUNC_OVERLOAD_CONST(matrix4, "transformVect", transformVect, void, vector3df&)
    LM_FUNC_OVERLOAD_CONST(matrix4, "transformVect", transformVect, void, vector3df&, vector3df const&)
    LM_FUNC_OVERLOAD_CONST(matrix4, "transformVectOfFloats", transformVect, void, f32*, vector3df const&)

    LM_CLASS(ns_irr_core, line3df)
    LM_FIELD(line3df, (start)(end))

    LM_CLASS(ns_irr_core, triangle3df)
    LM_CONSTR(triangle3df,,)

    LM_CLASS(ns_irr_core, vector3df)
    LM_CONSTR(vector3df,,)
    LM_CONSTR(vector3df,, float)
    LM_CONSTR(vector3df,, float, float, float)
    LM_CONSTR(vector3df, "newWithXYZ", float, float, float)
    LM_CONSTR(vector3df, "newFromVector3df", vector3df const&)
    LM_FIELD(vector3df, (X)(Y)(Z))
    LM_FUNC(vector3df, (getHorizontalAngle)(rotationToDirection)(dotProduct)(crossProduct))

    LM_OP_OVERLOAD(vector3df, const, +, vector3df, vector3df const&)
    LM_OP_OVERLOAD(vector3df, const, -, vector3df, vector3df const&)
    LM_OP_OVERLOAD(vector3df, const, *, vector3df, vector3df const&)
    LM_OP_OVERLOAD(vector3df, const, /, vector3df, vector3df const&)

    // Select which overload of "set" to bind by passing template parameter to bind_method.
    // Note how you must use "(core::vector3df::*)" to ensure it is a member function pointer;
    // that part is easy to forget and hard to debug if you forget it.
    vector3df_LM.bind_method("set", static_cast<core::vector3df& (core::vector3df::*)(float, float, float)>(&core::vector3df::set));

    typedef std::vector<vector3df> vector_of_vector3df;
    LM_CLASS(ns_irr_core, vector_of_vector3df)

    LM_CLASS(ns_irr_core, aabbox3df)
    LM_CONSTR(aabbox3df,,)
    LM_CONSTR(aabbox3df, "newWithMinMaxXYZ", f32, f32, f32, f32, f32, f32)

    typedef irr::core::list<irr::scene::ISceneNodeAnimator*> ListOfAnim;
    LM_CLASS(ns_irr_core, ListOfAnim)
    LM_FUNC(ListOfAnim, (clear)(empty)(erase)(getSize)(insert_after)(insert_before)(push_back)(push_front)(size)(swap))
    LM_FUNC_OVERLOAD_CONST(ListOfAnim, "begin_c", begin, ListOfAnim::ConstIterator)
    LM_FUNC_OVERLOAD(ListOfAnim, "begin_nc", begin, ListOfAnim::Iterator)
    LM_FUNC_OVERLOAD_CONST(ListOfAnim, "end_c", end, ListOfAnim::ConstIterator)
    LM_FUNC_OVERLOAD(ListOfAnim, "end_nc", end, ListOfAnim::Iterator)
    LM_FUNC_OVERLOAD_CONST(ListOfAnim, "getLast_c", getLast, ListOfAnim::ConstIterator)
    LM_FUNC_OVERLOAD(ListOfAnim, "getLast_nc", getLast, ListOfAnim::Iterator)

    typedef ListOfAnim::Iterator ListOfAnimIter;
    LM_CLASS(ns_irr_core, ListOfAnimIter)
    LM_OP(ListOfAnimIter, (*))
    LM_OP_OVERLOAD(ListOfAnimIter,, ++, ListOfAnimIter, s32)
    LM_OP_OVERLOAD(ListOfAnimIter,, --, ListOfAnimIter, s32)
    //LM_OP_OVERLOAD(ListOfAnimIter,, ++, ListOfAnimIter&)
    //LM_OP_OVERLOAD(ListOfAnimIter,, --, ListOfAnimIter&)
    LM_OP_OVERLOAD(ListOfAnimIter, const, +, ListOfAnimIter, s32)
    LM_OP_OVERLOAD(ListOfAnimIter, const, -, ListOfAnimIter, s32)

    typedef ListOfAnim::ConstIterator ListOfAnimConstIter;
    LM_CLASS(ns_irr_core, ListOfAnimConstIter)
    LM_OP(ListOfAnimConstIter, (*))
    LM_OP_OVERLOAD(ListOfAnimConstIter,, ++, ListOfAnimConstIter, s32)
    LM_OP_OVERLOAD(ListOfAnimConstIter,, --, ListOfAnimConstIter, s32)
    //LM_OP_OVERLOAD(ListOfAnimConstIter,, ++, ListOfAnimConstIter&)
    //LM_OP_OVERLOAD(ListOfAnimConstIter,, --, ListOfAnimConstIter&)
    LM_OP_OVERLOAD(ListOfAnimConstIter, const, +, ListOfAnimConstIter, s32)
    LM_OP_OVERLOAD(ListOfAnimConstIter, const, -, ListOfAnimConstIter, s32)

    typedef array<vector3df> ArrayOfVector3df;
    LM_CLASS(ns_irr_core, ArrayOfVector3df)
    LM_CONSTR(ArrayOfVector3df,,)
    LM_CONSTR(ArrayOfVector3df, "newWithSize", u32)
    LM_CONSTR(ArrayOfVector3df, "newCopy", ArrayOfVector3df)
    LM_FUNC(ArrayOfVector3df, (allocated_size)(binary_search_multi)(clear)(const_pointer)(empty)(insert)
            (linear_reverse_search)(linear_search)(pointer)(push_back)(push_front)(reallocate)(set_free_when_destroyed)
            (set_pointer)(set_sorted)(set_used)(setAllocStrategy)(size)(sort)(swap))

    LM_FUNC_OVERLOAD_CONST(ArrayOfVector3df, "binary_search", binary_search, s32, const vector3df&, s32, s32)
    LM_FUNC_OVERLOAD_CONST(ArrayOfVector3df, "binary_search_c", binary_search, s32, const vector3df&)
    LM_FUNC_OVERLOAD(ArrayOfVector3df, "binary_search_nc", binary_search, s32, const vector3df&)
    LM_FUNC_OVERLOAD(ArrayOfVector3df, "erase", erase, void, u32, s32)
    LM_FUNC_OVERLOAD(ArrayOfVector3df, "erase", erase, void, u32)
    LM_FUNC_OVERLOAD_BOTH(ArrayOfVector3df, getLast, vector3df&)

    LM_OP(ArrayOfVector3df, (!=)(==))
    LM_FUNC_OVERLOAD(ArrayOfVector3df, "squareBrackets", operator [], vector3df&, u32)
}

}}
