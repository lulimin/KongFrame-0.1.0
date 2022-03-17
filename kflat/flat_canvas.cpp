// flat_canvas.cpp
// Created by lulimin on 2020/9/17.

#include "flat_canvas.h"
#include "flat_image.h"
#include "../kgraphic/rhi_resource.h"
#include "../kgraphic/rhi_device.h"
#include "../kgraphic/i_render_service.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_file.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"
#include "../inc/pod_hash_table.h"
#include "../inc/auto_buffer.h"
#include "../inc/unicode.h"
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

// Batch types.
#define BATCH_TYPE_RANGE 1
#define BATCH_TYPE_SHAPE 2
#define BATCH_TYPE_IMAGE 3
#define BATCH_TYPE_TEXT 4

// Vertex data.
struct flat_vertex_t
{
	float fPosX;
	float fPosY;
	float fPosZ;
	unsigned int nColor;
	float fTexU;
	float fTexV;
};

// Render batch data.
struct flat_batch_t
{
	int nBatchType;
	int nPrimitiveType;
	unsigned int nStartVertex;
	unsigned int nVertexCount;
	ITexture* pTexture;
	RHIDynamicTexture* pDynamicTex;
	int nRectLeft;
	int nRectTop;
	int nRectWidth;
	int nRectHeight;
};

// View range.
struct flat_range_t
{
	int nLeft;
	int nTop;
	int nWidth;
	int nHeight;
};

// Font char data.
struct flat_font_char_t
{
	unsigned short nBlockIndex;
	unsigned short nCharWidth;
};

// Font character map.
typedef TPodHashTable<unsigned int, flat_font_char_t, TPodHash<unsigned int>,
	FrameAlloc> FontCharMap;

struct flat_font_type_t;

// Font block.
struct flat_font_block_t
{
	unsigned int nChar;
	unsigned int nLastUseFrame;
	flat_font_type_t* pFont;
};

// Font image.
struct flat_font_image_t
{
	int nRefCount;
	int nImageSize;
	int nBlockSize;
	int nBlockDims;
	int nBlockNum;
	flat_font_block_t* pBlocks;
	RHIDynamicTexture* pImageTex;
};

// Font data.
struct flat_font_type_t
{
	FrameString sName;
	unsigned int nNameHash;
	FT_Face hFontFace;
	int nImageGroup;
	int nCharHeight;
	int nInterval;
	bool bFixedWidth;
	bool bSmooth;
	bool bItalic;
	bool bBold;
	unsigned char* pFileData;
	size_t nFileSize;
	FontCharMap CharMap;
	flat_font_image_t* pFontImage;
};

// User interface render canvas.
struct flat_canvas_t
{
	IRenderService* pRenderService;
	int nRenderWidth;
	int nRenderHeight;
	RHIShaderProgram* pShapeShader;
	RHIShaderProgram* pImageShader;
	RHIShaderProgram* pTextShader;
	RHIVertexDescriptor* pVertexDesc;
	RHIDynamicVertexBuffer* pDynamicVB;
	RHISamplerState* pSamplerState;
	RHIRasterizeState* pRasterizeState;
	RHIDepthStencilState* pDepthStencilState;
	RHIBlendState* pBlendState;
	flat_vertex_t* pVertices;
	size_t nVertexSize;
	size_t nVertexNum;
	flat_batch_t* pBatchs;
	size_t nBatchSize;
	size_t nBatchNum;
	flat_range_t CurrentRange;
	TPodArray<flat_range_t, 1, FrameAlloc> RangeStack;
	unsigned int nFrameNum;
	FT_Library hFreeType;
	TPodArray<flat_font_type_t*, 1, FrameAlloc> Fonts;
	flat_font_type_t* pCurrentFont;
};

// Create canvas.
flat_canvas_t* canvas_create(IRenderService* pRenderService)
{
	Assert(pRenderService != NULL);

	flat_canvas_t* pCanvas = K_NEW(flat_canvas_t);

	pCanvas->pRenderService = pRenderService;
	pCanvas->nRenderWidth = pRenderService->GetRenderWidth();
	pCanvas->nRenderHeight = pRenderService->GetRenderHeight();
	pCanvas->pShapeShader = NULL;
	pCanvas->pImageShader = NULL;
	pCanvas->pTextShader = NULL;
	pCanvas->pVertexDesc = NULL;
	pCanvas->pDynamicVB = NULL;
	pCanvas->pSamplerState = NULL;
	pCanvas->pRasterizeState = NULL;
	pCanvas->pDepthStencilState = NULL;
	pCanvas->pBlendState = NULL;
	pCanvas->pVertices = NULL;
	pCanvas->nVertexSize = 0;
	pCanvas->nVertexNum = 0;
	pCanvas->pBatchs = NULL;
	pCanvas->nBatchSize = 0;
	pCanvas->nBatchNum = 0;
	pCanvas->CurrentRange.nLeft = 0;
	pCanvas->CurrentRange.nTop = 0;
	pCanvas->CurrentRange.nWidth = 0;
	pCanvas->CurrentRange.nHeight = 0;
	pCanvas->nFrameNum = 0;
	pCanvas->hFreeType = NULL;
	pCanvas->pCurrentFont = NULL;
	return pCanvas;
}

// Delete canvas.
void canvas_delete(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	canvas_unload(pCanvas);
	
	if (pCanvas->pVertices)
	{
		K_FREE(pCanvas->pVertices);
	}

	if (pCanvas->pBatchs)
	{
		K_FREE(pCanvas->pBatchs);
	}

	K_DELETE(pCanvas);
}

