// flat_ui.cpp
// Created by lulimin on 2020/11/9.

#include "flat_ui.h"
#include "flat_widget.h"
#include "flat_canvas.h"
#include "flat_state.h"
#include "flat_cursor.h"
#include "flat_font.h"
#include "flat_meta.h"
#include "flat_menu.h"
#include "flat_label.h"
#include "flat_config.h"
#include "flat_utility.h"
#include "../kgraphic/i_render_service.h"
#include "../kgraphic/i_input_service.h"

/// \file flat_ui.cpp
/// \brief Flat user interface management.

/// object: FlatUI
/// \brief Flat user interface management object.
DEF_OBJECT(FlatUI, IObject);

/// readonly: int Width
/// \brief Width of window.
DEF_READ(int, FlatUI, Width);
/// readonly: int Height
/// \brief Height of window.
DEF_READ(int, FlatUI, Height);
/// readonly: int SafeAreaLeft
/// \brief Safe area to left edge.
DEF_READ(int, FlatUI, SafeAreaLeft);
/// readonly: int SafeAreaTop
/// \brief Safe area to top edge.
DEF_READ(int, FlatUI, SafeAreaTop);
/// readonly: int SafeAreaRight
/// \brief Safe area to right edge.
DEF_READ(int, FlatUI, SafeAreaRight);
/// readonly: int SafeAreaBottom
/// \brief Safe area to bottom edge.
DEF_READ(int, FlatUI, SafeAreaBottom);
/// readonly: object TopDialog
/// \brief Top layer dialog object id.
DEF_READ_F(uniqid_t, FlatUI, TopDialog, GetTopDialogID);

/// property: bool Visible
/// \brief Flag of visible.
DEF_PROP(bool, FlatUI, Visible);
/// property: string CaretColor
/// \brief Input caret color.
DEF_PROP(ArgString, FlatUI, CaretColor);
/// property: string ConfigPath
/// \brief Widget configure file path.
DEF_PROP(const char*, FlatUI, ConfigPath);

/// function: bool Load()
/// \brief Load resource.
//DEF_FUNC_0(bool, FlatUI, Load);

/// function: object GetMeta(object widget_id)
/// \brief Get metadata object of widget.
/// \param widget_id Widget object id.
DEF_FUNC_1(uniqid_t, FlatUI, GetMeta, const uniqid_t&);

/// function: object Create(string name)
/// \brief Create widget.
/// \param name Widget name.
DEF_FUNC_1(uniqid_t, FlatUI, Create, const char*);
/// function: bool Delete(object id)
/// \brief Delete widget.
/// \param id Widget id.
DEF_FUNC_1(bool, FlatUI, Delete, const uniqid_t&);
/// function: object Clone(object id)
/// \brief Clone widget.
/// \param id Source widget id.
DEF_FUNC_1(uniqid_t, FlatUI, Clone, const uniqid_t&);

/// function: bool AddPlay(object id)
/// \brief Add play object.
/// \param id Object id.
DEF_FUNC_1(bool, FlatUI, AddPlay, const uniqid_t&);
/// function: bool RemovePlay(object id)
/// \brief Remove play object.
/// \param id Object id.
DEF_FUNC_1(bool, FlatUI, RemovePlay, const uniqid_t&);

/// function: bool AddFloating(object id)
/// \brief Add floating widget.
/// \param id Widget object id.
DEF_FUNC_1(bool, FlatUI, AddFloating, const uniqid_t&);
/// function: bool RemoveFloating(object id)
/// \brief Remove floating widget.
/// \param id Widget object id.
DEF_FUNC_1(bool, FlatUI, RemoveFloating, const uniqid_t&);
/// function: bool ClearFloatings
/// \brief Clear floating widgets.
DEF_FUNC_0(bool, FlatUI, ClearFloatings);

/// function: bool AddCursor(object id)
/// \brief Add cursor.
/// \param id Cursor object id.
DEF_FUNC_1(bool, FlatUI, AddCursor, const uniqid_t&);
/// function: bool RemoveCursor(string name)
/// \brief Remove cursor.
/// \param name Cursor name.
DEF_FUNC_1(bool, FlatUI, RemoveCursor, const char*);
/// function: bool SetCurrentCursor(string name)
/// \brief Set current cursor.
/// \param name Cursor name.
DEF_FUNC_1(bool, FlatUI, SetCurrentCursor, const char*);
/// function: bool ShowCurrentCursor()
/// \brief Show cursor.
DEF_FUNC_0(bool, FlatUI, ShowCurrentCursor);
/// function: bool HideCurrentCursor()
/// \brief Hide cursor.
DEF_FUNC_0(bool, FlatUI, HideCurrentCursor);
/// function: [table] GetCursorList()
/// \brief Get cursor name list.
DEF_FUNC_A(FlatUI, GetCursorList);

