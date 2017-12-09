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

bool ImGui::Asset(const char * label, const ImVec2 image_size, bool selected, ImTextureID image_id, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiID id(window->GetID(label));

	const char* label_end = FindRenderedTextEnd(label);
	const ImVec2 label_size = CalcTextSize(label, label_end, false);

	ImVec2 total_size(image_size + ImVec2(6, 6));
	total_size.y += label_size.y;

	ImRect image_bb(window->DC.CursorPos + ImVec2(3, 3), window->DC.CursorPos + image_size + ImVec2(3, 3));

	if (label_size.x > image_size.x + 6)
	{
		total_size.x = label_size.x;
		image_bb.Min.x = window->DC.CursorPos.x + label_size.x / 2 - image_size.x / 2;
		image_bb.Max = image_bb.Min + image_size;
	}	

	const ImVec2 text_pos (window->DC.CursorPos.x, image_bb.Max.y + 3);

	ImRect bb(window->DC.CursorPos, window->DC.CursorPos + total_size);

	ImGuiTreeNodeFlags flags = NULL;

	if (selected)
		flags = ImGuiTreeNodeFlags_Selected;

	bool hovered, held, pressed = ButtonBehavior(bb, id, &hovered, &held, flags);

	const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);

	if (hovered || (flags & ImGuiTreeNodeFlags_Selected))
		RenderFrame(bb.Min, bb.Max, col, false);

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	
	ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 3);

	if (border_col.w > 0.0f)
	{
		window->DrawList->AddRect(bb.Min, bb.Max, GetColorU32(border_col), 0.0f);
		window->DrawList->AddImage(image_id, image_bb.Min + ImVec2(1, 1), image_bb.Max - ImVec2(1, 1), uv0, uv1, GetColorU32(tint_col));
	}
	else
	{
		window->DrawList->AddImage(image_id, image_bb.Min, image_bb.Max, uv0, uv1, GetColorU32(tint_col));
	}

	RenderTextClipped(text_pos, bb.Max, label, label_end, &label_size);


	ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 3);

	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
		return false;

	if (g.IO.MouseDoubleClicked[0] && IsMouseHoveringRect(bb.Min, bb.Max))
		return true;
}

/*
ImGuiWindow* window = GetCurrentWindow();
if (window->SkipItems)
return;

ImRect image_bb(window->DC.CursorPos, window->DC.CursorPos + size);

ItemSize(image_bb);
if (!ItemAdd(image_bb, NULL))
return;*/
//------

/*ImGuiWindow* window = GetCurrentWindow();
if (window->SkipItems)
return;*/
/*
ImGuiID id(window->GetID(label));

ImGuiContext& g = *GImGui;
const ImGuiStyle& style = g.Style;
const bool display_frame = (flags & ImGuiTreeNodeFlags_Framed) != 0;
const ImVec2 padding = display_frame ? style.FramePadding : ImVec2(style.FramePadding.x, 0.0f);

const char* label_end = FindRenderedTextEnd(label);
const ImVec2 label_size = CalcTextSize(label, label_end, false);

// We vertically grow up to current line height up the typical widget height.
const float text_base_offset_y = ImMax(0.0f, window->DC.CurrentLineTextBaseOffset - padding.y); // Latch before ItemSize changes it
const float frame_height = ImMax(ImMin(window->DC.CurrentLineHeight, g.FontSize + style.FramePadding.y * 2), size.y + label_size.y + padding.y * 2);
ImRect bb = ImRect(window->DC.CursorPos, ImVec2(window->Pos.x + GetContentRegionMax().x, window->DC.CursorPos.y + frame_height));
if (display_frame)
{
	// Framed header expand a little outside the default padding
	bb.Min.x -= (float)(int)(window->WindowPadding.x*0.5f) - 1;
	bb.Max.x += (float)(int)(window->WindowPadding.x*0.5f) - 1;
}

const float text_offset_x = (g.FontSize + (display_frame ? padding.x * 3 : padding.x * 2));   // Collapser arrow width + Spacing
const float text_width = g.FontSize + (label_size.x > 0.0f ? label_size.x + padding.x * 2 : 0.0f);   // Include collapser
ItemSize(ImVec2(text_width, frame_height), text_base_offset_y);

// For regular tree nodes, we arbitrary allow to click past 2 worth of ItemSpacing
// (Ideally we'd want to add a flag for the user to specify we want want the hit test to be done up to the right side of the content or not)
const ImRect interact_bb = display_frame ? bb : ImRect(bb.Min.x, bb.Min.y, bb.Min.x + text_width + style.ItemSpacing.x * 2, bb.Max.y);*/
/*bool is_open = TreeNodeBehaviorIsOpen(id, flags);
if (!ItemAdd(interact_bb, &id))
{
if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
TreePushRawID(id);
return is_open;
}*/

