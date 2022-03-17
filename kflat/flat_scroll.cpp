// flat_scroll.cpp
// Created by lulimin on 2020/9/17.

#include "flat_scroll.h"
#include "flat_ui.h"
#include "flat_label.h"
#include "flat_v_scroll_bar.h"
#include "flat_h_scroll_bar.h"
#include "../inc/frame_mem.h"

// Scrollable data.
struct flat_scroll_t
{
	FlatWidget* pParent;
	FlatVScrollBar* pVScroll;
	FlatHScrollBar* pHScroll;
	FlatLabel* pCrossLabel;
	int nScrollBarSize;
	bool bVScrollAlways;
	bool bHScrollAlways;
	bool bDynamicSlide;
};

// Create scrollable.
flat_scroll_t* flat_scroll_create(FlatWidget* pParent)
{
	Assert(pParent != NULL);

	flat_scroll_t* pScroll = K_NEW(flat_scroll_t);

	pScroll->pParent = pParent;
	pScroll->pVScroll = NULL;
	pScroll->pHScroll = NULL;
	pScroll->pCrossLabel = NULL;
	pScroll->nScrollBarSize = 0;
	pScroll->bVScrollAlways = false;
	pScroll->bHScrollAlways = false;
	pScroll->bDynamicSlide = false;
	return pScroll;
}

// Delete scrollable.
void flat_scroll_delete(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	K_DELETE(pScroll);
}

// Check cross label.
static void check_cross_label(flat_scroll_t* pScroll, FlatUI* pFlatUI)
{
	Assert(pScroll != NULL);
	Assert(pFlatUI != NULL);

	if ((pScroll->pVScroll != NULL) && (pScroll->pHScroll != NULL))
	{
		if (NULL == pScroll->pCrossLabel)
		{
			pScroll->pCrossLabel = (FlatLabel*)pFlatUI->CreatePartWidget(
				pScroll->pParent, "FlatLabel");
		}
	}
	else
	{
		if (pScroll->pCrossLabel)
		{
			pFlatUI->DeletePartWidget(pScroll->pCrossLabel);
			pScroll->pCrossLabel = NULL;
		}
	}
}

// Initialize scrollable.
bool flat_scroll_initialize(flat_scroll_t* pScroll, FlatUI* pFlatUI,
	bool vertical, bool horizontal)
{
	Assert(pScroll != NULL);
	Assert(pFlatUI != NULL);

	flat_scroll_set_vscroll_exists(pScroll, pFlatUI, vertical);
	flat_scroll_set_hscroll_exists(pScroll, pFlatUI, horizontal);
	return true;
}

// Set vertical scroll bar exists.
bool flat_scroll_set_vscroll_exists(flat_scroll_t* pScroll, FlatUI* pFlatUI, 
	bool exists)
{
	Assert(pScroll != NULL);

	if (exists)
	{
		if (NULL == pScroll->pVScroll)
		{
			pScroll->pVScroll = (FlatVScrollBar*)pFlatUI->CreatePartWidget(
				pScroll->pParent, "FlatVScrollBar");
		}
	}
	else
	{
		if (pScroll->pVScroll)
		{
			pFlatUI->DeletePartWidget(pScroll->pVScroll);
			pScroll->pVScroll = NULL;
		}
	}
	
	check_cross_label(pScroll, pFlatUI);
	return true;
}

// Set horizontal scroll bar exists.
bool flat_scroll_set_hscroll_exists(flat_scroll_t* pScroll, FlatUI* pFlatUI, 
	bool exists)
{
	Assert(pScroll != NULL);

	if (exists)
	{
		if (NULL == pScroll->pHScroll)
		{
			pScroll->pHScroll = (FlatHScrollBar*)pFlatUI->CreatePartWidget(
				pScroll->pParent, "FlatHScrollBar");
		}
	}
	else
	{
		if (pScroll->pHScroll)
		{
			pFlatUI->DeletePartWidget(pScroll->pHScroll);
			pScroll->pHScroll = NULL;
		}
	}

	check_cross_label(pScroll, pFlatUI);
	return true;
}

// Get vertical scroll bar.
bool flat_scroll_get_vscroll_exists(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->pVScroll != NULL;
}

// Get horizontal scroll bar.
bool flat_scroll_get_hscroll_exists(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->pHScroll != NULL;
}

// Get vertical scroll bar object id.
uniqid_t flat_scroll_vscroll_id(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	if (NULL == pScroll->pVScroll)
	{
		return uniqid_t();
	}

	return pScroll->pVScroll->GetUID();
}

// Get horizontal scroll bar object id.
uniqid_t flat_scroll_hscroll_id(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	if (NULL == pScroll->pHScroll)
	{
		return uniqid_t();
	}

	return pScroll->pHScroll->GetUID();
}

