#include "imgui\imgui.h"
#include "ModuleConsole.h"


ModuleConsole::ModuleConsole(Application * parent, bool start_enabled): Module(parent, "Console", start_enabled)
{}

ModuleConsole::~ModuleConsole()
{}

UPDATE_STATUS ModuleConsole::Update(float dt)
{
	BROFILER_CATEGORY("Console Update", Profiler::Color::AliceBlue)

	ImGui::Begin("Console");
	for (std::deque<std::string>::const_iterator it = log_vec.begin(); it != log_vec.end(); ++it)
		ImGui::Text(it->c_str());
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

	if (log_vec.size() >= MAX_LOGS)
		log_vec.pop_back();

	log_vec.push_front(std::string(tmp_string2));
}
