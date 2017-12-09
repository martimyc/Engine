#ifndef IMGUI_DOCKING
#define IMGUI_DOCKING

#include "imgui.h"
#include "imgui_internal.h"

namespace ImGui
{
	enum Slot_
	{
		Slot_Left,
		Slot_Right,
		Slot_Top,
		Slot_Bottom,
		Slot_Tab,
		Slot_Float,
		Slot_None
	};


	enum EndAction_
	{
		EndAction_None,
		EndAction_Panel,
		EndAction_End,
		EndAction_EndChild
	};


	enum Status_
	{
		Status_Docked = 0,
		Status_Float,
		Status_Dragged
	};

	struct DockContext
	{
		struct Dock
		{
			char* label;
			ImU32 id;
			Dock* next_tab;
			Dock* prev_tab;
			Dock* children[2];
			Dock* parent;
			bool active;
			ImVec2 pos;
			ImVec2 size;
			Status_ status;
			char location[16];
			bool opened;
			bool first;
			int last_frame;

			Dock();
			~Dock();

			ImVec2 getMinSize() const;
			bool isHorizontal() const;
			void setParent(Dock* dock);
			Dock& getSibling();
			Dock& getFirstTab();
			void setActive();
			bool isContainer() const;
			void setChildrenPosSize(const ImVec2& _pos, const ImVec2& _size);
			void setPosSize(const ImVec2& _pos, const ImVec2& _size);
			
			void Save(JSON_Object* dock_object) const;
			void Load(const JSON_Object* dock_object);
			void LoadHierarchy(const JSON_Object* dock_object);
		};

		DockContext();
		~DockContext();

	private:
		ImVector<Dock*> m_docks;
		ImVec2 m_drag_offset;
		Dock* m_current = nullptr;
		int m_last_frame = 0;
		EndAction_ m_end_action;
		bool m_is_begin_open = false;

	public:
		bool BeginDock(const char* label, bool* opened = nullptr, ImGuiWindowFlags extra_flags = 0, const ImVec2& default_size = ImVec2(-1, -1));
		void EndDock();

		void SetWorkspacePosSize(ImVec2 pos_, ImVec2 size_, float width_difference = 1.0f, float height_difference = 1.0f);
		bool GetDockSize(const char * str, float & x, float & y, float & w, float & h) const;

		void LoadDocks();
		void SaveDocks();

	private:
		ImVec2 workspace_pos;
		ImVec2 workspace_size;
		void TabbarInContext(Dock* dock_to_tab);
		int GetDockPos(const Dock* dock) const;
		DockContext::Dock* GetDockbyPos(int position) const;

		void RootDock(const ImVec2& pos, const ImVec2& size);
		void SetDockActive(); 
		void ShutdownDock();

		Dock& getDock(const char* label, bool opened, const ImVec2& default_size);
		void putInBackground();
		void splits();
		void beginPanel();
		void endPanel();
		Dock* getDockAt(const ImVec2& pos) const;
		static ImRect getDockedRect(const ImRect& rect, Slot_ dock_slot);
		static ImRect getSlotRect(ImRect parent_rect, Slot_ dock_slot);
		static ImRect getSlotRectOnBorder(ImRect parent_rect, Slot_ dock_slot);
		Dock* getRootDock();
		bool dockSlots(Dock& dock, Dock* dest_dock, const ImRect& rect, bool on_border);
		void handleDrag(Dock& dock);
		void fillLocation(Dock& dock);
		void doUndock(Dock& dock);
		void drawTabbarListButton(Dock& dock);
		bool tabbar(Dock& dock, bool close_button);
		static void setDockPosSize(Dock& dest, Dock& dock, Slot_ dock_slot, Dock& container);
		void doDock(Dock& dock, Dock* dest, Slot_ dock_slot);
		void rootDock(const ImVec2& pos, const ImVec2& size);
		void setDockActive();
		static Slot_ getSlotFromLocationCode(char code);
		static char getLocationCode(Dock* dock);
		void tryDockToStoredLocation(Dock& dock);
		void cleanDocks();
		bool begin(const char* label, bool* opened, ImGuiWindowFlags extra_flags, const ImVec2& default_size);
		void end();
		int getDockIndex(Dock* dock);
	};
}

#endif // !IMGUI_DOCKING