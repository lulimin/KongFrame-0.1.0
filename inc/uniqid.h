// uniqid.h
// Created by lulimin on 2019/2/14.

#ifndef __UNIQID_H
#define __UNIQID_H

#include "common.h"

// Object unique identity.

struct uniqid_t
{
	union
	{
		struct
		{
			uint64_t nValue64;
		};

		struct
		{
			unsigned int nIndex;
			unsigned int nOrder;
		};
	};

	uniqid_t()
	{
		nValue64 = 0;
	}

	explicit uniqid_t(uint64_t value)
	{
		nValue64 = value;
	}
	
	uniqid_t(unsigned int index, unsigned int order)
	{
		nIndex = index;
		nOrder = order;
	}
};

inline bool uniqid_is_null(const uniqid_t& u1)
{
	return 0 == u1.nValue64;
}

inline bool uniqid_not_null(const uniqid_t& u1)
{
	return 0 != u1.nValue64;
}

inline bool uniqid_equal(const uniqid_t& u1, const uniqid_t& u2)
{
	return u1.nValue64 == u2.nValue64;
}

inline bool uniqid_not_equal(const uniqid_t& u1, const uniqid_t&u2)
{
	return u1.nValue64 != u2.nValue64;
}

#endif // __UNIQID_H
