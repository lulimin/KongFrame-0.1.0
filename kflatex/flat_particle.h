// flat_particle.h
// Created by lulimin on 2021/4/1.

#ifndef __FLAT_PARTICLE_H
#define __FLAT_PARTICLE_H

struct flat_particle_t;

// Create particle.
flat_particle_t* flat_particle_create();
// Delete particle.
void flat_particle_delete(flat_particle_t* pParticle);

#endif // __FLAT_PARTICLE_H
