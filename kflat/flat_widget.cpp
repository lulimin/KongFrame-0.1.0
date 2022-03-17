// flat_widget.cpp
// Created by lulimin on 2020/9/11.

#include "flat_widget.h"

/// \file flat_widget.cpp
/// \brief Base class of user interface widget.

/// object: FlatWidget
/// \brief Base class of user interface widget object.
DEF_BASE_OBJECT(FlatWidget, IObject);

/// readonly: int MinimumWidth
/// \brief Minimum widget width.
DEF_READ(int, FlatWidget, MinimumWidth);
/// readonly: int MinimumHeight
/// \brief Minimum widget height.
DEF_READ(int, FlatWidget, MinimumHeight);
/// readonly: int Right
/// \brief Right position.
DEF_READ(int, FlatWidget, Right);
/// readonly: int Bottom
/// \brief Bottom postion.
DEF_READ(int, FlatWidget, Bottom);
/// readonly: bool Capture
/// \brief Indicate capture mouse.
DEF_READ(bool, FlatWidget, Capture);
/// readonly: bool Focused
/// \brief Indicate focused mouse.
DEF_READ(bool, FlatWidget, Focused);
/// readonly: object Parent
/// \brief Parent widget id.
DEF_READ_F(uniqid_t, FlatWidget, Parent, GetParentID);
/// readonly: object Delegator
/// \brief Delegate widget id.
DEF_READ_F(uniqid_t, FlatWidget, Delegator, GetDelegatorID);
/// readonly: bool IsContainer
/// \brief Widget is container.
DEF_READ_F(bool, FlatWidget, IsContainer, IsContainer);

/// property: string Name
/// \brief Widget name.
DEF_PROP(const char*, FlatWidget, Name);
/// property: string Font
/// \brief Font name.
DEF_PROP(const char*, FlatWidget, Font);
/// property: string Cursor
/// \brief Cursor name.
DEF_PROP(const char*, FlatWidget, Cursor);
/// property: string Hint
/// \brief Hint information.
DEF_PROP(const char*, FlatWidget, Hint);
/// property: string ContextMenu
/// \brief Name of context menu.
DEF_PROP(const char*, FlatWidget, ContextMenu);
/// property: int GlobalLeft
/// \brief Global left position.
DEF_PROP(int, FlatWidget, GlobalLeft);
/// property: int GlobalTop
/// \brief Global top position.
DEF_PROP(int, FlatWidget, GlobalTop);
/// property: int Left
/// \brief Left position.
DEF_PROP(int, FlatWidget, Left);
/// property: int Top
/// \brief Top position.
DEF_PROP(int, FlatWidget, Top);
/// property: int Width
/// \brief Horizontal size.
DEF_PROP(int, FlatWidget, Width);
/// property: int Height
/// \brief Vertical size.
DEF_PROP(int, FlatWidget, Height);
/// property: int TabIndex
/// \brief Order when press 'Tab' key, 0 mean invalidate.
DEF_PROP(int, FlatWidget, TabIndex);
/// property: bool visible
/// \brief Visible flag.
DEF_PROP(bool, FlatWidget, Visible);
/// property: bool Enabled
/// \brief Enabled flag.
DEF_PROP(bool, FlatWidget, Enabled);
/// property: bool AutoSize
/// \brief Auto fit size of background image.
DEF_PROP(bool, FlatWidget, AutoSize);
/// property: bool DesignMode
/// \brief In design status.
DEF_PROP(bool, FlatWidget, DesignMode);
/// property: string ForeColor
/// \brief Foreground color.
DEF_PROP(ArgString, FlatWidget, ForeColor);
/// property: string BackColor
/// \brief Background color.
DEF_PROP(ArgString, FlatWidget, BackColor);
/// property: string BackImage
/// \brief Background image name.
DEF_PROP(const char*, FlatWidget, BackImage);
/// property: string ImageLayout
/// \brief Layout of image("Fit","Patch","HPatch","VPatch").
DEF_PROP(const char*, FlatWidget, ImageLayout);

/// function: object GetUI()
/// \brief Get user interface object.
DEF_FUNC_0(uniqid_t, FlatWidget, GetUI);
/// function: object FindForm()
/// \brief Find form belong to.
DEF_FUNC_0(uniqid_t, FlatWidget, FindForm);
/// function: bool BringToFront()
/// \brief Bring widget to foreground.
DEF_FUNC_0(bool, FlatWidget, BringToFront);
/// function: bool SendToBack(object id)
/// \brief Send widget to background.
DEF_FUNC_0(bool, FlatWidget, SendToBack);
/// function: bool IsContainer()
/// \brief Is container.
DEF_FUNC_0(bool, FlatWidget, IsContainer);
/// function: bool HasChildren
/// \brief Test container has children.
DEF_FUNC_0(bool, FlatWidget, HasChildren);
/// function: object GetInRegion(int x, int y)
/// \brief Get coordinate in which widget region.
/// \param x,y Coordinate.
DEF_FUNC_2(uniqid_t, FlatWidget, GetInRegion, int, int);
/// function: bool FitSafeArea()
/// \brief Fit rectangle to safe area.
DEF_FUNC_0(bool, FlatWidget, FitSafeArea);
