#ifndef IMGUI_USER
#define IMGUI_USER

#include "imgui.h"

namespace ImGui
{
	void					HelpMarker(const char* desc);

	IMGUI_API bool          SliderFloatNNoLabel(const char* label, float* v, int components, float v_min, float v_max, const char* display_format, float power);
	IMGUI_API bool          SliderIntNNoLabel(const char* label, int* v, int components, int v_min, int v_max, const char* display_format);

	IMGUI_API bool          SliderFloatNoLabel(const char* label, float* v, float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);     // adjust display_format to decorate the value with a prefix or a suffix for in-slider labels or unit display. Use power!=1.0 for logarithmic sliders
	IMGUI_API bool          SliderFloat2NoLabel(const char* label, float v[2], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderFloat3NoLabel(const char* label, float v[3], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderFloat4NoLabel(const char* label, float v[4], float v_min, float v_max, const char* display_format = "%.3f", float power = 1.0f);
	IMGUI_API bool          SliderAngleNoLabel(const char* label, float* v_rad, float v_degrees_min = -360.0f, float v_degrees_max = +360.0f);
	IMGUI_API bool          SliderIntNoLabel(const char* label, int* v, int v_min, int v_max, const char* display_format = "%.0f");
	IMGUI_API bool          SliderInt2NoLabel(const char* label, int v[2], int v_min, int v_max, const char* display_format = "%.0f");
	IMGUI_API bool          SliderInt3NoLabel(const char* label, int v[3], int v_min, int v_max, const char* display_format = "%.0f");
	IMGUI_API bool          SliderInt4NoLabel(const char* label, int v[4], int v_min, int v_max, const char* display_format = "%.0f");

	IMGUI_API bool 			Asset(const char * label, const ImVec2 size, bool selected, ImTextureID image_id, const ImVec2& uv0 = ImVec2(0, 1), const ImVec2& uv1 = ImVec2(1, 0), const ImVec4& tint_col = ImVec4(1, 1, 1, 1), const ImVec4& border_col = ImVec4(0, 0, 0, 0));
}

#endif //!IMGUI_USER