// Load resource.
bool canvas_load(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	FT_Error ft_error = FT_Init_FreeType(&pCanvas->hFreeType);

	if (ft_error != 0)
	{
		K_LOG("(canvas_load)Initialize freetype error.");
		abort();
		return false;
	}
	
	IRenderService* pRS = pCanvas->pRenderService;

	// Load shader for draw shape.
	pCanvas->pShapeShader = pRS->LoadShaderProgram("flat", "main", 
		"#define SHAPE 1\n", "flat", "main", "#define SHAPE 1\n");

	if (NULL == pCanvas->pShapeShader)
	{
		K_LOG("(canvas_load)Load shape shader error.");
		abort();
		return false;
	}

	// Load shader for draw image.
	pCanvas->pImageShader = pRS->LoadShaderProgram("flat", "main",
		"#define IMAGE 1\n", "flat", "main", "#define IMAGE 1\n");
	
	if (NULL == pCanvas->pImageShader)
	{
		K_LOG("(canvas_load)Load image shader error.");
		abort();
		return false;
	}

	pCanvas->pImageShader->SetSamplerUniformName(0, "tex_Diffuse");

	// Load shader for draw text.
	pCanvas->pTextShader = pRS->LoadShaderProgram("flat", "main",
		"#define TEXT 1\n", "flat", "main", "#define TEXT 1\n");

	if (NULL == pCanvas->pTextShader)
	{
		K_LOG("(canvas_load)Load text shader error.");
		abort();
		return false;
	}

	pCanvas->pTextShader->SetSamplerUniformName(0, "tex_Diffuse");

	rhi_vertex_attrib_t vtx_attribs[3] = {
		{ "position0", 0, RHI_VERTEX_DATA_FLOAT, 3, 0, 0, false },
		{ "color0", 1, RHI_VERTEX_DATA_UNSIGNED_BYTE, 4, 0, 12, true },
		{ "texture0", 2, RHI_VERTEX_DATA_FLOAT, 2, 0, 16, false }
	};

	pCanvas->pVertexDesc = pRS->CreateVertexDescriptor(vtx_attribs, 3, 
		sizeof(flat_vertex_t), pCanvas->pShapeShader);

	rhi_sampler_desc_t sampler_desc;

	sampler_desc.nMinFilter = RHI_TEXTURE_FILTER_LINEAR;
	sampler_desc.nMagFilter = RHI_TEXTURE_FILTER_LINEAR;
	sampler_desc.nMipFilter = RHI_TEXTURE_FILTER_LINEAR;
	sampler_desc.nAddressU = RHI_TEXTURE_ADDRESS_WRAP;
	sampler_desc.nAddressV = RHI_TEXTURE_ADDRESS_WRAP;
	sampler_desc.nAddressW = RHI_TEXTURE_ADDRESS_WRAP;
	sampler_desc.fMinLOD = 0.0F;
	sampler_desc.fMaxLOD = 0.0F;
	sampler_desc.nCompareMode = 0;
	sampler_desc.nCompareFunc = RHI_TEXTURE_COMPARISON_NEVER;
	pCanvas->pSamplerState = pRS->CreateSamplerState(sampler_desc);

	rhi_rasterize_desc_t raster_desc;

	raster_desc.nFillMode = RHI_FILL_SOLID;
	raster_desc.nCullMode = RHI_CULL_BACK;
	raster_desc.bFrontCounterClockwise = true;
	raster_desc.nDepthBias = 0;
	raster_desc.fDepthBiasClamp = 0.0F;
	raster_desc.fSlopeScaledDepthBias = 0.0F;
	raster_desc.bDepthClipEnable = false;
	raster_desc.bScissorEnable = true;
	raster_desc.bMultisampleEnable = false;
	raster_desc.bAntialiasedLineEnable = false;
	pCanvas->pRasterizeState = pRS->CreateRasterizeState(raster_desc);

	rhi_depth_stencil_desc_t depth_desc;

	memset(&depth_desc, 0, sizeof(depth_desc));
	pCanvas->pDepthStencilState = pRS->CreateDepthStencilState(depth_desc);

	rhi_blend_desc_t blend_desc;

	memset(&blend_desc, 0, sizeof(blend_desc));
	blend_desc.Blends[0].bBlendEnable = true;
	blend_desc.Blends[0].nBlendOp = RHI_BLEND_OP_ADD;
	blend_desc.Blends[0].nSrcBlend = RHI_BLEND_SRC_ALPHA;
	blend_desc.Blends[0].nDestBlend = RHI_BLEND_INV_SRC_ALPHA;
	blend_desc.Blends[0].nBlendOpAlpha = RHI_BLEND_OP_ADD;
	blend_desc.Blends[0].nSrcBlendAlpha = RHI_BLEND_SRC_ALPHA;
	blend_desc.Blends[0].nDestBlendAlpha = RHI_BLEND_ZERO;
	blend_desc.Blends[0].nWriteMask = 0xFFFFFFFF;
	pCanvas->pBlendState = pRS->CreateBlendState(blend_desc);
	return true;
}

// Delete font data.
static void delete_font(flat_font_type_t* pFont)
{
	if (pFont->hFontFace)
	{
		FT_Done_Face(pFont->hFontFace);
	}

	if (pFont->pFileData)
	{
		K_FREE(pFont->pFileData);
	}

	if (pFont->pFontImage)
	{
		flat_font_image_t* pImage = pFont->pFontImage;

		if (--pImage->nRefCount == 0)
		{
			if (pImage->pImageTex)
			{
				pImage->pImageTex->Release();
			}

			if (pImage->pBlocks)
			{
				K_FREE(pImage->pBlocks);
			}
			
			K_DELETE(pImage);
		}
	}

	K_DELETE(pFont);
}

// Unload resource.
bool canvas_unload(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	for (size_t i = 0; i < pCanvas->Fonts.Size(); ++i)
	{
		delete_font(pCanvas->Fonts[i]);
	}

	pCanvas->Fonts.Clear();
	pCanvas->pCurrentFont = NULL;
	FT_Done_FreeType(pCanvas->hFreeType);

	SAFE_RELEASE(pCanvas->pShapeShader);
	SAFE_RELEASE(pCanvas->pImageShader);
	SAFE_RELEASE(pCanvas->pTextShader);
	SAFE_RELEASE(pCanvas->pVertexDesc);
	SAFE_RELEASE(pCanvas->pDynamicVB);
	SAFE_RELEASE(pCanvas->pSamplerState);
	SAFE_RELEASE(pCanvas->pRasterizeState);
	SAFE_RELEASE(pCanvas->pDepthStencilState);
	SAFE_RELEASE(pCanvas->pBlendState);
	return true;
}

// Render begin process.
bool canvas_render_begin(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	IRenderService* pRS = pCanvas->pRenderService;
	
	// Increase frame number.
	pCanvas->nFrameNum++;
	// Clear all batchs.
	pCanvas->nBatchNum = 0;
	pCanvas->nVertexNum = 0;
	// Initialize range.
	pCanvas->CurrentRange.nLeft = 0;
	pCanvas->CurrentRange.nTop = 0;
	pCanvas->CurrentRange.nWidth = pRS->GetRenderWidth();
	pCanvas->CurrentRange.nHeight = pRS->GetRenderHeight();
	pCanvas->RangeStack.Clear();
	return true;
}

// Render end process.
bool canvas_render_end(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	IRenderService* pRS = pCanvas->pRenderService;
	RHIDevice* pDevice = pRS->GetRenderDevice();

	pDevice->SetSamplerState(0, pCanvas->pSamplerState);
	pDevice->SetRasterizeState(pCanvas->pRasterizeState);
	pDevice->SetDepthStencilState(pCanvas->pDepthStencilState, 0);
	pDevice->SetBlendState(pCanvas->pBlendState, NULL, 0xFFFFFFFF);

	if (pCanvas->nVertexNum > 0)
	{
		size_t vertex_num = pCanvas->nVertexNum;
		size_t buffer_size = vertex_num * sizeof(flat_vertex_t);
		RHIDynamicVertexBuffer* pVB = pCanvas->pDynamicVB;

		if (NULL == pVB)
		{
			pVB = pRS->CreateDynamicVertexBuffer(
				(unsigned int)buffer_size, sizeof(flat_vertex_t));
			pCanvas->pDynamicVB = pVB;
		}
		else if (buffer_size > pVB->GetBufferSize())
		{
			pVB->Release();
			pVB = pRS->CreateDynamicVertexBuffer(
				(unsigned int)buffer_size, sizeof(flat_vertex_t));
			pCanvas->pDynamicVB = pVB;
		}

		pDevice->UpdateDynamicVertexBuffer(pVB, 0, pCanvas->pVertices, 
			(unsigned int)vertex_num * sizeof(flat_vertex_t));

		RHIVertexBuffer* vbs[] = { pVB };

		pDevice->SetVertexArray(1, vbs, pCanvas->pVertexDesc);
	}
	
	size_t batch_num = pCanvas->nBatchNum;

	for (size_t i = 0; i < batch_num; ++i)
	{
		flat_batch_t* pBatch = pCanvas->pBatchs + i;
		int batch_type = pBatch->nBatchType;

		switch (batch_type)
		{
		case BATCH_TYPE_RANGE:
		{
			pDevice->SetScissorRect(pBatch->nRectLeft, pBatch->nRectTop,
				pBatch->nRectWidth, pBatch->nRectHeight);
			break;
		}
		case BATCH_TYPE_SHAPE:
		{
			pDevice->SetShaderProgram(pCanvas->pShapeShader);
			pDevice->Draw(pBatch->nPrimitiveType, pBatch->nStartVertex,
				pBatch->nVertexCount);
			break;
		}
		case BATCH_TYPE_IMAGE:
		{
			pDevice->SetShaderProgram(pCanvas->pImageShader);
			pDevice->SetShaderTexture(0, pBatch->pTexture->GetShaderTexture());
			pDevice->Draw(pBatch->nPrimitiveType, pBatch->nStartVertex,
				pBatch->nVertexCount);
			break;
		}
		case BATCH_TYPE_TEXT:
		{
			pDevice->SetShaderProgram(pCanvas->pTextShader);
			pDevice->SetDynamicTexture(0, pBatch->pDynamicTex);
			pDevice->Draw(pBatch->nPrimitiveType, pBatch->nStartVertex,
				pBatch->nVertexCount);
			break;
		}
		default:
			Assert(0);
			break;
		}
	}

	return true;
}

