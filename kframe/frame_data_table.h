// frame_data_table.h
// Created by lulimin on 2019/2/14.

#ifndef __FRAME_DATA_TABLE_H
#define __FRAME_DATA_TABLE_H

#include <new>
#include "../inc/common.h"
#include "../inc/utility.h"
#include "../inc/arg_data.h"
#include "../inc/i_data_table.h"

DEFINE_SYSTEM_ALLOC(FrameDataTable);

// Table of named parameters.

template<typename ALLOC = FrameDataTableAlloc, size_t BUFFER_SIZE = 4>
class TFrameDataTable : public IDataTable
{
private:
	typedef TFrameDataTable<ALLOC, BUFFER_SIZE> self_type;
	typedef TArgData<BUFFER_SIZE, ALLOC> arg_type;
	
	struct data_node_t
	{
		data_node_t* pNext;
		size_t nHash;
		arg_type Value;
		char szKey[1];
	};

public:
	explicit TFrameDataTable(size_t bucket_num = 0)
	{
		m_nBucketNum = bucket_num;
		m_nDataNum = 0;
		
		if (bucket_num > 0)
		{
			m_pBuckets = (data_node_t**)m_Alloc.Alloc(
				sizeof(data_node_t*) * bucket_num);
			memset(m_pBuckets, 0, sizeof(data_node_t*) * bucket_num);
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	TFrameDataTable(const self_type& src)
	{
		const size_t bucket_num = src.m_nBucketNum;

		m_nDataNum = 0;
		m_nBucketNum = bucket_num;

		if (bucket_num > 0)
		{
			m_pBuckets = (data_node_t**)m_Alloc.Alloc(
				sizeof(data_node_t*) * bucket_num);
			memset(m_pBuckets, 0, sizeof(data_node_t*) * bucket_num);

			for (size_t i = 0; i < bucket_num; ++i)
			{
				data_node_t* p = src.m_pBuckets[i];

				while (p)
				{
					this->Add(p->szKey, p->Value);
					p = p->pNext;
				}
			}
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	virtual ~TFrameDataTable()
	{
		this->Clear();

		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets);
		}
	}

	self_type& operator=(const self_type& src)
	{
		self_type temp(src);

		this->Swap(temp);
		return *this;
	}

	void Swap(self_type& src)
	{
		data_node_t** temp_buckets = src.m_pBuckets;
		size_t temp_bucket_num = src.m_nBucketNum;
		size_t temp_data_num = src.m_nDataNum;
		
		src.m_pBuckets = m_pBuckets;
		src.m_nBucketNum = m_nBucketNum;
		src.m_nDataNum = m_nDataNum;
		m_pBuckets = temp_buckets;
		m_nBucketNum = temp_bucket_num;
		m_nDataNum = temp_data_num;
	}

	// Add key and value.
	virtual IArgData* Add(const char* key, const IArgData& value)
	{
		Assert(key != NULL);

		if (m_nDataNum == m_nBucketNum)
		{
			this->Expand();
		}

		size_t hash = util_string_hash(key);
		size_t bucket = this->GetBucket(hash);
		data_node_t* p = this->NewNode(key, value);

		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		m_pBuckets[bucket] = p;
		m_nDataNum++;
		return &p->Value;
	}

	// Remove key and value.
	virtual bool Remove(const char* key)
	{
		Assert(key != NULL);

		if (0 == m_nBucketNum)
		{
			return false;
		}

		size_t hash = util_string_hash(key);
		size_t bucket = this->GetBucket(hash);
		data_node_t* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && (strcmp(p->szKey, key) == 0))
			{
				this->EraseNode(bucket, p);
				this->DeleteNode(p);
				m_nDataNum--;
				return true;
			}

			p = p->pNext;
		}

		return false;
	}

	// Clear all.
	virtual void Clear()
	{
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			data_node_t* p = m_pBuckets[i];

			while (p)
			{
				data_node_t* t = p->pNext;

				this->DeleteNode(p);
				p = t;
			}

			m_pBuckets[i] = NULL;
		}

		m_nDataNum = 0;
	}

	// Set value of key(add if not exists).
	virtual void Assign(const char* key, const IArgData& value)
	{
		data_node_t* pNode = this->FindNode(key);

		if (NULL == pNode)
		{
			this->Add(key, value);
		}
		else
		{
			pNode->Value.Assign(value);
		}
	}
	
	// Test key name exists.
	virtual bool Exists(const char* key)
	{
		return this->FindNode(key) != NULL;
	}

	// Get value of key.
	virtual IArgData* GetValue(const char* key)
	{
		data_node_t* pNode = this->FindNode(key);

		if (NULL == pNode)
		{
			return NULL;
		}

		return &pNode->Value;
	}

	// Get number of keys.
	virtual size_t GetCount()
	{
		return m_nDataNum;
	}

	// Get key name list.
	virtual void GetKeyList(IArgList* result)
	{
		result->Clear();
		
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			data_node_t* p = m_pBuckets[i];

			while (p)
			{
				result->AddString(p->szKey);
				p = p->pNext;
			}
		}
	}

private:
	size_t GetBucket(size_t hash) const
	{
		return hash % m_nBucketNum;
	}

	data_node_t* NewNode(const char* key, const IArgData& value)
	{
		const size_t len = strlen(key);
		const size_t size = sizeof(data_node_t) + len;
		data_node_t* p = (data_node_t*)m_Alloc.Alloc(size);

		memcpy(p->szKey, key, len + 1);
		new (&p->Value) arg_type(value);
		return p;
	}
	
	void DeleteNode(data_node_t* p)
	{
		p->Value.~arg_type();
		m_Alloc.Free(p);
	}

	void EraseNode(size_t bucket, data_node_t* p)
	{
		Assert(bucket < m_nBucketNum);
		Assert(p != NULL);
		
		data_node_t* t = m_pBuckets[bucket];
		
		if (t == p)
		{
			m_pBuckets[bucket] = p->pNext;
			return;
		}
		
		while (t)
		{
			if (t->pNext == p)
			{
				t->pNext = p->pNext;
				break;
			}

			t = t->pNext;
		}
	}

	data_node_t* FindNode(const char* key) const
	{
		Assert(key != NULL);
		
		if (0 == m_nBucketNum)
		{
			return NULL;
		}
		
		size_t hash = util_string_hash(key);
		size_t bucket = this->GetBucket(hash);
		data_node_t* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && (strcmp(p->szKey, key) == 0))
			{
				return p;
			}
			
			p = p->pNext;
		}
		
		return NULL;
	}
	
	void Expand()
	{
		size_t new_bucket_num = m_nBucketNum * 2 + 1;
		data_node_t** new_buckets = (data_node_t**)m_Alloc.Alloc(
			sizeof(data_node_t*) * new_bucket_num);
		
		memset(new_buckets, 0, sizeof(data_node_t*) * new_bucket_num);
		
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			data_node_t* p = m_pBuckets[i];
			
			while (p)
			{
				data_node_t* t = p->pNext;
				size_t bucket = (size_t)(p->nHash) % new_bucket_num;
				
				p->pNext = new_buckets[bucket];
				new_buckets[bucket] = p;
				p = t;
			}
		}
		
		if (m_pBuckets)
		{
			m_Alloc.Free(m_pBuckets);
		}
		
		m_pBuckets = new_buckets;
		m_nBucketNum = new_bucket_num;
	}

private:
	ALLOC m_Alloc;
	data_node_t** m_pBuckets;
	size_t m_nBucketNum;
	size_t m_nDataNum;
};

#endif // __FRAME_DATA_TABLE_H
