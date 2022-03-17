// flat_sprite.h
// Created by lulimin on 2021/3/29.

#ifndef __FLAT_SPRITE_H
#define __FLAT_SPRITE_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "flat_matrix.h"

// FlatSprite

struct flat_context_t;
struct flat_image_t;
class FlatUI;

class FlatSprite : public IObject
{
public:
	FlatSprite();
	virtual ~FlatSprite();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Update sprite.
	void DoRenew(flat_context_t* pContext, float frame_time);
	// Draw sprite.
	void DoPaint(flat_context_t* pContext);

	// Set position.
	bool SetPosition(float x, float y);
	// Set scaling.
	bool SetScaling(float sx, float sy);
	// Set rotation radian.
	bool SetRotation(float rad);
	// Set scaling and rotation.
	bool ScaleAndRotate(float sx, float sy, float rad);
	// Set rotation and scaling.
	bool RotateAndScale(float rad, float sx, float sy);

	// Get position.
	float GetPositionX() const;
	float GetPositionY() const;
	// Get scale.
	float GetScaleX() const;
	float GetScaleY() const;
	// Get rotation radian.
	float GetRotation() const;

	// Set offset.
	bool SetOffset(float x, float y);
	// Get offset.
	float GetOffsetX();
	float GetOffsetY();

	// Image name.
	void SetImage(const char* value);
	const char* GetImage() const;

private:
	FlatUI* m_pFlatUI;
	flat_matrix_t m_fmTransform;
	float m_fScaleX;
	float m_fScaleY;
	float m_fRotation;
	float m_fOffsetX;
	float m_fOffsetY;
	FrameString m_sImage;
	flat_image_t* m_pImage;
};

#endif // __FLAT_SPRITE_H
