#include "imgui\imgui.h"
#include "Brofiler\Brofiler.h"
#include "Console.h"

Console::Console(const char* name, bool start_enabled): Module(name, start_enabled)
{}

Console::~Console()
{}

UPDATE_STATUS Console::Update(float dt)
{
	BROFILER_CATEGORY("Console Update", Profiler::Color::AliceBlue)

		if (console_active)
		{
			ImGui::Begin("Console", &console_active);
			for (std::deque<std::string>::const_iterator it = log_vec.begin(); it != log_vec.end(); ++it)
				ImGui::Text(it->c_str());
			ImGui::End();
		}

	return UPDATE_CONTINUE;
}

void Console::Log(const char* format, ...)
{
	static char tmp_string[4096];
	static char tmp_string2[4096];
	static va_list  ap;

	// Construct the string from variable arguments
	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);
	sprintf_s(tmp_string2, 4096, "\n %s", tmp_string);
	OutputDebugString(tmp_string2);

	if (log_vec.size() >= MAX_LOGS)
		log_vec.pop_back();

	log_vec.push_front(std::string(tmp_string2));
}

void Console::OpenCloseConsoleWindow()
{
	console_active = !console_active;
}