// Get usable font image block.
static int get_font_block_index(flat_font_type_t* pFont)
{
	Assert(pFont != NULL);

	flat_font_image_t* pImage = pFont->pFontImage;

	Assert(pImage != NULL);

	int block_index = 0;
	unsigned int min_frame_num = pImage->pBlocks[0].nLastUseFrame;

	for (int i = 0; i < pImage->nBlockNum; ++i)
	{
		if (pImage->pBlocks[i].pFont == NULL)
		{
			// Got first available block.
			block_index = i;
			break;
		}
		
		if (pImage->pBlocks[i].nLastUseFrame < min_frame_num)
		{
			block_index = i;
			min_frame_num = pImage->pBlocks[i].nLastUseFrame;
		}
	}

	return block_index;
}

// Load character glyph to font image.
static bool load_char_to_image(flat_canvas_t* pCanvas, flat_font_type_t* pFont,
	unsigned int ch)
{
	Assert(pCanvas != NULL);
	Assert(pFont != NULL);
	
	FT_UInt char_index = FT_Get_Char_Index(pFont->hFontFace, ch);
	FT_Int32 load_flags;

	if (pFont->bSmooth || pFont->bItalic || pFont->bBold)
	{
		load_flags = FT_LOAD_NO_BITMAP;
	}
	else
	{
		load_flags = FT_LOAD_DEFAULT;
	}
	
	FT_Error ft_error = FT_Load_Glyph(pFont->hFontFace, char_index, load_flags);

	if (ft_error != 0)
	{
		return false;
	}

	if (pFont->bBold)
	{
		ft_error = FT_Outline_Embolden(&pFont->hFontFace->glyph->outline,
			0x1 << 6);
	}

	ft_error = FT_Render_Glyph(pFont->hFontFace->glyph, FT_RENDER_MODE_NORMAL);

	if (ft_error != 0)
	{
		return false;
	}

	flat_font_image_t* pImage = pFont->pFontImage;

	Assert(pImage != NULL);

	FT_GlyphSlot glyph = pFont->hFontFace->glyph;
	int block_size = pImage->nBlockSize;
	unsigned int block_data_size = block_size * block_size;
	TAutoBuffer<unsigned char, 1024, FrameAlloc> auto_buf(block_data_size);
	unsigned char* block_data = auto_buf.GetBuffer();
	int beg_row = 0;
	int beg_col = 0;

	if (glyph->bitmap_left > 0)
	{
		beg_col += glyph->bitmap_left;
	}

	if ((pFont->nCharHeight - glyph->bitmap_top) > 0)
	{
		beg_row += pFont->nCharHeight - glyph->bitmap_top;
	}

	unsigned char* dst = block_data + beg_row * block_size + beg_col;
	unsigned char* src = glyph->bitmap.buffer;
	unsigned int pitch = glyph->bitmap.pitch;
	unsigned int char_width = glyph->bitmap.width;
	int rows = glyph->bitmap.rows;
	int cols = glyph->bitmap.width;

	memset(block_data, 0, block_data_size);

	if (glyph->bitmap.pixel_mode == FT_PIXEL_MODE_MONO)
	{
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				unsigned char value = src[c >> 3];
				unsigned char mask = 0x1 << (7 - (c & 0x7));

				if (value & mask)
				{
					dst[c] = 0xFF;
				}
				else
				{
					dst[c] = 0x0;
				}
			}

			dst += block_size;
			src += pitch;
		}
	}
	else
	{
		for (int r = 0; r < rows; ++r)
		{
			memcpy(dst, src, cols);
			dst += block_size;
			src += pitch;
		}
	}

	int block_index = get_font_block_index(pFont);
	int block_dims = pImage->nBlockDims;
	int block_row = (unsigned int)block_index / (unsigned int)block_dims;
	int block_col = (unsigned int)block_index % (unsigned int)block_dims;
	int x_offset = block_col * block_size;
	int y_offset = block_row * block_size;
	RHIDevice* pDevice = pCanvas->pRenderService->GetRenderDevice();

	if (!pDevice->UpdateDynamicTextureRegion(pImage->pImageTex, 0, x_offset,
		y_offset, block_size, block_size, block_data, block_data_size))
	{
		return false;
	}

	flat_font_type_t* pOldFont = pImage->pBlocks[block_index].pFont;

	if (pOldFont)
	{
		unsigned int old_char = pImage->pBlocks[block_index].nChar;
		
		pOldFont->CharMap.Remove(old_char);
	}
	
	flat_font_char_t new_char;

	new_char.nBlockIndex = (unsigned short)block_index;
	new_char.nCharWidth = (unsigned short)char_width;
	pFont->CharMap.Add(ch, new_char);
	pImage->pBlocks[block_index].pFont = pFont;
	pImage->pBlocks[block_index].nChar = ch;
	pImage->pBlocks[block_index].nLastUseFrame = pCanvas->nFrameNum;
	return true;
}

