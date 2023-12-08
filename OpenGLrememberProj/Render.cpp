#include "Render.h"

#include <sstream>
#include <iostream>

#include <cmath>

#include <windows.h>
#include <GL\GL.h>
#include <GL\GLU.h>

#include "MyOGL.h"

#include "Camera.h"
#include "Light.h"
#include "Primitives.h"

#include "GUItextRectangle.h"

bool textureMode = true;
bool lightMode = true;

//����� ��� ��������� ������
class CustomCamera : public Camera
{
public:
	//��������� ������
	double camDist;
	//���� �������� ������
	double fi1, fi2;

	
	//������� ������ �� ���������
	CustomCamera()
	{
		camDist = 15;
		fi1 = 1;
		fi2 = 1;
	}

	
	//������� ������� ������, ������ �� ����� ��������, ���������� �������
	void SetUpCamera()
	{
		//�������� �� ������� ������ ������
		lookPoint.setCoords(0, 0, 0);

		pos.setCoords(camDist*cos(fi2)*cos(fi1),
			camDist*cos(fi2)*sin(fi1),
			camDist*sin(fi2));

		if (cos(fi2) <= 0)
			normal.setCoords(0, 0, -1);
		else
			normal.setCoords(0, 0, 1);

		LookAt();
	}

	void CustomCamera::LookAt()
	{
		//������� ��������� ������
		gluLookAt(pos.X(), pos.Y(), pos.Z(), lookPoint.X(), lookPoint.Y(), lookPoint.Z(), normal.X(), normal.Y(), normal.Z());
	}



}  camera;   //������� ������ ������


//����� ��� ��������� �����
class CustomLight : public Light
{
public:
	CustomLight()
	{
		//��������� ������� �����
		pos = Vector3(1, 1, 3);
	}

	
	//������ ����� � ����� ��� ���������� �����, ���������� �������
	void  DrawLightGhismo()
	{
		glDisable(GL_LIGHTING);

		
		glColor3d(0.9, 0.8, 0);
		Sphere s;
		s.pos = pos;
		s.scale = s.scale*0.08;
		s.Show();
		
		if (OpenGL::isKeyPressed('G'))
		{
			glColor3d(0, 0, 0);
			//����� �� ��������� ����� �� ����������
			glBegin(GL_LINES);
			glVertex3d(pos.X(), pos.Y(), pos.Z());
			glVertex3d(pos.X(), pos.Y(), 0);
			glEnd();

			//������ ���������
			Circle c;
			c.pos.setCoords(pos.X(), pos.Y(), 0);
			c.scale = c.scale*1.5;
			c.Show();
		}

	}

	void SetUpLight()
	{
		GLfloat amb[] = { 0.2, 0.2, 0.2, 0 };
		GLfloat dif[] = { 1.0, 1.0, 1.0, 0 };
		GLfloat spec[] = { .7, .7, .7, 0 };
		GLfloat position[] = { pos.X(), pos.Y(), pos.Z(), 1. };

		// ��������� ��������� �����
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		// �������������� ����������� �����
		// ������� ��������� (���������� ����)
		glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
		// ��������� ������������ �����
		glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);
		// ��������� ���������� ������������ �����
		glLightfv(GL_LIGHT0, GL_SPECULAR, spec);

		glEnable(GL_LIGHT0);
	}


} light;  //������� �������� �����




//������ ���������� ����
int mouseX = 0, mouseY = 0;

void mouseEvent(OpenGL *ogl, int mX, int mY)
{
	int dx = mouseX - mX;
	int dy = mouseY - mY;
	mouseX = mX;
	mouseY = mY;

	//������ ���� ������ ��� ������� ����� ������ ����
	if (OpenGL::isKeyPressed(VK_RBUTTON))
	{
		camera.fi1 += 0.01*dx;
		camera.fi2 += -0.01*dy;
	}

	
	//������� ���� �� ���������, � ����� ��� ����
	if (OpenGL::isKeyPressed('G') && !OpenGL::isKeyPressed(VK_LBUTTON))
	{
		LPPOINT POINT = new tagPOINT();
		GetCursorPos(POINT);
		ScreenToClient(ogl->getHwnd(), POINT);
		POINT->y = ogl->getHeight() - POINT->y;

		Ray r = camera.getLookRay(POINT->x, POINT->y);

		double z = light.pos.Z();

		double k = 0, x = 0, y = 0;
		if (r.direction.Z() == 0)
			k = 0;
		else
			k = (z - r.origin.Z()) / r.direction.Z();

		x = k*r.direction.X() + r.origin.X();
		y = k*r.direction.Y() + r.origin.Y();

		light.pos = Vector3(x, y, z);
	}

	if (OpenGL::isKeyPressed('G') && OpenGL::isKeyPressed(VK_LBUTTON))
	{
		light.pos = light.pos + Vector3(0, 0, 0.02*dy);
	}

	
}

