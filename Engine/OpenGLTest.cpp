#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "OpenGLTest.h"

OpenGLTest::OpenGLTest(Application * app, bool start_enabled): Module(app, "Level", start_enabled)
{}

OpenGLTest::~OpenGLTest()
{}

UPDATE_STATUS OpenGLTest::Configuration(float dt)
{
	BROFILER_CATEGORY("Open GL Test Configuration", Profiler::Color::BlanchedAlmond)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Triangle"))
	{
		if (ImGui::Button("Wireframe"))
			wireframe = !wireframe;
	}
	return ret;
}

bool OpenGLTest::Init()
{
	vertices[0] = 0.0;
	vertices[1] = 0.5;
	vertices[2] = 1.0;
	vertices[3] = -0.5;
	vertices[4] = -0.5;
	vertices[5] = 1.0;
	vertices[6] = 0.5;
	vertices[7] = -0.5;
	vertices[8] = 1.0;
	return true;
}

void OpenGLTest::DrawTriangle()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	if(!wireframe)
		glDrawArrays(GL_TRIANGLES, 0, 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}
