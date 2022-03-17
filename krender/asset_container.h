// asset_container.h
// Created by lulimin on 2020/10/16.

#ifndef __ASSET_CONTAINER_H
#define __ASSET_CONTAINER_H

#include "../inc/common.h"
#include "../inc/pod_array.h"

DEFINE_SYSTEM_ALLOC(AssetContainer);

// Asset container.

template<typename TYPE, typename ALLOC = AssetContainerAlloc>
class TAssetContainer
{
private:
	typedef TAssetContainer<TYPE, ALLOC> self_type;

public:
	TAssetContainer()
	{
	}

	~TAssetContainer()
	{
	}

	// Get avialable element index.
	size_t GetIndex() const
	{
		if (m_FreeIndices.Empty())
		{
			return m_Elements.Size();
		}
		else
		{
			return m_FreeIndices.Back();
		}
	}

	// Get element by index.
	TYPE* Get(size_t index) const
	{
		Assert(index < m_Elements.Size());

		return m_Elements[index];
	}

	// Add element, return index of element.
	size_t Add(TYPE* pData)
	{
		Assert(pData != NULL);

		if (m_FreeIndices.Empty())
		{
			size_t index = m_Elements.Size();
			
			m_Elements.PushBack(pData);
			return index;
		}
		else
		{
			size_t index = m_FreeIndices.Back();

			Assert(m_Elements[index] == NULL);
			m_Elements[index] = pData;
			return index;
		}
	}

	// Remove element.
	bool Remove(TYPE* pData, size_t index)
	{
		Assert(index < m_Elements.Size());
		Assert(m_Elements[index] == pData);
		
		m_FreeIndices.PushBack(index);
		m_Elements[index] = NULL;
		return true;
	}

	// Clear all elements.
	void Clear()
	{
		m_Elements.Clear();
		m_FreeIndices.Clear();
	}

	// Get number of elements.
	size_t GetCount() const
	{
		return m_Elements.Size() - m_FreeIndices.Size();
	}

	// Get size of container.
	size_t GetSize() const
	{
		return m_Elements.Size();
	}

	// Get array of elements.
	TYPE** GetElements()
	{
		return m_Elements.Data();
	}

private:
	TAssetContainer(const self_type&);
	self_type& operator=(const self_type&);

private:
	TPodArray<TYPE*, 1, ALLOC> m_Elements;
	TPodArray<size_t, 1, ALLOC> m_FreeIndices;
};

#endif // __ASSET_CONTAINER_H
