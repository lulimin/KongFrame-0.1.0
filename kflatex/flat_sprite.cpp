// flat_sprite.cpp
// Created by lulimin on 2021/3/29.

#include "flat_sprite.h"
#include "flat_context.h"
#include "../kflat/flat_image.h"
#include "../kflat/flat_utility.h"
#include "../kflat/flat_canvas.h"

/// \file flat_sprite.cpp
/// \brief Sprite in flat scene.

/// object: FlatSprite
/// \brief Sprite in flat scene object.
DEF_OBJECT(FlatSprite, IObject);

/// readonly: float PositionX
/// \brief X coordinate.
DEF_READ(float, FlatSprite, PositionX);
// readonly: float PositionY
/// \brief Y coordinate.
DEF_READ(float, FlatSprite, PositionY);
/// readonly: float ScaleX
/// \brief Scale value X.
DEF_READ(float, FlatSprite, ScaleX);
/// readonly: float ScaleY
/// \brief Scale value Y.
DEF_READ(float, FlatSprite, ScaleY);
/// readonly: float Rotation
/// \brief Rotation radian.
DEF_READ(float, FlatSprite, Rotation);
/// readonly: float OffsetX
/// \brief Offset percent X.
DEF_READ(float, FlatSprite, OffsetX);
/// readonly: float OffsetY
/// \brief Offset percent Y.
DEF_READ(float, FlatSprite, OffsetY);

/// property: string Image
/// \brief Image name.
DEF_PROP(const char*, FlatSprite, Image);

/// function: bool SetPosition(float x, float y)
/// \brief Set sprite position.
/// \param x,y Coordinate value.
DEF_FUNC_2(bool, FlatSprite, SetPosition, float, float);
/// function: bool SetScaling(float sx, float sy)
/// \brief Set sprite scaling.
/// \param sx,sy Scale value.
DEF_FUNC_2(bool, FlatSprite, SetScaling, float, float);
/// function: bool SetRotation(float radian)
/// \brief Set sprite rotation.
/// \param radian Angle value.
DEF_FUNC_1(bool, FlatSprite, SetRotation, float);
/// function: bool ScaleAndRotate(float sx, float sy, float radian)
/// \brief Set scaling and rotation.
/// \param sx,sy Scaling value.
/// \param radian Angle value.
DEF_FUNC_3(bool, FlatSprite, ScaleAndRotate, float, float, float);
/// function: bool RotateAndScale(float radian, float sx, float sy)
/// \brief Set rotation and scaling.
/// \param radian Angle value.
/// \param sx,sy Scaling value.
DEF_FUNC_3(bool, FlatSprite, RotateAndScale, float, float, float);
/// function: bool SetOffset(float x, float y)
/// \brief Set image offset.
/// \param x,y Offset percent value.
DEF_FUNC_2(bool, FlatSprite, SetOffset, float, float);

// FlatSprite

FlatSprite::FlatSprite()
{
	m_pFlatUI = NULL;
	m_fScaleX = 1.0F;
	m_fScaleY = 1.0F;
	m_fRotation = 0.0F;
	m_fOffsetX = 0.0F;
	m_fOffsetY = 0.0F;
	m_pImage = NULL;
	flat_matrix_set_identity(&m_fmTransform);
}

FlatSprite::~FlatSprite()
{
	if (m_pImage)
	{
		flat_image_delete(m_pImage);
	}
}

bool FlatSprite::Startup(const IArgList& args)
{
	FlatUI* pFlatUI = (FlatUI*)frame_get_object(args.GetUniqid(0));

	if (NULL == pFlatUI)
	{
		return false;
	}

	m_pFlatUI = pFlatUI;
	return true;
}

bool FlatSprite::Shutdown()
{
	return true;
}

void FlatSprite::DoRenew(flat_context_t* pContext, float frame_time)
{
	Assert(pContext != NULL);

}

