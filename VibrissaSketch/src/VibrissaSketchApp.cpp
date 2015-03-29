#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"
#include "cinder/MayaCamUI.h"
#include "cinder/Perlin.h"

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
    
protected:
    MayaCamUI mMayaCam;
    VibrissaElement mElement;
    Perlin mPerlin;
    
    void drawGrid(float size=100.0f, float step=5.0f);
    void drawVibrissa(Vec3f origin);
};


void VibrissaSketchApp::setup()
{
    mElement.setup(*this);
    
    CameraPersp cam;
    cam.setEyePoint( Vec3f(5.0f, 1.5f, -5.0f) );
    cam.setCenterOfInterestPoint( Vec3f(20.f, 5.f, 4.f) );
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
    gl::color( Colorf(0.17f, 0.17f, 0.17f) );
    for(float i=-size;i<=size;i+=step) {
        gl::drawLine( Vec3f(i, 0.0f, -size), Vec3f(i, 0.0f, size) );
        gl::drawLine( Vec3f(-size, 0.0f, i), Vec3f(size, 0.0f, i) );
    }
}

void VibrissaSketchApp::draw()
{
    gl::clear( Colorf(0.1f, 0.1f, 0.1f) );
    gl::setMatrices( mMayaCam.getCamera() );
    
    gl::enableDepthRead();
    gl::enableDepthWrite();
    
    drawGrid();
    
    const float kMetersPerInch = 0.0254f;
    const float kMetersPerFoot = kMetersPerInch * 12;

    float extent = 540 * kMetersPerFoot / 2;
    float spacing = 20 * kMetersPerFoot;
    
    // Line of elements along the front
    for (float x = -extent; x < extent; x += spacing) {
        mElement.draw(Vec3f(x, 0.f, 0.f));
    }
    
    float loop_center = (225 + 150) * kMetersPerFoot / 2;
    float loop_r = 80 * kMetersPerFoot / 2;
    float path_r = 6 * kMetersPerFoot;
    
    // Loops along path
    for (int side = -1; side <= 1; side += 2) {
        
        for (float t = 0; t < M_PI * 2; t += M_PI / 5) {
            float r = loop_r * (0.5f + 6.f * mPerlin.noise(sin(t) * 0.4f));

            for (int pair = 0; pair < 2; pair++) {
                mElement.draw(Vec3f(cos(t) * r * 2.f + loop_center * side, 0.f,
                                    sin(t) * r + loop_r * 1.5f));
                r += path_r;
            }
        }
    }
}


CINDER_APP_NATIVE( VibrissaSketchApp, RendererGl )