void mouseWheelEvent(OpenGL *ogl, int delta)
{

	if (delta < 0 && camera.camDist <= 1)
		return;
	if (delta > 0 && camera.camDist >= 100)
		return;

	camera.camDist += 0.01*delta;

}

void keyDownEvent(OpenGL *ogl, int key)
{
	if (key == 'L')
	{
		lightMode = !lightMode;
	}

	if (key == 'T')
	{
		textureMode = !textureMode;
	}

	if (key == 'R')
	{
		camera.fi1 = 1;
		camera.fi2 = 1;
		camera.camDist = 15;

		light.pos = Vector3(1, 1, 3);
	}

	if (key == 'F')
	{
		light.pos = camera.pos;
	}
}

void keyUpEvent(OpenGL *ogl, int key)
{
	
}



GLuint texId;

//����������� ����� ������ ��������
void initRender(OpenGL *ogl)
{
	//��������� �������

	//4 ����� �� �������� �������
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	//��������� ������ ��������� �������
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//�������� ��������
	glEnable(GL_TEXTURE_2D);
	

	//������ ����������� ���������  (R G B)
	RGBTRIPLE *texarray;

	//������ ��������, (������*������*4      4, ���������   ����, �� ������� ������������ �� 4 ����� �� ������� �������� - R G B A)
	char *texCharArray;
	int texW, texH;
	OpenGL::LoadBMP("texture.bmp", &texW, &texH, &texarray);
	OpenGL::RGBtoChar(texarray, texW, texH, &texCharArray);

	
	
	//���������� �� ��� ��������gggggg
	glGenTextures(1, &texId);
	//������ ��������, ��� ��� ����� ����������� � ���������, ����� ����������� �� ����� ��
	glBindTexture(GL_TEXTURE_2D, texId);

	//��������� �������� � �����������, � ���������� ��� ������  ��� �� �����
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texW, texH, 0, GL_RGBA, GL_UNSIGNED_BYTE, texCharArray);

	//�������� ������
	free(texCharArray);
	free(texarray);

	//������� ����
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


	//������ � ���� ����������� � "������"
	ogl->mainCamera = &camera;
	ogl->mainLight = &light;

	// ������������ �������� : �� ����� ����� ����� 1
	glEnable(GL_NORMALIZE);

	// ���������� ������������� ��� �����
	glEnable(GL_LINE_SMOOTH); 


	//   ������ ��������� ���������
	//  �������� GL_LIGHT_MODEL_TWO_SIDE - 
	//                0 -  ������� � ���������� �������� ���������(�� ���������), 
	//                1 - ������� � ���������� �������������� ������� ��������       
	//                �������������� ������� � ���������� ��������� ����������.    
	//  �������� GL_LIGHT_MODEL_AMBIENT - ������ ������� ���������, 
	//                �� ��������� �� ���������
	// �� ��������� (0.2, 0.2, 0.2, 1.0)

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, 0);

	camera.fi1 = -1.3;
	camera.fi2 = 0.8;
}


struct Point {
	double x, y, z;
};

class Figure {
private:
	std::vector<std::vector<Point>> triangles = {
		{ {8, 3, 0}, {4, 6, 0}, {9, 10, 0} },
		{ {9,10, 0}, {7, 14, 0}, {11, 17, 0} },
		{ {11, 17, 0}, {18, 14, 0}, {11, 7, 0} },
		{ {18, 14, 0}, {18, 5, 0}, {11, 7, 0} },
		{ {11, 7, 0}, {9, 10, 0}, {11, 17, 0} },
		{ {11, 7, 0}, {8,3, 0}, {9, 10, 0} } 
	};
	std::vector<std::vector<Point>> sides = {
		{{8, 3, 0}, {4, 6, 0}},
		{{4, 6, 0}, {9, 10, 0}},
		{{9, 10, 0}, {7, 14, 0}},
		{{7, 14, 0}, {11, 17, 0}},
		{{11, 17, 0}, {18, 14, 0}},
		{{18, 14, 0}, {18, 5, 0}},
		{{18, 5, 0}, {11, 7, 0}},
		{{11, 7, 0},{8, 3, 0}},
	};
	double height = 3;
public:
	Figure() {
		
	}

	Point normalize(int i, double height = 0) {
		// ���������� �������� ������� ��� ������� ������������
		Point AB = { triangles[i][2].x - triangles[i][0].x, triangles[i][2].y - triangles[i][0].y, (triangles[i][2].z + height) - (triangles[i][0].z + height) };
		Point AC = { triangles[i][1].x - triangles[i][0].x, triangles[i][1].y - triangles[i][0].y, (triangles[i][1].z + height) - (triangles[i][0].z + height) };
		Point normal = {
			AB.y * AC.z - AB.z * AC.y,
			-AB.x * AC.z + AC.x * AB.z,
			AB.x * AC.y - AB.y * AC.x
		};

		// ������������ ������� �������
		double length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= length;
		normal.y /= length;
		normal.z /= length;

		return normal;
	}