void FlatSprite::DoPaint(flat_context_t* pContext)
{
	Assert(pContext != NULL);

	//float px = flat_matrix_get_position_x(&m_fmTransform);
	//float py = flat_matrix_get_position_y(&m_fmTransform);
	//float x1 = px + pContext->fCenterX;
	//float y1 = py + pContext->fCenterY;

	if (m_pImage)
	{
		flat_matrix_t mtx;

		//flat_matrix_multiply(&mtx, &pContext->fmViewer, &m_fmTransform);
		flat_matrix_multiply(&mtx, &m_fmTransform, &pContext->fmViewer);

		int image_width = flat_image_get_width(m_pImage);
		int image_height = flat_image_get_height(m_pImage);
		float px1 = image_width * (-0.5F + m_fOffsetX);
		float py1 = image_height * (-0.5F + m_fOffsetY);
		float px2 = image_width * (0.5F + m_fOffsetX);
		float py2 = image_height * (0.5F + m_fOffsetY);
		float x[4];
		float y[4];
		
		flat_matrix_transform(&mtx, px1, py1, &x[0], &y[0]);
		flat_matrix_transform(&mtx, px2, py1, &x[1], &y[1]);
		flat_matrix_transform(&mtx, px1, py2, &x[2], &y[2]);
		flat_matrix_transform(&mtx, px2, py2, &x[3], &y[3]);

		for (size_t i = 0; i < 4; ++i)
		{
			x[i] += pContext->fCenterX;
			y[i] += pContext->fCenterY;
		}

		canvas_render_image(pContext->pCanvas, x[0], y[0], x[1], y[1], 
			x[2], y[2], x[3], y[3], m_pImage);

		//int image_width = flat_image_get_width(m_pImage);
		//int image_height = flat_image_get_height(m_pImage);
		//float x2 = x1 + image_width;
		//float y2 = y1 + image_height;

		//canvas_render_image(pContext->pCanvas, x1, y1, x2, y1, x1, y2, x2, y2,
		//	m_pImage);
	}
}

bool FlatSprite::SetPosition(float x, float y)
{
	flat_matrix_set_position(&m_fmTransform, x, y);
	return true;
}

bool FlatSprite::SetScaling(float sx, float sy)
{
	m_fScaleX = sx;
	m_fScaleY = sy;
	
	if (m_fRotation == 0.0F)
	{
		flat_matrix_set_scaling(&m_fmTransform, sx, sy);
	}
	else
	{
		flat_matrix_set_rotation_scaling(&m_fmTransform, m_fRotation, sx, sy);
	}

	return true;
}

bool FlatSprite::SetRotation(float rad)
{
	m_fRotation = rad;
	
	if ((m_fScaleX == 1.0F) && (m_fScaleY == 1.0F))
	{
		flat_matrix_set_rotation(&m_fmTransform, rad);
	}
	else
	{
		flat_matrix_set_scaling_rotation(&m_fmTransform, m_fScaleX, m_fScaleY,
			rad);
	}

	return true;
}

bool FlatSprite::ScaleAndRotate(float sx, float sy, float rad)
{
	m_fScaleX = sx;
	m_fScaleY = sy;
	m_fRotation = rad;
	flat_matrix_set_scaling_rotation(&m_fmTransform, sx, sy, rad);
	return true;
}

bool FlatSprite::RotateAndScale(float rad, float sx, float sy)
{
	m_fRotation = rad;
	m_fScaleX = sx;
	m_fScaleY = sy;
	flat_matrix_set_rotation_scaling(&m_fmTransform, rad, sx, sy);
	return true;
}

float FlatSprite::GetPositionX() const
{
	return flat_matrix_get_position_x(&m_fmTransform);
}

float FlatSprite::GetPositionY() const
{
	return flat_matrix_get_position_y(&m_fmTransform);
}

float FlatSprite::GetScaleX() const
{
	return m_fScaleX;
}

float FlatSprite::GetScaleY() const
{
	return m_fScaleY;
}

float FlatSprite::GetRotation() const
{
	return m_fRotation;
}

bool FlatSprite::SetOffset(float x, float y)
{
	m_fOffsetX = x;
	m_fOffsetY = y;
	return true;
}

float FlatSprite::GetOffsetX()
{
	return m_fOffsetX;
}

float FlatSprite::GetOffsetY()
{
	return m_fOffsetY;
}

void FlatSprite::SetImage(const char* value)
{
	Assert(value != NULL);

	m_sImage = value;
	m_pImage = flat_load_image(m_pFlatUI, m_pImage, value, false);
}

const char* FlatSprite::GetImage() const
{
	return m_sImage.CString();
}
