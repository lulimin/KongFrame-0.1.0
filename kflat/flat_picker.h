// flat_picker.h
// Created by lulimin on 2021/1/15.

#ifndef __FLAT_PICKER_H
#define __FLAT_PICKER_H

#include "flat_widget.h"

// FlatPicker

class FlatButton;

class FlatPicker : public FlatWidget
{
public:
	FlatPicker();
	virtual ~FlatPicker();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Get coordinate in which widget.
	virtual FlatWidget* GetRegionOf(int x, int y);

	// Callbacks.
	virtual int OnClickReset();
	virtual int OnLeftDown(int x, int y, unsigned int flags);
	virtual int OnLeftUp(int x, int y, unsigned int flags);
	virtual int OnLeftDrag(int x, int y, int gcx, int gcy, unsigned int flags);
	virtual int OnKeyDown(unsigned int key, unsigned int flags);
	virtual int OnSizeChanged();
	virtual int OnPartDrag(FlatWidget* pPart, int x, int y, int cx, int cy,
		unsigned int flags);

	// Border size.
	void SetBorderSize(int value);
	int GetBorderSize() const;

	// Move stop size.
	void SetStepSize(int value);
	int GetStepSize() const;

	// Actived status.
	void SetActived(bool value);
	bool GetActived() const;

	// Target widget id.
	void SetTarget(const uniqid_t& value);
	uniqid_t GetTarget() const;

	// Get part widget id.
	uniqid_t GetLeftEdge() const;
	uniqid_t GetRightEdge() const;
	uniqid_t GetUpEdge() const;
	uniqid_t GetDownEdge() const;
	uniqid_t GetLeftUpEdge() const;
	uniqid_t GetLeftDownEdge() const;
	uniqid_t GetRightUpEdge() const;
	uniqid_t GetRightDownEdge() const;

private:
	// Drag button.
	bool DragLeft(FlatButton* pEdge, int x, int cx, unsigned int flags);
	bool DragRight(FlatButton* pEdge, int x, int cx, unsigned int flags);
	bool DragUp(FlatButton* pEdge, int y, int cy, unsigned int flags);
	bool DragDown(FlatButton* pEdge, int y, int cy, unsigned int flags);
	// Set layout of part widgets.
	void PerformLayout();

private:
	int m_nBorderSize;
	int m_nStepSize;
	bool m_bActived;
	uniqid_t m_TargetID;
	FlatButton* m_pLeftEdge;
	FlatButton* m_pRightEdge;
	FlatButton* m_pUpEdge;
	FlatButton* m_pDownEdge;
	FlatButton* m_pLeftUpEdge;
	FlatButton* m_pLeftDownEdge;
	FlatButton* m_pRightUpEdge;
	FlatButton* m_pRightDownEdge;
	int m_nClickX;
	int m_nClickY;
};

#endif // __FLAT_PICKER_H