// Get cross label object id.
uniqid_t flat_scroll_cross_label_id(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	if (NULL == pScroll->pCrossLabel)
	{
		return uniqid_t();
	}

	return pScroll->pCrossLabel->GetUID();
}

// Set scroll bar size.
void flat_scroll_set_bar_size(flat_scroll_t* pScroll, int value)
{
	Assert(pScroll != NULL);
	Assert(value >= 0);

	pScroll->nScrollBarSize = value;
}

// Get scroll bar size.
int flat_scroll_get_bar_size(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->nScrollBarSize;
}

// Set always show vertical scroll bar.
void flat_scroll_set_vscroll_always(flat_scroll_t* pScroll, bool value)
{
	Assert(pScroll != NULL);

	pScroll->bVScrollAlways = value;
}

// Get always show vertical scroll bar.
bool flat_scroll_get_vscroll_always(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->bVScrollAlways;
}

// Set always show horizontal scroll bar.
void flat_scroll_set_hscroll_always(flat_scroll_t* pScroll, bool value)
{
	Assert(pScroll != NULL);

	pScroll->bHScrollAlways = value;
}

// Get always show horizontal scroll bar.
bool flat_scroll_get_hscroll_always(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->bHScrollAlways;
}

// Set dynamic size of slide bar.
void flat_scroll_set_dynamic_slide(flat_scroll_t* pScroll, bool value)
{
	Assert(pScroll != NULL);

	pScroll->bDynamicSlide = value;
}

// Get dynamic size of slide bar.
bool flat_scroll_get_dynamic_slide(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	return pScroll->bDynamicSlide;
}

// Get reveal width.
int flat_scroll_reveal_width(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatWidget* pParent = pScroll->pParent;
	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	bool vscroll_visible = (pVScroll != NULL) && pVScroll->GetVisible();

	if (vscroll_visible)
	{
		return pParent->GetWidth() - pVScroll->GetWidth();
	}
	else
	{
		return pParent->GetWidth();
	}
}

// Get reveal height.
int flat_scroll_reveal_height(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatWidget* pParent = pScroll->pParent;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	bool hscroll_visible = (pHScroll != NULL) && pHScroll->GetVisible();

	if (hscroll_visible)
	{
		return pParent->GetHeight() - pHScroll->GetHeight();
	}
	else
	{
		return pParent->GetHeight();
	}
}

// Perform layout.
void flat_scroll_perform_layout(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatWidget* pParent = pScroll->pParent;
	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	bool vscroll_visible = (pVScroll != NULL) && pVScroll->GetVisible();
	bool hscroll_visible = (pHScroll != NULL) && pHScroll->GetVisible();

	if (pScroll->nScrollBarSize > 0)
	{
		if (pVScroll)
		{
			pVScroll->SetWidth(pScroll->nScrollBarSize);
		}

		if (pHScroll)
		{
			pHScroll->SetHeight(pScroll->nScrollBarSize);
		}
	}

	if (vscroll_visible)
	{
		int height = pParent->GetHeight();

		if (hscroll_visible)
		{
			height -= pHScroll->GetHeight();
		}

		pVScroll->SetLeft(pParent->GetWidth() - pVScroll->GetWidth());
		pVScroll->SetTop(0);
		pVScroll->SetHeight(height);
	}

	if (hscroll_visible)
	{
		int width = pParent->GetWidth();

		if (vscroll_visible)
		{
			width -= pVScroll->GetWidth();
		}

		pHScroll->SetLeft(0);
		pHScroll->SetTop(pParent->GetHeight() - pHScroll->GetHeight());
		pHScroll->SetWidth(width);
	}

	FlatLabel* pCrossLabel = pScroll->pCrossLabel;

	if (pCrossLabel)
	{
		bool cross_visible = vscroll_visible && hscroll_visible;

		pCrossLabel->SetVisible(cross_visible);

		if (cross_visible)
		{
			int width = pVScroll->GetWidth();
			int height = pHScroll->GetHeight();

			pCrossLabel->SetLeft(pParent->GetWidth() - width);
			pCrossLabel->SetTop(pParent->GetHeight() - height);
			pCrossLabel->SetSize(width, height);
		}
	}
}

