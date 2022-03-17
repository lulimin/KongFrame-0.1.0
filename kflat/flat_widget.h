// flat_widget.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_WIDGET_H
#define __FLAT_WIDGET_H

#include "../inc/common.h"
#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "flat_container.h"
#include "flat_parts.h"
#include "flat_scroll.h"
#include "flat_canvas.h"
#include "flat_image.h"
#include "flat_utility.h"
#include "flat_metadata.h"

// Base class of user interface widget.

class FlatUI;

class FlatWidget : public IObject
{
public:
	FlatWidget()
	{
		m_pFlatUI = NULL;
		m_pParent = NULL;
		m_pDelegator = NULL;
		m_pContainer = NULL;
		m_pParts = NULL;
		m_pScroll = NULL;
		m_pImage = NULL;
		m_nNameHash = 0;
		m_nLeft = 0;
		m_nTop = 0;
		m_nWidth = 32;
		m_nHeight = 32;
		m_nTabIndex = 0;
		m_nImageLayout = 0;
		m_nForeColor = 0xFFFFFFFF;
		m_nBackColor = 0xFF000000;
		m_bVisible = true;
		m_bEnabled = true;
		m_bAutoSize = false;
		m_bCanFocus = false;
		m_bCanSelect = true;
		m_bCapture = false;
		m_bFocused = false;
		m_bDesignMode = false;
	}

	virtual ~FlatWidget()
	{
		if (m_pContainer)
		{
			flat_container_delete(m_pContainer);
		}

		if (m_pParts)
		{
			flat_parts_delete(m_pParts);
		}

		if (m_pScroll)
		{
			flat_scroll_delete(m_pScroll);
		}

		if (m_pImage)
		{
			flat_image_delete(m_pImage);
		}
	}

	// Object startup.
	virtual bool Startup(const IArgList& args)
	{
		FlatUI* pFlatUI = (FlatUI*)frame_get_object(args.GetUniqid(0));

		if (NULL == pFlatUI)
		{
			return false;
		}

		m_pFlatUI = pFlatUI;
		return true;
	}

	// Object shutdown.
	virtual bool Shutdown()
	{
		if (m_pParts)
		{
			flat_parts_erase_all(m_pParts);
		}

		if (m_pContainer)
		{
			flat_container_erase_all(m_pContainer);
		}

		return true;
	}

	// Is contianer.
	virtual bool IsContainer()
	{ 
		return m_pContainer != NULL;
	}

	// Test has children.
	virtual bool HasChildren() 
	{ 
		if (NULL == m_pContainer)
		{
			return false;
		}
		
		return !flat_container_empty(m_pContainer);
	}

	// Get minimum widget size.
	virtual int GetMinimumWidth() { return 1; }
	virtual int GetMinimumHeight() { return 1; }
	