	void triangleUpDown() {
		// ��������� ������� � ������ ��������� ������

		glColor3d(0, 0, 0.9);
		for (int i = 0; i < triangles.size(); i++) {

			if (i == 2) {
				int segments = 100;
				// ���� ����� ������� �� (x,y)

				double x_0 = triangles[i][0].x;
				double y_0 = triangles[i][0].y;
				
				double sideLength = sqrt(pow(triangles[i][1].x - triangles[i][0].x, 2) +
					pow(triangles[i][1].y - triangles[i][0].y, 2) +
					pow(triangles[i][1].z - triangles[i][0].z, 2));

				double centerX = (triangles[i][1].x + triangles[i][0].x) / 2;
				double centerY = (triangles[i][1].y + triangles[i][0].y) / 2;
				Point middle = {
					centerX,
					centerY,
					0
				};

				double radius = sideLength / 2;

				// ��������� ���� (� ��������) ��� ����� ��������
				double initialAngle = atan2(triangles[i][1].y - triangles[i][0].y, triangles[i][1].x - triangles[i][0].x);

				// ������ ������ ��������������
				glBegin(GL_TRIANGLE_FAN);
				glNormal3d(0, 0, -1); // ������� ���������� ����, ��� ��� ��� ������ ��������������
				glVertex3d(middle.x, middle.y, middle.z); //��������� ����� (�����)
				
				for (int j = 0; j <= segments; j++) {
					//double angle = 2.0 * PI * i / sides;
					double t = initialAngle + M_PI * j / segments;
					double x = middle.x + radius * cos(t);
					double y = middle.y + radius * sin(t);
					glVertex3d(x, y, 0);
				}
				glEnd();

				// ������ ������� ��������������
				glBegin(GL_TRIANGLE_FAN);
				glNormal3d(0, 0, 1); // ������� ���������� �����, ��� ��� ��� ������� ��������������
				glVertex3d(middle.x, middle.y, height); //��������� ����� (�����)

				for (int j = 0; j <= segments; j++) {
					//double angle = 2.0 * PI * i / sides;
					double t = initialAngle + M_PI * j / segments;
					double x = middle.x + radius * cos(t);
					double y = middle.y + radius * sin(t);
					glVertex3d(x, y, height);
				}
				glEnd();

				// ��������� ������� � ������ ��������������
				glBegin(GL_QUAD_STRIP);
				
				for (int j = 0; j <= segments; j++) {
					double t = initialAngle + M_PI * j / segments;
					double x = middle.x + radius * cos(t);
					double y = middle.y + radius * sin(t);
					glVertex3d(x, y, 0);
					glVertex3d(x, y, height);

					// ������������ ������ ����������� ��� ������� �����������
					double normalX = cos(t);
					double normalY = sin(t);
					double normalZ = 0; // � ������ ������ ������� ����������� ����������� ��������� XY

					// ����������� ������ �����������
					double length = sqrt(normalX * normalX + normalY * normalY + normalZ * normalZ);
					normalX /= length;
					normalY /= length;
					normalZ /= length;

					// ������������� ������� ��� ������� �����������
					glNormal3d(normalX, normalY, normalZ);

					// ������ �������
					glVertex3d(x, y, 0);
					glVertex3d(x, y, height);
				}
				glEnd();
				
			}
			glBegin(GL_TRIANGLES);

			// ������������ (���)
			Point normalDown = normalize(i);
			// �������� �������
			glNormal3d(normalDown.x, normalDown.y, -1);

			// ��� ������
			for (int j = 0; j < 3; j++)
				glVertex3d(triangles[i][j].x, triangles[i][j].y, triangles[i][j].z);

			// ������������ (����)
			Point normalUp = normalize(i, height);
			// �������� �������
			glNormal3d(normalUp.x, normalUp.y, 1);

			// ���� ������
			for (int j = 0; j < 3; j++)
				glVertex3d(triangles[i][j].x, triangles[i][j].y, triangles[i][j].z + height);

			glEnd();
		}
	}

	Point normalizeSides(int i) {
		// ���������� �������� ������� ��� ������� ������������
		Point AD = { sides[i][1].x - sides[i][0].x, sides[i][1].y - sides[i][0].y, sides[i][1].z - sides[i][0].z };
		Point AB = { sides[i][0].x - sides[i][0].x, sides[i][0].y - sides[i][0].y, (sides[i][0].z + height) - sides[i][0].z };
		Point normal = {
			AB.y * AD.z - AB.z * AD.y,
			-AB.x * AD.z + AD.x * AB.z,
			AB.x * AD.y - AB.y * AD.x
		};

		// ������������ ������� �������
		double length = std::sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= length;
		normal.y /= length;
		normal.z /= length;

		return normal;
	}

