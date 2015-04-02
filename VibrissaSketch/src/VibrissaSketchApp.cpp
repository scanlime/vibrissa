#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"
#include "cinder/gl/Fbo.h"

#include "VibrissaElement.h"

using namespace ci;
using namespace ci::app;
using namespace std;


class VibrissaSketchApp : public AppNative {
  public:
	void setup();
	void update();
	void draw();
    void prepareSettings(Settings *settings);

    void mouseDown(MouseEvent event);
    void mouseDrag(MouseEvent event);
    void resize();
    void keyDown(KeyEvent event);
    
protected:
    MayaCamUI mMayaCam;
    VibrissaElement mElement;
    Perlin mPerlin;

    gl::Fbo mFboScene;
    gl::Fbo mFboBlur1;
    gl::Fbo mFboBlur2;
    gl::GlslProg mBlurShader;
    gl::GlslProg mPostShader;
    
    void drawScene();
    void drawElements();
    void drawGrid(float size=100.0f, float step=5.0f);
    void drawVibrissa(Vec3f origin);
};


void VibrissaSketchApp::keyDown(KeyEvent event)
{
    if (event.getCode() == KeyEvent::KEY_f) {
        bool fs = !isFullScreen();
        setFullScreen(fs);
        if (fs) {
            hideCursor();
        } else {
            showCursor();
        }
    }
}


void VibrissaSketchApp::setup()
{
    gl::Fbo::Format fmt;
    fmt.setColorInternalFormat(GL_RGB32F_ARB);
    
    mFboScene = gl::Fbo(getWindowWidth() * 2, getWindowHeight() * 2, fmt);
    mFboBlur1 = gl::Fbo(getWindowWidth() / 2, getWindowHeight() / 2, fmt);
    mFboBlur2 = gl::Fbo(getWindowWidth() / 2, getWindowHeight() / 2, fmt);

    mBlurShader = gl::GlslProg(loadResource("blur.glslv"), loadResource("blur.glslf"));
    mPostShader = gl::GlslProg(loadResource("post.glslv"), loadResource("post.glslf"));
    
    mElement.setup(*this);
    
    CameraPersp cam;
    cam.setEyePoint( Vec3f(0.f, 30.0f, -60.0f) );
    cam.setCenterOfInterestPoint( Vec3f(0.f, 1.f, 5.f) );
    cam.setPerspective( 60.0f, getWindowAspectRatio(), 1.0f, 1000.0f );
    mMayaCam.setCurrentCam( cam );
}

void VibrissaSketchApp::prepareSettings( Settings *settings )
{
    settings->setWindowSize( 1920, 1080 );
    settings->setFrameRate( 60.0f );
}

void VibrissaSketchApp::mouseDown( MouseEvent event )
{
    mMayaCam.mouseDown( event.getPos() );
}

void VibrissaSketchApp::mouseDrag( MouseEvent event )
{
    mMayaCam.mouseDrag(event.getPos(),
                       event.isLeftDown() && !event.isShiftDown(),
                       event.isMiddleDown() || event.isShiftDown(),
                       event.isRightDown() );
}

void VibrissaSketchApp::resize()
{
    CameraPersp cam = mMayaCam.getCamera();
    cam.setAspectRatio( getWindowAspectRatio() );
    mMayaCam.setCurrentCam( cam );
}

void VibrissaSketchApp::update()
{
    mElement.update();
}

void VibrissaSketchApp::drawGrid(float size, float step)
{
    float gray = 0.12f;
    gl::color( Colorf(gray, gray, gray) );

    for (float i=-size;i<=size;i+=step) {
        gl::drawLine( Vec3f(i, 0.0f, -size), Vec3f(i, 0.0f, size) );
        gl::drawLine( Vec3f(-size, 0.0f, i), Vec3f(size, 0.0f, i) );
    }
}

void VibrissaSketchApp::draw()
{
    mFboScene.bindFramebuffer();
    gl::setViewport(mFboScene.getBounds());
    gl::enableDepthRead();
    gl::enableDepthWrite();

    drawScene();

    mFboScene.unbindFramebuffer();
    gl::disableDepthRead();
    gl::disableDepthWrite();
    gl::color(1.f, 1.f, 1.f);
    
    const float atten = 6.0f;
    const unsigned blur_steps = 3;

    for (unsigned step = 0; step < blur_steps; step++) {
        
        // Horizontal blur
        mFboBlur1.bindFramebuffer();
        gl::setViewport(mFboBlur1.getBounds());
        gl::setMatricesWindow(mFboBlur1.getSize());
        mBlurShader.bind();
        mBlurShader.uniform("tex0", 0);
        mBlurShader.uniform("sample_offset", Vec2f(1.f / mFboBlur1.getWidth(), 0.f));
        mBlurShader.uniform("attenuation", step ? 1.f : atten);
        gl::draw(step ? mFboBlur2.getTexture() : mFboScene.getTexture(),
                 Rectf(Vec2f(0,0), mFboBlur1.getSize()));
        mBlurShader.unbind();
        mFboBlur1.unbindFramebuffer();

        // Vertical blur
        mFboBlur2.bindFramebuffer();
        gl::setViewport(mFboBlur2.getBounds());
        gl::setMatricesWindow(mFboBlur2.getSize());
        mBlurShader.bind();
        mBlurShader.uniform("tex0", 0);
        mBlurShader.uniform("sample_offset", Vec2f(0.f, 1.f / mFboBlur2.getHeight()));
        mBlurShader.uniform("attenuation", 1.f);
        gl::draw(mFboBlur1.getTexture(), Rectf(Vec2f(0,0), mFboBlur2.getSize()));
        mBlurShader.unbind();
        mFboBlur2.unbindFramebuffer();
    }
        
    // Final scene, combine blurred and unblurred
    gl::setViewport(getWindowBounds());
    gl::setMatricesWindow(getWindowSize(), false);
    mPostShader.bind();
    mPostShader.uniform("scene", 0);
    mPostShader.uniform("blurred", 1);
    mFboBlur2.getTexture().bind(1);
    gl::draw(mFboScene.getTexture(), Rectf(Vec2f(0,0), getWindowSize()));
    mPostShader.unbind();
}

void VibrissaSketchApp::drawScene()
{
    float gray = 0.02f;
    gl::clear(Colorf(gray, gray, gray));

    gl::setMatrices(mMayaCam.getCamera());
    drawGrid();
    drawElements();
    mElement.drawProjection();
}
    
void VibrissaSketchApp::drawElements()
{
    const float kMetersPerInch = 0.0254f;
    const float kMetersPerFoot = kMetersPerInch * 12;
    
    float loop_center = (225 + 150) * kMetersPerFoot / 2;
    float loop_r = 80 * kMetersPerFoot / 2;
    float path_r = 6 * kMetersPerFoot;
    
    // Loops along path
    for (int side = -1; side <= 1; side += 2) {
        
        for (float t = 0.f; t < 1.f; t += 0.08f) {
            float a = t * M_PI * 2.f;
            float r = loop_r * (0.5f + 6.f * mPerlin.noise(sin(a) * 0.4f));

            for (int pair = 0; pair < 2; pair++) {
                mElement.draw(Vec3f(cos(a) * r * 2.f + loop_center * side, 0.f,
                                    sin(a) * r + loop_r * 1.5f), t);
                r += path_r;
            }
        }
    }
}

CINDER_APP_NATIVE( VibrissaSketchApp, RendererGl )
