#include <string>
#include "..\Parson\parson.h"
#include "imgui.h"

#define IMGUI_DEFINE_PLACEMENT_NEW
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui_internal.h"
#include "imgui_dock.h"

namespace ImGui
{
	static DockContext g_dock;

	DockContext::DockContext()
	{
	}

	DockContext::~DockContext()
	{
	}

	void DockContext::ShutdownDock()
	{
		for (int i = 0; i < g_dock.m_docks.size(); ++i)
		{
			g_dock.m_docks[i]->~Dock();
			MemFree(g_dock.m_docks[i]);
		}
		g_dock.m_docks.clear();
	}


	void DockContext::TabbarInContext(Dock* dock_to_tab)
	{
		Dock* dest_dock = nullptr;
		for (int i = 0; i < m_docks.size(); ++i)
		{
			Dock& dock = *m_docks[i];
			if (dock.isContainer()) continue;
			if (dock.status != Status_Docked) continue;
			{
				dest_dock = &dock;
			}
		}
		doDock(*dock_to_tab, dest_dock, Slot_Tab);
	}

	int DockContext::GetDockPos(const Dock * dock) const
	{
		if (dock == nullptr)
			return -1;

		for (int i = 0; i < g_dock.m_docks.size(); i++)
			if (dock == g_dock.m_docks[i])
				return i;

		return -1;
	}

	DockContext::Dock* DockContext::GetDockbyPos(int position) const
	{
		if (position < m_docks.size() && position >= 0)
			return m_docks[position];

		return nullptr;
	}

	void DockContext::RootDock(const ImVec2& pos, const ImVec2& size)
	{
		g_dock.rootDock(pos, size);
	}


	void DockContext::SetDockActive()
	{
		g_dock.setDockActive();
	}


	bool DockContext::BeginDock(const char* label, bool* opened, ImGuiWindowFlags extra_flags, const ImVec2& default_size)
	{
		return g_dock.begin(label, opened, extra_flags, default_size);
	}


	void DockContext::EndDock()
	{
		g_dock.end();
	}

	void DockContext::SetWorkspacePosSize(ImVec2 pos_, ImVec2 size_, float width_difference, float height_difference)
	{
		if (g_dock.workspace_pos != pos_ || g_dock.workspace_size != size_)
		{
			g_dock.ShutdownDock();
			LoadDocks();
			for (int i = 0; i < g_dock.m_docks.size(); i++)
			{
				g_dock.m_docks[i]->pos.x *= width_difference;
				g_dock.m_docks[i]->pos.y *= height_difference;
				g_dock.m_docks[i]->size.x *= width_difference;
				g_dock.m_docks[i]->size.y *= height_difference;
			}
			g_dock.workspace_pos = pos_;
			g_dock.workspace_size = size_;
			SaveDocks();
		}
	}

	bool DockContext::GetDockSize(const char * str, float & x, float & y, float & w, float & h) const
	{
		for (int i = 0; i < g_dock.m_docks.size(); i++)
		{
			std::string name(g_dock.m_docks[i]->label);
			if (name == str)
			{
				x = g_dock.m_docks[i]->pos.x;
				y = g_dock.m_docks[i]->pos.y;
				w = g_dock.m_docks[i]->size.x;
				h = g_dock.m_docks[i]->size.y;
				return true;
			}
		}
		return false;
	}

	void DockContext::LoadDocks()
	{
		const JSON_Value* dock_configuration = json_parse_file("docks.json");
		const JSON_Object* root_object = json_value_get_object(dock_configuration);
		JSON_Object* docks_start = json_object_dotget_object(root_object, "Docking Config");

		int num_docks = json_object_get_number(docks_start, "docks_num");
		JSON_Object* dock_object = nullptr;
		
		for (int i = 0; i < num_docks; i++)
		{
			char dock_name[10];
			sprintf(dock_name, "dock_%i", i);
			dock_object = json_object_dotget_object(docks_start, dock_name);

			Dock* new_dock = new Dock();
			new_dock = (Dock *)MemAlloc(sizeof(Dock));
			new_dock->Load(dock_object);

			g_dock.m_docks.push_back(new_dock);
		}

		for (int i = 0; i < num_docks; i++)
		{
			char dock_name[10];
			sprintf(dock_name, "dock_%i", i);
			dock_object = json_object_dotget_object(docks_start, dock_name);
			g_dock.m_docks[i]->LoadHierarchy(dock_object);
		}
	}
	void DockContext::SaveDocks()
	{
		const JSON_Value* docks_configuration = json_parse_file("docks.json");
		const JSON_Object* root_object = json_value_get_object(docks_configuration);

		JSON_Object* docks_start = json_object_dotget_object(root_object, "Docking Config");

		for (int i = 0; i < g_dock.m_docks.size(); ++i)
		{
			char dock_name[10];
			sprintf(dock_name, "dock_%i", i);

			json_object_set_value(docks_start, dock_name, json_value_init_object());

			JSON_Object* dock_object = json_object_dotget_object(docks_start, dock_name);
			g_dock.m_docks[i]->Save(dock_object);
		}

		json_object_dotset_number(docks_start, "docks_num", g_dock.m_docks.size());

		json_serialize_to_file(docks_configuration, "docks.json");
		json_value_free((JSON_Value *)docks_configuration);
	}

