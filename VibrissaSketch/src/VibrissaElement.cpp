// Experimental art sketch (c) 2015 Micah Elizabeth Scott
// MIT license

#include "VibrissaElement.h"

using namespace ci;
using namespace std;


void VibrissaElement::setup(ci::app::App &app)
{
    mTimer.start();

    mCylinderShader = gl::GlslProg(app.loadResource("cylinder.glslv"),
                                   app.loadResource("cylinder.glslf"));

    mProjectionShader = gl::GlslProg(app.loadResource("projection.glslv"),
                                     app.loadResource("projection.glslf"));

    mColors = loadImage(app.loadResource("colors.png"));
    mShapes = loadImage(app.loadResource("shapes.png"));
    mModulator = loadImage(app.loadResource("modulator.png"));
}

void VibrissaElement::update()
{
}

void VibrissaElement::draw(ci::Vec3f origin, float t)
{
    const float kMetersPerInch = 0.0254f;
    const float kMetersPerFoot = kMetersPerInch * 12.f;

    float r = 8.5f / 2.0f * kMetersPerInch;
    float led_h = 6.f * kMetersPerFoot;
    float base_h = 12.f * kMetersPerFoot;
    float cap_h = 1.5f * kMetersPerInch;
    const int segments = 16;

    gl::pushModelView();
    gl::translate(origin);

    // Utility pole base
    float gray = 0.05f;
    gl::color(gray, gray, gray);
    gl::drawCylinder(r, r, base_h, segments);

    // Endcap
    gl::pushModelView();
    gl::translate(0, base_h + led_h, 0);
    gl::drawCylinder(r, r, cap_h, segments);
    gl::translate(0, cap_h, 0);
    gl::rotate(Vec3f(90, 0, 0));
    gl::drawSolidCircle(Vec2f(0, 0), r, segments);
    gl::popModelView();
    
    // LED diffuser cylinder
    gl::pushModelView();
    gl::translate(0, base_h, 0);

    mCylinderShader.bind();
    mCylinderShader.uniform("timer", (float) mTimer.getSeconds());
    mCylinderShader.uniform("t", t);
    mCylinderShader.uniform("origin", origin);
    
    mCylinderShader.uniform("colors", 0);
    mColors.bind(0);

    mCylinderShader.uniform("shapes", 1);
    mShapes.bind(1);

    gl::drawCylinder(r, r, led_h, segments);
    
    gl::disable(GL_TEXTURE_2D);
    mCylinderShader.unbind();

    gl::popModelView();
    gl::popModelView();
}

void VibrissaElement::drawProjection()
{
    mProjectionShader.bind();
    mProjectionShader.uniform("timer", (float) mTimer.getSeconds());
    
    mProjectionShader.uniform("colors", 0);
    mColors.bind(0);
    
    mProjectionShader.uniform("shapes", 1);
    mShapes.bind(1);
    
    mProjectionShader.uniform("modulator", 2);
    mModulator.bind(2);

    gl::pushModelView();
    gl::rotate(Vec3f(90.f, 0.f, 0.f));
    gl::scale(22.f, 12.f, 1.f);
    gl::drawSolidRect(Rectf(-1, -1, 1, 1));
    gl::popModelView();
    
    gl::disable(GL_TEXTURE_2D);
    mProjectionShader.unbind();
}