// Update vertical scroll bar.
static bool update_vscroll(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatVScrollBar* pVScroll = pScroll->pVScroll;

	if (NULL == pVScroll)
	{
		return false;
	}

	if (!pVScroll->GetVisible())
	{
		return false;
	}

	FlatWidget* pParent = pScroll->pParent;
	int whole = pParent->GetVerticalWhole();
	int reveal = pParent->GetVerticalReveal();
	int begin = pParent->GetVerticalBegin();
	int minimum = 0;
	int maximum = whole - reveal;

	if (maximum < 0)
	{
		maximum = 0;
	}

	int value = begin;

	if (value < minimum)
	{
		value = minimum;
	}
	
	if (value > maximum)
	{
		value = maximum;
	}

	pVScroll->SetEnabled(reveal < whole);
	pVScroll->SetMinimum(minimum);
	pVScroll->SetMaximum(maximum);
	pVScroll->SetValue(value);

	if (whole > 0)
	{
		int small_change = reveal / 10;

		if (small_change < 1)
		{
			small_change = 1;
		}

		if (small_change > 16)
		{
			small_change = 16;
		}
		
		pVScroll->SetSmallChange(small_change);
		pVScroll->SetLargeChange(reveal);
	}

	if (pScroll->bDynamicSlide)
	{
		if (reveal < whole)
		{
			float rate = (float)reveal / (float)whole;

			pVScroll->AdjustSlideSize(rate, 8);
		}
	}

	return true;
}

// Update horizontal scroll bar.
static bool update_hscroll(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatHScrollBar* pHScroll = pScroll->pHScroll;

	if (NULL == pHScroll)
	{
		return false;
	}

	if (!pHScroll->GetVisible())
	{
		return false;
	}

	FlatWidget* pParent = pScroll->pParent;
	int whole = pParent->GetHorizontalWhole();
	int reveal = pParent->GetHorizontalReveal();
	int begin = pParent->GetHorizontalBegin();
	int minimum = 0;
	int maximum = whole - reveal;

	if (maximum < 0)
	{
		maximum = 0;
	}

	int value = begin;

	if (value < minimum)
	{
		value = minimum;
	}

	if (value > maximum)
	{
		value = maximum;
	}

	pHScroll->SetEnabled(reveal < whole);
	pHScroll->SetMinimum(minimum);
	pHScroll->SetMaximum(maximum);
	pHScroll->SetValue(value);

	if (whole > 0)
	{
		int small_change = reveal / 10;

		if (small_change < 1)
		{
			small_change = 1;
		}

		if (small_change > 16)
		{
			small_change = 16;
		}

		pHScroll->SetSmallChange(small_change);
		pHScroll->SetLargeChange(reveal);
	}

	if (pScroll->bDynamicSlide)
	{
		if (reveal < whole)
		{
			float rate = (float)reveal / (float)whole;

			pHScroll->AdjustSlideSize(rate, 8);
		}
	}

	return true;
}

// Get current scroll flags.
static int get_old_flags(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	bool vscroll_visible = (pVScroll != NULL) && pVScroll->GetVisible();
	bool hscroll_visible = (pHScroll != NULL) && pHScroll->GetVisible();
	int flags = 0;

	if (vscroll_visible)
	{
		flags |= 0x1;
	}

	if (hscroll_visible)
	{
		flags |= 0x2;
	}

	return flags;
}

// Get new scroll flags.
static int get_new_flags(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatWidget* pParent = pScroll->pParent;
	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	int flags = 0;

	if (pVScroll)
	{
		if (pScroll->bVScrollAlways)
		{
			flags |= 0x1;
		}
		else
		{
			int v_whole = pParent->GetVerticalWhole();
			int v_reveal = pParent->GetVerticalReveal();

			if (v_reveal < v_whole)
			{
				flags |= 0x1;
			}
		}
	}

	if (pHScroll)
	{
		if (pScroll->bHScrollAlways)
		{
			flags |= 0x2;
		}
		else
		{
			int h_whole = pParent->GetHorizontalWhole();
			int h_reveal = pParent->GetHorizontalReveal();

			if (h_reveal < h_whole)
			{
				flags |= 0x2;
			}
		}
	}

	return flags;
}

// Test vertical scroll bar enabled.
static bool vscroll_enabled(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatVScrollBar* pVScroll = pScroll->pVScroll;

	return (pVScroll != NULL) && pVScroll->GetVisible() && 
		pVScroll->GetEnabled();
}

// Test horizontal scroll bar enabled.
static bool hscroll_enabled(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatHScrollBar* pHScroll = pScroll->pHScroll;

	return (pHScroll != NULL) && pHScroll->GetVisible() &&
		pHScroll->GetEnabled();
}

// Update scroll bar.
bool flat_scroll_update(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	int old_flags = get_old_flags(pScroll);
	int cur_flags = old_flags;
	int new_flags = get_new_flags(pScroll);
	int count = 0;

	while (cur_flags != new_flags)
	{
		if (pVScroll)
		{
			pVScroll->SetVisible((new_flags & 0x1) != 0);
		}

		if (pHScroll)
		{
			pHScroll->SetVisible((new_flags & 0x2) != 0);
		}

		cur_flags = new_flags;
		new_flags = get_new_flags(pScroll);

		if (++count > 1)
		{
			break;
		}
	}

	if (new_flags != old_flags)
	{
		flat_scroll_perform_layout(pScroll);
	}

	if (!vscroll_enabled(pScroll))
	{
		pScroll->pParent->SetVerticalBegin(0);
	}

	if (!hscroll_enabled(pScroll))
	{
		pScroll->pParent->SetHorizontalBegin(0);
	}
	
	update_vscroll(pScroll);
	update_hscroll(pScroll);
	return true;
}