	// Flush per frame.
	virtual void Play(float frame_time) {}
	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas) {}

	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD)
	{
		Assert(pMD != NULL);

		FrameArgData def_zero_int32(DT_INT32, 0);
		FrameArgData def_true(DT_BOOLEAN, true);
		FrameArgData def_false(DT_BOOLEAN, false);
		FrameArgData def_empty_string(DT_STRING, "");
		FrameArgData def_fore_color(DT_STRING, "255,255,255,255");
		FrameArgData def_back_color(DT_STRING, "0,0,0,255");

		flat_metadata_add_prop(pMD, "Left", FLAT_METATYPE_INT32, NULL);
		flat_metadata_add_prop(pMD, "Top", FLAT_METATYPE_INT32, NULL);
		flat_metadata_add_prop(pMD, "Width", FLAT_METATYPE_INT32, NULL);
		flat_metadata_add_prop(pMD, "Height", FLAT_METATYPE_INT32, NULL);
		flat_metadata_add_prop(pMD, "TabIndex", FLAT_METATYPE_INT32,
			&def_zero_int32);
		flat_metadata_add_prop(pMD, "Enabled", FLAT_METATYPE_BOOLEAN, 
			&def_true);
		flat_metadata_add_prop(pMD, "AutoSize", FLAT_METATYPE_BOOLEAN,
			&def_false);
		flat_metadata_add_prop(pMD, "Font", FLAT_METATYPE_FONT,
			&def_empty_string);
		flat_metadata_add_prop(pMD, "Cursor", FLAT_METATYPE_CURSOR,
			&def_empty_string);
		flat_metadata_add_prop(pMD, "Hint", FLAT_METATYPE_STRING,
			&def_empty_string);
		flat_metadata_add_prop(pMD, "ContextMenu", FLAT_METATYPE_STRING,
			&def_empty_string);
		flat_metadata_add_prop(pMD, "ForeColor", FLAT_METATYPE_COLOR,
			&def_fore_color);
		flat_metadata_add_prop(pMD, "BackColor", FLAT_METATYPE_COLOR,
			&def_back_color);
		flat_metadata_add_prop(pMD, "BackImage", FLAT_METATYPE_IMAGE,
			&def_empty_string);
		flat_metadata_add_prop(pMD, "ImageLayout", FLAT_METATYPE_STRING,
			&def_empty_string);
	}

	// Get coordinate in which widget.
	virtual FlatWidget* GetRegionOf(int x, int y)
	{
		if (!m_bVisible)
		{
			return NULL;
		}

		if (!this->InSelfRegion(x, y))
		{
			// Not in region of widget.
			return NULL;
		}

		FlatWidget* pWidget = this->InChildRegion(x, y);

		if (pWidget)
		{
			return pWidget;
		}

		if (m_bDesignMode)
		{
			// In design status.
			return this;
		}

		pWidget = this->InPartRegion(x, y);

		if (pWidget)
		{
			return pWidget;
		}

		if (this->OnTestTransparency(x, y))
		{
			// Is transparency part.
			return NULL;
		}

		return this;
	}

	// Scroll related.
	virtual void DrawScrollRegion(flat_canvas_t* pCanvas) {}
	virtual int GetVerticalWhole() { return 0; }
	virtual int GetVerticalReveal() { return 0; }
	virtual int GetVerticalBegin() { return 0; }
	virtual void SetVerticalBegin(int value) {}
	virtual int GetHorizontalWhole() { return 0; }
	virtual int GetHorizontalReveal() { return 0; }
	virtual int GetHorizontalBegin() { return 0; }
	virtual void SetHorizontalBegin(int value) {}

	// Event handlers.
	virtual int OnClickReset() { return 0; }
	virtual int OnMouseMove(int x, int y, unsigned int flags) { return 0; }
	virtual int OnMouseWheel(int x, int y, double delta, 
		unsigned int flags) { return 0; }
	virtual int OnMouseHover(int x, int y) { return 0; }
	virtual int OnLeftDown(int x, int y, unsigned int flags) { return 0; }
	virtual int OnLeftUp(int x, int y, unsigned int flags) { return 0; }
	virtual int OnLeftDual(int x, int y, unsigned int flags) { return 0; }
	virtual int OnLeftDrag(int x, int y, int gcx, int gcy, 
		unsigned int flags) { return 0; }
	virtual int OnLeftDragUp(int x, int y, int gcx, int gcy, 
		unsigned int flags) { return 0; }
	virtual int OnKeyDown(unsigned int key, unsigned int flags) { return 0; }
	virtual int OnKeyUp(unsigned int key, unsigned int flags) { return 0; }
	virtual int OnChar(unsigned int ch, unsigned int flags) { return 0; }
	virtual int OnGotCapture() { return 0; }
	virtual int OnLostCapture() { return 0; }
	virtual int OnGotFocus() { return 0; }
	virtual int OnLostFocus() { return 0; }
	virtual int OnPopup() { return 0; }
	virtual int OnTestTransparency(int x, int y) { return 0; }
	virtual int OnContextMenu(const uniqid_t& menu_id) { return 0; }
	virtual int OnSizeChanged() { return 0; }
	virtual int OnPartClick(FlatWidget* pPart, int x, int y) { return 0; }
	virtual int OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
		unsigned int flags) { return 0; }
	virtual int OnPartSelect(FlatWidget* pPart, int new_value,
		int old_value) { return 0; }

	// Return hint information.
	virtual const char* OnDisplayHint(int x, int y)
	{
		return m_sHint.CString();
	}

	// Return context menu name.
	virtual const char* OnDisplayContextMenu(int x, int y)
	{
		return m_sContextMenu.CString();
	}
	
	// Get coordinate in which widget region.
	uniqid_t GetInRegion(int x, int y)
	{
		FlatWidget* pWidget = this->GetRegionOf(x, y);

		if (NULL == pWidget)
		{
			return uniqid_t();
		}

		return pWidget->GetUID();
	}

	// Test coordinate in region of widget.
	bool InSelfRegion(int x, int y) const
	{
		int x1 = this->GetGlobalLeft();
		int y1 = this->GetGlobalTop();
		int x2 = x1 + m_nWidth;
		int y2 = y1 + m_nHeight;

		return (x >= x1) && (x < x2) && (y >= y1) && (y < y2);
	}

	// Get in which child region.
	FlatWidget* InChildRegion(int x, int y)
	{
		if (m_pContainer)
		{
			size_t size = flat_container_get_size(m_pContainer);
			FlatWidget** data = flat_container_get_data(m_pContainer);

			for (size_t i = 0; i < size; ++i)
			{
				FlatWidget* pChild = data[size - 1 - i];

				if (NULL == pChild)
				{
					continue;
				}

				FlatWidget* pWidget = pChild->GetRegionOf(x, y);

				if (pWidget)
				{
					return pWidget;
				}
			}
		}
		
		return NULL;
	}

	// Get in which part widget.
	FlatWidget* InPartRegion(int x, int y)
	{
		if (m_pParts)
		{
			size_t size = flat_parts_get_size(m_pParts);
			FlatWidget** data = flat_parts_get_data(m_pParts);

			for (size_t i = 0; i < size; ++i)
			{
				FlatWidget* pPart = data[size - 1 - i];

				if (NULL == pPart)
				{
					continue;
				}

				FlatWidget* pWidget = pPart->GetRegionOf(x, y);

				if (pWidget)
				{
					return pWidget;
				}
			}
		}
		
		return NULL;
	}

	// Set widget name.
	void SetName(const char* value)
	{
		Assert(value != NULL);

		m_sName = value;
		m_nNameHash = util_string_hash(value);
	}

	// Get widget name.
	const char* GetName() const
	{
		return m_sName.CString();
	}

	// Get hash value of name.
	unsigned int GetNameHash() const
	{
		return m_nNameHash;
	}

	// Set widget font.
	void SetFont(const char* value)
	{
		Assert(value != NULL);

		m_sFont = value;
	}

	// Get widget font.
	const char* GetFont() const
	{
		return m_sFont.CString();
	}

	// Get available font name.
	const char* GetAvailableFont()
	{
		if (m_sFont.Empty())
		{
			if (m_pParent)
			{
				return m_pParent->GetAvailableFont();
			}
			else
			{
				return "default";
			}
		}
		else
		{
			return m_sFont.CString();
		}
	}

	// Set cursor name.
	void SetCursor(const char* value)
	{
		Assert(value != NULL);

		m_sCursor = value;
	}

	// Get cursor name.
	const char* GetCursor() const
	{
		return m_sCursor.CString();
	}

	// Set hint information.
	void SetHint(const char* value)
	{
		Assert(value != NULL);

		m_sHint = value;
	}

	// Get hint information.
	const char* GetHint() const
	{
		return m_sHint.CString();
	}

	// Set name of context menu.
	void SetContextMenu(const char* value)
	{
		Assert(value != NULL);

		m_sContextMenu = value;
	}

	// Get name of context menu.
	const char* GetContextMenu() const
	{
		return m_sContextMenu.CString();
	}

	// Set left position.
	void SetLeft(int value)
	{
		m_nLeft = value;
	}

	// Get left position.
	int GetLeft() const
	{
		return m_nLeft;
	}

	// Get right position.
	int GetRight() const
	{
		return m_nLeft + m_nWidth;
	}

	// Set top position.
	void SetTop(int value)
	{
		m_nTop = value;
	}

	// Get top position.
	int GetTop() const
	{
		return m_nTop;
	}

	// Get bottom position.
	int GetBottom() const
	{
		return m_nTop + m_nHeight;
	}

	// Set widget size.
	void SetSize(int width, int height)
	{
		if (width < this->GetMinimumWidth())
		{
			width = this->GetMinimumWidth();
		}
		
		if (height < this->GetMinimumHeight())
		{
			height = this->GetMinimumHeight();
		}

		m_nWidth = width;
		m_nHeight = height;
		this->OnSizeChanged();
	}

	// Set horizontal size.
	void SetWidth(int value)
	{
		if (value < this->GetMinimumWidth())
		{
			value = this->GetMinimumWidth();
		}

		m_nWidth = value;
		this->OnSizeChanged();
	}

	// Get horizontal size.
	int GetWidth() const
	{
		return m_nWidth;
	}

	// Set vertical size.
	void SetHeight(int value)
	{
		if (value < this->GetMinimumHeight())
		{
			value = this->GetMinimumHeight();
		}
		
		m_nHeight = value;
		this->OnSizeChanged();
	}

	// Get vertical size.
	int GetHeight() const
	{
		return m_nHeight;
	}

	// Set tab index.
	void SetTabIndex(int value)
	{
		m_nTabIndex = value;
	}

	// Get tab index.
	int GetTabIndex() const
	{
		return m_nTabIndex;
	}

	// Set visible flag.
	void SetVisible(bool value)
	{
		m_bVisible = value;
	}

	// Get visible flag.
	bool GetVisible() const
	{
		return m_bVisible;
	}

	// Set enabled flag.
	void SetEnabled(bool value)
	{
		m_bEnabled = value;
	}

	// Get enabled flag.
	bool GetEnabled() const
	{
		return m_bEnabled;
	}

	// Set auto fit size of background image.
	void SetAutoSize(bool value)
	{
		if (value == m_bAutoSize)
		{
			return;
		}
		
		m_bAutoSize = value;
	
		if (m_bAutoSize && (m_pImage != NULL))
		{
			int width = flat_image_get_width(m_pImage);
			int height = flat_image_get_height(m_pImage);

			if ((width > 0) && (height > 0))
			{
				this->SetSize(width, height);
			}
		}
	}

	// Get auto fit size of background image.
	bool GetAutoSize() const
	{
		return m_bAutoSize;
	}

	// Set can focus.
	void SetCanFocus(bool value)
	{
		m_bCanFocus = value;
	}

	// Get can focus.
	bool GetCanFocus() const
	{
		return m_bCanFocus;
	}

	// Set can select.
	void SetCanSelect(bool value)
	{
		m_bCanSelect = value;
	}

	// Get can select.
	bool GetCanSelect() const
	{
		return m_bCanSelect;
	}

	// Set capture mouse.
	void SetCapture(bool value)
	{
		m_bCapture = value;
	}

	// Get capture mouse.
	bool GetCapture() const
	{
		return m_bCapture;
	}

	// Set in focused status.
	void SetFocused(bool value)
	{
		m_bFocused = value;
	}

	// Get in focused status.
	bool GetFocused() const
	{
		return m_bFocused;
	}

	// Set in design status.
	void SetDesignMode(bool value)
	{
		m_bDesignMode = value;
	}

	// Get in design status.
	bool GetDesignMode() const
	{
		return m_bDesignMode;
	}

	// Set foreground color.
	void SetForeColor(const char* value)
	{
		Assert(value != NULL);

		m_nForeColor = flat_string_to_rgba(value);
	}

	// Get foreground color.
	ArgString GetForeColor() const
	{
		char buffer[32];
		
		return ArgString(flat_rgba_to_string(m_nForeColor, buffer, 
			sizeof(buffer)));
	}

	// Set foreground color value.
	void SetForeColorValue(unsigned int value)
	{
		m_nForeColor = value;
	}

	// Get foreground color value.
	unsigned int GetForeColorValue() const
	{
		return m_nForeColor;
	}

	// Set background color.
	void SetBackColor(const char* value)
	{
		Assert(value != NULL);

		m_nBackColor = flat_string_to_rgba(value);
	}

	// Get background color.
	ArgString GetBackColor() const
	{
		char buffer[32];
		
		return ArgString(flat_rgba_to_string(m_nBackColor, buffer, 
			sizeof(buffer)));
	}

	// Set background color value.
	void SetBackColorValue(unsigned int value)
	{
		m_nBackColor = value;
	}

	// Get background color value.
	unsigned int GetBackColorValue() const
	{
		return m_nBackColor;
	}

	// Set background image.
	void SetBackImage(const char* value)
	{
		Assert(value != NULL);

		m_sBackImage = value;
		m_pImage = flat_load_image(m_pFlatUI, m_pImage, value, false);

		if (m_bAutoSize && (m_pImage != NULL))
		{
			int width = flat_image_get_width(m_pImage);
			int height = flat_image_get_height(m_pImage);

			if ((width > 0) && (height > 0))
			{
				this->SetSize(width, height);
			}
		}
	}

	// Get background image.
	const char* GetBackImage() const
	{
		return m_sBackImage.CString();
	}

	// Set layout of background image.
	void SetImageLayout(const char* value)
	{
		Assert(value != NULL);

		if (util_string_empty(value))
		{
			m_nImageLayout = FLAT_LAYOUT_FIT;
		}
		else if (stricmp(value, "Patch") == 0)
		{
			m_nImageLayout = FLAT_LAYOUT_PATCH;
		}
		else if (stricmp(value, "HPatch") == 0)
		{
			m_nImageLayout = FLAT_LAYOUT_HPATCH;
		}
		else if (stricmp(value, "VPatch") == 0)
		{
			m_nImageLayout = FLAT_LAYOUT_VPATCH;
		}
	}

	// Get layout of background image.
	const char* GetImageLayout() const
	{
		switch (m_nImageLayout)
		{
		case FLAT_LAYOUT_FIT:
			return "";
		case FLAT_LAYOUT_PATCH:
			return "Patch";
		case FLAT_LAYOUT_HPATCH:
			return "HPatch";
		case FLAT_LAYOUT_VPATCH:
			return "VPatch";
		default:
			Assert(0);
			break;
		}
		
		return "";
	}

	// Get image layout value.
	int GetImageLayoutValue() const
	{
		return m_nImageLayout;
	}

	// Get flat user interface object.
	FlatUI* GetFlatUI() const
	{
		return m_pFlatUI;
	}

	// Get user interface object id.
	uniqid_t GetUI() const
	{
		return ((IObject*)m_pFlatUI)->GetUID();
	}

	// Set parent widget.
	void SetParent(FlatWidget* value)
	{
		m_pParent = value;
	}

	// Get parent widget.
	FlatWidget* GetParent() const
	{
		return m_pParent;
	}

	// Get parent widget id.
	uniqid_t GetParentID() const
	{
		if (NULL == m_pParent)
		{
			return uniqid_t();
		}

		return m_pParent->GetUID();
	}

	// Set delegate widget.
	void SetDelegator(FlatWidget* value)
	{
		m_pDelegator = value;
	}

	// Get delegate widget.
	FlatWidget* GetDelegator() const
	{
		return m_pDelegator;
	}

	// Get delegate widget id.
	uniqid_t GetDelegatorID() const
	{
		if (NULL == m_pDelegator)
		{
			return uniqid_t();
		}

		return m_pDelegator->GetUID();
	}

	// Create children container.
	void CreateContainer()
	{
		Assert(NULL == m_pContainer);

		m_pContainer = flat_container_create(this);
	}

	// Create part container.
	void CreateParts()
	{
		Assert(NULL == m_pParts);

		m_pParts = flat_parts_create(this);
	}

	// Create scrollable.
	void CreateScroll()
	{
		Assert(NULL == m_pScroll);

		m_pScroll = flat_scroll_create(this);
	}

	// Get children container.
	flat_container_t* GetContainer() const
	{
		return m_pContainer;
	}

	// Get part container.
	flat_parts_t* GetParts() const
	{
		return m_pParts;
	}

	// Get scrollable.
	flat_scroll_t* GetScroll() const
	{
		return m_pScroll;
	}

	// Get background image.
	flat_image_t* GetImage() const
	{
		return m_pImage;
	}

	// Get root widget.
	FlatWidget* GetRootWidget()
	{
		FlatWidget* pParent = this->GetParent();

		if (NULL == pParent)
		{
			return this;
		}

		return pParent->GetRootWidget();
	}

	// Get root delegator widget.
	FlatWidget* GetRootDelegator()
	{
		FlatWidget* pDelegator = this->GetDelegator();

		if (NULL == pDelegator)
		{
			return this;
		}

		return pDelegator->GetRootDelegator();
	}

	// Find form belong to.
	uniqid_t FindForm()
	{
		FlatWidget* pParent = this->GetParent();

		while (pParent)
		{
			if (pParent->GetDesc()->DeriveFrom("FlatForm"))
			{
				return pParent->GetUID();
			}

			pParent = pParent->GetParent();
		}

		return uniqid_t();
	}

	// Bring widget to foreground.
	bool BringToFront()
	{
		if (NULL == m_pParent)
		{
			return false;
		}

		flat_container_t* pChildren = m_pParent->GetContainer();

		if (NULL == pChildren)
		{
			return false;
		}

		return flat_container_to_front(pChildren, this);
	}

	// Send widget to background.
	bool SendToBack()
	{
		if (NULL == m_pParent)
		{
			return false;
		}

		flat_container_t* pChildren = m_pParent->GetContainer();

		if (NULL == pChildren)
		{
			return false;
		}

		return flat_container_to_back(pChildren, this);
	}

	// Get global left position.
	int GetGlobalLeft() const
	{
		if (NULL == m_pParent)
		{
			return m_nLeft;
		}

		return m_nLeft + m_pParent->GetGlobalLeft();
	}

	// Get global top position.
	int GetGlobalTop() const
	{
		if (NULL == m_pParent)
		{
			return m_nTop;
		}

		return m_nTop + m_pParent->GetGlobalTop();
	}

	// Set global left position.
	void SetGlobalLeft(int value)
	{
		if (m_pParent)
		{
			this->SetLeft(value - m_pParent->GetGlobalLeft());
		}
		else
		{
			this->SetLeft(value);
		}
	}

	// Set global top position.
	void SetGlobalTop(int value)
	{
		if (m_pParent)
		{
			this->SetTop(value - m_pParent->GetGlobalTop());
		}
		else
		{
			this->SetTop(value);
		}
	}

	// Set global position.
	void SetGlobalLeftTop(int left, int top)
	{
		if (m_pParent)
		{
			this->SetLeft(left - m_pParent->GetGlobalLeft());
			this->SetTop(top - m_pParent->GetGlobalTop());
		}
		else
		{
			this->SetLeft(left);
			this->SetTop(top);
		}
	}

	// Test background image exists.
	bool ExistBackImage() const
	{
		if (NULL == m_pImage)
		{
			return false;
		}

		return true;
	}

	// Draw background image.
	bool DrawBackImage(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2)
	{
		Assert(m_pImage != NULL);

		return canvas_draw_image(pCanvas, x1, y1, x2, y2, m_pImage, 
			m_nImageLayout);
	}

	// Fit rectangle to safe area.
	bool FitSafeArea()
	{
		return flat_fit_safe_area(m_pFlatUI, this);
	}

private:
	FlatUI* m_pFlatUI;
	FlatWidget* m_pParent;
	FlatWidget* m_pDelegator;
	flat_container_t* m_pContainer;
	flat_parts_t* m_pParts;
	flat_scroll_t* m_pScroll;
	flat_image_t* m_pImage;
	unsigned int m_nNameHash;
	FrameString m_sName;
	FrameString m_sFont;
	FrameString m_sCursor;
	FrameString m_sHint;
	FrameString m_sContextMenu;
	FrameString m_sBackImage;
	int m_nLeft;
	int m_nTop;
	int m_nWidth;
	int m_nHeight;
	int m_nTabIndex;
	int m_nImageLayout;
	unsigned int m_nForeColor;
	unsigned int m_nBackColor;
	bool m_bVisible;
	bool m_bEnabled;
	bool m_bAutoSize;
	bool m_bCanFocus;
	bool m_bCanSelect;
	bool m_bCapture;
	bool m_bFocused;
	bool m_bDesignMode;
};

#endif // __FLAT_WIDGET_H
