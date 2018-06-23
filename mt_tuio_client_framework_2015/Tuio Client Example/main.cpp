#include <iostream>
#include <process.h>
#include <vector>
#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <Windows.h>

#include "TuioClient.h"
#include "TuioListener.h"

#include <SFML/Graphics.hpp>
#include <GL/glut.h> // GLUT 3.0.0.1
#include "..\geasture\include\GeometricRecognizer.h"
#include "..\geasture\include\PathWriter.h"

using namespace TUIO;
using namespace std;
using namespace sf;
using namespace DollarRecognizer;



struct Pfade
{
	Path2D gesten[100];
	bool gesetzt[100];

};

struct Objekt {
	
	string typ;
	double x;
	double y;

};

utils util;
int o;
int hoehe = 600;
int breite = 800;

Pfade xGesten;
vector<Objekt> objekte;

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


constexpr
unsigned int xhash(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (xhash(str, h + 1) * 33) ^ str[h];	
}

void drawStar() {
	glBegin(GL_POLYGON);
	glColor3f(1, 1, 0);
	glVertex2f(0.0, 0.2);
	glVertex2f(0.1, 0.1);
	glVertex2f(0.2, 0.05);
	glVertex2f(0.1, 0.0);
	glVertex2f(0.2, -0.1);
	glVertex2f(0.0, 0.0);
	glVertex2f(-0.2, -0.1);
	glVertex2f(-0.1, 0.0);
	glVertex2f(-0.2, 0.05);
	glVertex2f(-0.1, 0.1);
	glEnd();
	glFlush();
}


void drawCircle(float posx, float posy) {

	glBegin(GL_POLYGON);
	for (int ii = 0; ii < 200; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(200);//get the current angle

		float x = 0.2 * cosf(theta);//calculate the x component
		float y = 0.2 * sinf(theta);//calculate the y component

		glVertex2f(x + posx,y + posy);//output vertex

	}
	glEnd();
	glFlush();
}



void drawRectangel() {
	glRectd(0.2, 0.2, -0.2, -0.2);
	glFlush();
}

void drawTriangel(float posX, float posY) {
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0); // red
	glVertex2f(-0.2 + posX, -0.2 + posY);
	glColor3f(0, 1, 0); // green
	glVertex2f(0.2+ posX, -0.2 + posY);
	glColor3f(0, 0, 1); // blue
	glVertex2f(0 + posX, 0.3 + posY);
	glEnd();
	glFlush();
}

