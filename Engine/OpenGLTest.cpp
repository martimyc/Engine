#include "glew\include\GL\glew.h"
#include "MathGeoLib\src\MathGeoLib.h"
#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "Application.h"
#include "Window.h"
#include "Input.h"
#include "OpenGLTest.h"

OpenGLTest::OpenGLTest(Application * app, bool start_enabled): Module(app, "Level", start_enabled)
{}

OpenGLTest::~OpenGLTest()
{}

bool OpenGLTest::Init()
{
	GLfloat cube_vert[8 * 3] =
	{
		0.f, 0.f, 0.f,		//0
		1.f, 0.f, 0.f,		//1
		1.f, 1.f, 0.f,		//2
		0.f, 1.f, 0.f,		//3
		0.f, 0.f, 1.f,		//4
		1.f, 0.f, 1.f,		//5
		1.f, 1.f, 1.f,		//6
		0.f, 1.f, 1.f		//7
	};

	GLushort cube_indices[36] =
	{
		6,7,5, 7,4,5,	//Front
		6,5,1, 2,6,1,	//Right
		3,2,1, 0,3,1,	//Back
		7,3,0, 7,0,4,	//Left
		4,1,5, 4,0,1,	//Bottom
		3,6,2, 3,7,6	//Top
		
	};

	//Load Geometry to VRAM
	glGenBuffers(1, (GLuint*) &(cube_id));
	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vert), cube_vert, GL_STATIC_DRAW);
	/*glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat)*5, NULL);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 5, (void*)(sizeof(GLfloat) * 2));*/
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glGenBuffers(1, (GLuint*) &(cube_indices_id));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	//checkers texture
	GLubyte checkImage[CHECKERS_HEIGHT][CHECKERS_WIDTH][4];
	for (int i = 0; i < CHECKERS_HEIGHT; i++) {
		for (int j = 0; j < CHECKERS_WIDTH; j++) {
			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
			checkImage[i][j][0] = (GLubyte)c;
			checkImage[i][j][1] = (GLubyte)c;
			checkImage[i][j][2] = (GLubyte)c;
			checkImage[i][j][3] = (GLubyte)255;
		}
	}

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glGenTextures(1, &img_id);
	glBindTexture(GL_TEXTURE_2D, img_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, CHECKERS_WIDTH, CHECKERS_HEIGHT,
		0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	return true;
}

UPDATE_STATUS OpenGLTest::Configuration(float dt)
{
	BROFILER_CATEGORY("Open GL Test Configuration", Profiler::Color::BlanchedAlmond)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("OpenGL Test"))
	{
		ImGui::Checkbox("Wireframe", &wireframe);

		ImGui::Checkbox("Draw debug point", &debug_point);

		if(debug_point)
		{
			ImGui::InputFloat("X", &x);
			ImGui::InputFloat("Y", &y);
			ImGui::InputFloat("Z", &z);
		}
		
	}
	return ret;
}

void OpenGLTest::Draw3DPoint(const GLfloat x, const GLfloat y, const GLfloat z) const
{
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnable(GL_POINT_SMOOTH);
	glPointSize(10);
	glBegin(GL_POINTS);
	glVertex3f(x, y, z);
	glEnd();
	glDisable(GL_POINT_SMOOTH);
	glDisableClientState(GL_VERTEX_ARRAY);
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
}

void OpenGLTest::DrawDebugPoint() const
{
	if(debug_point)
		Draw3DPoint(x, y, z);
}

void OpenGLTest::DrawCubeDirectMode() const
{
	glBindTexture(GL_TEXTURE_2D, img_id);

	glBegin(GL_TRIANGLES);

	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);

	//Front
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.f, 1.f, 1.f);	//E
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.f, 1.f, 1.f);	//D	
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.f, 0.f, 1.f);	//C
	
	glTexCoord2f(1.0f, 1.0f); glVertex3f(1.f, 1.f, 1.f);	//E
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.f, 0.f, 1.f);	//C	
	glTexCoord2f(1.0f, 0.0f); glVertex3f(1.f, 0.f, 1.f);	//F	

	//Left	
	glTexCoord2f(0.0f, 1.0f); glVertex3f(0.f, 0.f, 0.f);	//B
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.f, 0.f, 1.f);	//C
	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.f, 1.f, 0.f);	//A

	glTexCoord2f(0.0f, 0.0f); glVertex3f(0.f, 1.f, 0.f);	//A
	glTexCoord2f(1.0f, 1.0f); glVertex3f(0.f, 0.f, 1.f);	//C
	glTexCoord2f(1.0f, 0.0f); glVertex3f(0.f, 1.f, 1.f);	//D

	//Back
	glVertex3f(1.f, 0.f, 0.f);		//H
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.f, 0.f, 0.f);		//B
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(0.f, 1.f, 0.f);		//A
	glTexCoord2f(0.0f, 0.0f);

	glVertex3f(1.f, 0.f, 0.f);		//H
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(0.f, 1.f, 0.f);		//A
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(1.f, 1.f, 0.f);		//G
	glTexCoord2f(1.0f, 0.0f);

	//Bottom
	glVertex3f(0.f, 0.f, 0.f);		//B
	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(0.f, 0.f, 1.f);		//C

	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 0.f, 1.f);		//F
	glVertex3f(0.f, 0.f, 1.f);		//C

	//Top
	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(1.f, 1.f, 0.f);		//G

	glVertex3f(0.f, 1.f, 0.f);		//A
	glVertex3f(0.f, 1.f, 1.f);		//D
	glVertex3f(1.f, 1.f, 1.f);		//E

	//Right
	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 1.f, 0.f);		//G
	glVertex3f(1.f, 1.f, 1.f);		//E

	glVertex3f(1.f, 0.f, 0.f);		//H
	glVertex3f(1.f, 1.f, 1.f);		//E
	glVertex3f(1.f, 0.f, 1.f);		//F

	glEnd();

	glBindTexture(GL_TEXTURE_2D, 0);
}

void OpenGLTest::DrawCubeIndicesVertex() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glBindBuffer(GL_ARRAY_BUFFER, cube_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube_indices_id);
	glVertexPointer(3, GL_FLOAT, 0, NULL);

	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, NULL);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glDisableClientState(GL_VERTEX_ARRAY);
}
