// flat_matrix.h
// Created by lulimin on 2021/4/25.

#ifndef __FLAT_MATRIX_H
#define __FLAT_MATRIX_H

// 2D transform maxtrix.
struct flat_matrix_t
{
	float data[6];
};

// Set translation.
inline void flat_matrix_set_position(flat_matrix_t* pMtx, float x, float y)
{
	pMtx->data[4] = x;
	pMtx->data[5] = y;
}

// Set X coordinate of translation.
inline float flat_matrix_get_position_x(const flat_matrix_t* pMtx)
{
	return pMtx->data[4];
}

// Set Y coordinate of translation.
inline float flat_matrix_get_position_y(const flat_matrix_t* pMtx)
{
	return pMtx->data[5];
}

// Make identity matrix.
void flat_matrix_set_identity(flat_matrix_t* pMtx);
// Matrix set translation.
void flat_matrix_set_translation(flat_matrix_t* pMtx, float tx, float ty);
// Matrix set rotation.
void flat_matrix_set_rotation(flat_matrix_t* pMtx, float radian);
// Matrix set scaling.
void flat_matrix_set_scaling(flat_matrix_t* pMtx, float sx, float sy);
// Matrix set rotation and scaling.
void flat_matrix_set_rotation_scaling(flat_matrix_t* pMtx, float radian,
	float sx, float sy);
// Matrix set scaling and rotation.
void flat_matrix_set_scaling_rotation(flat_matrix_t* pMtx, float sx, float sy,
	float radian);
// Matrix translate.
void flat_matrix_translate(flat_matrix_t* pMtx, float tx, float ty);
// Matrix rotate.
void flat_matrix_rotate(flat_matrix_t* pMtx, float radian);
// Matrix scaling.
void flat_matrix_scale(flat_matrix_t* pMtx, float sx, float sy);
// Matrix multiply.
void flat_matrix_multiply(flat_matrix_t* pMtx, const flat_matrix_t* pM1,
	const flat_matrix_t* pM2);
// Matrix inverse.
bool flat_matrix_inverse(flat_matrix_t* pMtx, const flat_matrix_t* pMtxIn);
// Matrix transform point.
void flat_matrix_transform(flat_matrix_t* pMtx, float x, float y, 
	float* pOutX, float* pOutY);

#endif // __FLAT_MATRIX_H