// Add font.
flat_font_type_t* canvas_add_font(flat_canvas_t* pCanvas,
	const flat_font_desc_t& desc)
{
	Assert(pCanvas != NULL);

	char file_name[512];

	plat_safe_sprintf(file_name, sizeof(file_name), "%s%s",
		frame_get_asset_path(), desc.pResource);

	void* fp = frame_fopen(file_name, "rb");

	if (NULL == fp)
	{
		K_LOG("(canvas_add_font)Open font file error.");
		K_LOG(desc.pResource);
		return NULL;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	size_t file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	unsigned char* file_data = (unsigned char*)K_ALLOC(file_size);

	if (frame_fread(file_data, 1, file_size, fp) != file_size)
	{
		frame_fclose(fp);
		K_FREE(file_data);
		K_LOG("(canvas_add_font)Read font file error.");
		K_LOG(desc.pResource);
		return NULL;
	}

	frame_fclose(fp);

	FT_Face font_face;
	FT_Error ft_error = FT_New_Memory_Face(pCanvas->hFreeType, 
		(FT_Byte*)file_data, (FT_Long)file_size, 0, &font_face);

	if (ft_error != 0)
	{
		K_FREE(file_data);
		
		if (ft_error == FT_Err_Unknown_File_Format)
		{
			K_LOG("(canvas_add_font)Font file format error.");
			K_LOG(desc.pResource);
			return NULL;
		}
		
		K_LOG("(canvas_add_font)FT_New_Memory_Face error.");
		K_LOG(desc.pResource);
		return NULL;
	}

	ft_error = FT_Set_Pixel_Sizes(font_face, 0, desc.nCharHeight);

	if (ft_error != 0)
	{
		K_FREE(file_data);
		K_LOG("(canvas_add_font)FT_Set_Pixel_Sizes error.");
		K_LOG(desc.pResource);
		return NULL;
	}

	if (desc.bItalic)
	{
		FT_Matrix mtx;

		mtx.xx = 0x10000;
		mtx.xy = (FT_Fixed)(0.5F * (float)0x10000);
		mtx.yx = 0;
		mtx.yx = 0x10000;
		FT_Set_Transform(font_face, &mtx, NULL);
	}

	int char_size = desc.nCharHeight + 3;
	int block_size = 16;

	while (block_size < char_size)
	{
		block_size *= 2;
	}

	Assert(block_size <= 256);

	flat_font_image_t* pImage = NULL;

	if (desc.nImageGroup > 0)
	{
		// Search shareable font image.
		for (size_t i = 0; i < pCanvas->Fonts.Size(); ++i)
		{
			flat_font_type_t* font = pCanvas->Fonts[i];

			if ((font->nImageGroup == desc.nImageGroup) &&
				(font->pFontImage->nImageSize == desc.nImageSize) &&
				(font->pFontImage->nBlockSize == block_size))
			{
				pImage = font->pFontImage;
				// Increase reference count of font image.
				pImage->nRefCount += 1;
				break;
			}
		}
	}

	if (NULL == pImage)
	{
		int block_dims = desc.nImageSize / block_size;
		int block_num = block_dims * block_dims;
		RHIDynamicTexture* pTex = pCanvas->pRenderService->CreateDynamicTexture(
			desc.nImageSize, desc.nImageSize, RHI_FORMAT_A8, 1);
		
		pImage = K_NEW(flat_font_image_t);
		pImage->nRefCount = 1;
		pImage->pImageTex = pTex;
		pImage->nImageSize = desc.nImageSize;
		pImage->nBlockSize = block_size;
		pImage->nBlockDims = block_dims;
		pImage->nBlockNum = block_num;
		pImage->pBlocks = (flat_font_block_t*)K_ALLOC(
			block_num * sizeof(flat_font_block_t));
		memset(pImage->pBlocks, 0, block_num * sizeof(flat_font_block_t));
	}

	flat_font_type_t* pFont = K_NEW(flat_font_type_t);

	pFont->sName = desc.pName;
	pFont->nNameHash = util_string_hash(desc.pName);
	pFont->hFontFace = font_face;
	pFont->nImageGroup = desc.nImageGroup;
	pFont->nCharHeight = desc.nCharHeight;
	pFont->nInterval = desc.nInterval;
	pFont->bFixedWidth = desc.bFixedWidth;
	pFont->bSmooth = desc.bSmooth;
	pFont->bItalic = desc.bItalic;
	pFont->bBold = desc.bBold;
	pFont->pFileData = file_data;
	pFont->nFileSize = file_size;
	pFont->pFontImage = pImage;
	pCanvas->Fonts.PushBack(pFont);
	return pFont;
}

// Remove font.
bool canvas_remove_font(flat_canvas_t* pCanvas, const char* font_name)
{
	Assert(pCanvas != NULL);
	Assert(font_name != NULL);

	unsigned int name_hash = util_string_hash(font_name);

	for (size_t i = 0; i < pCanvas->Fonts.Size(); ++i)
	{
		flat_font_type_t* pFont = pCanvas->Fonts[i];

		if ((pFont->nNameHash == name_hash) && (pFont->sName == font_name))
		{
			flat_font_image_t* pImage = pFont->pFontImage;

			Assert(pImage != NULL);

			// Is shared font image.
			if (pImage->nRefCount > 1)
			{
				// clear font relate information in font image
				for (size_t k = 0; k < pImage->nBlockNum; ++k)
				{
					if (pImage->pBlocks[k].pFont == pFont)
					{
						pImage->pBlocks[k].pFont = NULL;
						pImage->pBlocks[k].nChar = 0;
						pImage->pBlocks[k].nLastUseFrame = 0;
					}
				}
			}

			if (pCanvas->pCurrentFont == pFont)
			{
				pCanvas->pCurrentFont = NULL;
			}
			
			delete_font(pFont);
			pCanvas->Fonts.Remove(i);
			return true;
		}
	}

	return false;
}

// Set current font.
bool canvas_set_font(flat_canvas_t* pCanvas, flat_font_type_t* pFontType)
{
	Assert(pCanvas != NULL);

	pCanvas->pCurrentFont = pFontType;
	return true;
}

// Get vertex data buffer.
static flat_vertex_t* get_vertex(flat_canvas_t* pCanvas, size_t vertex_num)
{
	Assert(pCanvas != NULL);

	size_t need_size = pCanvas->nVertexNum + vertex_num;

	if (need_size > pCanvas->nVertexSize)
	{
		size_t new_size = pCanvas->nVertexSize * 2;

		if (0 == new_size)
		{
			new_size = 1024;
		}

		if (new_size < need_size)
		{
			new_size = need_size;
		}

		flat_vertex_t* p = (flat_vertex_t*)K_ALLOC(
			sizeof(flat_vertex_t) * new_size);

		if (pCanvas->pVertices)
		{
			memcpy(p, pCanvas->pVertices, 
				sizeof(flat_vertex_t) * pCanvas->nVertexNum);
			K_FREE(pCanvas->pVertices);
		}

		pCanvas->nVertexSize = new_size;
		pCanvas->pVertices = p;
	}

	return pCanvas->pVertices + pCanvas->nVertexNum;
}

// Get render batch.
static flat_batch_t* get_batch(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);

	size_t need_size = pCanvas->nBatchNum + 1;

	if (need_size > pCanvas->nBatchSize)
	{
		size_t new_size = pCanvas->nBatchSize * 2;

		if (0 == new_size)
		{
			new_size = 256;
		}

		flat_batch_t* p = (flat_batch_t*)K_ALLOC(
			sizeof(flat_batch_t) * new_size);

		if (pCanvas->pBatchs)
		{
			memcpy(p, pCanvas->pBatchs, 
				sizeof(flat_batch_t) * pCanvas->nBatchNum);
			K_FREE(pCanvas->pBatchs);
		}

		pCanvas->nBatchSize = new_size;
		pCanvas->pBatchs = p;
	}
	
	return pCanvas->pBatchs + pCanvas->nBatchNum;
}

// Get X coordinate in screen space.
static inline float get_screen_x(int x, int render_width)
{
	return (((float)x + 0.25F) / (float)render_width - 0.5F) * 2.0F;
}

// Get Y coordinate in screen space.
static inline float get_screen_y(int y, int render_height)
{
	return -(((float)y + 0.25F) / (float)render_height - 0.5F) * 2.0F;
}

// Get texture U coordinate.
static inline float get_tex_u(int x, int tex_width)
{
	return (float)x / (float)tex_width;
}

// Get texture V coordinate.
static inline float get_tex_v(int y, int tex_height)
{
	return (float)y / (float)tex_height;
}

// Set view range.
bool canvas_set_range(flat_canvas_t* pCanvas, int left, int top, int width,
	int height)
{
	Assert(pCanvas != NULL);

	flat_range_t& cr = pCanvas->CurrentRange;
	int cr_left = cr.nLeft;
	int cr_top = cr.nTop;
	int cr_right = cr_left + cr.nWidth;
	int cr_bottom = cr_top + cr.nHeight;

	if ((left >= cr_right) || (top >= cr_bottom) ||
		((left + width) < cr_left) || ((top + height) < cr_top))
	{
		return false;
	}

	if (left < cr_left)
	{
		width = width - (cr_left - left);
		left = cr_left;
	}

	if ((left + width) > cr_right)
	{
		width = cr_right - left;
	}

	if (top < cr_top)
	{
		height = height - (cr_top - top);
		top = cr_top;
	}

	if ((top + height) > cr_bottom)
	{
		height = cr_bottom - top;
	}

	if ((0 == width) || (0 == height))
	{
		return false;
	}

	pCanvas->RangeStack.PushBack(pCanvas->CurrentRange);
	pCanvas->CurrentRange.nLeft = left;
	pCanvas->CurrentRange.nTop = top;
	pCanvas->CurrentRange.nWidth = width;
	pCanvas->CurrentRange.nHeight = height;

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_RANGE;
	pBatch->nRectLeft = left;
	pBatch->nRectTop = top;
	pBatch->nRectWidth = width;
	pBatch->nRectHeight = height;
	pCanvas->nBatchNum++;
	return true;
}

