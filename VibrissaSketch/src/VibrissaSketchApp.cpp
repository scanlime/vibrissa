#include "cinder/app/AppNative.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class VibrissaSketchApp : public AppNative {
  public:
	void setup();
	void mouseDown( MouseEvent event );	
	void update();
	void draw();
};

void VibrissaSketchApp::setup()
{
}

void VibrissaSketchApp::mouseDown( MouseEvent event )
{
}

void VibrissaSketchApp::update()
{
}

void VibrissaSketchApp::draw()
{
	// clear out the window with black
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP_NATIVE( VibrissaSketchApp, RendererGl )
