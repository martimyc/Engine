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
	triangle_vertices[0] = 0.0;
	triangle_vertices[1] = 0.5;
	triangle_vertices[2] = 1.0;
	triangle_vertices[3] = -0.5;
	triangle_vertices[4] = -0.5;
	triangle_vertices[5] = 1.0;
	triangle_vertices[6] = 0.5;
	triangle_vertices[7] = -0.5;
	triangle_vertices[8] = 1.0;


	quad_vertices[0] = 0.0f;
	quad_vertices[1] = 300.0f;
	quad_vertices[2] = 1.0f;
	quad_vertices[3] = 300.0f;
	quad_vertices[4] = 300.0f;
	quad_vertices[5] = 1.0f;
	quad_vertices[6] = 300.0f;
	quad_vertices[7] = 0.0f;
	quad_vertices[8] = 1.0f;
	quad_vertices[9] = 0.0f;
	quad_vertices[10] = 0.0f;
	quad_vertices[11] = 1.0f;
		

	return true;
}

void OpenGLTest::DrawTriangle() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, triangle_vertices);
	if(!wireframe)
		glDrawArrays(GL_TRIANGLES, 0, 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}

void OpenGLTest::DrawQuad() const
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, quad_vertices);
	if (!wireframe)
		glDrawArrays(GL_QUADS, 0, 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}