// Reset view range.
bool canvas_reset_range(flat_canvas_t* pCanvas)
{
	Assert(pCanvas != NULL);
	Assert(pCanvas->RangeStack.Size() > 0);

	if (pCanvas->RangeStack.Empty())
	{
		return false;
	}

	flat_range_t& range = pCanvas->RangeStack.Back();
	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_RANGE;
	pBatch->nRectLeft = range.nLeft;
	pBatch->nRectTop = range.nTop;
	pBatch->nRectWidth = range.nWidth;
	pBatch->nRectHeight = range.nHeight;
	pCanvas->nBatchNum++;
	pCanvas->CurrentRange = range;
	pCanvas->RangeStack.PopBack();
	return true;
}

// Draw lines.
bool canvas_draw_lines(flat_canvas_t* pCanvas, int* coords, int line_num,
	unsigned int color)
{
	Assert(pCanvas != NULL);
	Assert(coords != NULL);
	Assert(line_num > 0);

	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	int vertex_num = line_num * 2;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

	memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);

	for (int i = 0; i < line_num; ++i)
	{
		vtx[0].fPosX = get_screen_x(coords[0], render_width);
		vtx[0].fPosY = get_screen_y(coords[1], render_height);
		vtx[0].nColor = color;
		vtx[1].fPosX = get_screen_x(coords[2], render_width);
		vtx[1].fPosY = get_screen_y(coords[3], render_height);
		vtx[1].nColor = color;
		vtx += 2;
		coords += 4;
	}

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_SHAPE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_LINE_LIST;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return true;
}

// Draw line frame.
bool canvas_draw_frame(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color)
{
	Assert(pCanvas != NULL);

	/*
	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	int vertex_num = 5;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

	memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);
	vtx[0].fPosX = get_screen_x(x1, render_width);
	vtx[0].fPosY = get_screen_y(y1, render_height);
	vtx[0].nColor = color;
	vtx[1].fPosX = get_screen_x(x2, render_width);
	vtx[1].fPosY = get_screen_y(y1, render_height);
	vtx[1].nColor = color;
	vtx[2].fPosX = get_screen_x(x2, render_width);
	vtx[2].fPosY = get_screen_y(y2, render_height);
	vtx[2].nColor = color;
	vtx[3].fPosX = get_screen_x(x1, render_width);
	vtx[3].fPosY = get_screen_y(y2, render_height);
	vtx[3].nColor = color;
	vtx[4].fPosX = get_screen_x(x1, render_width);
	vtx[4].fPosY = get_screen_y(y1, render_height);
	vtx[4].nColor = color;

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_SHAPE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_LINE_STRIP;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	*/
	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	int vertex_num = 8;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

	memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);
	
	vtx[0].fPosX = get_screen_x(x1, render_width);
	vtx[0].fPosY = get_screen_y(y1, render_height);
	vtx[0].nColor = color;
	vtx[1].fPosX = get_screen_x(x2, render_width);
	vtx[1].fPosY = get_screen_y(y1, render_height);
	vtx[1].nColor = color;

	vtx[2].fPosX = get_screen_x(x1, render_width);
	vtx[2].fPosY = get_screen_y(y1, render_height);
	vtx[2].nColor = color;
	vtx[3].fPosX = get_screen_x(x1, render_width);
	vtx[3].fPosY = get_screen_y(y2, render_height);
	vtx[3].nColor = color;

	vtx[4].fPosX = get_screen_x(x2 - 1, render_width);
	vtx[4].fPosY = get_screen_y(y1, render_height);
	vtx[4].nColor = color;
	vtx[5].fPosX = get_screen_x(x2 - 1, render_width);
	vtx[5].fPosY = get_screen_y(y2, render_height);
	vtx[5].nColor = color;

	vtx[6].fPosX = get_screen_x(x1, render_width);
	vtx[6].fPosY = get_screen_y(y2 - 1, render_height);
	vtx[6].nColor = color;
	vtx[7].fPosX = get_screen_x(x2, render_width);
	vtx[7].fPosY = get_screen_y(y2 - 1, render_height);
	vtx[7].nColor = color;

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_SHAPE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_LINE_LIST;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return true;
}

// Draw solid box.
bool canvas_draw_block(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	unsigned int color)
{
	Assert(pCanvas != NULL);

	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	int vertex_num = 4;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

	memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);
	vtx[0].fPosX = get_screen_x(x1, render_width);
	vtx[0].fPosY = get_screen_y(y1, render_height);
	vtx[0].nColor = color;
	vtx[1].fPosX = get_screen_x(x1, render_width);
	vtx[1].fPosY = get_screen_y(y2, render_height);
	vtx[1].nColor = color;
	vtx[2].fPosX = get_screen_x(x2, render_width);
	vtx[2].fPosY = get_screen_y(y1, render_height);
	vtx[2].nColor = color;
	vtx[3].fPosX = get_screen_x(x2, render_width);
	vtx[3].fPosY = get_screen_y(y2, render_height);
	vtx[3].nColor = color;

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_SHAPE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_TRIANGLE_STRIP;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return true;
}

// Set image vertex data.
static inline flat_vertex_t* set_image_vertex(flat_vertex_t* vtx,
	float px1, float py1, float px2, float py2, unsigned int color,
	float tu1, float tv1, float tu2, float tv2)
{
	vtx[0].fPosX = px1;
	vtx[0].fPosY = py1;
	vtx[0].nColor = color;
	vtx[0].fTexU = tu1;
	vtx[0].fTexV = tv1;
	vtx[1].fPosX = px2;
	vtx[1].fPosY = py1;
	vtx[1].nColor = color;
	vtx[1].fTexU = tu2;
	vtx[1].fTexV = tv1;
	vtx[2].fPosX = px1;
	vtx[2].fPosY = py2;
	vtx[2].nColor = color;
	vtx[2].fTexU = tu1;
	vtx[2].fTexV = tv2;
	vtx[3].fPosX = px1;
	vtx[3].fPosY = py2;
	vtx[3].nColor = color;
	vtx[3].fTexU = tu1;
	vtx[3].fTexV = tv2;
	vtx[4].fPosX = px2;
	vtx[4].fPosY = py1;
	vtx[4].nColor = color;
	vtx[4].fTexU = tu2;
	vtx[4].fTexV = tv1;
	vtx[5].fPosX = px2;
	vtx[5].fPosY = py2;
	vtx[5].nColor = color;
	vtx[5].fTexU = tu2;
	vtx[5].fTexV = tv2;
	return vtx + 6;
}

