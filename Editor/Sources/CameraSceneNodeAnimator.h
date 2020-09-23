#pragma once

#include <irrlicht.h>

#include "ISceneNodeAnimator.h"

/*
* Custom camera animator, allowing for both spinning camera like FPS animator and moving camera around both
* as Maya animator (translation in the camera view plane) and forward-backward (as opposed to Maya animator, which
* limits this to zooming up to a certain limit).
* 
* Controls:
* 
* - Middle mouse + mouse move - translate in camera view plane
* - Right mouse + mouse move - rotate camera FPS-style
* - Mouse wheel - move camera forward / backward
*/
class CameraSceneNodeAnimator : public irr::scene::ISceneNodeAnimator {
public:
    CameraSceneNodeAnimator(
        irr::gui::ICursorControl* cursorControl,
        irr::f32 rotateSpeed = 1000.f,
        irr::f32 moveSpeed = 1500.f
    );

    virtual ~CameraSceneNodeAnimator();

    virtual irr::scene::ISceneNodeAnimator* createClone(irr::scene::ISceneNode* node, irr::scene::ISceneManager* newManager = 0);

    virtual bool isEventReceiverEnabled() const;

    virtual void animateNode(irr::scene::ISceneNode* node, irr::u32 timestamp);

    virtual bool OnEvent(const irr::SEvent& event);

    virtual irr::f32 getMoveSpeed() const;

    virtual void setMoveSpeed(irr::f32 moveSpeed);

    virtual irr::f32 getRotateSpeed() const;

    virtual void setRotateSpeed(irr::f32 rotateSpeed);

protected:
    virtual bool isCapturingMouseInput() const;

    virtual bool shouldAnimate(irr::scene::ISceneNode* sceneNode) const;

    irr::gui::ICursorControl* cursorControl;

    irr::f32 moveSpeed;
    irr::f32 rotateSpeed;

    irr::u32 previousTimestamp;

    irr::core::vector2di currentMousePosition;
    irr::core::vector2di previousMousePosition;

    irr::f32 mouseWheelMotion;

    bool isLeftMouseButtonPressed;
    bool isMiddleMouseButtonPressed;
    bool isRightMouseButtonPressed;
};