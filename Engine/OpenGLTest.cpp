#include "glew\include\GL\glew.h"
#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "Application.h"
#include "Input.h"
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

UPDATE_STATUS OpenGLTest::Update(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		point[0] = App->input->GetMouseX();
		point[1] = App->input->GetMouseY();
	}

	return UPDATE_CONTINUE;
}

void OpenGLTest::DrawTriangle() const
{
	glVertexPointer(3, GL_FLOAT, 0, triangle_vertices);
	if(!wireframe)
		glDrawArrays(GL_TRIANGLES, 0, 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 3);
}

void OpenGLTest::DrawQuad() const
{
	glVertexPointer(3, GL_FLOAT, 0, quad_vertices);
	
	if (!wireframe)
		glDrawArrays(GL_QUADS, 0, 4);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 4);
}

void OpenGLTest::Draw2DPoint() const
{
	glVertexPointer(2, GL_FLOAT, 0, point);
	glDrawArrays(GL_POINTS, 0, 2);
}
