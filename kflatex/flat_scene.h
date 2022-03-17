// flat_scene.h
// Created by lulimin on 2021/3/29.

#ifndef __FLAT_SCENE_H
#define __FLAT_SCENE_H

#include "../kflat/flat_widget.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

// FlatScene

struct flat_context_t;
class FlatSprite;

class FlatScene : public FlatWidget
{
public:
	FlatScene();
	virtual ~FlatScene();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Flush per frame.
	virtual void Play(float frame_time);
	// Draw widget.
	virtual void Draw(flat_canvas_t* pCanvas);
	// Set widget metadata.
	virtual void SetMetadata(flat_metadata_t* pMD);

	// Draw background.
	void SetBackground(bool value);
	bool GetBackground() const;

	// Draw border.
	void SetBorder(bool value);
	bool GetBorder() const;

	// Create sprite.
	uniqid_t CreateSprite(const char* name);
	// Delete sprite
	bool DeleteSprite(const uniqid_t& id);

	// Add sprite.
	bool AddSprite(const uniqid_t& id);
	// Remove sprite.
	bool RemoveSprite(const uniqid_t& id);
	// Clear all sprites.
	bool ClearSprite();

	// Set position of viewer.
	bool SetViewerPosition(float x, float y);
	// Get position of viewer.
	float GetViewerPositionX();
	float GetViewerPositionY();

private:
	flat_context_t* m_pContext;
	bool m_bBackground;
	bool m_bBorder;
	TPodArray<FlatSprite*, 1, FrameAlloc> m_Sprites;
};

#endif // __FLAT_SCENE_H
