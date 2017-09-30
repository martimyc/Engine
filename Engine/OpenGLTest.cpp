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
		ImGui::SliderInt("Circle sides", &circle_sides, 3, 100, "%i");
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

void OpenGLTest::DrawTriangleFan() const
{
	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, poly_draw_mode);
	glVertexPointer(3, GL_FLOAT, 0, polygon);
	glDrawArrays(GL_TRIANGLE_FAN, 0, 5);
	glPopAttrib();
}

void OpenGLTest::DrawCircle(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat radius, const GLint number_of_sides) const
{
	GLint number_of_vertices = number_of_sides + 2;
	GLfloat double_pi = pi * 2.0f;

	GLfloat* circle_vertices_x = new GLfloat[number_of_vertices];
	GLfloat* circle_vertices_y = new GLfloat[number_of_vertices];
	GLfloat* circle_vertices_z = new GLfloat[number_of_vertices];

	circle_vertices_x[0] = x;
	circle_vertices_y[0] = y;
	circle_vertices_z[0] = z;

	for (int i = 1; i < number_of_vertices; i++)
	{
		circle_vertices_x[i] = x + (radius * cos(i * double_pi / number_of_sides));
		circle_vertices_y[i] = y + (radius * sin(i * double_pi / number_of_sides));
		circle_vertices_z[i] = z;
	}

	GLfloat* all_circle_vertices = new GLfloat[number_of_vertices * 3];

	for (int i = 0; i < number_of_vertices; i++)
	{
		all_circle_vertices[i * 3] = circle_vertices_x[i];
		all_circle_vertices[(i * 3) + 1] = circle_vertices_y[i];
		all_circle_vertices[(i * 3) + 2] = circle_vertices_z[i];
	}

	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, poly_draw_mode);
	glVertexPointer(3, GL_FLOAT, 0, all_circle_vertices);
	glDrawArrays(GL_TRIANGLE_FAN, 0, number_of_vertices);
	glPopAttrib();

	delete[] circle_vertices_x;
	delete[] circle_vertices_y;
	delete[] circle_vertices_z;
	delete[] all_circle_vertices;
}

void OpenGLTest::DrawHollowCircle(const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat radius, const GLint number_of_sides) const
{
	GLint number_of_vertices = number_of_sides + 1;
	GLfloat double_pi = pi * 2.0f;

	GLfloat* circle_vertices_x = new GLfloat[number_of_vertices];
	GLfloat* circle_vertices_y = new GLfloat[number_of_vertices];
	GLfloat* circle_vertices_z = new GLfloat[number_of_vertices];

	for (int i = 0; i < number_of_vertices; i++)
	{
		circle_vertices_x[i] = x + (radius * cos(i * double_pi / number_of_sides));
		circle_vertices_y[i] = y + (radius * sin(i * double_pi / number_of_sides));
		circle_vertices_z[i] = z;
	}

	GLfloat* all_circle_vertices = new GLfloat[number_of_vertices * 3];

	for (int i = 0; i < number_of_vertices; i++)
	{
		all_circle_vertices[i * 3] = circle_vertices_x[i];
		all_circle_vertices[(i * 3) + 1] = circle_vertices_y[i];
		all_circle_vertices[(i * 3) + 2] = circle_vertices_z[i];
	}

	glPushAttrib(GL_POLYGON_BIT);
	glPolygonMode(GL_FRONT_AND_BACK, poly_draw_mode);
	glVertexPointer(3, GL_FLOAT, 0, all_circle_vertices);
	glDrawArrays(GL_LINE_STRIP, 0, number_of_vertices);
	glPopAttrib();

	delete[] circle_vertices_x;
	delete[] circle_vertices_y;
	delete[] circle_vertices_z;
	delete[] all_circle_vertices;
}
