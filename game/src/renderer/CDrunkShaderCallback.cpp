#include "CDrunkShaderCallback.h"

CDrunkShaderCallback::CDrunkShaderCallback()
    : time(0.0f), worldViewProj()
{
}

CDrunkShaderCallback::~CDrunkShaderCallback()
{
}

void CDrunkShaderCallback::OnSetConstants(irr::video::IMaterialRendererServices* services, irr::s32 userData)
{
    services->setPixelShaderConstant("time", &time, 1);

    if (services->getVideoDriver()->getDriverType() == irr::video::EDT_DIRECT3D9)
    {
        services->setVertexShaderConstant("mWorldViewProj", worldViewProj.pointer(), 16);
    }
}

void CDrunkShaderCallback::setTime(irr::f32 _time)
{
    time = _time;
}

void CDrunkShaderCallback::setWorldViewProjectionMatrix(irr::core::matrix4 _worldViewProj)
{
    worldViewProj = _worldViewProj;
}
