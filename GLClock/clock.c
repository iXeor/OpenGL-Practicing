#define NDEBUG
#define SECURE_NO_WARNINGS

#include <GL/glut.h>
#include <time.h>
#include <Windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

float hour;
float minute;
float second;
int week;
int day;
int year;
int mon;
float lastSecond;

GLuint textList;

char* weekStr[] = { "SUN" , "MON" , "TUE" , "WED" , "THU" , "FRI" , "SAT" };

void getTime()
{
	time_t now;
	struct tm timenow;

	time(&now);
	localtime_s(&timenow, &now);

	lastSecond = second;

	hour = timenow.tm_hour;
	minute = timenow.tm_min;
	second = timenow.tm_sec;
	day = timenow.tm_mday;
	year = timenow.tm_year + 1900;
	mon = timenow.tm_mon + 1;
	week = timenow.tm_wday;
}

/*字体渲染器*/
void selectFont(int size, const char* face)
{
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);

	glDeleteLists(textList, 256);

	textList = glGenLists(256);
	wglUseFontBitmaps(wglGetCurrentDC(), 0, 256, textList);
}

/*绘制字符串的方法*/
void drawString(int x, int y, char* str)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glLoadIdentity();
	glRasterPos2i(x, y);

	int len = 0;
	int i = 0;
	wchar_t* wstr;
	HDC hDC = wglGetCurrentDC();
	GLuint list = glGenLists(1);

	for (i = 0; str[i] != '\0'; ++i)
	{
		if (IsDBCSLeadByte(str[i]))
		{
			++i;
		}
		++len;
	}

	wstr = (wchar_t*)malloc((len + 1) * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, str, -1, wstr, len);
	wstr[len] = L'\0';

	for (i = 0; i < len; ++i)
	{
		wglUseFontBitmapsW(hDC, wstr[i], 1, list);
		glCallList(list);
	}

	free(wstr);

	glDeleteLists(list, 1);
}

/*绘制圆形*/
void drawCircle(float x, float y, float r)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_STRIP);

	float step = 0.1f;
	float dx = x;
	float dy = y;

	for (float angle = 0.0f; angle <= 3.14159265f * 2.0f; angle += step)
	{
		dx = x + r * sin(angle);
		dy = y + r * cos(angle);

		glVertex3f(dx, dy, 0);
	}
	dx = x + r * sin(0.0f);
	dy = y + r * cos(0.0f);
	glVertex3f(dx, dy, 0);

	glEnd();
}

void init()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	glShadeModel(GL_SMOOTH);

	glViewport(0, 0, 800, 600);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glOrtho(-400.0f, 400.0f, -300.0f, 300.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

}

void render()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	// 表盘
	glLineWidth(5.0f);
	drawCircle(0.0f, 0.0f, 250.0f);
	glLineWidth(3.0f);
	drawCircle(-145.0f, 0.0f, 50.0f);
	glLoadIdentity();
	glLineWidth(3.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2i(155, 15);
	glVertex2i(155, -15);
	glVertex2i(225, -15);
	glVertex2i(225, 15);
	glEnd();
	glLineWidth(2.0f);
	drawCircle(0.0f, 0.0f, 255.0f);
	glLineWidth(5.0f);
	drawCircle(0.0f, 0.0f, 5.0f);
	glLineWidth(1.0f);
	for (float w = 0.0f; w < 360.f; w += 6.0f)
	{
		glLoadIdentity();
		glRotatef(w, 0, 0, -1);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex2i(0, 250);
		glVertex2i(0, 245);
		glEnd();
	}

	glLineWidth(3.0f);
	for (float z = 0.0f; z < 360.f; z += 30.0f)
	{
		glLoadIdentity();
		glRotatef(z, 0, 0, -1);
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_LINES);
		glVertex2i(0, 250);
		glVertex2i(0, 240);
		glEnd();
	}

	// 文字
	char str[100];
	char str2[20];
	sprintf_s(str2, 20, "%d\0", day);
	selectFont(48, "Bank Gothic Medium BT");
	drawString(175, -10, str2);
	selectFont(64, "Agency FB");

	if ((int)second % 10 < 5)
	{
		sprintf_s(str, 100, "%02d:%02d:%02d\0", (int)hour, (int)minute, (int)second);
	}
	else
	{
		sprintf_s(str, 100, "%04d-%02d\0", (int)year, (int)mon);
	}
	drawString(-58, -200, str);
	selectFont(36, "BatmanForeverAlternate");
	drawString(-185, -10, weekStr[week]);
	selectFont(30, "Freestyle Script");
	drawString(-55, 175, "Powered by OpenGL");
	glLineWidth(5.0f);
	// 时针
	glLoadIdentity();
	glRotatef(((float)((int)hour % 12) / 12.0f) * 360.0f + (minute / 60.0f) * 30.0f, 0, 0, -1);
	glColor3f(0.7f, 0.7f, 0.7f);
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(0, 100);
	glVertex2i(0, 100);
	glEnd();
	glLineWidth(3.0f);
	// 分针
	glLoadIdentity();
	glRotatef((minute / 60.0f) * 360.0f + (second / 60.0f) * 6.0f, 0, 0, -1);
	glColor3f(0.7f, 0.7f, 0.7f);
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(0, 200);
	glVertex2i(0, 200);
	glEnd();
	glLineWidth(1.5f);
	// 秒针
	glLoadIdentity();
	glRotatef((second / 60.0f) * 360.0f, 0, 0, -1);
	glColor3f(0.5f, 1.0f, 0.3f);
	glBegin(GL_LINES);
	glVertex2i(0, 0);
	glVertex2i(0, 240);
	glVertex2i(0, 240);
	glEnd();

	glFlush();
}

void update()
{
	getTime();

	if (lastSecond != second)
	{
		glutPostRedisplay();
	}
}

void timeTicks(int value)
{
	glutTimerFunc(1000, timeTicks, 1);
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
	glutInitWindowPosition(300, 300);
	glutInitWindowSize(1024, 768);
	glutCreateWindow("OpenGL - Clock");

	glutDisplayFunc(render);
	glutIdleFunc(update);

	glutTimerFunc(1000, timeTicks, 1);

	init();

	glutMainLoop();

	return 0;
}