// Draw image.
bool canvas_draw_image(flat_canvas_t* pCanvas, int x1, int y1, int x2, int y2,
	flat_image_t* pImage, int layout)
{
	Assert(pCanvas != NULL);
	Assert(pImage != NULL);

	int tex_left;
	int tex_top;
	int tex_right;
	int tex_bottom;

	flat_image_get_rectangle(pImage, &tex_left, &tex_top, &tex_right, 
		&tex_bottom);

	ITexture* pTex = flat_image_get_texture(pImage);

	Assert(pTex != NULL);
	Assert(pTex->GetShaderTexture() != NULL);

	int tex_width = pTex->GetWidth();
	int tex_height = pTex->GetHeight();
	float tu1 = get_tex_u(tex_left, tex_width);
	float tv1 = get_tex_v(tex_top, tex_height);
	float tu2 = get_tex_u(tex_right, tex_width);
	float tv2 = get_tex_v(tex_bottom, tex_height);
	unsigned int color = 0xFFFFFFFF;
	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	float px1 = get_screen_x(x1, render_width);
	float py1 = get_screen_y(y1, render_height);
	float px2 = get_screen_x(x2, render_width);
	float py2 = get_screen_y(y2, render_height);

	if (layout == FLAT_LAYOUT_FIT)
	{
		int vertex_num = 4;
		flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

		memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);
		vtx[0].fPosX = px1;
		vtx[0].fPosY = py1;
		vtx[0].nColor = color;
		vtx[0].fTexU = tu1;
		vtx[0].fTexV = tv1;
		vtx[1].fPosX = px1;
		vtx[1].fPosY = py2;
		vtx[1].nColor = color;
		vtx[1].fTexU = tu1;
		vtx[1].fTexV = tv2;
		vtx[2].fPosX = px2;
		vtx[2].fPosY = py1;
		vtx[2].nColor = color;
		vtx[2].fTexU = tu2;
		vtx[2].fTexV = tv1;
		vtx[3].fPosX = px2;
		vtx[3].fPosY = py2;
		vtx[3].nColor = color;
		vtx[3].fTexU = tu2;
		vtx[3].fTexV = tv2;

		flat_batch_t* pBatch = get_batch(pCanvas);

		memset(pBatch, 0, sizeof(flat_batch_t));
		pBatch->nBatchType = BATCH_TYPE_IMAGE;
		pBatch->nPrimitiveType = RHI_PRIMITIVE_TRIANGLE_STRIP;
		pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
		pBatch->nVertexCount = vertex_num;
		pBatch->pTexture = pTex;
		pCanvas->nBatchNum++;
		pCanvas->nVertexNum += vertex_num;
		return true;
	}

	int vertex_num;

	if (layout == FLAT_LAYOUT_PATCH)
	{
		int width = x2 - x1;
		int height = y2 - y1;
		int tex_w = tex_right - tex_left;
		int tex_h = tex_bottom - tex_top;

		if ((tex_w <= width) || (tex_h <= height))
		{
			// Texture small then rectangle.
			return canvas_draw_image(pCanvas, x1, y1, x2, y2, pImage,
				FLAT_LAYOUT_FIT);
		}

		int quarter_w = width / 4;
		int quarter_h = height / 4;
		int quarter_tex_w = tex_w / 4;
		int quarter_tex_h = tex_h / 4;
		int hori_w = width - quarter_w - quarter_w;
		int vert_h = height - quarter_h - quarter_h;
		int hori_tex_w = tex_w - quarter_tex_w - quarter_tex_w;
		int vert_tex_h = tex_h - quarter_tex_h - quarter_tex_h;
		int hori_num = (hori_w + hori_tex_w - 1) / hori_tex_w;
		int vert_num = (vert_h + vert_tex_h - 1) / vert_tex_h;

		vertex_num = hori_num * vert_num + hori_num * 2 + vert_num * 2 + 4;

		flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

		memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);

		float px3 = get_screen_x(x1 + quarter_tex_w, render_width);
		float px4 = get_screen_x(x2 - quarter_tex_w, render_width);
		float py3 = get_screen_y(y1 + quarter_tex_h, render_height);
		float py4 = get_screen_y(y2 - quarter_tex_h, render_height);
		float tu3 = get_tex_u(tex_left + quarter_tex_w, tex_width);
		float tu4 = get_tex_u(tex_right - quarter_tex_w, tex_width);
		float tv3 = get_tex_v(tex_top + quarter_tex_h, tex_height);
		float tv4 = get_tex_v(tex_bottom - quarter_tex_h, tex_height);
		int total_w;
		int total_h;

		vtx = set_image_vertex(vtx, px1, py1, px3, py3, color, tu1, tv1, tu3,
			tv3);
		total_w = 0;

		for (int i = 0; i < hori_num; ++i)
		{
			int block_w = hori_tex_w;
			float tu5 = tu3;
			float tu6 = tu4;

			if ((total_w + block_w) > hori_w)
			{
				block_w = hori_w - total_w;
				tu6 = get_tex_u(tex_left + quarter_tex_w + block_w, tex_width);
			}

			float px5 = get_screen_x(x1 + quarter_tex_w + total_w,
				render_width);
			float px6 = get_screen_x(x1 + quarter_tex_w + total_w + block_w,
				render_width);

			vtx = set_image_vertex(vtx, px5, py1, px6, py3, color, tu5, tv1,
				tu6, tv3);
			total_w += block_w;
		}

		vtx = set_image_vertex(vtx, px4, py1, px2, py3, color, tu4, tv1, tu2,
			tv3);
		total_h = 0;

		for (int r = 0; r < vert_num; ++r)
		{
			int block_h = vert_tex_h;
			float tv5 = tv3;
			float tv6 = tv4;

			if ((total_h + block_h) > vert_h)
			{
				block_h = vert_h - total_h;
				tv6 = get_tex_v(tex_top + quarter_tex_h + block_h, tex_height);
			}

			float py5 = get_screen_y(y1 + quarter_tex_h + total_h,
				render_height);
			float py6 = get_screen_y(y1 + quarter_tex_h + total_h + block_h,
				render_height);

			vtx = set_image_vertex(vtx, px1, py5, px3, py6, color, tu1, tv5,
				tu3, tv6);
			total_w = 0;

			for (int c = 0; c < hori_num; ++c)
			{
				int block_w = hori_tex_w;
				float tu5 = tu3;
				float tu6 = tu4;

				if ((total_w + block_w) > hori_w)
				{
					block_w = hori_w - total_w;
					tu6 = get_tex_u(tex_left + quarter_tex_w + block_w, 
						tex_width);
				}

				float px5 = get_screen_x(x1 + quarter_tex_w + total_w,
					render_width);
				float px6 = get_screen_x(x1 + quarter_tex_w + total_w + block_w,
					render_width);

				vtx = set_image_vertex(vtx, px5, py5, px6, py6, color, tu5,
					tv5, tu6, tv6);
				total_w += block_w;
			}

			vtx = set_image_vertex(vtx, px4, py5, px2, py6, color, tu4, tv5,
				tu2, tv6);
			total_h += block_h;
		}

		vtx = set_image_vertex(vtx, px1, py4, px3, py2, color, tu1, tv4, tu3,
			tv2);
		total_w = 0;

		for (int k = 0; k < hori_num; ++k)
		{
			int block_w = hori_tex_w;
			float tu5 = tu3;
			float tu6 = tu4;

			if ((total_w + block_w) > hori_w)
			{
				block_w = hori_w - total_w;
				tu6 = get_tex_u(tex_left + quarter_tex_w + block_w, tex_width);
			}

			float px5 = get_screen_x(x1 + quarter_tex_w + total_w, 
				render_width);
			float px6 = get_screen_x(x1 + quarter_tex_w + total_w + block_w,
				render_width);

			vtx = set_image_vertex(vtx, px5, py4, px6, py2, color, tu5, tv4,
				tu6, tv2);
			total_w += block_w;
		}

		vtx = set_image_vertex(vtx, px4, py4, px2, py2, color, tu4, tv4, tu2,
			tv2);
	}
	else if (layout == FLAT_LAYOUT_HPATCH)
	{
		int width = x2 - x1;
		int tex_w = tex_right - tex_left;
		
		if (tex_w <= width)
		{
			// Texture small then rectangle.
			return canvas_draw_image(pCanvas, x1, y1, x2, y2, pImage,
				FLAT_LAYOUT_FIT);
		}
		
		int quarter_w = width / 4;
		int quarter_tex_w = tex_w / 4;
		int hori_w = width - quarter_w - quarter_w;
		int hori_tex_w = tex_w - quarter_tex_w - quarter_tex_w;
		int hori_num = (hori_w + hori_tex_w - 1) / hori_tex_w;

		vertex_num = (hori_num + 2) * 6;
		
		flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

		memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);

		float px3 = get_screen_x(x1 + quarter_tex_w, render_width);
		float px4 = get_screen_x(x2 - quarter_tex_w, render_width);
		float tu3 = get_tex_u(tex_left + quarter_tex_w, tex_width);
		float tu4 = get_tex_u(tex_right - quarter_tex_w, tex_width);

		vtx = set_image_vertex(vtx, px1, py1, px3, py2, color, tu1, tv1, tu3,
			tv2);

		int total_w = 0;

		for (int i = 0; i < hori_num; ++i)
		{
			int block_w = hori_tex_w;
			float tu5 = tu3;
			float tu6 = tu4;

			if ((total_w + block_w) > hori_w)
			{
				block_w = hori_w - total_w;
				tu6 = get_tex_u(tex_left + quarter_tex_w + block_w, tex_width);
			}

			float px5 = get_screen_x(x1 + quarter_tex_w + total_w,
				render_width);
			float px6 = get_screen_x(x1 + quarter_tex_w + total_w + block_w,
				render_width);

			vtx = set_image_vertex(vtx, px5, py1, px6, py2, color, tu5, tv1,
				tu6, tv2);
			total_w += block_w;
		}

		vtx = set_image_vertex(vtx, px4, py1, px2, py2, color, tu4, tv1, tu2,
			tv2);
	}
	else if (layout == FLAT_LAYOUT_VPATCH)
	{
		int height = y2 - y1;
		int tex_h = tex_bottom - tex_top;
		
		if (tex_h <= height)
		{
			// Texture small then rectangle.
			return canvas_draw_image(pCanvas, x1, y1, x2, y2, pImage,
				FLAT_LAYOUT_FIT);
		}

		int quarter_h = height / 4;
		int quarter_tex_h = tex_h / 4;
		int vert_h = height - quarter_h - quarter_h;
		int vert_tex_h = tex_h - quarter_tex_h - quarter_tex_h;
		int vert_num = (vert_h + vert_tex_h - 1) / vert_tex_h;

		vertex_num = (vert_num + 2) * 6;

		flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

		memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);

		float py3 = get_screen_y(y1 + quarter_tex_h, render_height);
		float py4 = get_screen_y(y2 - quarter_tex_h, render_height);
		float tv3 = get_tex_v(tex_top + quarter_tex_h, tex_height);
		float tv4 = get_tex_v(tex_bottom - quarter_tex_h, tex_height);

		vtx = set_image_vertex(vtx, px1, py1, px2, py3, color, tu1, tv1, tu2,
			tv3);

		int total_h = 0;

		for (int i = 0; i < vert_num; ++i)
		{
			int block_h = vert_tex_h;
			float tv5 = tv3;
			float tv6 = tv4;

			if ((total_h + block_h) > vert_h)
			{
				block_h = vert_h - total_h;
				tv6 = get_tex_v(tex_top + quarter_tex_h + block_h, tex_height);
			}

			float py5 = get_screen_y(y1 + quarter_tex_h + total_h,
				render_height);
			float py6 = get_screen_y(y1 + quarter_tex_h + total_h + block_h,
				render_height);

			vtx = set_image_vertex(vtx, px1, py5, px2, py6, color, tu1, tv5,
				tu2, tv6);
			total_h += block_h;
		}

		vtx = set_image_vertex(vtx, px1, py4, px2, py2, color, tu1, tv4, tu2,
			tv2);
	}
	else
	{
		Assert(0);
		return false;
	}

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_IMAGE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_TRIANGLE_LIST;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pBatch->pTexture = pTex;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return true;
}