	void triangleSides() {
		glColor3d(0, 0, 0.9);
		for (int i = 0; i < sides.size(); i++) {
			glBegin(GL_TRIANGLE_STRIP);

			// ������������ (������� �������)
			Point normalSide = normalizeSides(i);
			glNormal3d(normalSide.x, normalSide.y, normalSide.z);

			// ������� �������
			glVertex3d(sides[i][0].x, sides[i][0].y, sides[i][0].z);
			glVertex3d(sides[i][0].x, sides[i][0].y, sides[i][0].z + height);
			glVertex3d(sides[i][1].x, sides[i][1].y, sides[i][1].z);
			glVertex3d(sides[i][1].x, sides[i][1].y, sides[i][1].z + height);

			glEnd();
			
		}
	}
};


void Render(OpenGL *ogl)
{



	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);

	glEnable(GL_DEPTH_TEST);
	if (textureMode)
		glEnable(GL_TEXTURE_2D);

	if (lightMode)
		glEnable(GL_LIGHTING);


	//��������������
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	//��������� ���������
	GLfloat amb[] = { 0.2, 0.2, 0.1, 1. };
	GLfloat dif[] = { 0.4, 0.65, 0.5, 1. };
	GLfloat spec[] = { 0.9, 0.8, 0.3, 1. };
	GLfloat sh = 0.1f * 256;


	//�������
	glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
	//��������
	glMaterialfv(GL_FRONT, GL_DIFFUSE, dif);
	//����������
	glMaterialfv(GL_FRONT, GL_SPECULAR, spec); \
		//������ �����
		glMaterialf(GL_FRONT, GL_SHININESS, sh);

	//���� ���� �������, ��� ����������� (����������� ���������)
	glShadeModel(GL_SMOOTH);
	//===================================
	//������� ���  

	Figure figure;
	figure.triangleUpDown();
	figure.triangleSides();

	//������ ��������� ���������� ��������
	double A[2] = { -4, -4 };
	double B[2] = { 4, -4 };
	double C[2] = { 4, 4 };
	double D[2] = { -4, 4 };

	glBindTexture(GL_TEXTURE_2D, texId);

	glColor3d(0.6, 0.6, 0.6);
	glBegin(GL_QUADS);

	glNormal3d(0, 0, 1);
	glTexCoord2d(0, 0);
	glVertex2dv(A);
	glTexCoord2d(1, 0);
	glVertex2dv(B);
	glTexCoord2d(1, 1);
	glVertex2dv(C);
	glTexCoord2d(0, 1);
	glVertex2dv(D);

	glEnd();
	//����� ��������� ���������� ��������


   //��������� ������ ������

	
	glMatrixMode(GL_PROJECTION);	//������ �������� ������� ��������. 
	                                //(���� ��������� ��������, ����� �� ������������.)
	glPushMatrix();   //��������� ������� ������� ������������� (������� ��������� ������������� ��������) � ���� 				    
	glLoadIdentity();	  //��������� ��������� �������
	glOrtho(0, ogl->getWidth(), 0, ogl->getHeight(), 0, 1);	 //������� ����� ������������� ��������

	glMatrixMode(GL_MODELVIEW);		//������������� �� �����-��� �������
	glPushMatrix();			  //��������� ������� ������� � ���� (��������� ������, ����������)
	glLoadIdentity();		  //���������� �� � ������

	glDisable(GL_LIGHTING);



	GuiTextRectangle rec;		   //������� ����� ��������� ��� ������� ������ � �������� ������.
	rec.setSize(300, 150);
	rec.setPosition(10, ogl->getHeight() - 150 - 10);


	std::stringstream ss;
	ss << "T - ���/���� �������" << std::endl;
	ss << "L - ���/���� ���������" << std::endl;
	ss << "F - ���� �� ������" << std::endl;
	ss << "G - ������� ���� �� �����������" << std::endl;
	ss << "G+��� ������� ���� �� ���������" << std::endl;
	ss << "�����. �����: (" << light.pos.X() << ", " << light.pos.Y() << ", " << light.pos.Z() << ")" << std::endl;
	ss << "�����. ������: (" << camera.pos.X() << ", " << camera.pos.Y() << ", " << camera.pos.Z() << ")" << std::endl;
	ss << "��������� ������: R="  << camera.camDist << ", fi1=" << camera.fi1 << ", fi2=" << camera.fi2 << std::endl;
	
	rec.setText(ss.str().c_str());
	rec.Draw();

	glMatrixMode(GL_PROJECTION);	  //��������������� ������� �������� � �����-��� �������� �� �����.
	glPopMatrix();


	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

}