/// function: bool AddFont(object id)
/// \brief Add font.
/// \param id Font object id.
DEF_FUNC_1(bool, FlatUI, AddFont, const uniqid_t&);
/// function: bool RemoveFont(string name)
/// \brief Remove font.
/// \param name Font name.
DEF_FUNC_1(bool, FlatUI, RemoveFont, const char*);
/// function: bool SetCurrentFont(string name)
/// \brief Set current font.
/// \param name Font name.
DEF_FUNC_1(bool, FlatUI, SetCurrentFont, const char*);
/// function: [table] GetFontList()
/// \brief Get font name list.
DEF_FUNC_A(FlatUI, GetFontList);

/// function: bool AddContextMenu(object id)
/// \brief Add context menu.
/// \param id Menu object id.
DEF_FUNC_1(bool, FlatUI, AddContextMenu, const uniqid_t&);
/// function: bool RemoveContextMenu(string name)
/// \brief Remove context menu.
/// \param name Menu name.
DEF_FUNC_1(bool, FlatUI, RemoveContextMenu, const char*);
/// function: bool ClearContextMenus()
/// \brief Clear context menu.
DEF_FUNC_0(bool, FlatUI, ClearContextMenus);

/// function: bool AddHintLabel(object id)
/// \brief Add hint label.
/// \param id Label object id.
DEF_FUNC_1(bool, FlatUI, AddHintLabel, const uniqid_t&);
/// function: bool RemoveHintLabel(string name)
/// \brief Remove hint label.
/// \param name Label name.
DEF_FUNC_1(bool, FlatUI, RemoveHintLabel, const char*);
/// function: bool ClearHintLabels()
/// \brief Clear hint label.
DEF_FUNC_0(bool, FlatUI, ClearHintLabels);

/// function: bool SetHintLabel(object id)
/// \brief Set hint label object.
/// \param id Label object id.
//DEF_FUNC_1(bool, FlatUI, SetHintLabel, const uniqid_t&);

/// function: bool AddScrollStyle(object id)
/// \brief Add scroll style.
/// \param id Scroll style widget id.
//DEF_FUNC_1(bool, FlatUI, AddScrollStyle, const uniqid_t&);
/// function: bool RemoveScrollStyle(string name)
/// \brief Remove scroll style.
/// \param name Scroll style name.
//DEF_FUNC_1(bool, FlatUI, RemoveScrollStyle, const char*);
/// function: bool ClearScrollStyle()
/// \brief Clear all scroll styles.
//DEF_FUNC_0(bool, FlatUI, ClearScrollStyle)

/// function: object LoadForm(string file_name)
/// \brief Load form.
/// \param file_name Form configure file.
DEF_FUNC_1(uniqid_t, FlatUI, LoadForm, const char*);
/// function: object LoadDesignForm(string file_name)
/// \brief Load form in design mode.
/// \param file_name Form configure file.
DEF_FUNC_1(uniqid_t, FlatUI, LoadDesignForm, const char*);
/// function: bool SaveForm(object form, string file_name)
/// \brief Save form to file.
/// \param form Form object id.
/// \param file_name Form configure file.
DEF_FUNC_2(bool, FlatUI, SaveForm, const uniqid_t&, const char*);
/// function: object LoadWidget(string file_name)
/// \brief Load widget from file .
/// \param file_name Widget configure file.
DEF_FUNC_1(uniqid_t, FlatUI, LoadWidget, const char*);
/// function: bool SaveWidget(object widget, string file_name)
/// \brief Save widget to file.
/// \param widget Form widget id.
/// \param file_name Widget configure file.
DEF_FUNC_2(bool, FlatUI, SaveWidget, const uniqid_t&, const char*);

/// function: bool OpenFile(string file_name)
/// \brief Open file by operation system.
/// \param file_name Name of file.
DEF_FUNC_1(bool, FlatUI, OpenFile, const char*);

/// function: bool FocusTo(object id)
/// \brief Set current focused widget.
/// \param id Widget object id.
DEF_FUNC_1(bool, FlatUI, FocusTo, const uniqid_t&);
/// function: object GetCurrentFocused()
/// \brief Get current focused widget.
DEF_FUNC_0(uniqid_t, FlatUI, GetCurrentFocused);

// Interval of caret displaying.
#define CARET_SHOW_INTERVAL 0.6F
#define CARET_HIDE_INTERVAL 0.6F

// Change focused widget.
bool flat_change_focus(flat_state_t* pState, FlatWidget* pNewFocus);

// Input processor.
int flat_ui_process_input(void* context, const IInputService::input_t& msg)
{
	FlatUI* pFlatUI = (FlatUI*)context;

	return pFlatUI->ProcessInput(&msg);
}

// FlatUI

FlatUI::FlatUI()
{
	m_pRenderService = NULL;
	m_pInputService = NULL;
	m_pCanvas = NULL;
	m_pState = NULL;
	m_pCurrentCursor = NULL;
	m_pCurrentFont = NULL;
	m_bVisible = true;
	m_nCaretColor = 0xFF0000FF;
	m_fCaretCounter = 0.0F;
	m_bCaretVisible = true;
}

