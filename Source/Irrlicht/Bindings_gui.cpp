// LikeMagic C++ Binding Library
// Copyright 2008-2011 Dennis Ferron
// Co-founder DropEcho Studios, LLC.
// Visit our website at dropecho.com.
//
// LikeMagic is BSD-licensed.
// (See the license file in LikeMagic/Licenses.)

#include "Bindings/Irrlicht/Bindings.hpp"

#include "irrlicht.h"
#include "vector2d.h"


#include "LikeMagic/Utility/UserMacros.hpp"

using namespace LikeMagic;
using namespace irr;
using namespace irr::gui;

namespace Bindings { namespace Irrlicht {

void add_bindings_gui(RuntimeTypeSystem& type_sys)
{
    auto ns_irr_gui = Namespace::global(type_sys).subspace("irr").subspace("gui");

    LM_CLASS(ns_irr_gui, IReferenceCounted)

    LM_CLASS(ns_irr_gui, IGUIEnvironment)
    LM_BASE(IGUIEnvironment, IReferenceCounted)
    LM_FUNC(IGUIEnvironment, (getBuiltInFont)
    )

    LM_CLASS(ns_irr_gui, IGUIFont)
    LM_BASE(IGUIFont, IReferenceCounted)
    LM_FUNC(IGUIFont, (draw)(getDimension)(getKerningHeight)(getKerningWidth)(setKerningHeight)(setKerningWidth)
    )

    LM_CLASS(ns_irr_gui, ICursorControl)
    LM_BASE(ICursorControl, IReferenceCounted)
    LM_FUNC(ICursorControl, (getPosition)(getRelativePosition)(isVisible)(setReferenceRect)(setVisible))
    LM_FUNC_OVERLOAD(ICursorControl, "setPosition", setPosition, void, s32, s32)
    LM_FUNC_OVERLOAD(ICursorControl, "setRelativePosition", setPosition, void, f32, f32)
    LM_FUNC_OVERLOAD(ICursorControl, "setPosition", setPosition, void, irr::core::vector2di const&)
    LM_FUNC_OVERLOAD(ICursorControl, "setRelativePosition", setPosition, void, irr::core::vector2df const&)

    LM_ENUM(ns_irr_gui, EKEY_CODE)
    LM_ENUM(ns_irr_gui, EEVENT_TYPE)
    LM_ENUM(ns_irr_gui, EMOUSE_INPUT_EVENT)
    LM_ENUM(ns_irr_gui, E_MOUSE_BUTTON_STATE_MASK)
    LM_ENUM(ns_irr_gui, EGUI_EVENT_TYPE)

    LM_CLASS(ns_irr_gui, SEvent)
    LM_FIELD(SEvent, (EventType)(GUIEvent)(MouseInput)(KeyInput)(JoystickEvent)(LogEvent)(UserEvent))

    typedef SEvent::SGUIEvent SGUIEvent;
    LM_CLASS(ns_irr_gui, SGUIEvent)

    typedef SEvent::SMouseInput SMouseInput;
    LM_CLASS(ns_irr_gui, SMouseInput)
    LM_FIELD(SMouseInput, (Event)(X)(Y)(Wheel)(ButtonStates))
    LM_BIT_FIELD(SMouseInput, Shift)
    LM_BIT_FIELD(SMouseInput, Control)
    LM_FUNC(SMouseInput, (isLeftPressed)(isRightPressed)(isMiddlePressed))

    typedef SEvent::SKeyInput SKeyInput;
    LM_CLASS(ns_irr_gui, SKeyInput)
    LM_FIELD(SKeyInput, (Char)(Key))
    LM_BIT_FIELD(SKeyInput, PressedDown)
    LM_BIT_FIELD(SKeyInput, Shift)
    LM_BIT_FIELD(SKeyInput, Control)

    typedef SEvent::SJoystickEvent SJoystickEvent;
    LM_CLASS(ns_irr_gui, SJoystickEvent)

    typedef SEvent::SLogEvent SLogEvent;
    LM_CLASS(ns_irr_gui, SLogEvent)

    typedef SEvent::SUserEvent SUserEvent;
    LM_CLASS(ns_irr_gui, SUserEvent)
}


}}
