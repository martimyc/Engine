#include "glew\include\GL\glew.h"
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

UPDATE_STATUS OpenGLTest::Configuration(float dt)
{
	BROFILER_CATEGORY("Open GL Test Configuration", Profiler::Color::BlanchedAlmond)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("OpenGL Test"))
	{
		if (ImGui::Button("Wireframe"))
			wireframe = !wireframe;
		ImGui::Text("Pollygon mode:");
		if (ImGui::Button("Fill"))
			poly_draw_mode = GL_FILL;
		ImGui::SameLine();
		if (ImGui::Button("Line"))
			poly_draw_mode = GL_LINE;
		ImGui::SameLine();
		if (ImGui::Button("Point"))
			poly_draw_mode = GL_POINT;
	}
	return ret;
}

UPDATE_STATUS OpenGLTest::Update(float dt)
{
	if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		point[0] = App->input->GetMouseX();
		point[1] = App->window->GetHeight() - App->input->GetMouseY();
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
	glEnable(GL_POINT_SMOOTH);
	glVertexPointer(2, GL_FLOAT, 0, point);
	glPointSize(10);
	glDrawArrays(GL_POINTS, 0, 1);
	glDisable(GL_POINT_SMOOTH);
}

void OpenGLTest::DrawLine() const
{
	glEnable(GL_LINE_SMOOTH);
	glVertexPointer(3, GL_FLOAT, 0, line);
	glDrawArrays(GL_LINES, 0, 2);
	glDisable(GL_LINE_SMOOTH);
}

void OpenGLTest::DrawPolygon() const
{	
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, poly_draw_mode);
	glVertexPointer(3, GL_FLOAT, 0, polygon);
	glDrawArrays(GL_POLYGON, 0, 5);
	glPopAttrib();
}

void OpenGLTest::DrawTriangleStrip() const
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, poly_draw_mode);
	glVertexPointer(3, GL_FLOAT, 0, triangle_strip);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 5);
	glPopAttrib();
}
