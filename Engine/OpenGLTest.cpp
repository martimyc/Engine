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
	/*if (App->input->GetMouseButton(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		point[0] = App->input->GetMouseX();
		point[1] = App->window->GetHeight() - App->input->GetMouseY();
	}*/

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

void OpenGLTest::Draw2DPoint(const float x, const float y) const
{
	GLfloat point[2] = { x, y };

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

void OpenGLTest::DrawCubeDirectMode() const
{
	glBegin(GL_TRIANGLES);
	// face 1
	glVertex3f(triangle_cube[0], triangle_cube[1], triangle_cube[2]);
	glVertex3f(triangle_cube[3], triangle_cube[4], triangle_cube[5]);
	glVertex3f(triangle_cube[6], triangle_cube[7], triangle_cube[8]);
	glVertex3f(triangle_cube[9], triangle_cube[10], triangle_cube[11]);
	glVertex3f(triangle_cube[12], triangle_cube[13], triangle_cube[14]);
	glVertex3f(triangle_cube[15], triangle_cube[16], triangle_cube[17]);
	// face 2
	glVertex3f(triangle_cube[18], triangle_cube[19], triangle_cube[20]);
	glVertex3f(triangle_cube[21], triangle_cube[22], triangle_cube[23]);
	glVertex3f(triangle_cube[24], triangle_cube[25], triangle_cube[26]);
	glVertex3f(triangle_cube[27], triangle_cube[28], triangle_cube[29]);
	glVertex3f(triangle_cube[30], triangle_cube[31], triangle_cube[32]);
	glVertex3f(triangle_cube[33], triangle_cube[34], triangle_cube[35]);
	// face 3
	glVertex3f(triangle_cube[36], triangle_cube[37], triangle_cube[38]);
	glVertex3f(triangle_cube[39], triangle_cube[40], triangle_cube[41]);
	glVertex3f(triangle_cube[42], triangle_cube[43], triangle_cube[44]);
	glVertex3f(triangle_cube[45], triangle_cube[46], triangle_cube[47]);
	glVertex3f(triangle_cube[48], triangle_cube[49], triangle_cube[50]);
	glVertex3f(triangle_cube[51], triangle_cube[52], triangle_cube[53]);
	// face 4
	glVertex3f(triangle_cube[54], triangle_cube[55], triangle_cube[56]);
	glVertex3f(triangle_cube[57], triangle_cube[58], triangle_cube[59]);
	glVertex3f(triangle_cube[60], triangle_cube[61], triangle_cube[62]);
	glVertex3f(triangle_cube[63], triangle_cube[64], triangle_cube[65]);
	glVertex3f(triangle_cube[66], triangle_cube[67], triangle_cube[68]);
	glVertex3f(triangle_cube[69], triangle_cube[70], triangle_cube[71]);
	// face 5
	glVertex3f(triangle_cube[72], triangle_cube[73], triangle_cube[74]);
	glVertex3f(triangle_cube[75], triangle_cube[76], triangle_cube[77]);
	glVertex3f(triangle_cube[78], triangle_cube[79], triangle_cube[80]);
	glVertex3f(triangle_cube[81], triangle_cube[82], triangle_cube[83]);
	glVertex3f(triangle_cube[84], triangle_cube[85], triangle_cube[86]);
	glVertex3f(triangle_cube[87], triangle_cube[88], triangle_cube[89]);
	// face 6
	glVertex3f(triangle_cube[90], triangle_cube[91], triangle_cube[92]);
	glVertex3f(triangle_cube[93], triangle_cube[94], triangle_cube[95]);
	glVertex3f(triangle_cube[96], triangle_cube[97], triangle_cube[98]);
	glVertex3f(triangle_cube[99], triangle_cube[100], triangle_cube[101]);
	glVertex3f(triangle_cube[102], triangle_cube[103], triangle_cube[104]);
	glVertex3f(triangle_cube[105], triangle_cube[106], triangle_cube[107]);

	glEnd();
}