FlatUI::~FlatUI()
{
	if (m_pCanvas)
	{
		canvas_delete(m_pCanvas);
	}

	if (m_pState)
	{
		flat_state_delete(m_pState);
	}

	MetadataTable::iterator it = m_MetadataTable.Begin();
	MetadataTable::iterator end = m_MetadataTable.End();

	for (; it != end; ++it)
	{
		flat_metadata_delete(it.GetData());
	}
}

bool FlatUI::Startup(const IArgList& args)
{
	m_pRenderService = (IRenderService*)frame_get_service("RenderService");

	if (NULL == m_pRenderService)
	{
		K_LOG("(FlatUI::Startup)No render service.");
		return false;
	}

	m_pInputService = (IInputService*)frame_get_service("FlatInputService");

	if (NULL == m_pInputService)
	{
		K_LOG("(FlatUI::Startup)No input service.");
		return false;
	}

	m_pCanvas = canvas_create(m_pRenderService);
	
	if (!canvas_load(m_pCanvas))
	{
		K_LOG("(FlatUI::Startup)Load canvas error.");
	}

	m_sConfigPath = frame_get_coder_path() + FrameString("widget/");
	m_pState = flat_state_create(this);
	m_pInputService->AddProcessor(flat_ui_process_input, this, uniqid_t());
	return true;
}

bool FlatUI::Shutdown()
{
	m_pInputService->RemoveProcessor(flat_ui_process_input, this, uniqid_t());
	
	if (m_pCanvas)
	{
		canvas_unload(m_pCanvas);
	}

	for (size_t i = 0; i < m_Cursors.Size(); ++i)
	{
		m_Cursors[i]->Release();
	}

	m_Cursors.Clear();
	
	for (size_t k = 0; k < m_Fonts.Size(); ++k)
	{
		m_Fonts[k]->Release();
	}

	m_Fonts.Clear();

	this->ClearContextMenus();
	this->ClearHintLabels();
	//this->ClearScrollStyle();

	MetadataTable::iterator it = m_MetadataTable.Begin();
	MetadataTable::iterator end = m_MetadataTable.End();

	for (; it != end; ++it)
	{
		uniqid_t meta_id = flat_metadata_get_object(it.GetData());
		
		frame_delete_object(meta_id);
	}

	//SAFE_RELEASE(m_pHintLabel);
	SAFE_RELEASE(m_pInputService);
	SAFE_RELEASE(m_pRenderService);
	return true;
}

void FlatUI::Update(float frame_time)
{
	// Play widgets.
	size_t i = 0; 

	while (i < m_Plays.Size())
	{
		FlatWidget* pWidget = (FlatWidget*)frame_get_object(m_Plays[i]);

		if (pWidget)
		{
			pWidget->Play(frame_time);
			++i;
		}
		else
		{
			m_Plays.Remove(i);
		}
	}
	
	// Fresh input caret.
	m_fCaretCounter += frame_time;

	if (m_bCaretVisible)
	{
		if (m_fCaretCounter >= CARET_SHOW_INTERVAL)
		{
			m_fCaretCounter -= CARET_SHOW_INTERVAL;
			m_bCaretVisible = false;
		}
	}
	else
	{
		if (m_fCaretCounter >= CARET_HIDE_INTERVAL)
		{
			m_fCaretCounter -= CARET_HIDE_INTERVAL;
			m_bCaretVisible = true;
		}
	}

	// State machine timer input.
	flat_state_machine(m_pState, FLAT_EVENT_TIMER, FrameArgList(), &frame_time);
}

void FlatUI::Render(float frame_time)
{
	if (!m_bVisible)
	{
		return;
	}
	
	canvas_render_begin(m_pCanvas);

	for (size_t i = 0; i < m_DialogStack.Size(); ++i)
	{
		FlatWidget* pWidget = flat_get_widget(m_DialogStack[i]);

		if (pWidget)
		{
			pWidget->Draw(m_pCanvas);
		}
	}

	for (size_t k = 0; k < m_Floatings.Size(); ++k)
	{
		FlatWidget* pFloating = flat_get_widget(m_Floatings[k]);

		if (pFloating)
		{
			pFloating->Draw(m_pCanvas);
		}
	}

	canvas_render_end(m_pCanvas);
}

bool FlatUI::Load()
{ 
//	if (!canvas_load(m_pCanvas))
//	{
//		return false;
//	}
	
	return true;
}

bool FlatUI::GetLoadSucceed()
{ 
	return true;
}

bool FlatUI::GetLoadEnded()
{ 
	return true;
}

int FlatUI::GetWidth() const
{
	return m_pRenderService->GetWindowWidth();
}

int FlatUI::GetHeight() const
{
	return m_pRenderService->GetWindowHeight();
}

int FlatUI::GetSafeAreaLeft() const
{
	return m_pRenderService->GetSafeAreaLeft();
}

int FlatUI::GetSafeAreaTop() const
{
	return m_pRenderService->GetSafeAreaTop();
}

int FlatUI::GetSafeAreaRight() const
{
	return m_pRenderService->GetSafeAreaRight();
}

