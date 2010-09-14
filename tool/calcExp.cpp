// $(CXX) -lGL -glut calcExp.cpp

#include <cstdlib>
#include <iostream>
#include <vector>

#include <GL/gl.h>
#include <GL/glut.h>


int calcExp(int const level, int const basic, int const increase, int const correction)
{
	int result = 0;

	#if (RPG2K_IS_PSP || RPG2K_IS_IPHONE)
		float standard = basic, additional = 1.5f + (increase * 0.01f);
		for(int i = 0; i < (level - 1); i++) {
			result += int(standard);
			standard *= additional;
			additional = (level * 0.002f + 0.8f) * (additional - 1.0f) + 1.0f;
		}
	#else
		double standard = basic, additional = 1.5 + (increase * 0.01);
		for(int i = 0; i < (level - 1); i++) {
			result += int(standard);
			standard *= additional;
			additional = (level * 0.002 + 0.8) * (additional - 1.0) + 1.0;
		}
	#endif
	result += correction * (level - 1);

	return result;
	// if (result < 1000000) return result;
	// else return 1000000;
}

/*
int calcExp(int currentLevel, int stdIncrease, int addIncrease, int correction)
{
        double standard;
        double additional;
        int i;
        int result = 0;
 
        standard = stdIncrease;
        additional = 1.5 + (addIncrease * 0.01);
        for (i = currentLevel - 1; i >= 1; i--)
        {
                result = result + (correction + int(standard));
                standard = standard * additional;
                additional = (currentLevel * 0.002 + 0.8) * (additional - 1) + 1;
        }
        if (result < 1000000) return result;
        else return 1000000;
}
 */

namespace
{
	enum { EXP_MAX = 1000000, LEVEL_MAX = 50, DIM = 3, };
	static GLdouble const COORD = 1.0;
	int basic = 24, increase = 37, correction = 1;

	void resize(int w, int h)
	{
		glViewport(0, 0, w, h);
		glLoadIdentity();
		glOrtho(0.0, COORD, 0.0, COORD, -1.0, 1.0);
	}
	void display(void)
	{
		glClear(GL_COLOR_BUFFER_BIT);

		static GLdouble buf[DIM * LEVEL_MAX];

		for (int i = 0; i <= LEVEL_MAX; i++) {
			buf[i * DIM + 0] = COORD * GLdouble(i) / GLdouble(LEVEL_MAX);
			buf[i * DIM + 1] = COORD *
				GLdouble( calcExp(i, basic, increase, correction) ) / GLdouble(EXP_MAX);
			buf[i * DIM + 2] = 0.0f;
		}

		glEnableClientState(GL_VERTEX_ARRAY);
			glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
			glVertexPointer(DIM, GL_DOUBLE, 0, &(buf[0]));
			glDrawArrays(GL_LINE_STRIP, 0, LEVEL_MAX);
		glDisableClientState(GL_VERTEX_ARRAY);

		glutSwapBuffers();
	}
} // namespace

int main(int argc, char** argv)
{
	switch(argc) {
	case 4:
		correction = std::atoi(argv[3]);
	case 3:
		increase = std::atoi(argv[2]);
		basic = std::atoi(argv[1]);
		break;
	}

	for (int i = 1; i <= LEVEL_MAX; i++) {
		std::cout << calcExp(i, basic, increase, correction) << std::endl;
	}

	glutInit(&argc, argv);

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutInitWindowSize(640, 480);
	glutCreateWindow(argv[0]);
	glutIdleFunc(&glutPostRedisplay);
	glutReshapeFunc(&resize);
	glutDisplayFunc(&display);

	// glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	// glEnable(GL_LINE_SMOOTH);
	glClearColor(1.0, 1.0, 1.0, 1.0);

	glutMainLoop();

	return EXIT_SUCCESS;
}
