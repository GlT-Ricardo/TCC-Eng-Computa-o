#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "KinectProjector/KinectProjector.h"
#include "SandSurfaceRenderer/SandSurfaceRenderer.h"
#include "Games/MapGameController.h"
#include "Games/BoidGameController.h"
#include "Games/SurvivalGameController.h"    // Adicionados
#include "Games/FeedingGameController.h"     // Adicionados

class ofApp : public ofBaseApp {

public:
	void setup();

	void update();

	void draw();
	void drawProjWindow(ofEventArgs& args);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	std::shared_ptr<ofAppBaseWindow> projWindow;

private:
	std::shared_ptr<KinectProjector> kinectProjector;
	SandSurfaceRenderer* sandSurfaceRenderer;
	CMapGameController mapGameController;
	CBoidGameController boidGameController;
	CSurvivalGameController survivalGameController;  // Adicionados
	CFeedingGameController feedingGameController;    // Adicionados

													 
	ofRectangle mainWindowROI;			// Main window ROI 	
};
