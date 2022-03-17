// flat_matrix.cpp
// Created by lulimin on 2021/4/25.

#include "flat_matrix.h"
#include <math.h>

void flat_matrix_set_identity(flat_matrix_t* pMtx)
{
	float* d = pMtx->data;

	d[0] = 1.0F;
	d[1] = 0.0F;
	d[2] = 0.0F;
	d[3] = 1.0F;
	d[4] = 0.0F;
	d[5] = 0.0F;
}

void flat_matrix_set_translation(flat_matrix_t* pMtx, float tx, float ty)
{
	float* d = pMtx->data;

	d[4] = tx;
	d[5] = ty;
}

void flat_matrix_set_rotation(flat_matrix_t* pMtx, float radian)
{
	float* d = pMtx->data;
	float sin_value = sinf(radian);
	float cos_value = cosf(radian);

	d[0] = cos_value;
	d[1] = sin_value;
	d[2] = -sin_value;
	d[3] = cos_value;
}

void flat_matrix_set_scaling(flat_matrix_t*pMtx, float sx, float sy)
{
	float* d = pMtx->data;

	d[0] = sx;
	d[1] = 0.0F;
	d[2] = 0.0F;
	d[3] = sy;
}

void flat_matrix_set_rotation_scaling(flat_matrix_t* pMtx, float radian, float sx,
	float sy)
{
	float* d = pMtx->data;
	float sin_value = sinf(radian);
	float cos_value = cosf(radian);
	float cos_v0 = cos_value * sx;
	float sin_v0 = sin_value * sx;
	float cos_v1 = cos_value * sy;
	float sin_v1 = sin_value * sy;

	d[0] = cos_v0;
	d[1] = sin_v0;
	d[2] = -sin_v1;
	d[3] = cos_v1;
}

void flat_matrix_set_scaling_rotation(flat_matrix_t* pMtx, float sx, float sy,
	float radian)
{
	float* d = pMtx->data;
	float sin_value = sinf(radian);
	float cos_value = cosf(radian);
	float cos_v0 = cos_value * sx;
	float sin_v0 = sin_value * sx;
	float cos_v1 = cos_value * sy;
	float sin_v1 = sin_value * sy;

	d[0] = cos_v0;
	d[1] = sin_v1;
	d[2] = -sin_v0;
	d[3] = cos_v1;
}

void flat_matrix_translate(flat_matrix_t* pMtx, float tx, float ty)
{
	float* d = pMtx->data;
	
	d[4] = d[4] + tx;
	d[5] = d[5] + ty;
}

void flat_matrix_rotate(flat_matrix_t* pMtx, float radian)
{
	float* d = pMtx->data;
	float sin_value = sinf(radian);
	float cos_value = cosf(radian);
	float sin_v0 = d[0] * sin_value;
	float cos_v0 = d[0] * cos_value;
	float sin_v1 = d[1] * sin_value;
	float cos_v1 = d[1] * cos_value;
	float sin_v2 = d[2] * sin_value;
	float cos_v2 = d[2] * cos_value;
	float sin_v3 = d[3] * sin_value;
	float cos_v3 = d[3] * cos_value;
	float sin_v4 = d[4] * sin_value;
	float cos_v4 = d[4] * cos_value;
	float sin_v5 = d[5] * sin_value;
	float cos_v5 = d[5] * cos_value;

	d[0] = cos_v0 - sin_v1;
	d[1] = sin_v0 + cos_v1;
	d[2] = cos_v2 - sin_v3;
	d[3] = sin_v2 + cos_v3;
	d[4] = cos_v4 - sin_v5;
	d[5] = sin_v4 + cos_v5;
}

void flat_matrix_scale(flat_matrix_t* pMtx, float sx, float sy)
{
	float* d = pMtx->data;
	
	if (sx != 1.0F)
	{
		d[0] = d[0] * sx;
		d[2] = d[2] * sx;
		d[4] = d[4] * sx;
	}

	if (sy != 1.0F)
	{
		d[1] = d[1] * sy;
		d[3] = d[3] * sy;
		d[5] = d[5] * sy;
	}
}

void flat_matrix_multiply(flat_matrix_t* pMtx, const flat_matrix_t* pM1,
	const flat_matrix_t* pM2)
{
	float* d = pMtx->data;
	const float* d1 = pM1->data;
	const float* d2 = pM2->data;

	d[0] = d1[0] * d2[0] + d1[1] * d2[2];
	d[1] = d1[0] * d2[1] + d1[1] * d2[3];
	d[2] = d1[2] * d2[0] + d1[3] * d2[2];
	d[3] = d1[2] * d2[1] + d1[3] * d2[3];
	d[4] = d1[4] * d2[0] + d1[5] * d2[2] + d2[4];
	d[5] = d1[4] * d2[1] + d1[5] * d2[3] + d2[5];
}

bool flat_matrix_inverse(flat_matrix_t* pMtx, const flat_matrix_t* pMtxIn)
{
	float* d = pMtx->data;
	const float* in = pMtxIn->data;

	if ((in[1] == 0.0F) && (in[2] == 0.0F))
	{
		if ((in[0] == 0.0F) && (in[3] == 0.0F))
		{
			return false;
		}

		d[0] = 1.0F / in[0];
		d[1] = 0.0F;
		d[2] = 0.0F;;
		d[3] = 1.0F / in[3];
		d[4] = -(in[4] * d[0]);
		d[5] = -(in[5] * d[3]);
		return true;
	}

	if ((in[0] == 0.0F) && (in[3] == 0.0F))
	{
		if ((in[1] == 0.0F) && (in[2] == 0.0F))
		{
			return false;
		}

		d[0] = 0.0F;
		d[1] = 1.0F / in[2];
		d[2] = 1.0F / in[1];
		d[3] = 0.0F;
		d[4] = -(in[5] * in[2]);
		d[5] = -(in[4] * in[1]);
		return true;
	}

	float det = in[0] * in[3] - in[1] * in[2];

	if (det == 0.0F)
	{
		return false;
	}

	d[0] = in[3] / det;
	d[1] = -in[1] / det;
	d[2] = -in[2] / det;
	d[3] = in[0] / det;
	d[4] = -(in[4] * d[0] + in[5] * d[2]);
	d[5] = -(in[4] * d[1] + in[5] * d[3]);
	return true;
}

void flat_matrix_transform(flat_matrix_t* pMtx, float x, float y,
	float* pOutX, float* pOutY)
{
	const float* d = pMtx->data;
	float ox = d[0] * x + d[2] * y + d[4];
	float oy = d[1] * x + d[3] * y + d[5];

	*pOutX = ox;
	*pOutY = oy;
}
