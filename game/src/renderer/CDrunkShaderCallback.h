#pragma once

#include <irrlicht.h>

class CDrunkShaderCallback : public irr::video::IShaderConstantSetCallBack
{
public:
    CDrunkShaderCallback();

    ~CDrunkShaderCallback();

    virtual void OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData) override;

    void setTime(irr::f32 _time);

    void setWorldViewProjectionMatrix(irr::core::matrix4 worldViewProj);

private:
    irr::f32 time;
    irr::core::matrix4 worldViewProj;
};
