/***********************************************************************
ofApp.cpp - main openframeworks app
Copyright (c) 2016-2017 Thomas Wolf and Rasmus R. Paulsen (people.compute.dtu.dk/rapa)

This file is part of the Magic Sand.

The Magic Sand is free software; you can redistribute it
and/or modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of the
License, or (at your option) any later version.

The Magic Sand is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with the Augmented Reality Sandbox; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
***********************************************************************/

#include "ofApp.h"

void ofApp::setup() {
	// OF basics
	ofSetFrameRate(60);
	ofBackground(0);
	ofSetVerticalSync(true);

	// Setup kinectProjector
	kinectProjector = std::make_shared<KinectProjector>(projWindow);
	kinectProjector->setup(true);

	// Setup sandSurfaceRenderer
	sandSurfaceRenderer = new SandSurfaceRenderer(kinectProjector, projWindow);
	sandSurfaceRenderer->setup(true);

	// Retrieve variables
	ofVec2f kinectRes = kinectProjector->getKinectRes();
	ofVec2f projRes = ofVec2f(projWindow->getWidth(), projWindow->getHeight());
	ofRectangle kinectROI = kinectProjector->getKinectROI();
	mainWindowROI = ofRectangle((ofGetWindowWidth() - kinectRes.x) / 2, (ofGetWindowHeight() - kinectRes.y) / 2, kinectRes.x, kinectRes.y);

	// Setup dos controllers existentes
	mapGameController.setup(kinectProjector);
	mapGameController.setProjectorRes(projRes);
	mapGameController.setKinectRes(kinectRes);
	mapGameController.setKinectROI(kinectROI);

	boidGameController.setup(kinectProjector);
	boidGameController.setProjectorRes(projRes);
	boidGameController.setKinectRes(kinectRes);
	boidGameController.setKinectROI(kinectROI);

	// Setup dos novos controllers
	survivalGameController.setup(kinectProjector);
	survivalGameController.setProjectorRes(projRes);
	survivalGameController.setKinectRes(kinectRes);
	survivalGameController.setKinectROI(kinectROI);

	feedingGameController.setup(kinectProjector);
	feedingGameController.setProjectorRes(projRes);
	feedingGameController.setKinectRes(kinectRes);
	feedingGameController.setKinectROI(kinectROI);

}


void ofApp::update() {
	kinectProjector->update();
	sandSurfaceRenderer->update();

	if (kinectProjector->getKinectROI() != mapGameController.getKinectROI())
	{
		ofRectangle kinectROI = kinectProjector->getKinectROI();
		mapGameController.setKinectROI(kinectROI);
		boidGameController.setKinectROI(kinectROI);
		survivalGameController.setKinectROI(kinectROI);
		feedingGameController.setKinectROI(kinectROI);
	}

	// Atualizar todos os controllers
	mapGameController.update();
	boidGameController.update();
	survivalGameController.update();
	feedingGameController.update();
}


void ofApp::draw() 
{
	float x = mainWindowROI.x;
	float y = mainWindowROI.y;
	float w = mainWindowROI.width;
	float h = mainWindowROI.height;

	if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
	{
		sandSurfaceRenderer->drawMainWindow(x, y, w, h);
		boidGameController.drawMainWindow(x, y, w, h);
		survivalGameController.drawMainWindow(x, y, w, h);
		feedingGameController.drawMainWindow(x, y, w, h);
	}

	kinectProjector->drawMainWindow(x, y, w, h);
}

void ofApp::drawProjWindow(ofEventArgs &args) 
{
	if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
	{
		sandSurfaceRenderer->drawProjectorWindow();
		mapGameController.drawProjectorWindow();
		boidGameController.drawProjectorWindow();
		survivalGameController.drawProjectorWindow();
		feedingGameController.drawProjectorWindow();
	}
	kinectProjector->drawProjectorWindow();
}

void ofApp::keyPressed(int key)
{
	if (key == OF_KEY_ESC) // TECLA ESC - volta ao menu
	{
		if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
		{
			// Verifica se está na introdução e volta ao menu
			if (survivalGameController.isInIntro()) {
				survivalGameController.goBackToIdle();
			}
			else if (feedingGameController.isInIntro()) {
				feedingGameController.goBackToIdle();
			}
		}
	}
	else if (key == ' ')
	{
		if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING)
		{
			// PRIMEIRO: Verifica se está na introdução e inicia o jogo
			if (survivalGameController.isInIntro()) {
				survivalGameController.startFromIntro();
			}
			else if (feedingGameController.isInIntro()) {
				feedingGameController.startFromIntro();
			}
			// DEPOIS: Verifica o jogo antigo (boid game)
			else if (boidGameController.isIdle())
			{
				if (mapGameController.isIdle())
				{
					mapGameController.setDebug(kinectProjector->getDumpDebugFiles());
					mapGameController.StartGame();
				}
				else
				{
					mapGameController.ButtonPressed();
				}
			}
		}
		else if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_SETUP)
		{
			// Try to start the application
			kinectProjector->startApplication();
		}
	}
	else if (key == '1') // Survival Game
	{
		if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING &&
			survivalGameController.isIdle())
		{
			survivalGameController.StartGame(); // Vai para INTRO
		}
	}
	else if (key == '2') // Feeding Game
	{
		if (kinectProjector->GetApplicationState() == KinectProjector::APPLICATION_STATE_RUNNING &&
			feedingGameController.isIdle())
		{
			feedingGameController.StartGame(); // Vai para INTRO
		}
	}
}

void ofApp::keyReleased(int key) {

}

void ofApp::mouseMoved(int x, int y) {

}

void ofApp::mouseDragged(int x, int y, int button) {

	// We assume that we only use this during ROI annotation
	kinectProjector->mouseDragged(x - mainWindowROI.x, y - mainWindowROI.y, button);
}

void ofApp::mousePressed(int x, int y, int button) 
{
	if (mainWindowROI.inside((float)x, (float)y))
	{
		kinectProjector->mousePressed(x-mainWindowROI.x, y-mainWindowROI.y, button);
	}
}

void ofApp::mouseReleased(int x, int y, int button) {
	// We assume that we only use this during ROI annotation
	kinectProjector->mouseReleased(x - mainWindowROI.x, y - mainWindowROI.y, button);

}

void ofApp::mouseEntered(int x, int y) {

}

void ofApp::mouseExited(int x, int y) {

}

void ofApp::windowResized(int w, int h) {

}

void ofApp::gotMessage(ofMessage msg) {

}

void ofApp::dragEvent(ofDragInfo dragInfo) {

}

