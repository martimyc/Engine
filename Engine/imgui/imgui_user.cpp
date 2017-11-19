#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_PLACEMENT_NEW
#include "imgui_internal.h"

#include <ctype.h>      // toupper, isprint
#include <stdlib.h>     // NULL, malloc, free, qsort, atoi
#include <stdio.h>      // vsnprintf, sscanf, printf
#include <limits.h>     // INT_MIN, INT_MAX
#if defined(_MSC_VER) && _MSC_VER <= 1500 // MSVC 2008 or earlier
#include <stddef.h>     // intptr_t
#else
#include <stdint.h>     // intptr_t
#endif

#include "imgui_user.h"

static void PushMultiItemsWidths(int components, float w_full = 0.0f);

static void PushMultiItemsWidths(int components, float w_full)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	const ImGuiStyle& style = GImGui->Style;
	if (w_full <= 0.0f)
		w_full = ImGui::CalcItemWidth();
	const float w_item_one = ImMax(1.0f, (float)(int)((w_full - (style.ItemInnerSpacing.x) * (components - 1)) / (float)components));
	const float w_item_last = ImMax(1.0f, (float)(int)(w_full - (w_item_one + style.ItemInnerSpacing.x) * (components - 1)));
	window->DC.ItemWidthStack.push_back(w_item_last);
	for (int i = 0; i < components - 1; i++)
		window->DC.ItemWidthStack.push_back(w_item_one);
	window->DC.ItemWidth = window->DC.ItemWidthStack.back();
}

void ImGui::HelpMarker(const char * desc)
{
	TextDisabled("(?)");
	if (IsItemHovered())
	{
		BeginTooltip();
		PushTextWrapPos(450.0f);
		TextUnformatted(desc);
		PopTextWrapPos();
		EndTooltip();
	}
}

IMGUI_API bool ImGui::SliderFloatNNoLabel(const char * label, float * v, int components, float v_min, float v_max, const char * display_format, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	BeginGroup();
	PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		PushID(i);
		value_changed |= SliderFloatNoLabel("##v", &v[i], v_min, v_max, display_format, power);
		SameLine(0, g.Style.ItemInnerSpacing.x);
		PopID();
		PopItemWidth();
	}
	PopID();

	TextUnformatted(label, FindRenderedTextEnd(label));
	EndGroup();

	return value_changed;
}

IMGUI_API bool ImGui::SliderIntNNoLabel(const char * label, int * v, int components, int v_min, int v_max, const char * display_format)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	bool value_changed = false;
	BeginGroup();
	PushID(label);
	PushMultiItemsWidths(components);
	for (int i = 0; i < components; i++)
	{
		PushID(i);
		value_changed |= SliderIntNoLabel("##v", &v[i], v_min, v_max, display_format);
		SameLine(0, g.Style.ItemInnerSpacing.x);
		PopID();
		PopItemWidth();
	}
	PopID();

	TextUnformatted(label, FindRenderedTextEnd(label));
	EndGroup();

	return value_changed;
}

bool ImGui::SliderFloatNoLabel(const char * label, float * v, float v_min, float v_max, const char * display_format, float power)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const float w = CalcItemWidth();

	const ImVec2 label_size = CalcTextSize(label, NULL, true);
	const ImRect frame_bb(window->DC.CursorPos, window->DC.CursorPos + ImVec2(w, label_size.y + style.FramePadding.y*2.0f));
	const ImRect total_bb(frame_bb.Min, frame_bb.Max + ImVec2(label_size.x > 0.0f ? style.ItemInnerSpacing.x + label_size.x : 0.0f, 0.0f));

	// NB- we don't call ItemSize() yet because we may turn into a text edit box below
	if (!ItemAdd(total_bb, &id))
	{
		ItemSize(total_bb, style.FramePadding.y);
		return false;
	}

	const bool hovered = IsHovered(frame_bb, id);
	if (hovered)
		SetHoveredID(id);

	if (!display_format)
		display_format = "%.3f";
	int decimal_precision = ParseFormatPrecision(display_format, 3);

	// Tabbing or CTRL-clicking on Slider turns it into an input box
	bool start_text_input = false;
	const bool tab_focus_requested = FocusableItemRegister(window, id);
	if (tab_focus_requested || (hovered && g.IO.MouseClicked[0]))
	{
		SetActiveID(id, window);
		FocusWindow(window);
		if (tab_focus_requested || g.IO.KeyCtrl)
		{
			start_text_input = true;
			g.ScalarAsInputTextId = 0;
		}
	}
	if (start_text_input || (g.ActiveId == id && g.ScalarAsInputTextId == id))
		return InputScalarAsWidgetReplacement(frame_bb, label, ImGuiDataType_Float, v, id, decimal_precision);

	// Actual slider behavior + render grab
	ItemSize(total_bb, style.FramePadding.y);
	const bool value_changed = SliderBehavior(frame_bb, id, v, v_min, v_max, power, decimal_precision);

	// Display value using user-provided display format so user can add prefix/suffix/decorations to the value.
	char value_buf[64];
	const char* value_buf_end = value_buf + ImFormatString(value_buf, IM_ARRAYSIZE(value_buf), display_format, *v);
	RenderTextClipped(frame_bb.Min, frame_bb.Max, value_buf, value_buf_end, NULL, ImVec2(0.5f, 0.5f));

	return value_changed;
}

IMGUI_API bool ImGui::SliderFloat2NoLabel(const char * label, float v[2], float v_min, float v_max, const char * display_format, float power)
{
	return SliderFloatNNoLabel(label, v, 2, v_min, v_max, display_format, power);
}

IMGUI_API bool ImGui::SliderFloat3NoLabel(const char * label, float v[3], float v_min, float v_max, const char * display_format, float power)
{
	return SliderFloatNNoLabel(label, v, 3, v_min, v_max, display_format, power);
}

IMGUI_API bool ImGui::SliderFloat4NoLabel(const char * label, float v[4], float v_min, float v_max, const char * display_format, float power)
{
	return SliderFloatNNoLabel(label, v, 4, v_min, v_max, display_format, power);
}

IMGUI_API bool ImGui::SliderAngleNoLabel(const char * label, float * v_rad, float v_degrees_min, float v_degrees_max)
{
	float v_deg = (*v_rad) * 360.0f / (2 * IM_PI);
	bool value_changed = SliderFloatNoLabel(label, &v_deg, v_degrees_min, v_degrees_max, "%.0f deg", 1.0f);
	*v_rad = v_deg * (2 * IM_PI) / 360.0f;
	return value_changed;
}

IMGUI_API bool ImGui::SliderIntNoLabel(const char * label, int * v, int v_min, int v_max, const char * display_format)
{
	if (!display_format)
		display_format = "%.0f";
	float v_f = (float)*v;
	bool value_changed = SliderFloatNoLabel(label, &v_f, (float)v_min, (float)v_max, display_format, 1.0f);
	*v = (int)v_f;
	return value_changed;
}

IMGUI_API bool ImGui::SliderInt2NoLabel(const char * label, int v[2], int v_min, int v_max, const char * display_format)
{
	return SliderIntNNoLabel(label, v, 2, v_min, v_max, display_format);
}

IMGUI_API bool ImGui::SliderInt3NoLabel(const char * label, int v[3], int v_min, int v_max, const char * display_format)
{
	return SliderIntNNoLabel(label, v, 3, v_min, v_max, display_format);
}

IMGUI_API bool ImGui::SliderInt4NoLabel(const char * label, int v[4], int v_min, int v_max, const char * display_format)
{
	return SliderIntNNoLabel(label, v, 4, v_min, v_max, display_format);
}