int FlatUI::GetSafeAreaBottom() const
{
	return m_pRenderService->GetSafeAreaBottom();
}

void FlatUI::SetVisible(bool value)
{ 
	m_bVisible = true;
}

bool FlatUI::GetVisible() const
{ 
	return m_bVisible;
}

void FlatUI::SetCaretColor(const char* value)
{
	Assert(value != NULL);

	m_nCaretColor = flat_string_to_rgba(value);
}

ArgString FlatUI::GetCaretColor() const
{
	char buffer[32];

	return flat_rgba_to_string(m_nCaretColor, buffer, sizeof(buffer));
}

void FlatUI::SetConfigPath(const char* value)
{
	Assert(value != NULL);

	m_sConfigPath = value;
}

const char* FlatUI::GetConfigPath() const
{
	return m_sConfigPath.CString();
}

flat_metadata_t* FlatUI::GetMetadata(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	const char* name = pWidget->GetDesc()->GetObjectName();
	MetadataTable::iterator it = m_MetadataTable.Find(name);

	if (it == m_MetadataTable.End())
	{
		flat_metadata_t* pMD = flat_metadata_create();

		pWidget->SetMetadata(pMD);
		m_MetadataTable.Add(name, pMD);
		return pMD;
	}
	else
	{
		return it.GetData();
	}
}

uniqid_t FlatUI::GetMeta(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	flat_metadata_t* pMD = this->GetMetadata(pWidget);
	uniqid_t meta_id = flat_metadata_get_object(pMD);
	FlatMeta* pMeta = (FlatMeta*)frame_get_object(meta_id);

	if (NULL == pMeta)
	{
		pMeta = (FlatMeta*)frame_create_object("FlatMeta");
		pMeta->SetMetadata(pMD);
		meta_id = pMeta->GetUID();
		flat_metadata_set_object(pMD, meta_id);
	}

	return meta_id;
}

FlatWidget* FlatUI::CreateWidget(const char* name)
{
	Assert(name != NULL);

	FlatWidget* pWidget = (FlatWidget*)frame_create_object_args(name, 
		FrameArgList() << this->GetUID());

	if (NULL == pWidget)
	{
		return NULL;
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatWidget"))
	{
		pWidget->Release();
		return NULL;
	}

	return pWidget;
}

uniqid_t FlatUI::Create(const char* name)
{
	FlatWidget* pWidget = this->CreateWidget(name);

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	return pWidget->GetUID();
}

bool FlatUI::Delete(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}

	FlatWidget* pParent = pWidget->GetParent();

	if (pParent)
	{
		flat_container_t* pContainer = pParent->GetContainer();

		if (pContainer)
		{
			flat_container_remove(pContainer, pWidget);
		}
	}

	pWidget->Release();
	return true;
}

static FlatWidget* get_part_widget(FlatWidget* pWidget, const char* part_name)
{
	Assert(pWidget != NULL);
	Assert(part_name != NULL);

	FrameArgData prop_value;

	if (!frame_get_property(pWidget, part_name, &prop_value))
	{
		return NULL;
	}

	if (prop_value.GetType() != DT_UNIQID)
	{
		return NULL;
	}

	return flat_get_widget(prop_value.GetUniqid());
}

static void clone_widget(FlatUI* pFlatUI, FlatWidget* pWidget,
	FlatWidget* pSrcWidget)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);
	Assert(pSrcWidget != NULL);

	FrameArgList prop_names;
	FrameArgData prop_value;
	IObjectDesc* pSrcDesc = pSrcWidget->GetDesc();

	pSrcDesc->GetPropertyAll(&prop_names);

	for (size_t i = 0; i < prop_names.GetCount(); ++i)
	{
		const char* prop_name = prop_names.GetString(i);

		if (frame_get_property(pSrcWidget, prop_name, &prop_value))
		{
			frame_set_property(pWidget, prop_name, prop_value);
		}
	}

	// Clone part widget.
	flat_metadata_t* pMD = pFlatUI->GetMetadata(pSrcWidget);
	size_t part_num = flat_metadata_part_count(pMD);

	for (size_t k = 0; k < part_num; ++k)
	{
		const char* part_name = flat_metadata_part_name(pMD, k);
		FlatWidget* pSrcPart = get_part_widget(pSrcWidget, part_name);
		FlatWidget* pDstPart = get_part_widget(pWidget, part_name);

		if ((NULL == pSrcPart) || (NULL == pDstPart))
		{
			continue;
		}

		clone_widget(pFlatUI, pDstPart, pSrcPart);
	}
}

uniqid_t FlatUI::Clone(const uniqid_t& id)
{
	FlatWidget* pSrcWidget = flat_get_widget(id);

	if (NULL == pSrcWidget)
	{
		return uniqid_t();
	}

	FlatWidget* pWidget = this->CreateWidget(
		pSrcWidget->GetDesc()->GetObjectName());

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	clone_widget(this, pWidget, pSrcWidget);
	return pWidget->GetUID();
}

