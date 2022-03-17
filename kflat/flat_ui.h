// flat_ui.h
// Created by lulimin on 2020/11/9.

#ifndef __FLAT_UI_H
#define __FLAT_UI_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"
#include "../inc/string_hash_table.h"
#include "../kgraphic/i_renderable.h"

// FlatUI

class FlatWidget;
class FlatCursor;
class FlatFont;
class FlatMenu;
class FlatLabel;
class IInputService;
class IRenderService;
struct flat_canvas_t;
struct flat_state_t;
struct flat_metadata_t;

class FlatUI : public IRenderable
{
private:
	typedef TStringHashTable<char, flat_metadata_t*, TCharTraits<char>,
		FrameAlloc> MetadataTable;

public:
	FlatUI();
	virtual ~FlatUI();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Update states per frame.
	virtual void Update(float frame_time);
	// Render graphics.
	virtual void Render(float frame_time);

	// Load resource.
	virtual bool Load();
	// Get whether loading succeed.
	virtual bool GetLoadSucceed();
	// Get whether loading ended.
	virtual bool GetLoadEnded();

	// Get render service.
	IRenderService* GetRenderService() const { return m_pRenderService; }
	// Get render canvas.
	flat_canvas_t* GetCanvas() const { return m_pCanvas; }

	// Screen size.
	int GetWidth() const;
	int GetHeight() const;
	// Get safe area.
	int GetSafeAreaLeft() const;
	int GetSafeAreaTop() const;
	int GetSafeAreaRight() const;
	int GetSafeAreaBottom() const;

	// Visible flag.
	void SetVisible(bool value);
	bool GetVisible() const;

	// Input caret color.
	void SetCaretColor(const char* value);
	ArgString GetCaretColor() const;

	// Widget configure file path.
	void SetConfigPath(const char* value);
	const char* GetConfigPath() const;

	// Get metadata of widget.
	flat_metadata_t* GetMetadata(FlatWidget* pWidget);
	// Get metadata object.
	uniqid_t GetMeta(const uniqid_t& id);

	// Create widget.
	FlatWidget* CreateWidget(const char* name);
	// Create widget.
	uniqid_t Create(const char* name);
	// Delete widget.
	bool Delete(const uniqid_t& id);
	// Clone widget.
	uniqid_t Clone(const uniqid_t& id);

	// Create part widget.
	FlatWidget* CreatePartWidget(FlatWidget* pParent, const char* name);
	// Delete part widget.
	void DeletePartWidget(FlatWidget* pWidget);

	// Add play widget.
	bool AddPlayWidget(FlatWidget* pWidget);
	// Remove play widget.
	bool RemovePlayWidget(FlatWidget* pWidget);
	// Add play.
	bool AddPlay(const uniqid_t& id);
	// Remove play.
	bool RemovePlay(const uniqid_t& id);

	// Add dialog to stack.
	bool AddDialog(FlatWidget* pWidget);
	// Remove dialog from stack.
	bool RemoveDialog(FlatWidget* pWidget);
	// Get top layer dialog.
	FlatWidget* GetTopDialog() const;
	// Get top layer dialog object id.
	uniqid_t GetTopDialogID() const;

	// Add floating widget.
	bool AddFloating(const uniqid_t& id);
	// Remove floating widget.
	bool RemoveFloating(const uniqid_t& id);
	// Find floating widget.
	bool FindFloating(const uniqid_t& id);
	// Clear all floating widgets.
	bool ClearFloatings();
	// Get floating container size.
	size_t GetFloatingSize() const { return m_Floatings.Size(); }
	// Get floating container data.
	const uniqid_t* GetFloatingData() const { return m_Floatings.Data(); }

	// Add cursor.
	bool AddCursor(const uniqid_t& id);
	// Remove cursor.
	bool RemoveCursor(const char* name);
	// Set current cursor.
	bool SetCurrentCursor(const char* name);
	// Show current cursor.
	bool ShowCurrentCursor();
	// Hide current cursor.
	bool HideCurrentCursor();
	// Get cursor name list.
	void GetCursorList(const IArgList& args, IArgList* res);

	// Add font.
	bool AddFont(const uniqid_t& id);
	// Remove font.
	bool RemoveFont(const char* name);
	// Set current font.
	bool SetCurrentFont(const char* name);
	// Get font name list.
	void GetFontList(const IArgList& args, IArgList* res);

	// Add context menu.
	bool AddContextMenu(const uniqid_t& id);
	// Remove context menu.
	bool RemoveContextMenu(const char* name);
	// Clear context menu.
	bool ClearContextMenus();
	// Get context menu by name.
	FlatMenu* GetContextMenu(const char* name);

	// Add hint label.
	bool AddHintLabel(const uniqid_t& id);
	// Remove hint label.
	bool RemoveHintLabel(const char* name);
	// Clear hint label.
	bool ClearHintLabels();
	// Get hint label by name.
	FlatLabel* GetHintLabel(const char* name);
	
	// Add scroll style widget.
	//bool AddScrollStyle(const uniqid_t& id);
	// Remove scroll style.
	//bool RemoveScrollStyle(const char* name);
	// Clear all scroll style.
	//bool ClearScrollStyle();
	// Get scroll style.
	//FlatWidget* GetScrollStyle(const char* name) const;

	// Load form.
	uniqid_t LoadForm(const char* file_name);
	// Load form in design mode.
	uniqid_t LoadDesignForm(const char* file_name);
	// Save form.
	bool SaveForm(const uniqid_t& form_id, const char* file_name);
	// Load configuration of widget.
	uniqid_t LoadWidget(const char* file_name);
	// Save configuration of widget.
	bool SaveWidget(const uniqid_t& widget_id, const char* file_name);

	// Open file by operation system.
	bool OpenFile(const char* file_name);

	// Draw input caret.
	bool DrawCaret(flat_canvas_t* pCanvas, int x, int y, int height);
	// Reset caret counter.
	bool ResetCaret();

	// Focus to widget.
	bool FocusToWidget(FlatWidget* pWidget);
	bool FocusTo(const uniqid_t& id);
	// Get current focused widget.
	uniqid_t GetCurrentFocused();

	// Get width of text.
	int GetTextWidth(const char* font, const char* text, int start,
		int char_num);

	// Process input.
	int ProcessInput(const void* pMsg);

private:
	IRenderService* m_pRenderService;
	IInputService* m_pInputService;
	flat_canvas_t* m_pCanvas;
	flat_state_t* m_pState;
	MetadataTable m_MetadataTable;
	TPodArray<uniqid_t, 1, FrameAlloc> m_Plays;
	TPodArray<uniqid_t, 1, FrameAlloc> m_DialogStack;
	TPodArray<uniqid_t, 1, FrameAlloc> m_Floatings;
	TPodArray<FlatCursor*, 1, FrameAlloc> m_Cursors;
	FlatCursor* m_pCurrentCursor;
	TPodArray<FlatFont*, 1, FrameAlloc> m_Fonts;
	FlatFont* m_pCurrentFont;
	TPodArray<FlatMenu*, 1, FrameAlloc> m_ContextMenus;
	TPodArray<FlatLabel*, 1, FrameAlloc> m_HintLabels;
	//TPodArray<FlatWidget*, 1, FrameAlloc> m_ScrollStyles;
	bool m_bVisible;
	unsigned int m_nCaretColor;
	float m_fCaretCounter;
	bool m_bCaretVisible;
	FrameString m_sConfigPath;
};

#endif // __FLAT_UI_H
