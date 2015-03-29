// Experimental art sketch (c) 2015 Micah Elizabeth Scott
// MIT license

#pragma once

#include "cinder/app/App.h"
#include "cinder/Vector.h"
#include "cinder/gl/GlslProg.h"
#include "cinder/gl/Texture.h"


class VibrissaElement
{
public:
    void setup(ci::app::App &app);
    void update();

    void draw(ci::Vec3f origin, float t);
    
private:
    ci::gl::GlslProg mCylinderShader;
    ci::gl::Texture mColors;
    ci::gl::Texture mShapes;
    ci::Timer mTimer;
};
