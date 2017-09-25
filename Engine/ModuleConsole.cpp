#include "imgui\imgui.h"
#include "ModuleConsole.h"


ModuleConsole::ModuleConsole(Application * parent, bool start_enabled): Module(parent, start_enabled)
{}

ModuleConsole::~ModuleConsole()
{}

UPDATE_STATUS ModuleConsole::Update(float dt)
{
	ImGui::Begin("Console");
	for (std::vector<std::string>::const_reverse_iterator it = log_vec.rbegin(); it != log_vec.rend(); ++it)
	{
		ImGui::Text(it->c_str());
	}
	ImGui::End();
	return UPDATE_CONTINUE;
}

void ModuleConsole::Log(const char file[], int line, const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n%s(%d) : %s", file, line, tmp_string);
	OutputDebugString(tmp_string2);

	log_vec.push_back(std::string(tmp_string2));
}