// Draw text.
static int draw_text(flat_canvas_t* pCanvas, int x, int y, unsigned int color,
	const char* text, size_t start, size_t char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	Assert(pFont != NULL);

	flat_font_image_t* pImage = pFont->pFontImage;

	Assert(pImage != NULL);

	size_t ws_size = plat_utf8_to_unicode_size(text);
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf(ws_size);
	wchar_t* ws_text = auto_buf.GetBuffer();

	plat_utf8_to_unicode(ws_text, ws_size * sizeof(wchar_t), text);

	size_t ws_len = ws_size - 1;

	if (start >= ws_len)
	{
		return 0;
	}

	if ((char_num > 0) && (ws_len > (start + char_num)))
	{
		ws_len = start + char_num;
	}

	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	int vertex_num = (int)(ws_len - start) * 6;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);
	int image_size = pImage->nImageSize;
	int block_size = pImage->nBlockSize;
	int block_dims = pImage->nBlockDims;
	int x1 = x;
	int y1 = y;
	int y2 = y1 + block_size;
	
	for (size_t i = start; i < ws_len; ++i)
	{
		unsigned int ch = ws_text[i];
		flat_font_char_t font_char;

		if (!pFont->CharMap.GetData(ch, &font_char))
		{
			if (!load_char_to_image(pCanvas, pFont, ch))
			{
				return 0;
			}

			if (!pFont->CharMap.GetData(ch, &font_char))
			{
				return 0;
			}
		}

		int block_index = font_char.nBlockIndex;
		int block_row = (unsigned int)block_index / (unsigned int)block_dims;
		int block_col = (unsigned int)block_index % (unsigned int)block_dims;
		int tex_x = block_col * block_size;
		int tex_y = block_row * block_size;
		float u1 = (float)tex_x / (float)image_size;
		float v1 = (float)tex_y / (float)image_size;
		float u2 = (float)(tex_x + block_size) / (float)image_size;
		float v2 = (float)(tex_y + block_size) / (float)image_size;
		int x2 = x1 + block_size;

		vtx[0].fPosX = get_screen_x(x1, render_width);
		vtx[0].fPosY = get_screen_y(y1, render_height);
		vtx[0].fPosZ = 0.0F;
		vtx[0].nColor = color;
		vtx[0].fTexU = u1;
		vtx[0].fTexV = v1;
		vtx[1].fPosX = get_screen_x(x1, render_width);
		vtx[1].fPosY = get_screen_y(y2, render_height);
		vtx[1].fPosZ = 0.0F;
		vtx[1].nColor = color;
		vtx[1].fTexU = u1;
		vtx[1].fTexV = v2;
		vtx[2].fPosX = get_screen_x(x2, render_width);
		vtx[2].fPosY = get_screen_y(y1, render_height);
		vtx[2].fPosZ = 0.0F;
		vtx[2].nColor = color;
		vtx[2].fTexU = u2;
		vtx[2].fTexV = v1;
		vtx[3].fPosX = get_screen_x(x2, render_width);
		vtx[3].fPosY = get_screen_y(y1, render_height);
		vtx[3].fPosZ = 0.0F;
		vtx[3].nColor = color;
		vtx[3].fTexU = u2;
		vtx[3].fTexV = v1;
		vtx[4].fPosX = get_screen_x(x1, render_width);
		vtx[4].fPosY = get_screen_y(y2, render_height);
		vtx[4].fPosZ = 0.0F;
		vtx[4].nColor = color;
		vtx[4].fTexU = u1;
		vtx[4].fTexV = v2;
		vtx[5].fPosX = get_screen_x(x2, render_width);
		vtx[5].fPosY = get_screen_y(y2, render_height);
		vtx[5].fPosZ = 0.0F;
		vtx[5].nColor = color;
		vtx[5].fTexU = u2;
		vtx[5].fTexV = v2;
		vtx += 6;

		int add_width;

		if (pFont->bFixedWidth)
		{
			if (ch < 256)
			{
				add_width = (pFont->nCharHeight + pFont->nInterval + 1) / 2;
			}
			else
			{
				add_width = pFont->nCharHeight + pFont->nInterval;
			}
		}
		else
		{
			if (ch == 0x20)
			{
				add_width = (pFont->nCharHeight + pFont->nInterval + 1) / 2;
			}
			else if (ch < 256)
			{
				add_width = font_char.nCharWidth + (pFont->nInterval + 1) / 2;
			}
			else
			{
				add_width = font_char.nCharWidth + pFont->nInterval;
			}
		}

		x1 += add_width;
		// Update last use frame number of font image block.
		pImage->pBlocks[block_index].nLastUseFrame = pCanvas->nFrameNum;
	}

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_TEXT;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_TRIANGLE_LIST;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pBatch->pDynamicTex = pImage->pImageTex;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return x1 - x;
}

