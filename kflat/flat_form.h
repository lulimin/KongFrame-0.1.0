// flat_form.h
// Created by lulimin on 2020/9/11.

#ifndef __FLAT_FORM_H
#define __FLAT_FORM_H

#include "flat_widget.h"

// FlatForm

class FlatForm : public FlatWidget
{
public:
	FlatForm();
	virtual ~FlatForm();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Get minimum widget size.
	virtual int GetMinimumWidth() { return 8; }
	virtual int GetMinimumHeight() { return 8; }

	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// In dialog mode.
	bool GetDialogMode() const;

	// Transparent mode.
	void SetTransparent(bool value);
	bool GetTransparent() const;

	// Step size in design mode.
	void SetStepSize(int value);
	int GetStepSize() const;

	// Add child widget.
	bool AddChild(const uniqid_t& id);
	// Remove child widget.
	bool RemoveChild(const uniqid_t& id);
	// Clear child widgets.
	bool ClearChildren();
	// Get child widget list.
	void GetChildList(const IArgList& args, IArgList* res);

	// Show form.
	bool Show();
	// Show in dialog mode.
	bool ShowDialog();
	// Close form.
	bool Close();

private:
	bool m_bDialogMode;
	bool m_bTransparent;
	int m_nStepSize;
};

#endif // __FLAT_FORM_H