FlatWidget* FlatUI::CreatePartWidget(FlatWidget* pParent, const char* name)
{
	Assert(pParent != NULL);
	Assert(name != NULL);

	FlatWidget* pWidget = (FlatWidget*)frame_create_object_args(name,
		FrameArgList() << this->GetUID());

	if (NULL == pWidget)
	{
		return NULL;
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatWidget"))
	{
		pWidget->Release();
		return NULL;
	}

	flat_parts_t* pParts = pParent->GetParts();

	Assert(pParts != NULL);

	flat_parts_add(pParts, pWidget);
	pWidget->SetDelegator(pParent);
	return pWidget;
}

void FlatUI::DeletePartWidget(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	FlatWidget* pParent = pWidget->GetParent();

	if (pParent)
	{
		flat_parts_t* pParts = pParent->GetParts();

		if (pParts)
		{
			flat_parts_remove(pParts, pWidget);
		}
	}

	pWidget->Release();
}

bool FlatUI::AddPlayWidget(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	m_Plays.PushBack(pWidget->GetUID());
	return true;
}

bool FlatUI::RemovePlayWidget(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	uniqid_t id = pWidget->GetUID();

	for (size_t i = 0; i < m_Plays.Size(); ++i)
	{
		if (uniqid_equal(m_Plays[i], id))
		{
			m_Plays.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatUI::AddPlay(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}
	
	return this->AddPlayWidget(pWidget);
}

bool FlatUI::RemovePlay(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}

	return this->RemovePlayWidget(pWidget);
}

bool FlatUI::AddDialog(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	uniqid_t id = pWidget->GetUID();

	for (size_t i = 0; i < m_DialogStack.Size(); ++i)
	{
		if (uniqid_equal(m_DialogStack[i], id))
		{
			m_DialogStack.Remove(i);
			break;
		}
	}

	m_DialogStack.PushBack(id);
	return true;
}

bool FlatUI::RemoveDialog(FlatWidget* pWidget)
{
	Assert(pWidget != NULL);

	uniqid_t id = pWidget->GetUID();

	for (size_t i = 0; i < m_DialogStack.Size(); ++i)
	{
		if (uniqid_equal(m_DialogStack[i], id))
		{
			m_DialogStack.Remove(i);
			return true;
		}
	}
	
	return false;
}

FlatWidget* FlatUI::GetTopDialog() const
{
	if (m_DialogStack.Empty())
	{
		return NULL;
	}

	uniqid_t id = m_DialogStack.Back();

	return flat_get_widget(id);
}

uniqid_t FlatUI::GetTopDialogID() const
{
	if (m_DialogStack.Empty())
	{
		return uniqid_t();
	}

	return m_DialogStack.Back();
}

bool FlatUI::AddFloating(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	if (NULL == pWidget)
	{
		return false;
	}
	
	m_Floatings.PushBack(id);
	pWidget->OnPopup();
	return true;
}

bool FlatUI::RemoveFloating(const uniqid_t& id)
{
	for (size_t i = 0; i < m_Floatings.Size(); ++i)
	{
		if (uniqid_equal(m_Floatings[i], id))
		{
			m_Floatings.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatUI::FindFloating(const uniqid_t& id)
{
	for (size_t i = 0; i < m_Floatings.Size(); ++i)
	{
		if (uniqid_equal(m_Floatings[i], id))
		{
			return true;
		}
	}

	return false;
}

bool FlatUI::ClearFloatings()
{
	m_Floatings.Clear();
	return true;
}

bool FlatUI::AddCursor(const uniqid_t& id)
{
	FlatCursor* pCursor = (FlatCursor*)frame_get_object(id);

	if (NULL == pCursor)
	{
		return false;
	}

	if (!pCursor->GetDesc()->DeriveFrom("FlatCursor"))
	{
		return false;
	}

	m_Cursors.PushBack(pCursor);
	return true;
}

bool FlatUI::RemoveCursor(const char* name)
{
	Assert(name != NULL);

	unsigned int name_hash = util_string_hash(name);

	for (size_t i = 0; i < m_Cursors.Size(); ++i)
	{
		FlatCursor* pCursor = m_Cursors[i];
		
		if ((pCursor->GetNameHash() == name_hash) &&
			(strcmp(pCursor->GetName(), name) == 0))
		{
			if (m_pCurrentCursor == pCursor)
			{
				m_pCurrentCursor = NULL;
			}

			pCursor->Release();
			m_Cursors.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatUI::SetCurrentCursor(const char* name)
{
	Assert(name != NULL);

	if (util_string_empty(name))
	{
		name = "default";
	}

	unsigned int name_hash = util_string_hash(name);

	if (m_pCurrentCursor)
	{
		if ((m_pCurrentCursor->GetNameHash() == name_hash) &&
			(strcmp(m_pCurrentCursor->GetName(), name) == 0))
		{
			return true;
		}
	}

	for (size_t i = 0; i < m_Cursors.Size(); ++i)
	{
		FlatCursor* pCursor = m_Cursors[i];

		if ((pCursor->GetNameHash() == name_hash) &&
			(strcmp(pCursor->GetName(), name) == 0))
		{
			m_pCurrentCursor = pCursor;
			pCursor->Apply();
			return true;
		}
	}
	
	return false;
}

bool FlatUI::ShowCurrentCursor()
{
	return flat_show_cursor();
}

bool FlatUI::HideCurrentCursor()
{
	return flat_hide_cursor();
}

void FlatUI::GetCursorList(const IArgList& args, IArgList* res)
{
	res->Clear();
	
	for (size_t i = 0; i < m_Cursors.Size(); ++i)
	{
		FlatCursor* pCursor = m_Cursors[i];

		res->AddString(pCursor->GetName());
	}
}

bool FlatUI::AddFont(const uniqid_t& id)
{
	FlatFont* pFont = (FlatFont*)frame_get_object(id);

	if (NULL == pFont)
	{
		return false;
	}

	if (!pFont->GetDesc()->DeriveFrom("FlatFont"))
	{
		return false;
	}

	if (!pFont->Load(m_pCanvas))
	{
		return false;
	}
	
	m_Fonts.PushBack(pFont);
	return true;
}

bool FlatUI::RemoveFont(const char* name)
{
	Assert(name != NULL);

	unsigned int name_hash = util_string_hash(name);

	for (size_t i = 0; i < m_Fonts.Size(); ++i)
	{
		FlatFont* pFont = m_Fonts[i];

		if ((pFont->GetNameHash() == name_hash) &&
			(strcmp(pFont->GetName(), name) == 0))
		{
			if (m_pCurrentFont == pFont)
			{
				m_pCurrentFont = NULL;
			}

			pFont->Release();
			m_Fonts.Remove(i);
			canvas_remove_font(m_pCanvas, name);
			return true;
		}
	}
	
	return false;
}

bool FlatUI::SetCurrentFont(const char* name)
{
	Assert(name != NULL);

	unsigned int name_hash = util_string_hash(name);

	if (m_pCurrentFont)
	{
		if ((m_pCurrentFont->GetNameHash() == name_hash) &&
			(strcmp(m_pCurrentFont->GetName(), name) == 0))
		{
			// Not change.
			return true;
		}
	}

	for (size_t i = 0; i < m_Fonts.Size(); ++i)
	{
		FlatFont* pFont = m_Fonts[i];

		if ((pFont->GetNameHash() == name_hash) &&
			(strcmp(pFont->GetName(), name) == 0))
		{
			if (!canvas_set_font(m_pCanvas, pFont->GetFontType()))
			{
				return false;
			}

			m_pCurrentFont = pFont;
			return true;
		}
	}

	return false;
}

void FlatUI::GetFontList(const IArgList& args, IArgList* res)
{
	res->Clear();

	for (size_t i = 0; i < m_Fonts.Size(); ++i)
	{
		FlatFont* pFont = m_Fonts[i];

		res->AddString(pFont->GetName());
	}
}

bool FlatUI::AddContextMenu(const uniqid_t& id)
{
	FlatMenu* pMenu = (FlatMenu*)frame_get_object(id);

	if (NULL == pMenu)
	{
		return false;
	}

	if (!pMenu->GetDesc()->DeriveFrom("FlatMenu"))
	{
		return false;
	}
	
	m_ContextMenus.PushBack(pMenu);
	return true;
}

bool FlatUI::RemoveContextMenu(const char* name)
{
	Assert(name != NULL);
	
	unsigned int hash = util_string_hash(name);

	for (size_t i = 0; i < m_ContextMenus.Size(); ++i)
	{
		FlatMenu* pMenu = m_ContextMenus[i];

		if ((pMenu->GetNameHash() == hash) &&
			(strcmp(pMenu->GetName(), name) == 0))
		{
			pMenu->Release();
			m_ContextMenus.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatUI::ClearContextMenus()
{
	for (size_t i = 0; i < m_ContextMenus.Size(); ++i)
	{
		FlatMenu* pMenu = m_ContextMenus[i];

		pMenu->Release();
	}

	m_ContextMenus.Clear();
	return true;
}

FlatMenu* FlatUI::GetContextMenu(const char* name)
{
	Assert(name != NULL);

	unsigned int hash = util_string_hash(name);

	for (size_t i = 0; i < m_ContextMenus.Size(); ++i)
	{
		FlatMenu* pMenu = m_ContextMenus[i];

		if ((pMenu->GetNameHash() == hash) &&
			(strcmp(pMenu->GetName(), name) == 0))
		{
			return pMenu;
		}
	}

	return NULL;
}

bool FlatUI::AddHintLabel(const uniqid_t& id)
{
	FlatLabel* pLabel = (FlatLabel*)frame_get_object(id);

	if (NULL == pLabel)
	{
		return false;
	}

	if (!pLabel->GetDesc()->DeriveFrom("FlatLabel"))
	{
		return false;
	}

	m_HintLabels.PushBack(pLabel);
	return true;
}

bool FlatUI::RemoveHintLabel(const char* name)
{
	Assert(name != NULL);

	unsigned int hash = util_string_hash(name);

	for (size_t i = 0; i < m_HintLabels.Size(); ++i)
	{
		FlatLabel* pLabel = m_HintLabels[i];

		if ((pLabel->GetNameHash() == hash) &&
			(strcmp(pLabel->GetName(), name) == 0))
		{
			pLabel->Release();
			m_HintLabels.Remove(i);
			return true;
		}
	}

	return false;
}

bool FlatUI::ClearHintLabels()
{
	for (size_t i = 0; i < m_HintLabels.Size(); ++i)
	{
		FlatLabel* pLabel = m_HintLabels[i];

		pLabel->Release();
	}

	m_HintLabels.Clear();
	return true;
}

FlatLabel* FlatUI::GetHintLabel(const char* name)
{
	Assert(name != NULL);

	if (util_string_empty(name))
	{
		if (m_HintLabels.Empty())
		{
			return NULL;
		}

		return m_HintLabels[0];
	}

	unsigned int hash = util_string_hash(name);

	for (size_t i = 0; i < m_HintLabels.Size(); ++i)
	{
		FlatLabel* pLabel = m_HintLabels[i];

		if ((pLabel->GetNameHash() == hash) &&
			(strcmp(pLabel->GetName(), name) == 0))
		{
			return pLabel;
		}
	}

	return NULL;
}

//bool FlatUI::AddScrollStyle(const uniqid_t& id)
//{
//	FlatWidget* pWidget = flat_get_widget(id);
//
//	if (NULL == pWidget)
//	{
//		return false;
//	}
//
//	flat_scroll_t* pScroll = pWidget->GetScroll();
//
//	if (NULL == pScroll)
//	{
//		return false;
//	}
//	
//	m_ScrollStyles.PushBack(pWidget);
//	return true;
//}

//bool FlatUI::RemoveScrollStyle(const char* name)
//{
//	Assert(name != NULL);
//
//	unsigned int hash = util_string_hash(name);
//
//	for (size_t i = 0; i < m_ScrollStyles.Size(); ++i)
//	{
//		FlatWidget* pWidget = m_ScrollStyles[i];
//
//		if ((pWidget->GetNameHash() == hash) &&
//			(strcmp(pWidget->GetName(), name) == 0))
//		{
//			m_ScrollStyles.Remove(i);
//			return true;
//		}
//	}
//	
//	return false;
//}

//bool FlatUI::ClearScrollStyle()
//{
//	for (size_t t = 0; t < m_ScrollStyles.Size(); ++t)
//	{
//		m_ScrollStyles[t]->Release();
//	}
//
//	m_ScrollStyles.Clear();
//	return true;
//}

//FlatWidget* FlatUI::GetScrollStyle(const char* name) const
//{
//	Assert(name != NULL);
//
//	unsigned int hash = util_string_hash(name);
//
//	for (size_t i = 0; i < m_ScrollStyles.Size(); ++i)
//	{
//		FlatWidget* pWidget = m_ScrollStyles[i];
//
//		if ((pWidget->GetNameHash() == hash) &&
//			(strcmp(pWidget->GetName(), name) == 0))
//		{
//			return pWidget;
//		}
//	}
//
//	return NULL;
//}

// Get full path of asset file.
static const char* get_asset_name(const char* file_name,
	const char* config_path, char* buffer, size_t size)
{
	Assert(file_name != NULL);
	Assert(config_path != NULL);
	Assert(buffer != NULL);
	Assert(size > 0);

	bool is_full_path = false;

	for (const char* s = file_name; *s; ++s)
	{
		char ch = *s;

		if ((ch == '\\') || (ch == '/'))
		{
			is_full_path = true;
			break;
		}
		else if ((ch == ':') || (ch == ' ') || (ch == '\t'))
		{
			continue;
		}
		else if (s[1] == ':')
		{
			continue;
		}
		else
		{
			break;
		}
	}

	if (is_full_path)
	{
		return file_name;
	}

	plat_safe_sprintf(buffer, size, "%s%s", config_path, file_name);
	return buffer;
}

uniqid_t FlatUI::LoadForm(const char* file_name)
{
	Assert(file_name != NULL);

	char buffer[512];
	const char* asset_name = get_asset_name(file_name, m_sConfigPath.CString(),
		buffer, sizeof(buffer));
	FlatWidget* pWidget = flat_load_config(this, asset_name, false);

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatForm"))
	{
		K_LOG("(FlatUI::LoadForm)Root widget not FlatForm.");
		K_LOG(file_name);
		return uniqid_t();
	}

	return pWidget->GetUID();
}

uniqid_t FlatUI::LoadDesignForm(const char* file_name)
{
	Assert(file_name != NULL);

	char buffer[512];
	const char* asset_name = get_asset_name(file_name, m_sConfigPath.CString(),
		buffer, sizeof(buffer));
	FlatWidget* pWidget = flat_load_config(this, asset_name, true);

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatForm"))
	{
		K_LOG("(FlatUI::LoadDesignForm)Root widget not FlatForm.");
		K_LOG(file_name);
		return uniqid_t();
	}

	return pWidget->GetUID();
}

bool FlatUI::SaveForm(const uniqid_t& form_id, const char* file_name)
{
	Assert(file_name != NULL);

	FlatWidget* pWidget = flat_get_widget(form_id);

	if (NULL == pWidget)
	{
		return false;
	}

	if (!pWidget->GetDesc()->DeriveFrom("FlatForm"))
	{
		K_LOG("(FlatUI::SaveForm)Widget not FlatForm.");
		K_LOG(file_name);
		return false;
	}

	char buffer[512];
	const char* asset_name = get_asset_name(file_name, m_sConfigPath.CString(),
		buffer, sizeof(buffer));
	return flat_save_config(this, pWidget, asset_name);
}

uniqid_t FlatUI::LoadWidget(const char* file_name)
{
	Assert(file_name != NULL);

	char buffer[512];
	const char* asset_name = get_asset_name(file_name, m_sConfigPath.CString(),
		buffer, sizeof(buffer));
	FlatWidget* pWidget = flat_load_config(this, asset_name, false);

	if (NULL == pWidget)
	{
		return uniqid_t();
	}

	return pWidget->GetUID();
}

bool FlatUI::SaveWidget(const uniqid_t& widget_id, const char* file_name)
{
	Assert(file_name != NULL);

	FlatWidget* pWidget = flat_get_widget(widget_id);

	if (NULL == pWidget)
	{
		return false;
	}

	char buffer[512];
	const char* asset_name = get_asset_name(file_name, m_sConfigPath.CString(),
		buffer, sizeof(buffer));
	return flat_save_config(this, pWidget, asset_name);
}

bool FlatUI::OpenFile(const char* file_name)
{
	Assert(file_name != NULL);

#ifdef K_PLATFORM_WINDOWS
	HINSTANCE result = ShellExecute(NULL, "open", file_name, NULL, NULL,
		SW_SHOWNORMAL);

	return (size_t)result > 32;
#else
	return false;
#endif // K_PLATFORM_WINDOWS
}

bool FlatUI::DrawCaret(flat_canvas_t* pCanvas, int x, int y, int height)
{
	Assert(pCanvas != NULL);

	if (m_bCaretVisible)
	{
		canvas_draw_block(pCanvas, x, y, x + 2, y + height, m_nCaretColor);
	}

	return true;
}

bool FlatUI::ResetCaret()
{
	m_fCaretCounter = 0.0F;
	m_bCaretVisible = true;
	return true;
}

bool FlatUI::FocusToWidget(FlatWidget* pWidget)
{
	return flat_change_focus(m_pState, pWidget);
}

bool FlatUI::FocusTo(const uniqid_t& id)
{
	FlatWidget* pWidget = flat_get_widget(id);

	return flat_change_focus(m_pState, pWidget);
}

uniqid_t FlatUI::GetCurrentFocused()
{
	return m_pState->FocusedID;
}

int FlatUI::GetTextWidth(const char* font, const char* text, int start, 
	int char_num)
{
	Assert(font != NULL);
	Assert(text != NULL);

	return canvas_get_text_width(m_pCanvas, text, 0, char_num);
}

int FlatUI::ProcessInput(const void* pMsg)
{
	Assert(pMsg != NULL);

	const IInputService::input_t* pInput = (const IInputService::input_t*)pMsg;
	
	switch (pInput->nType)
	{
	case IInputService::TYPE_MOUSE_MOVE:
	case IInputService::TYPE_MOUSE_WHEEL:
	case IInputService::TYPE_MOUSE_LEAVE:
	case IInputService::TYPE_LEFT_DOWN:
	case IInputService::TYPE_LEFT_UP:
	case IInputService::TYPE_LEFT_DUAL:
	case IInputService::TYPE_RIGHT_DOWN:
	case IInputService::TYPE_RIGHT_UP:
	case IInputService::TYPE_RIGHT_DUAL:
	case IInputService::TYPE_MIDDLE_DOWN:
	case IInputService::TYPE_MIDDLE_UP:
	case IInputService::TYPE_MIDDLE_DUAL:
	case IInputService::TYPE_KEY_DOWN:
	case IInputService::TYPE_KEY_UP:
	case IInputService::TYPE_CHAR:
	case IInputService::TYPE_ACTIVE:
	case IInputService::TYPE_INACTIVE:
		return flat_state_machine(m_pState, FLAT_EVENT_INPUT, FrameArgList(),
			pMsg);
	case IInputService::TYPE_CURSOR:
	{
		if (NULL == m_pCurrentCursor)
		{
			break;
		}

		if (m_pCurrentCursor->Apply())
		{
			return 1;
		}

		break;
	}
	case IInputService::TYPE_CLOSE:
	{
		FrameArgList res;

		if (frame_process_event(this, 0, "close", FrameArgList(), &res))
		{
			return res.GetInt32(0);
		}

		break;
	}
	default:
		break;
	}
	
	return 0;
}
