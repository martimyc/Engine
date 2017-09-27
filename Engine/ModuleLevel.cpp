#include "glew-2.1.0\include\GL\glew.h"
#include "imgui\imgui.h"
#include "ModuleLevel.h"


ModuleLevel::ModuleLevel(Application * app, bool start_enabled): Module(app, "Level", start_enabled)
{}

ModuleLevel::~ModuleLevel()
{}

UPDATE_STATUS ModuleLevel::Configuration(float dt)
{
	BROFILER_CATEGORY("Hardware Configuration", Profiler::Color::BlanchedAlmond)

		UPDATE_STATUS ret = UPDATE_CONTINUE;

	if (ImGui::CollapsingHeader("Triangle"))
	{
		if (ImGui::Button("Wireframe"))
			wireframe = !wireframe;
	}
	return ret;
}

bool ModuleLevel::Init()
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

void ModuleLevel::DrawTriangle()
{
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, vertices);
	if(!wireframe)
		glDrawArrays(GL_TRIANGLES, 0, 3);
	else
		glDrawArrays(GL_LINE_LOOP, 0, 3);
	glDisableClientState(GL_VERTEX_ARRAY);
}