void draw()
{
	// openGL draw function
	std::list<TuioCursor*> cursorList;
	std::list<TuioCursor*> drawings;
	// get the actual cursor list from the tuioClient
	tuioClient->lockCursorList();
	cursorList = tuioClient->getTuioCursors();
	tuioClient->unlockCursorList();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Bild Buffer löchen
	
	for (int i = 0; i < 100; i++) { // alle Finger auf nicht gesetzt setzen
		xGesten.gesetzt[i] = false;
	}
	

	for (std::list<TuioCursor*>::iterator cursorListIter = cursorList.begin(); cursorListIter != cursorList.end(); ++cursorListIter)
	{

		int id = (int)(*cursorListIter)->getCursorID();
		double x= (double)(*cursorListIter)->getX();
		double y = (double)(*cursorListIter)->getY();
		list<TuioPoint> points = (*cursorListIter)->getPath();
		bool first = true;

		if (id >= 0) {
			// Frames speichern

			Point2D temp(x, y);
			xGesten.gesten[id].push_back(temp); // Punkt speichern
			xGesten.gesetzt[id] = true;			// geste noch nicht fertig = true 
			
			cout << "ID: " << id << "X: " << x << "Y: " << y << endl;

		}

		

		glBegin(GL_LINE_STRIP);
		glColor3f(1, 0, 0);



		for (std::list<TuioPoint>::iterator it = points.begin(); it != points.end(); ++it)
		{
			
				glVertex2f(((*it).getX() - 0.5) * 2, (((*it).getY()) - 0.5) * (-2));
			
			
		}

		
		glEnd();


	}
	// alte Gesten löschen

	for (int j = 0; j < 100; j++) {
		if (!xGesten.gesetzt[j] && !xGesten.gesten[j].empty()) {
			// Geste erkennen

			GeometricRecognizer g;
			g.loadTemplates();


/*
mögliche Gesten: 
Arrow
Caret
CheckMark
Circle
Delete,
Diamond
LeftCurlyBrace
LeftSquareBracket
LeftToRightLine
LineDownDiagonal
Pigtail
QuestionMark
Rectangle
RightCurlyBrace
RightSquareBracket
RightToLeftLine
RightToLeftLine2
RightToLeftSlashDown
Spiral
Star
Triangle
V
X
*/
			vector<string> gesturesList;
			gesturesList.push_back("Circle"); // Verfügbare Gesten
			gesturesList.push_back("V");
			gesturesList.push_back("Triangle");
			gesturesList.push_back("CheckMark");
			gesturesList.push_back("Star");
			gesturesList.push_back("Rectangle");
			gesturesList.push_back("0");		// muss immer am Ende sein
			g.activateTemplates(gesturesList);

			RecognitionResult r = g.recognize(xGesten.gesten[j]);
			cout << "Recognized gesture: " << r.name << endl;
			cout << "1$ Score:" << r.score << endl;
			//Sleep(4000);

			

			if (r.score > 0.4) {
				Objekt oTemp;
				switch (xhash(r.name.c_str()))
				{
				case xhash("Star"):
					
					oTemp.typ = "Star";
					oTemp.x = 0;
					oTemp.y = 0;

					objekte.push_back(oTemp);
					
					break;
				case xhash("Circle"):

					oTemp.typ = "Circle";
					oTemp.x = 0;
					oTemp.y = 0;

					objekte.push_back(oTemp);

					break;

				case xhash("Rectangle"):

					oTemp.typ = "Rectangle";
					oTemp.x = 0;
					oTemp.y = 0;

					objekte.push_back(oTemp);

					break;
				case xhash("Triangle"):

					oTemp.typ = "Triangle";
					oTemp.x = 0;
					oTemp.y = 0;

					objekte.push_back(oTemp);

					break;
				default:
					cout << "Fehler Objekt nicht erkannt" << endl;
					break;
				}
			
			}
				
			


			cout << "alten Fingerverlauf loeschen" << endl;

			// Geste Ausgeben/Zeichnen
			xGesten.gesten[j].clear(); // lösche Geste
			
		}
		
	}

	// Gespeicherte Objekte zeichnen
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Bild Buffer löchen

	for (int i = 0; i < objekte.size(); i++)
	{
		string typ = objekte.at(i).typ;

		switch (xhash(typ.c_str()))
		{
			case xhash("Star") :
				drawStar();
				break;
			case xhash("Rectangle"):
				drawRectangel();
				break;
			case xhash("Triangle") :
				drawTriangel(objekte.at(i).x, objekte.at(i).y);
				break;
			case xhash("Circle"):
				drawCircle(0,0);
				break;
		default:
			break;
		}

	}

	glutSwapBuffers();
	std::cout << "End Printing Frame " << std::endl;
	//Sleep(50);
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

MultiStrokeGesture getGestureP()
{
	MultiStrokeGesture MultiStrokes;
	MultiStrokes.push_back(util.addPointsToMakePath(Point2D(507, 8), Point2D(507, 87)));
	MultiStrokes.push_back(util.addPointsToMakePath(Point2D(510, 7), Point2D(528, 7), Point2D(530, 8), Point2D(544, 10), Point2D(550, 12), Point2D(550, 15), Point2D(558, 20), Point2D(560, 22), Point2D(561, 27), Point2D(562, 33), Point2D(561, 37), Point2D(559, 42), Point2D(556, 45), Point2D(550, 48), Point2D(544, 51), Point2D(538, 53), Point2D(532, 54), Point2D(525, 55), Point2D(519, 55), Point2D(513, 55), Point2D(510, 55)));
	return MultiStrokes;
}

Path2D sampleGest()
{
	Path2D path;
	path.push_back(Point2D(68, 222));
	path.push_back(Point2D(70, 220));
	path.push_back(Point2D(73, 218));
	path.push_back(Point2D(75, 217));
	path.push_back(Point2D(77, 215));
	path.push_back(Point2D(80, 213));
	path.push_back(Point2D(82, 212));
	path.push_back(Point2D(84, 210));
	path.push_back(Point2D(87, 209));
	path.push_back(Point2D(89, 208));
	path.push_back(Point2D(92, 206));
	path.push_back(Point2D(95, 204));
	path.push_back(Point2D(101, 201));
	path.push_back(Point2D(106, 198));
	path.push_back(Point2D(112, 194));
	path.push_back(Point2D(118, 191));
	path.push_back(Point2D(124, 187));
	path.push_back(Point2D(127, 186));
	path.push_back(Point2D(132, 183));
	path.push_back(Point2D(138, 181));
	path.push_back(Point2D(141, 180));
	path.push_back(Point2D(146, 178));
	path.push_back(Point2D(154, 173));
	path.push_back(Point2D(159, 171));
	path.push_back(Point2D(161, 170));
	path.push_back(Point2D(166, 167));
	path.push_back(Point2D(168, 167));
	path.push_back(Point2D(171, 166));
	path.push_back(Point2D(174, 164));
	path.push_back(Point2D(177, 162));
	path.push_back(Point2D(180, 160));
	path.push_back(Point2D(182, 158));
	path.push_back(Point2D(183, 156));
	path.push_back(Point2D(181, 154));
	path.push_back(Point2D(178, 153));
	path.push_back(Point2D(171, 153));
	path.push_back(Point2D(164, 153));
	path.push_back(Point2D(160, 153));
	path.push_back(Point2D(150, 154));
	path.push_back(Point2D(147, 155));
	path.push_back(Point2D(141, 157));
	path.push_back(Point2D(137, 158));
	path.push_back(Point2D(135, 158));
	path.push_back(Point2D(137, 158));
	path.push_back(Point2D(140, 157));
	path.push_back(Point2D(143, 156));
	path.push_back(Point2D(151, 154));
	path.push_back(Point2D(160, 152));
	path.push_back(Point2D(170, 149));
	path.push_back(Point2D(179, 147));
	path.push_back(Point2D(185, 145));
	path.push_back(Point2D(192, 144));
	path.push_back(Point2D(196, 144));
	path.push_back(Point2D(198, 144));
	path.push_back(Point2D(200, 144));
	path.push_back(Point2D(201, 147));
	path.push_back(Point2D(199, 149));
	path.push_back(Point2D(194, 157));
	path.push_back(Point2D(191, 160));
	path.push_back(Point2D(186, 167));
	path.push_back(Point2D(180, 176));
	path.push_back(Point2D(177, 179));
	path.push_back(Point2D(171, 187));
	path.push_back(Point2D(169, 189));
	path.push_back(Point2D(165, 194));
	path.push_back(Point2D(164, 196));

	return path;
}

Path2D getGestureArrow()
{
	Path2D path;
	path.push_back(Point2D(68, 222));
	path.push_back(Point2D(70, 220));
	path.push_back(Point2D(73, 218));
	path.push_back(Point2D(75, 217));
	path.push_back(Point2D(77, 215));
	path.push_back(Point2D(80, 213));
	path.push_back(Point2D(82, 212));
	path.push_back(Point2D(84, 210));
	path.push_back(Point2D(87, 209));
	path.push_back(Point2D(89, 208));
	path.push_back(Point2D(92, 206));
	path.push_back(Point2D(95, 204));
	path.push_back(Point2D(101, 201));
	path.push_back(Point2D(106, 198));
	path.push_back(Point2D(112, 194));
	path.push_back(Point2D(118, 191));
	path.push_back(Point2D(124, 187));
	path.push_back(Point2D(127, 186));
	path.push_back(Point2D(132, 183));
	path.push_back(Point2D(138, 181));
	path.push_back(Point2D(141, 180));
	path.push_back(Point2D(146, 178));
	path.push_back(Point2D(154, 173));
	path.push_back(Point2D(159, 171));
	path.push_back(Point2D(161, 170));
	path.push_back(Point2D(166, 167));
	path.push_back(Point2D(168, 167));
	path.push_back(Point2D(171, 166));
	path.push_back(Point2D(174, 164));
	path.push_back(Point2D(177, 162));
	path.push_back(Point2D(180, 160));
	path.push_back(Point2D(182, 158));
	path.push_back(Point2D(183, 156));
	path.push_back(Point2D(181, 154));
	path.push_back(Point2D(178, 153));
	path.push_back(Point2D(171, 153));
	path.push_back(Point2D(164, 153));
	path.push_back(Point2D(160, 153));
	path.push_back(Point2D(150, 154));
	path.push_back(Point2D(147, 155));
	path.push_back(Point2D(141, 157));
	path.push_back(Point2D(137, 158));
	path.push_back(Point2D(135, 158));
	path.push_back(Point2D(137, 158));
	path.push_back(Point2D(140, 157));
	path.push_back(Point2D(143, 156));
	path.push_back(Point2D(151, 154));
	path.push_back(Point2D(160, 152));
	path.push_back(Point2D(170, 149));
	path.push_back(Point2D(179, 147));
	path.push_back(Point2D(185, 145));
	path.push_back(Point2D(192, 144));
	path.push_back(Point2D(196, 144));
	path.push_back(Point2D(198, 144));
	path.push_back(Point2D(200, 144));
	path.push_back(Point2D(201, 147));
	path.push_back(Point2D(199, 149));
	path.push_back(Point2D(194, 157));
	path.push_back(Point2D(191, 160));
	path.push_back(Point2D(186, 167));
	path.push_back(Point2D(180, 176));
	path.push_back(Point2D(177, 179));
	path.push_back(Point2D(171, 187));
	path.push_back(Point2D(169, 189));
	path.push_back(Point2D(165, 194));
	path.push_back(Point2D(164, 196));

	return path;
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
	glutInitWindowSize(breite , hoehe);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("MultiTouchGame Johannes, Sven");

	// openGL init
	glInit();

	// Register callbacks:
	glutDisplayFunc(draw);
	glutIdleFunc(idle);
	glutMainLoop();
	
	
	return 0;

}