// Draw scroll.
void flat_scroll_draw(flat_scroll_t* pScroll, flat_canvas_t* pCanvas)
{
	Assert(pScroll != NULL);
	Assert(pCanvas != NULL);

	FlatVScrollBar* pVScroll = pScroll->pVScroll;
	FlatHScrollBar* pHScroll = pScroll->pHScroll;
	FlatLabel* pCrossLabel = pScroll->pCrossLabel;
	int vscroll_width = 0;
	int hscroll_height = 0;

	if ((pVScroll != NULL) && pVScroll->GetVisible())
	{
		vscroll_width = pVScroll->GetWidth();
	}

	if ((pHScroll != NULL) && pHScroll->GetVisible())
	{
		hscroll_height = pHScroll->GetHeight();
	}

	FlatWidget* pParent = pScroll->pParent;
	int width = pParent->GetWidth() - vscroll_width;
	int height = pParent->GetHeight() - hscroll_height;
	int x1 = pParent->GetGlobalLeft();
	int y1 = pParent->GetGlobalTop();

	if (canvas_set_range(pCanvas, x1, y1, width, height))
	{
		pParent->DrawScrollRegion(pCanvas);
		canvas_reset_range(pCanvas);
	}
	
	if (pCrossLabel)
	{
		pCrossLabel->Draw(pCanvas);
	}
	
	if (pVScroll)
	{
		pVScroll->Draw(pCanvas);
	}

	if (pHScroll)
	{
		pHScroll->Draw(pCanvas);
	}
}

// Scroll value changed.
void flat_scroll_changed(flat_scroll_t* pScroll, FlatWidget* pPartWidget,
	int new_value, int old_value)
{
	Assert(pScroll != NULL);
	Assert(pPartWidget != NULL);

	if (pPartWidget == pScroll->pVScroll)
	{
		pScroll->pParent->SetVerticalBegin(new_value);
	}
	else if (pPartWidget == pScroll->pHScroll)
	{
		pScroll->pParent->SetHorizontalBegin(new_value);
	}
}

// Adjust vertical scroll position.
bool flat_scroll_adjust_vscroll(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	if (NULL == pScroll->pVScroll)
	{
		return false;
	}

	int value = pScroll->pParent->GetVerticalBegin();

	return pScroll->pVScroll->AdjustValue(value);
}

// Adjust horizontal scroll position.
bool flat_scroll_adjust_hscroll(flat_scroll_t* pScroll)
{
	Assert(pScroll != NULL);

	if (NULL == pScroll->pHScroll)
	{
		return false;
	}

	int value = pScroll->pParent->GetHorizontalBegin();

	return pScroll->pHScroll->AdjustValue(value);
}

// Clone scroll style.
bool flat_scroll_clone_style(flat_scroll_t* pScroll, flat_scroll_t* pSrc)
{
	Assert(pScroll != NULL);
	Assert(pSrc != NULL);

	FlatVScrollBar* pDstVScroll = pScroll->pVScroll;
	FlatVScrollBar* pSrcVScroll = pSrc->pVScroll;

	if ((pDstVScroll != NULL) && (pSrcVScroll != NULL))
	{
		pDstVScroll->SetBackImage(pSrcVScroll->GetBackImage());
		pDstVScroll->SetImageLayout(pSrcVScroll->GetImageLayout());
	}
	
	FlatHScrollBar* pDstHScroll = pScroll->pHScroll;
	FlatHScrollBar* pSrcHScroll = pSrc->pHScroll;

	if ((pDstHScroll != NULL) && (pSrcHScroll != NULL))
	{
		pDstHScroll->SetBackImage(pSrcHScroll->GetBackImage());
		pDstHScroll->SetImageLayout(pSrcHScroll->GetImageLayout());
	}

	FlatLabel* pDstLabel = pScroll->pCrossLabel;
	FlatLabel* pSrcLabel = pSrc->pCrossLabel;

	if ((pDstLabel != NULL) && (pSrcLabel != NULL))
	{
		pDstLabel->SetBackImage(pSrcLabel->GetBackImage());
		pDstLabel->SetImageLayout(pSrcLabel->GetImageLayout());
	}

	pScroll->nScrollBarSize = pSrc->nScrollBarSize;
	pScroll->bDynamicSlide = pSrc->bDynamicSlide;
	return true;
}
