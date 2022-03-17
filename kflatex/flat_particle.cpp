// flat_particle.cpp
// Created by lulimin on 2021/4/1.

#include "flat_particle.h"
#include "../inc/frame_mem.h"

struct flat_par_data_t
{
	float fAlpha;
	float fAlphaDeviation;
	float fSize;
	float fSizeDeviation;
	float fSpin;
	float fSpinDeviation;
	float fAngle;
	float fAngleDeviation;
	float fSpeed;
	float fSpeedDeviation;
	float fLive;
	float fLiveDeviation;
	float fAccel;
	float fAccelDeviation;
	float fTanAccel;
	float fTanAccelDeviation;
	float fRadius;
	float fRadiusDeviation;
};

struct flat_par_piece_t
{
	flat_par_data_t* pData;
	float fPosX;
	float fPosY;
	float fSize;
	float fSpin;
	float fAccel;
	float fTanAccel;
	float fLiveTime;
};

struct flat_particle_t
{
	flat_par_data_t* pData;
	flat_par_piece_t** pPieces;
	int nPieceNum;
};

flat_particle_t* flat_particle_create()
{
	flat_particle_t* pParticle = K_NEW(flat_particle_t);

	return pParticle;
}

void flat_particle_delete(flat_particle_t* pParticle)
{
	Assert(pParticle != NULL);

	K_DELETE(pParticle);
}