	DockContext::Dock::Dock() : id(0)
		, next_tab(nullptr)
		, prev_tab(nullptr)
		, parent(nullptr)
		, pos(0, 0)
		, size(-1, -1)
		, active(true)
		, status(Status_Float)
		, label(nullptr)
		, opened(false)
		, first(true)
		, last_frame(0)
	{
		location[0] = 0;
		children[0] = children[1] = nullptr;
	}

	DockContext::Dock::~Dock()
	{
		MemFree(label);
	}

	ImVec2 DockContext::Dock::getMinSize() const
	{
		{
			if (!children[0]) return ImVec2(16, 16 + GetTextLineHeightWithSpacing());

			ImVec2 s0 = children[0]->getMinSize();
			ImVec2 s1 = children[1]->getMinSize();
			return isHorizontal() ? ImVec2(s0.x + s1.x, ImMax(s0.y, s1.y))
				: ImVec2(ImMax(s0.x, s1.x), s0.y + s1.y);
		}
	}

	bool DockContext::Dock::isHorizontal() const
	{
		return children[0]->pos.x < children[1]->pos.x;
	}

	void DockContext::Dock::setParent(Dock * dock)
	{
		parent = dock;
		for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab) tmp->parent = dock;
		for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab) tmp->parent = dock;
	}

	DockContext::Dock & DockContext::Dock::getSibling()
	{
		IM_ASSERT(parent);
		if (parent->children[0] == &getFirstTab()) return *parent->children[1];
		return *parent->children[0];
	}

	DockContext::Dock & DockContext::Dock::getFirstTab()
	{
		Dock* tmp = this;
		while (tmp->prev_tab) tmp = tmp->prev_tab;
		return *tmp;
	}

	void DockContext::Dock::setActive()
	{
		active = true;
		for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab) tmp->active = false;
		for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab) tmp->active = false;
	}

	bool DockContext::Dock::isContainer() const
	{
		return children[0] != nullptr;
	}

	void DockContext::Dock::setChildrenPosSize(const ImVec2 & _pos, const ImVec2 & _size)
	{
		ImVec2 s = children[0]->size;
		if (isHorizontal())
		{
			s.y = _size.y;
			s.x = (float)int(
				_size.x * children[0]->size.x / (children[0]->size.x + children[1]->size.x));
			if (s.x < children[0]->getMinSize().x)
			{
				s.x = children[0]->getMinSize().x;
			}
			else if (_size.x - s.x < children[1]->getMinSize().x)
			{
				s.x = _size.x - children[1]->getMinSize().x;
			}
			children[0]->setPosSize(_pos, s);

			s.x = _size.x - children[0]->size.x;
			ImVec2 p = _pos;
			p.x += children[0]->size.x;
			children[1]->setPosSize(p, s);
		}
		else
		{
			s.x = _size.x;
			s.y = (float)int(
				_size.y * children[0]->size.y / (children[0]->size.y + children[1]->size.y));
			if (s.y < children[0]->getMinSize().y)
			{
				s.y = children[0]->getMinSize().y;
			}
			else if (_size.y - s.y < children[1]->getMinSize().y)
			{
				s.y = _size.y - children[1]->getMinSize().y;
			}
			children[0]->setPosSize(_pos, s);

			s.y = _size.y - children[0]->size.y;
			ImVec2 p = _pos;
			p.y += children[0]->size.y;
			children[1]->setPosSize(p, s);
		}
	}

	void DockContext::Dock::setPosSize(const ImVec2 & _pos, const ImVec2 & _size)
	{
		size = _size;
		pos = _pos;
		for (Dock* tmp = prev_tab; tmp; tmp = tmp->prev_tab)
		{
			tmp->size = _size;
			tmp->pos = _pos;
		}
		for (Dock* tmp = next_tab; tmp; tmp = tmp->next_tab)
		{
			tmp->size = _size;
			tmp->pos = _pos;
		}

		if (!isContainer()) return;
		setChildrenPosSize(_pos, _size);
	}

	void DockContext::Dock::Save(JSON_Object * dock_object) const
	{
		json_object_dotset_string(dock_object, "label", label);
		json_object_dotset_number(dock_object, "id", id);
		json_object_dotset_number(dock_object, "next tab", g_dock.GetDockPos(next_tab));
		json_object_dotset_number(dock_object, "prev tab", g_dock.GetDockPos(prev_tab));
		json_object_dotset_number(dock_object, "first child", g_dock.GetDockPos(children[0]));
		json_object_dotset_number(dock_object, "second child", g_dock.GetDockPos(children[1]));
		json_object_dotset_number(dock_object, "parent", g_dock.GetDockPos(parent));
		json_object_dotset_boolean(dock_object, "active", active);
		json_object_dotset_number(dock_object, "position x", pos.x);
		json_object_dotset_number(dock_object, "position y", pos.y);
		json_object_dotset_number(dock_object, "size x", size.x);
		json_object_dotset_number(dock_object, "size y", size.y);
		json_object_dotset_number(dock_object, "status", status);
		json_object_dotset_string(dock_object, "location", location);
		json_object_dotset_boolean(dock_object, "opended", opened);
		json_object_dotset_boolean(dock_object, "first", first);
		json_object_dotset_number(dock_object, "last frame", last_frame);
	}

	void DockContext::Dock::Load(const JSON_Object * dock_object)
	{
		label = ImStrdup(json_object_get_string(dock_object, "label"));
		id = json_object_get_number(dock_object, "id");
		active = json_object_get_boolean(dock_object, "active");
		pos.x = json_object_get_number(dock_object, "position x");
		pos.y = json_object_get_number(dock_object, "position y");
		size.x = json_object_get_number(dock_object, "size x");
		size.y = json_object_get_number(dock_object, "size y");
		status = (ImGui::Status_)(int)json_object_get_number(dock_object, "status");
		sprintf(location, json_object_get_string(dock_object, "location"));
		opened = json_object_get_boolean(dock_object, "opended");
		first = json_object_get_boolean(dock_object, "first");
		last_frame = json_object_get_number(dock_object, "last frame");
	}

	void DockContext::Dock::LoadHierarchy(const JSON_Object * dock_object)
	{
		next_tab = g_dock.GetDockbyPos(json_object_get_number(dock_object, "next tab"));
		prev_tab = g_dock.GetDockbyPos(json_object_get_number(dock_object, "prev tab"));
		children[0] = g_dock.GetDockbyPos(json_object_get_number(dock_object, "first child"));
		children[1] = g_dock.GetDockbyPos(json_object_get_number(dock_object, "second child"));
		parent = g_dock.GetDockbyPos(json_object_get_number(dock_object, "parent"));
	}

	DockContext::Dock & DockContext::getDock(const char * label, bool opened, const ImVec2 & default_size)
	{
		ImU32 id = ImHash(label, 0);
		for (int i = 0; i < m_docks.size(); ++i)
		{
			if (m_docks[i]->id == id) return *m_docks[i];
		}

		Dock* new_dock = (Dock*)MemAlloc(sizeof(Dock));
		IM_PLACEMENT_NEW(new_dock) Dock();
		m_docks.push_back(new_dock);
		new_dock->label = ImStrdup(label);
		IM_ASSERT(new_dock->label);
		new_dock->id = id;
		new_dock->setActive();
		new_dock->status = Status_Float;
		new_dock->pos = g_dock.workspace_pos;
		new_dock->size.x = g_dock.workspace_size.x;
		new_dock->size.y = g_dock.workspace_size.y;
		new_dock->opened = opened;
		new_dock->first = true;
		new_dock->location[0] = 0;
		TabbarInContext(new_dock);
		
		return *new_dock;
	}

	void DockContext::putInBackground()
	{
		ImGuiWindow* win = GetCurrentWindow();
		ImGuiContext& g = *GImGui;
		if (g.Windows[0] == win) return;

		for (int i = 0; i < g.Windows.Size; i++)
		{
			if (g.Windows[i] == win)
			{
				for (int j = i - 1; j >= 0; --j)
				{
					g.Windows[j + 1] = g.Windows[j];
				}
				g.Windows[0] = win;
				break;
			}
		}
	}

	void DockContext::splits()
	{
		if (GetFrameCount() == m_last_frame) return;
		m_last_frame = GetFrameCount();

		putInBackground();

		ImU32 color = GetColorU32(ImGuiCol_Button);
		ImU32 color_hovered = GetColorU32(ImGuiCol_ButtonHovered);
		ImDrawList* draw_list = GetWindowDrawList();
		ImGuiIO& io = GetIO();
		for (int i = 0; i < m_docks.size(); ++i)
		{
			Dock& dock = *m_docks[i];
			if (!dock.isContainer()) continue;

			PushID(i);
			if (!IsMouseDown(0)) dock.status = Status_Docked;

			ImVec2 size = dock.children[0]->size;
			ImVec2 dsize(0, 0);
			SetCursorScreenPos(dock.children[1]->pos);
			ImVec2 min_size0 = dock.children[0]->getMinSize();
			ImVec2 min_size1 = dock.children[1]->getMinSize();
			if (dock.isHorizontal())
			{
				InvisibleButton("split", ImVec2(3, dock.size.y));
				if (dock.status == Status_Dragged) dsize.x = io.MouseDelta.x;
				dsize.x = -ImMin(-dsize.x, dock.children[0]->size.x - min_size0.x);
				dsize.x = ImMin(dsize.x, dock.children[1]->size.x - min_size1.x);
			}
			else
			{
				InvisibleButton("split", ImVec2(dock.size.x, 3));
				if (dock.status == Status_Dragged) dsize.y = io.MouseDelta.y;
				dsize.y = -ImMin(-dsize.y, dock.children[0]->size.y - min_size0.y);
				dsize.y = ImMin(dsize.y, dock.children[1]->size.y - min_size1.y);
			}
			ImVec2 new_size0 = dock.children[0]->size + dsize;
			ImVec2 new_size1 = dock.children[1]->size - dsize;
			ImVec2 new_pos1 = dock.children[1]->pos + dsize;
			dock.children[0]->setPosSize(dock.children[0]->pos, new_size0);
			dock.children[1]->setPosSize(new_pos1, new_size1);

			if (IsItemHovered() && IsMouseClicked(0))
			{
				dock.status = Status_Dragged;
			}

			draw_list->AddRectFilled(
				GetItemRectMin(), GetItemRectMax(), IsItemHovered() ? color_hovered : color);
			PopID();
		}
	}

	void DockContext::beginPanel()
	{
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_ShowBorders |
			ImGuiWindowFlags_NoBringToFrontOnFocus;
		Dock* root = getRootDock();
		if (root)
		{
			SetNextWindowPos(root->pos);
			SetNextWindowSize(root->size);
		}
		else
		{
			SetNextWindowPos(g_dock.workspace_pos);
			SetNextWindowSize(g_dock.workspace_size);
		}
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0);
		Begin("###DockPanel", nullptr, flags);
		splits();
	}

	void DockContext::endPanel()
	{
		End();
		ImGui::PopStyleVar();
	}

	DockContext::Dock * DockContext::getDockAt(const ImVec2 & pos) const
	{
		for (int i = 0; i < m_docks.size(); ++i)
		{
			Dock& dock = *m_docks[i];
			if (dock.isContainer()) continue;
			if (dock.status != Status_Docked) continue;
			if (IsMouseHoveringRect(dock.pos, dock.pos + dock.size, false))
			{
				return &dock;
			}
		}

		return nullptr;
	}

	ImRect DockContext::getDockedRect(const ImRect & rect, Slot_ dock_slot)
	{
		ImVec2 half_size = rect.GetSize() * 0.5f;
		switch (dock_slot)
		{
		default: return rect;
		case Slot_Top: return ImRect(rect.Min, rect.Min + ImVec2(rect.Max.x, half_size.y));
		case Slot_Right: return ImRect(rect.Min + ImVec2(half_size.x, 0), rect.Max);
		case Slot_Bottom: return ImRect(rect.Min + ImVec2(0, half_size.y), rect.Max);
		case Slot_Left: return ImRect(rect.Min, rect.Min + ImVec2(half_size.x, rect.Max.y));
		}
	}

	ImRect DockContext::getSlotRect(ImRect parent_rect, Slot_ dock_slot)
	{
		ImVec2 size = parent_rect.Max - parent_rect.Min;
		ImVec2 center = parent_rect.Min + size * 0.5f;
		switch (dock_slot)
		{
		default: return ImRect(center - ImVec2(20, 20), center + ImVec2(20, 20));
		case Slot_Top: return ImRect(center + ImVec2(-20, -50), center + ImVec2(20, -30));
		case Slot_Right: return ImRect(center + ImVec2(30, -20), center + ImVec2(50, 20));
		case Slot_Bottom: return ImRect(center + ImVec2(-20, +30), center + ImVec2(20, 50));
		case Slot_Left: return ImRect(center + ImVec2(-50, -20), center + ImVec2(-30, 20));
		}
	}

	ImRect DockContext::getSlotRectOnBorder(ImRect parent_rect, Slot_ dock_slot)
	{
		ImVec2 size = parent_rect.Max - parent_rect.Min;
		ImVec2 center = parent_rect.Min + size * 0.5f;
		switch (dock_slot)
		{
		case Slot_Top:
			return ImRect(ImVec2(center.x - 20, parent_rect.Min.y + 10),
				ImVec2(center.x + 20, parent_rect.Min.y + 30));
		case Slot_Left:
			return ImRect(ImVec2(parent_rect.Min.x + 10, center.y - 20),
				ImVec2(parent_rect.Min.x + 30, center.y + 20));
		case Slot_Bottom:
			return ImRect(ImVec2(center.x - 20, parent_rect.Max.y - 30),
				ImVec2(center.x + 20, parent_rect.Max.y - 10));
		case Slot_Right:
			return ImRect(ImVec2(parent_rect.Max.x - 30, center.y - 20),
				ImVec2(parent_rect.Max.x - 10, center.y + 20));
		default: IM_ASSERT(false);
		}
		IM_ASSERT(false);
		return ImRect();
	}

	DockContext::Dock * DockContext::getRootDock()
	{
		for (int i = 0; i < m_docks.size(); ++i)
		{
			if (!m_docks[i]->parent &&
				(m_docks[i]->status == Status_Docked || m_docks[i]->children[0]))
			{
				return m_docks[i];
			}
		}
		return nullptr;
	}

	bool DockContext::dockSlots(Dock & dock, Dock * dest_dock, const ImRect & rect, bool on_border)
	{
		ImDrawList* canvas = GetWindowDrawList();
		ImU32 color = GetColorU32(ImGuiCol_Button);
		ImU32 color_hovered = GetColorU32(ImGuiCol_ButtonHovered);
		ImVec2 mouse_pos = GetIO().MousePos;
		for (int i = 0; i < (on_border ? 4 : 5); ++i)
		{
			ImRect r =
				on_border ? getSlotRectOnBorder(rect, (Slot_)i) : getSlotRect(rect, (Slot_)i);
			bool hovered = r.Contains(mouse_pos);
			canvas->AddRectFilled(r.Min, r.Max, hovered ? color_hovered : color);
			if (!hovered) continue;

			if (!IsMouseDown(0))
			{
				doDock(dock, dest_dock ? dest_dock : getRootDock(), (Slot_)i);
				return true;
			}
			ImRect docked_rect = getDockedRect(rect, (Slot_)i);
			canvas->AddRectFilled(docked_rect.Min, docked_rect.Max, GetColorU32(ImGuiCol_Button));
		}
		return false;
	}

	void DockContext::handleDrag(Dock & dock)
	{
		Dock* dest_dock = getDockAt(GetIO().MousePos);

		Begin("##Overlay",
			NULL,
			ImGuiWindowFlags_Tooltip | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings |
			ImGuiWindowFlags_AlwaysAutoResize);
		ImDrawList* canvas = GetWindowDrawList();

		canvas->PushClipRectFullScreen();

		ImU32 docked_color = GetColorU32(ImGuiCol_FrameBg);
		docked_color = (docked_color & 0x00ffFFFF) | 0x80000000;
		dock.pos = GetIO().MousePos - m_drag_offset;
		if (dest_dock)
		{
			if (dockSlots(dock,
				dest_dock,
				ImRect(dest_dock->pos, dest_dock->pos + dest_dock->size),
				false))
			{
				canvas->PopClipRect();
				End();
				return;
			}
		}
		if (dockSlots(dock, nullptr, ImRect(g_dock.workspace_pos, g_dock.workspace_size), true))	//Position where the rects appear
		{
			canvas->PopClipRect();
			End();
			return;
		}
		canvas->AddRectFilled(dock.pos, dock.pos + dock.size, docked_color);
		canvas->PopClipRect();

		if (!IsMouseDown(0))
		{
			dock.status = Status_Float;
			dock.location[0] = 0;
			dock.setActive();
		}

		End();
	}

	void DockContext::fillLocation(Dock & dock)
	{
		if (dock.status == Status_Float) return;
		char* c = dock.location;
		Dock* tmp = &dock;
		while (tmp->parent)
		{
			*c = getLocationCode(tmp);
			tmp = tmp->parent;
			++c;
		}
		*c = 0;
	}

	void DockContext::doUndock(Dock & dock)
	{
		if (dock.prev_tab)
			dock.prev_tab->setActive();
		else if (dock.next_tab)
			dock.next_tab->setActive();
		else
			dock.active = false;
		Dock* container = dock.parent;

		if (container)
		{
			Dock& sibling = dock.getSibling();
			if (container->children[0] == &dock)
			{
				container->children[0] = dock.next_tab;
			}
			else if (container->children[1] == &dock)
			{
				container->children[1] = dock.next_tab;
			}

			bool remove_container = !container->children[0] || !container->children[1];
			if (remove_container)
			{
				if (container->parent)
				{
					Dock*& child = container->parent->children[0] == container
						? container->parent->children[0]
						: container->parent->children[1];
					child = &sibling;
					child->setPosSize(container->pos, container->size);
					child->setParent(container->parent);
				}
				else
				{
					if (container->children[0])
					{
						container->children[0]->setParent(nullptr);
						container->children[0]->setPosSize(container->pos, container->size);
					}
					if (container->children[1])
					{
						container->children[1]->setParent(nullptr);
						container->children[1]->setPosSize(container->pos, container->size);
					}
				}
				for (int i = 0; i < m_docks.size(); ++i)
				{
					if (m_docks[i] == container)
					{
						m_docks.erase(m_docks.begin() + i);
						break;
					}
				}
				container->~Dock();
				MemFree(container);
			}
		}
		if (dock.prev_tab) dock.prev_tab->next_tab = dock.next_tab;
		if (dock.next_tab) dock.next_tab->prev_tab = dock.prev_tab;
		dock.parent = nullptr;
		dock.prev_tab = dock.next_tab = nullptr;
	}

	void DockContext::drawTabbarListButton(Dock & dock)
	{
		if (!dock.next_tab) return;

		ImDrawList* draw_list = GetWindowDrawList();
		if (InvisibleButton("list", ImVec2(16, 16)))
		{
			OpenPopup("tab_list_popup");
		}
		if (BeginPopup("tab_list_popup"))
		{
			Dock* tmp = &dock;
			while (tmp)
			{
				bool dummy = false;
				if (Selectable(tmp->label, &dummy))
				{
					tmp->setActive();
				}
				tmp = tmp->next_tab;
			}
			EndPopup();
		}

		bool hovered = IsItemHovered();
		ImVec2 min = GetItemRectMin();
		ImVec2 max = GetItemRectMax();
		ImVec2 center = (min + max) * 0.5f;
		ImU32 text_color = GetColorU32(ImGuiCol_Text);
		ImU32 color_active = GetColorU32(ImGuiCol_FrameBgActive);
		draw_list->AddRectFilled(ImVec2(center.x - 4, min.y + 3),
			ImVec2(center.x + 4, min.y + 5),
			hovered ? color_active : text_color);
		draw_list->AddTriangleFilled(ImVec2(center.x - 4, min.y + 7),
			ImVec2(center.x + 4, min.y + 7),
			ImVec2(center.x, min.y + 12),
			hovered ? color_active : text_color);
	}

	bool DockContext::tabbar(Dock & dock, bool close_button)
	{
		float tabbar_height = 2 * GetTextLineHeightWithSpacing();
		ImVec2 size(dock.size.x, tabbar_height);
		bool tab_closed = false;

		SetCursorScreenPos(dock.pos);
		char tmp[20];
		ImFormatString(tmp, IM_ARRAYSIZE(tmp), "tabs%d", (int)dock.id);

		if (BeginChild(tmp, size, true))
		{
			Dock* dock_tab = &dock;

			ImDrawList* draw_list = GetWindowDrawList();
			ImU32 color = GetColorU32(ImGuiCol_FrameBg);
			ImU32 color_active = GetColorU32(ImGuiCol_FrameBgActive);
			ImU32 color_hovered = GetColorU32(ImGuiCol_FrameBgHovered);
			ImU32 button_hovered = GetColorU32(ImGuiCol_ButtonHovered);
			ImU32 text_color = GetColorU32(ImGuiCol_Text);
			float line_height = GetTextLineHeightWithSpacing();
			float tab_base;

			drawTabbarListButton(dock);

			while (dock_tab)
			{
				SameLine(0, 15);

				const char* text_end = FindRenderedTextEnd(dock_tab->label);
				ImVec2 size(CalcTextSize(dock_tab->label, text_end).x, line_height);
				if (InvisibleButton(dock_tab->label, size))
				{
					dock_tab->setActive();
				}

				if (IsItemActive() && IsMouseDragging())
				{
					m_drag_offset = GetMousePos() - dock_tab->pos;
					doUndock(*dock_tab);
					dock_tab->status = Status_Dragged;
				}

				if (dock_tab->active && close_button) size.x += 16 + GetStyle().ItemSpacing.x;

				bool hovered = IsItemHovered();
				ImVec2 pos = GetItemRectMin();
				tab_base = pos.y;
				draw_list->PathClear();
				draw_list->PathLineTo(pos + ImVec2(-15, size.y));
				draw_list->PathBezierCurveTo(
					pos + ImVec2(-10, size.y), pos + ImVec2(-5, 0), pos + ImVec2(0, 0), 10);
				draw_list->PathLineTo(pos + ImVec2(size.x, 0));
				draw_list->PathBezierCurveTo(pos + ImVec2(size.x + 5, 0),
					pos + ImVec2(size.x + 10, size.y),
					pos + ImVec2(size.x + 15, size.y),
					10);
				draw_list->PathFillConvex(
					hovered ? color_hovered : (dock_tab->active ? color_active : color));
				draw_list->AddText(pos, text_color, dock_tab->label, text_end);

				if (dock_tab->active && close_button)
				{
					SameLine();
					tab_closed = InvisibleButton("close", ImVec2(16, 16));
					ImVec2 center = (GetItemRectMin() + GetItemRectMax()) * 0.5f;
					if (IsItemHovered())
					{
						draw_list->AddRectFilled(center + ImVec2(-6.0f, -6.0f), center + ImVec2(7.0f, 7.0f), button_hovered);
					}
					draw_list->AddLine(
						center + ImVec2(-3.5f, -3.5f), center + ImVec2(3.5f, 3.5f), text_color);
					draw_list->AddLine(
						center + ImVec2(3.5f, -3.5f), center + ImVec2(-3.5f, 3.5f), text_color);
				}

				dock_tab = dock_tab->next_tab;
			}
			ImVec2 cp(dock.pos.x, tab_base + line_height);
			draw_list->AddLine(cp, cp + ImVec2(dock.size.x, 0), color);
		}
		EndChild();
		return tab_closed;
	}

	void DockContext::setDockPosSize(Dock & dest, Dock & dock, Slot_ dock_slot, Dock & container)
	{
		IM_ASSERT(!dock.prev_tab && !dock.next_tab && !dock.children[0] && !dock.children[1]);

		dest.pos = container.pos;
		dest.size = container.size;
		dock.pos = container.pos;
		dock.size = container.size;

		switch (dock_slot)
		{
		case Slot_Bottom:
			dest.size.y *= 0.5f;
			dock.size.y *= 0.5f;
			dock.pos.y += dest.size.y;
			break;
		case Slot_Right:
			dest.size.x *= 0.5f;
			dock.size.x *= 0.5f;
			dock.pos.x += dest.size.x;
			break;
		case Slot_Left:
			dest.size.x *= 0.5f;
			dock.size.x *= 0.5f;
			dest.pos.x += dock.size.x;
			break;
		case Slot_Top:
			dest.size.y *= 0.5f;
			dock.size.y *= 0.5f;
			dest.pos.y += dock.size.y;
			break;
		default: IM_ASSERT(false); break;
		}
		dest.setPosSize(dest.pos, dest.size);

		if (container.children[1]->pos.x < container.children[0]->pos.x ||
			container.children[1]->pos.y < container.children[0]->pos.y)
		{
			Dock* tmp = container.children[0];
			container.children[0] = container.children[1];
			container.children[1] = tmp;
		}
	}

	void DockContext::doDock(Dock & dock, Dock * dest, Slot_ dock_slot)
	{
		IM_ASSERT(!dock.parent);
		if (!dest)
		{
			dock.status = Status_Docked;
			dock.setPosSize(g_dock.workspace_pos, g_dock.workspace_size);
		}
		else if (dock_slot == Slot_Tab)
		{
			Dock* tmp = dest;
			while (tmp->next_tab)
			{
				tmp = tmp->next_tab;
			}

			tmp->next_tab = &dock;
			dock.prev_tab = tmp;
			dock.size = tmp->size;
			dock.pos = tmp->pos;
			dock.parent = dest->parent;
			dock.status = Status_Docked;
		}
		else if (dock_slot == Slot_None)
		{
			dock.status = Status_Float;
		}
		else
		{
			Dock* container = (Dock*)MemAlloc(sizeof(Dock));
			IM_PLACEMENT_NEW(container) Dock();
			m_docks.push_back(container);
			container->children[0] = &dest->getFirstTab();
			container->children[1] = &dock;
			container->next_tab = nullptr;
			container->prev_tab = nullptr;
			container->parent = dest->parent;
			container->size = dest->size;
			container->pos = dest->pos;
			container->status = Status_Docked;
			container->label = ImStrdup("");

			if (!dest->parent)
			{
			}
			else if (&dest->getFirstTab() == dest->parent->children[0])
			{
				dest->parent->children[0] = container;
			}
			else
			{
				dest->parent->children[1] = container;
			}

			dest->setParent(container);
			dock.parent = container;
			dock.status = Status_Docked;

			setDockPosSize(*dest, dock, dock_slot, *container);
		}
		dock.setActive();
	}

	void DockContext::rootDock(const ImVec2 & pos, const ImVec2 & size)
	{
		Dock* root = getRootDock();
		if (!root) return;

		ImVec2 min_size = root->getMinSize();
		ImVec2 requested_size = size;
		root->setPosSize(pos, ImMax(min_size, requested_size));
	}

	void DockContext::setDockActive()
	{
		IM_ASSERT(m_current);
		if (m_current) m_current->setActive();
	}

	Slot_ DockContext::getSlotFromLocationCode(char code)
	{
		switch (code)
		{
		case '1': return Slot_Left;
		case '2': return Slot_Top;
		case '3': return Slot_Bottom;
		default: return Slot_Right;
		}
	}

	char DockContext::getLocationCode(Dock * dock)
	{
		if (!dock) return '0';

		if (dock->parent->isHorizontal())
		{
			if (dock->pos.x < dock->parent->children[0]->pos.x) return '1';
			if (dock->pos.x < dock->parent->children[1]->pos.x) return '1';
			return '0';
		}
		else
		{
			if (dock->pos.y < dock->parent->children[0]->pos.y) return '2';
			if (dock->pos.y < dock->parent->children[1]->pos.y) return '2';
			return '3';
		}
	}

	void DockContext::tryDockToStoredLocation(Dock & dock)
	{
		if (dock.status == Status_Docked) return;
		if (dock.location[0] == 0) return;

		Dock* tmp = getRootDock();
		if (!tmp) return;

		Dock* prev = nullptr;
		char* c = dock.location + strlen(dock.location) - 1;
		while (c >= dock.location && tmp)
		{
			prev = tmp;
			tmp = *c == getLocationCode(tmp->children[0]) ? tmp->children[0] : tmp->children[1];
			if (tmp) --c;
		}
		if (tmp && tmp->children[0]) tmp = tmp->parent;
		doDock(dock, tmp ? tmp : prev, tmp && !tmp->children[0] ? Slot_Tab : getSlotFromLocationCode(*c));
	}

	void DockContext::cleanDocks()
	{
	restart:
		for (int i = 0, c = m_docks.size(); i < c; ++i)
		{
			Dock& dock = *m_docks[i];
			if (dock.last_frame == 0 && dock.status != Status_Float && !dock.children[0])
			{
				fillLocation(*m_docks[i]);
				doUndock(*m_docks[i]);
				m_docks[i]->status = Status_Float;
				goto restart;
			}
		}
	}

	bool DockContext::begin(const char * label, bool * opened, ImGuiWindowFlags extra_flags, const ImVec2 & default_size)
	{
		IM_ASSERT(!m_is_begin_open);
		m_is_begin_open = true;
		Dock& dock = getDock(label, !opened || *opened, default_size);
		if (dock.last_frame != 0 && m_last_frame != ImGui::GetFrameCount())
		{
			cleanDocks();
		}
		dock.last_frame = ImGui::GetFrameCount();
		if (!dock.opened && (!opened || *opened)) tryDockToStoredLocation(dock);
		if (strcmp(dock.label, label) != 0)
		{
			MemFree(dock.label);
			dock.label = ImStrdup(label);
		}

		m_end_action = EndAction_None;

		if (dock.first && opened) *opened = dock.opened;
		dock.first = false;
		if (opened && !*opened)
		{
			if (dock.status != Status_Float)
			{
				fillLocation(dock);
				doUndock(dock);
				dock.status = Status_Float;
			}
			dock.opened = false;
			return false;
		}
		dock.opened = true;

		m_end_action = EndAction_Panel;
		beginPanel();

		m_current = &dock;
		if (dock.status == Status_Dragged) handleDrag(dock);

		bool is_float = dock.status == Status_Float;

		if (is_float)
		{
			SetNextWindowPos(dock.pos);
			SetNextWindowSize(dock.size, ImGuiCond_FirstUseEver);
			bool ret = Begin(label,
				opened,
				ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | extra_flags);
			m_end_action = EndAction_End;
			dock.pos = GetWindowPos();
			dock.size = GetWindowSize();

			ImGuiContext& g = *GImGui;

			if (g.ActiveId == GetCurrentWindow()->MoveId && g.IO.MouseDown[0])
			{
				m_drag_offset = GetMousePos() - dock.pos;
				doUndock(dock);
				dock.status = Status_Dragged;
			}
			return ret;
		}

		if (!dock.active && dock.status != Status_Dragged) return false;

		m_end_action = EndAction_EndChild;

		PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
		PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
		float tabbar_height = GetTextLineHeightWithSpacing();
		if (tabbar(dock.getFirstTab(), opened != nullptr))
		{
			fillLocation(dock);
			*opened = false;
		}
		ImVec2 pos = dock.pos;
		ImVec2 size = dock.size;
		pos.y += tabbar_height + GetStyle().WindowPadding.y;
		size.y -= tabbar_height + GetStyle().WindowPadding.y;

		SetCursorScreenPos(pos);
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus |
			extra_flags;
		char tmp[256];
		strcpy(tmp, label);
		strcat(tmp, "_docked"); // to avoid https://github.com/ocornut/imgui/issues/713
		bool ret = BeginChild(tmp, size, true, flags);
		PopStyleColor();
		PopStyleColor();
		return ret;
	}

	void DockContext::end()
	{
		if (m_end_action == EndAction_End)
		{
			End();
		}
		else if (m_end_action == EndAction_EndChild)
		{
			PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
			PushStyleColor(ImGuiCol_BorderShadow, ImVec4(0, 0, 0, 0));
			EndChild();
			PopStyleColor();
			PopStyleColor();
		}
		m_current = nullptr;
		if (m_end_action > EndAction_None) endPanel();
		m_is_begin_open = false;
	}

	int DockContext::getDockIndex(Dock * dock)
	{
		if (!dock) return -1;

		for (int i = 0; i < g_dock.m_docks.size(); ++i)
		{
			if (dock == g_dock.m_docks[i]) return i;
		}

		IM_ASSERT(false);
		return -1;
	}

} // namespace ImGui