// Flags that affects opening behavior:
// - 0(default) ..................... single-click anywhere to open
// - OpenOnDoubleClick .............. double-click anywhere to open
// - OpenOnArrow .................... single-click on arrow to open
// - OpenOnDoubleClick|OpenOnArrow .. single-click on arrow or double-click anywhere to open
/*ImGuiButtonFlags button_flags = ImGuiButtonFlags_NoKeyModifiers | ((flags & ImGuiTreeNodeFlags_AllowOverlapMode) ? ImGuiButtonFlags_AllowOverlapMode : 0);
if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
button_flags |= ImGuiButtonFlags_PressedOnDoubleClick | ((flags & ImGuiTreeNodeFlags_OpenOnArrow) ? ImGuiButtonFlags_PressedOnClickRelease : 0);
bool hovered, held, pressed = ButtonBehavior(interact_bb, id, &hovered, &held, button_flags);*/
/*if (pressed && !(flags & ImGuiTreeNodeFlags_Leaf))
{
bool toggled = !(flags & (ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick));
if (flags & ImGuiTreeNodeFlags_OpenOnArrow)
toggled |= IsMouseHoveringRect(interact_bb.Min, ImVec2(interact_bb.Min.x + text_offset_x, interact_bb.Max.y));
if (flags & ImGuiTreeNodeFlags_OpenOnDoubleClick)
toggled |= g.IO.MouseDoubleClicked[0];
if (toggled)
{
is_open = !is_open;
window->DC.StateStorage->SetInt(id, is_open);
}
}*/
/*if (flags & ImGuiTreeNodeFlags_AllowOverlapMode)
SetItemAllowOverlap();

// Render
const ImU32 col = GetColorU32((held && hovered) ? ImGuiCol_HeaderActive : hovered ? ImGuiCol_HeaderHovered : ImGuiCol_Header);
//const ImVec2 text_pos = bb.Min + ImVec2(text_offset_x, padding.y + text_base_offset_y);
const ImVec2 text_pos = bb.Min + ImVec2(0, padding.y + text_base_offset_y + size.y); // no text offset
if (display_frame)
{
	// Framed type
	RenderFrame(bb.Min, bb.Max, col, true, style.FrameRounding);*/
	/*RenderCollapseTriangle(bb.Min + padding + ImVec2(0.0f, text_base_offset_y), is_open, 1.0f);
	if (g.LogEnabled)
	{
	// NB: '##' is normally used to hide text (as a library-wide feature), so we need to specify the text range to make sure the ## aren't stripped out here.
	const char log_prefix[] = "\n##";
	const char log_suffix[] = "##";
	LogRenderedText(text_pos, log_prefix, log_prefix + 3);
	RenderTextClipped(text_pos, bb.Max, label, label_end, &label_size);
	LogRenderedText(text_pos, log_suffix + 1, log_suffix + 3);
	}
	else
	{
	RenderTextClipped(text_pos, bb.Max, label, label_end, &label_size);
	}*/
/*
	RenderTextClipped(text_pos, bb.Max, label, label_end, &label_size);
}
else
{
	// Unframed typed for tree nodes
	if (hovered || (flags & ImGuiTreeNodeFlags_Selected))
		RenderFrame(bb.Min, bb.Max, col, false);*/

	/*if (flags & ImGuiTreeNodeFlags_Bullet)
	RenderBullet(bb.Min + ImVec2(text_offset_x * 0.5f, g.FontSize*0.50f + text_base_offset_y));
	else if (!(flags & ImGuiTreeNodeFlags_Leaf))
	RenderCollapseTriangle(bb.Min + ImVec2(padding.x, g.FontSize*0.15f + text_base_offset_y), is_open, 0.70f);
	if (g.LogEnabled)
	LogRenderedText(text_pos, ">");*/
	/*RenderText(text_pos, label, label_end, false);
}*/

/*if (is_open && !(flags & ImGuiTreeNodeFlags_NoTreePushOnOpen))
TreePushRawID(id);*/
//return;

//-----

/*ImGui::Button
ImGui::Image((void*)image, size);
ImGui::Text(name);*/