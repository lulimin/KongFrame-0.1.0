// flat_scene.cpp
// Created by lulimin on 2021/3/29.

#include "flat_scene.h"
#include "flat_sprite.h"
#include "flat_context.h"
#include "../kflat/flat_ui.h"

/// \file flat_scene.cpp
/// \brief User interface scene.

/// object: FlatScene
/// \brief User interface scene object.
DEF_OBJECT(FlatScene, FlatWidget);

/// property: bool Background
/// \brief Need draw background.
DEF_PROP(bool, FlatScene, Background);
/// property: bool Border
/// \brief Need draw border.
DEF_PROP(bool, FlatScene, Border);

/// function: object CreateSprite(string name)
/// \brief Create sprite.
/// \param name Sprite name.
DEF_FUNC_1(uniqid_t, FlatScene, CreateSprite, const char*);
/// function: bool DeleteSprite(object id)
/// \brief Delete sprite.
/// \param id Sprite id.
DEF_FUNC_1(bool, FlatScene, DeleteSprite, const uniqid_t&);
/// function: bool AddSprite(object id)
/// \brief Add sprite to scene.
/// \param id Sprite object id.
DEF_FUNC_1(bool, FlatScene, AddSprite, const uniqid_t&);
/// function: bool RemoveSprite(object id)
/// \brief Remove sprite from scene.
/// \param id Sprite object id.
DEF_FUNC_1(bool, FlatScene, RemoveSprite, const uniqid_t&);
/// function: bool ClearSprite()
/// \brief Clear all sprites.
DEF_FUNC_0(bool, FlatScene, ClearSprite);
/// function: bool SetViewerPosition(float x, float y)
/// \brief Set position of viewer.
/// \param x,y Position value.
DEF_FUNC_2(bool, FlatScene, SetViewerPosition, float, float);
/// function: float GetViewerPositionX()
/// \brief Get position X of viewer.
DEF_FUNC_0(float, FlatScene, GetViewerPositionX);
/// function: float GetViewerPositionY()
/// \brief Get position Y of viewer.
DEF_FUNC_0(float, FlatScene, GetViewerPositionY);

// FlatScene

FlatScene::FlatScene()
{
	m_pContext = NULL;
	m_bBackground = false;
	m_bBorder = true;
}

FlatScene::~FlatScene()
{
	if (m_pContext)
	{
		flat_context_delete(m_pContext);
	}
}

bool FlatScene::Startup(const IArgList& args)
{
	if (!FlatWidget::Startup(args))
	{
		return false;
	}
	
	m_pContext = flat_context_create();
	return true;
}

bool FlatScene::Shutdown()
{
	this->ClearSprite();
	return FlatWidget::Shutdown();
}

void FlatScene::Play(float frame_time)
{
	size_t i = 0;

	while (i < m_Sprites.Size())
	{
		FlatSprite* pSprite = m_Sprites[i];

		pSprite->DoRenew(m_pContext, frame_time);
		++i;
	}
}

void FlatScene::Draw(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	if (!this->GetVisible())
	{
		return;
	}
	
	int width = this->GetWidth();
	int height = this->GetHeight();
	int x1 = this->GetGlobalLeft();
	int y1 = this->GetGlobalTop();
	int x2 = x1 + width;
	int y2 = y1 + height;

	if (m_bBackground)
	{
		if (this->ExistBackImage())
		{
			this->DrawBackImage(pCanvas, x1, y1, x2, y2);
		}
		else
		{
			canvas_draw_block(pCanvas, x1, y1, x2, y2,
				this->GetBackColorValue());
		}
	}

	if (canvas_set_range(pCanvas, x1, y1, width, height))
	{
		m_pContext->pCanvas = pCanvas;
		m_pContext->fCenterX = (float)x1 + width * 0.5F;
		m_pContext->fCenterY = (float)y1 + height * 0.5F;

		size_t i = 0;

		while (i < m_Sprites.Size())
		{
			FlatSprite* pSprite = m_Sprites[i];

			pSprite->DoPaint(m_pContext);
			++i;
		}

		canvas_reset_range(pCanvas);
	}

	if (m_bBorder)
	{
		canvas_draw_frame(pCanvas, x1, y1, x2, y2, this->GetForeColorValue());
	}
}

void FlatScene::SetMetadata(flat_metadata_t* pMD)
{
	Assert(pMD != NULL);

	FlatWidget::SetMetadata(pMD);

	FrameArgData def_false(DT_BOOLEAN, false);

	flat_metadata_add_prop(pMD, "Background", FLAT_METATYPE_BOOLEAN,
		&def_false);
	flat_metadata_add_prop(pMD, "Border", FLAT_METATYPE_BOOLEAN,
		&def_false);
}

void FlatScene::SetBackground(bool value)
{ 
	m_bBackground = value;
}

bool FlatScene::GetBackground() const
{ 
	return m_bBackground; 
}

void FlatScene::SetBorder(bool value)
{ 
	m_bBorder = value;
}

bool FlatScene::GetBorder() const
{ 
	return m_bBorder;
}

uniqid_t FlatScene::CreateSprite(const char* name)
{
	FlatSprite* pSprite = (FlatSprite*)frame_create_object_args(name,
		FrameArgList() << this->GetFlatUI()->GetUID());

	if (NULL == pSprite)
	{
		return uniqid_t();
	}

	if (!pSprite->GetDesc()->DeriveFrom("FlatSprite"))
	{
		pSprite->Release();
		return uniqid_t();
	}

	return pSprite->GetUID();
}

bool FlatScene::DeleteSprite(const uniqid_t& id)
{
	FlatSprite* pSprite = (FlatSprite*)frame_get_object(id);

	if (NULL == pSprite)
	{
		return false;
	}

	if (!pSprite->GetDesc()->DeriveFrom("FlatSprite"))
	{
		return false;
	}

	pSprite->Release();
	return true;
}

bool FlatScene::AddSprite(const uniqid_t& id)
{
	FlatSprite* pSprite = (FlatSprite*)frame_get_object(id);

	if (NULL == pSprite)
	{
		return false;
	}

	if (!pSprite->GetDesc()->DeriveFrom("FlatSprite"))
	{
		return false;
	}
	
	m_Sprites.PushBack(pSprite);
	return true;
}

bool FlatScene::RemoveSprite(const uniqid_t& id)
{
	for (size_t i = 0; i < m_Sprites.Size(); ++i)
	{
		FlatSprite* pSprite = m_Sprites[i];

		if (uniqid_equal(pSprite->GetUID(), id))
		{
			m_Sprites.Remove(i);
			return true;
		}
	}
	
	return false;
}

bool FlatScene::ClearSprite()
{
	for (size_t i = 0; i < m_Sprites.Size(); ++i)
	{
		FlatSprite* pSprite = m_Sprites[i];

		pSprite->Release();
	}
	
	m_Sprites.Clear();
	return true;
}

bool FlatScene::SetViewerPosition(float x, float y)
{
	flat_matrix_set_position(&m_pContext->fmViewer, x, y);
	return true;
}

float FlatScene::GetViewerPositionX()
{
	return flat_matrix_get_position_x(&m_pContext->fmViewer);
}

float FlatScene::GetViewerPositionY()
{
	return flat_matrix_get_position_y(&m_pContext->fmViewer);
}
