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
int hoehe = 530;
int breite = 630;

bool tragging = false;
int traggingobjid = 0;
bool drawing = false;



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
	void Client::removeTuioCursor(TuioCursor *tcur){
	


		if (tragging)
		{
			tragging = false;
			drawing = false;
		}
		
		
		traggingobjid = 0;

	};

	void  Client::refresh(TuioTime frameTime){};
};


constexpr
unsigned int xhash(const char* str, int h = 0)
{
	return !str[h] ? 5381 : (xhash(str, h + 1) * 33) ^ str[h];	
}

//new


void drawCircle(float posx, float posy) {

	glBegin(GL_POLYGON);
	glColor3f(1, 0, 1); 
	for (int ii = 0; ii < 200; ii++)
	{
		float theta = 2.0f * 3.1415926f * float(ii) / float(200);//get the current angle

		float x = 0.2 * cosf(theta);//calculate the x component
		float y = 0.2 * sinf(theta);//calculate the y component

		glVertex2f(x + posx, y + posy);//output vertex

	}
	glEnd();
	glFlush();
}



void drawTriangel(float posX, float posY) {
	glBegin(GL_TRIANGLES);
	glColor3f(1, 0, 0); // red
	glVertex2f(-0.2 + posX, -0.2 + posY);
	glColor3f(0, 1, 0); // green
	glVertex2f(0.2 + posX, -0.2 + posY);
	glColor3f(0, 0, 1); // blue
	glVertex2f(0 + posX, 0.3 + posY);
	glEnd();
	glFlush();
}


// old

void drawStar(float translateX, float translateY) {
	glPushMatrix();
	glColor3f(1, 1, 0);
	glTranslatef(translateX, translateY, 0);
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
	glPopMatrix();
}

bool isInObject(double fingerX, double fingerY, double objectX, double objectY, double radius) {
	

	double diffX = fabs((fingerX + 10) - (objectX + 10));
	double diffY = fabs((fingerY + 10) - (objectY + 10));

	if (diffX <= radius && diffY <= radius) {
		cout << "Finger in Object" << endl;
		//Sleep(5000); 
		return true;
	}

	return false;
	
	/*
	if ((fabs(fingerX - objectX)) <=  radius && (fabs(fingerY - objectY)) <= radius) {
		cout << "Finger in Object" << endl;
		//Sleep(5000);
		return true;
	}
	*/
	



	return false;
}

float getRandomBtw() {
	return ((((float)(rand() % 100)) / 100) * 2) - 1;
}


void drawRectangel(float translateX, float translateY) {
	glPushMatrix();
	glColor3f(1, 0, 0);
	glTranslatef(translateX, translateY, 0);
	glRectd(0.2, 0.2, -0.2, -0.2);
	glFlush();
	glPopMatrix();
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
		double y = ((double)(*cursorListIter)->getY());
		tuioClient->lockCursorList();
		list<TuioPoint> points = (*cursorListIter)->getPath();
		tuioClient->unlockCursorList();
		bool first = true;

		if (id >= 0) {
			// Frames speichern

			Point2D temp(x, y);
			xGesten.gesten[id].push_back(temp); // Punkt speichern
			xGesten.gesetzt[id] = true;			// geste noch nicht fertig = true 
			
			cout << "ID: " << id << "X: " << x << "Y: " << y << endl;

		}

		// Pfad zeichnen
		glPushMatrix();
		glBegin(GL_LINE_STRIP);
		glColor3f(0, 0, 1);

		for (std::list<TuioPoint>::iterator it = points.begin(); it != points.end(); ++it)
		{
			
				glVertex2f(((*it).getX() - 0.5) * 2, (((*it).getY()) - 0.5) * (-2));
			
			
		}
		
		glEnd();
		glPopMatrix();



		// erkennen ob Geste in einem Objekt beginnt

		if (!objekte.empty()) {
		

			if (!tragging && drawing)
			{
				for (int i = 0; i < objekte.size(); i++)
				{

					
					if (isInObject(((*points.begin()).getX() - 0.5) * 2, ((*points.begin()).getY() - 0.5)*(-2), objekte.at(i).x, objekte.at(i).y, 0.30)) {

						cout << "found object !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;

						drawing = false;
						tragging = true;
						traggingobjid = i;


						Objekt temp;
						temp.typ = objekte.at(i).typ;
						temp.x = (x - 0.5) * 2;
						temp.y = (y - 0.5) * (-2);//(((*points.begin()).getY()) - 0.5) * 2;

						objekte.erase(objekte.begin() + i);
						objekte.push_back(temp);

					}
					else {

						drawing = true;
						tragging = false;

					}


				}
			}
			else {

				tragging = true;

				Objekt temp;
				temp.typ = objekte.at(traggingobjid).typ;
				temp.x = (x - 0.5) * 2;
				temp.y = (y - 0.5) * (-2);//(((*points.begin()).getY()) - 0.5) * 2;

				objekte.erase(objekte.begin() + traggingobjid);
				objekte.push_back(temp);


			}

			
		
		}
		else { drawing = true; }


	}
	// alte Gesten löschen

	for (int j = 0; j < 100; j++) {
		if (!xGesten.gesetzt[j] && !xGesten.gesten[j].empty()) {
			
			// REMOVE OLD GEASTURE

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

				/*for (int  i = 0; i < 10; i++)
				{
					float x = ((((float)(rand() % 100))/100)*2)-1;
					cout << "ZUfall " <<  x  << endl;
				}

				*/
				
				cout << "Typ:" << r.name << endl;

				if(drawing){
				switch (xhash(r.name.c_str()))
				{
				case xhash("Star"):
					
					oTemp.typ = "Star";
					oTemp.x = getRandomBtw();
					oTemp.y = getRandomBtw()*(-1);
					
					

					objekte.push_back(oTemp);
					
					break;
				case xhash("Circle"):

					oTemp.typ = "Circle";
					oTemp.x = getRandomBtw();
					oTemp.y = getRandomBtw()*(-1);

					cout << "Circle : X: " << oTemp.x << "Y: "<<oTemp.y << endl;
					//Sleep(2000);

					objekte.push_back(oTemp);

					break;

				case xhash("Rectangle"):

					oTemp.typ = "Rectangle";
					oTemp.x = getRandomBtw();
					oTemp.y = getRandomBtw()*(-1);

					objekte.push_back(oTemp);

					break;
				case xhash("Triangle"):

					oTemp.typ = "Triangle";
					oTemp.x = getRandomBtw();
					oTemp.y = getRandomBtw()*(-1);

					objekte.push_back(oTemp);

					break;
				default:
					cout << "Fehler Objekt nicht erkannt" << endl;
					break;
				}
				}
				
				if (!tragging)
				{
					drawing = true;
				}
				//Sleep(2000);
			
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
				drawStar(objekte.at(i).x, objekte.at(i).y);
				break;
			case xhash("Rectangle"):
				drawRectangel(objekte.at(i).x, objekte.at(i).y);
				break;
			case xhash("Triangle") :
				drawTriangel(objekte.at(i).x, objekte.at(i).y);
				break;
			case xhash("Circle"):
				drawCircle(objekte.at(i).x, objekte.at(i).y);
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

int main(int argc, char** argv)
{
	

	srand(time(NULL));



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