// Get text width.
static int get_text_width(flat_canvas_t* pCanvas, const char* text,
	size_t start, size_t char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	Assert(pFont != NULL);

	size_t ws_size = plat_utf8_to_unicode_size(text);
	TAutoBuffer<wchar_t, 256, FrameAlloc> auto_buf(ws_size);
	wchar_t* ws_text = auto_buf.GetBuffer();

	plat_utf8_to_unicode(ws_text, ws_size * sizeof(wchar_t), text);

	size_t ws_len = ws_size - 1;

	if (start >= ws_len)
	{
		return 0;
	}

	if ((char_num > 0) && (ws_len > (start + char_num)))
	{
		ws_len = start + char_num;
	}

	int text_width = 0;
	
	if (pFont->bFixedWidth)
	{
		for (size_t i = start; i < ws_len; ++i)
		{
			unsigned int ch = ws_text[i];

			if (ch < 256)
			{
				text_width += (pFont->nCharHeight + pFont->nInterval + 1) / 2;
			}
			else
			{
				text_width += pFont->nCharHeight + pFont->nInterval;
			}
		}

		return text_width;
	}

	for (size_t i = start; i < ws_len; ++i)
	{
		unsigned int ch = ws_text[i];
		flat_font_char_t font_char;

		if (!pFont->CharMap.GetData(ch, &font_char))
		{
			if (!load_char_to_image(pCanvas, pFont, ch))
			{
				return 0;
			}

			if (!pFont->CharMap.GetData(ch, &font_char))
			{
				return 0;
			}
		}

		if (ch < 256)
		{
			text_width += font_char.nCharWidth + ((pFont->nInterval + 1) / 2);
		}
		else
		{
			text_width += font_char.nCharWidth + pFont->nInterval;
		}
	}

	return text_width;
}

// Draw text.
int canvas_draw_text(flat_canvas_t* pCanvas, int x, int y, unsigned int color,
	const char* text, int start, int char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);
	Assert(start >= 0);
	Assert(char_num >= 0);

	if (NULL == pCanvas->pCurrentFont)
	{
		return 0;
	}

	return draw_text(pCanvas, x, y, color, text, start, char_num);
}

// Draw text at left.
int canvas_draw_text_left(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);
	Assert(start >= 0);
	Assert(char_num >= 0);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	if (NULL == pFont)
	{
		return 0;
	}

	int x = x1;
	int y = y1 + ((y2 - y1 - pFont->nCharHeight + 1) / 2);

	return draw_text(pCanvas, x, y, color, text, start, char_num);
}

// Draw text at center.
int canvas_draw_text_center(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);
	Assert(start >= 0);
	Assert(char_num >= 0);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	if (NULL == pFont)
	{
		return 0;
	}

	int text_width = get_text_width(pCanvas, text, start, char_num);
	int x = x1 + ((x2 - x1 - text_width + 1) / 2);
	int y = y1 + ((y2 - y1 - pFont->nCharHeight + 1) / 2);

	return draw_text(pCanvas, x, y, color, text, start, char_num);
}

// Draw text at right.
int canvas_draw_text_right(flat_canvas_t* pCanvas, int x1, int y1, int x2,
	int y2, unsigned int color, const char* text, int start, int char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);
	Assert(start >= 0);
	Assert(char_num >= 0);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	if (NULL == pFont)
	{
		return 0;
	}

	int text_width = get_text_width(pCanvas, text, start, char_num);
	int x = x2 - text_width;
	int y = y1 + ((y2 - y1 - pFont->nCharHeight + 1) / 2);

	return draw_text(pCanvas, x, y, color, text, start, char_num);
}

// Get width for draw text.
int canvas_get_text_width(flat_canvas_t* pCanvas, const char* text, int start,
	int char_num)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);
	Assert(start >= 0);
	Assert(char_num >= 0);

	if (NULL == pCanvas->pCurrentFont)
	{
		return 0;
	}

	return get_text_width(pCanvas, text, start, char_num);
}

// Get height for draw text.
int canvas_get_text_height(flat_canvas_t* pCanvas, const char* text)
{
	Assert(pCanvas != NULL);
	Assert(text != NULL);

	flat_font_type_t* pFont = pCanvas->pCurrentFont;

	if (NULL == pFont)
	{
		return 0;
	}

	return pFont->nCharHeight;
}

// Get X coordinate in screen space.
static inline float get_render_x(float x, int render_width)
{
	return (x / (float)render_width - 0.5F) * 2.0F;
}

// Get Y coordinate in screen space.
static inline float get_render_y(float y, int render_height)
{
	return -(y / (float)render_height - 0.5F) * 2.0F;
}

// Draw image use float coordinate.
bool canvas_render_image(flat_canvas_t* pCanvas, float x1, float y1, float x2,
	float y2, float x3, float y3, float x4, float y4, flat_image_t* pImage)
{
	Assert(pCanvas != NULL);
	Assert(pImage != NULL);

	int tex_left;
	int tex_top;
	int tex_right;
	int tex_bottom;

	flat_image_get_rectangle(pImage, &tex_left, &tex_top, &tex_right,
		&tex_bottom);

	ITexture* pTex = flat_image_get_texture(pImage);

	Assert(pTex != NULL);
	Assert(pTex->GetShaderTexture() != NULL);

	int tex_width = pTex->GetWidth();
	int tex_height = pTex->GetHeight();
	float tu1 = get_tex_u(tex_left, tex_width);
	float tv1 = get_tex_v(tex_top, tex_height);
	float tu2 = get_tex_u(tex_right, tex_width);
	float tv2 = get_tex_v(tex_bottom, tex_height);
	unsigned int color = 0xFFFFFFFF;
	int render_width = pCanvas->nRenderWidth;
	int render_height = pCanvas->nRenderHeight;
	float px1 = get_render_x(x1, render_width);
	float py1 = get_render_y(y1, render_height);
	float px2 = get_render_x(x2, render_width);
	float py2 = get_render_y(y2, render_height);
	float px3 = get_render_x(x3, render_width);
	float py3 = get_render_y(y3, render_height);
	float px4 = get_render_x(x4, render_width);
	float py4 = get_render_y(y4, render_height);
	int vertex_num = 4;
	flat_vertex_t* vtx = get_vertex(pCanvas, vertex_num);

	memset(vtx, 0, sizeof(flat_vertex_t) * vertex_num);
	vtx[0].fPosX = px1;
	vtx[0].fPosY = py1;
	vtx[0].nColor = color;
	vtx[0].fTexU = tu1;
	vtx[0].fTexV = tv1;
	vtx[1].fPosX = px3;
	vtx[1].fPosY = py3;
	vtx[1].nColor = color;
	vtx[1].fTexU = tu1;
	vtx[1].fTexV = tv2;
	vtx[2].fPosX = px2;
	vtx[2].fPosY = py2;
	vtx[2].nColor = color;
	vtx[2].fTexU = tu2;
	vtx[2].fTexV = tv1;
	vtx[3].fPosX = px4;
	vtx[3].fPosY = py4;
	vtx[3].nColor = color;
	vtx[3].fTexU = tu2;
	vtx[3].fTexV = tv2;

	flat_batch_t* pBatch = get_batch(pCanvas);

	memset(pBatch, 0, sizeof(flat_batch_t));
	pBatch->nBatchType = BATCH_TYPE_IMAGE;
	pBatch->nPrimitiveType = RHI_PRIMITIVE_TRIANGLE_STRIP;
	pBatch->nStartVertex = (unsigned int)pCanvas->nVertexNum;
	pBatch->nVertexCount = vertex_num;
	pBatch->pTexture = pTex;
	pCanvas->nBatchNum++;
	pCanvas->nVertexNum += vertex_num;
	return true;
}
