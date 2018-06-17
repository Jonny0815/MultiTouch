#include <iostream>
#include <process.h>

#include "TuioClient.h"
#include "TuioListener.h"

#include <SFML/Graphics.hpp>
#include <GL/glut.h> // GLUT 3.0.0.1

// NUGet SFML:	Install-Package sfml-window -Version 2.4.2 


using namespace TUIO;
using namespace std;
using namespace sf;

TUIO::TuioClient *tuioClient; // global tuioClient for testing

class Client : public TuioListener {
	// these methods need to be implemented here since they're virtual methods
	// these methods will be called whenever a new package is received

	void Client::addTuioObject(TuioObject *tobj){};
	void Client::updateTuioObject(TuioObject *tobj){};
	void Client::removeTuioObject(TuioObject *tobj){};
	
	void Client::addTuioCursor(TuioCursor *tcur)
	{
		//std::cout << "new finger detected: (id=" << tcur->getSessionID() << ", coordinates=" << tcur->getX() << "," << tcur->getY() << ")\n";
	};
	void Client::updateTuioCursor(TuioCursor *tcur){};
	void Client::removeTuioCursor(TuioCursor *tcur){};

	void  Client::refresh(TuioTime frameTime){};
};


void draw()
{
	// openGL draw function
	std::list<TuioCursor*> cursorList;

	// get the actual cursor list from the tuioClient
	tuioClient->lockCursorList();
	cursorList = tuioClient->getTuioCursors();
	tuioClient->unlockCursorList();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Bild Buffer l�chen

	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter)
	{
		std::cout << "id : "<< (*cursorListIter)->getCursorID() << endl;
		std::cout << "X : " << (*cursorListIter)->getX() << endl;
		std::cout << "Y : " << (*cursorListIter)->getY() << endl;

		// do things with the cursors	

		glBegin(GL_POINTS);
		glColor3f(1, 0, 0);
		glVertex2f((*cursorListIter)->getX(), (*cursorListIter)->getY());
		glEnd();


	}

	glutSwapBuffers();
	std::cout << "End Printing " << std::endl;
}

void tuioThread(void*)
{
	Client *app = new Client();
	tuioClient = new TUIO::TuioClient();
	tuioClient->addTuioListener(app);
	tuioClient->connect(true);
}

void idle(void)
{
	// this might be needed on some systems, otherwise the draw() function will only be called once
	glutPostRedisplay();
}

void glInit()
{	
}


int main(int argc, char** argv)
{

	
	// create a second thread for the TUIO listener
	HANDLE hThread_TUIO;
	unsigned threadID;
	//hThread = (HANDLE)_beginthreadex( NULL, 0, &tuioThread, NULL, 0, &threadID );
	hThread_TUIO = (HANDLE)_beginthread( tuioThread, 0, NULL );

	// GLUT Window Initialization (just an example):
	glutInit(&argc, argv);
	glutInitWindowSize(752, 480);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("MultiTouchGame Johannes, Sven ");

	// openGL init
	glInit();

	// Register callbacks:
	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutMainLoop();
	
	
	


	return 0;
}