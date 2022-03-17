// string_hash_table.h
// Created by lulimin on 2019/2/17.

#ifndef __STRING_HASH_TABLE_H
#define __STRING_HASH_TABLE_H

#include "common.h"
#include "char_traits.h"

DEFINE_SYSTEM_ALLOC(StringHashTable);

template<
	typename TYPE,
	typename DATA, 
	typename TRAITS = TCharTraits<TYPE>,
	typename ALLOC = StringHashTableAlloc>
class TStringHashTable;

// TStringHashTableNode

template<typename TYPE, typename DATA>
class TStringHashTableNode
{
public:
	TStringHashTableNode* pNext;
	size_t nHash;
	DATA Data;
	TYPE sName[1];
};

// TStringHashTableIterator

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TStringHashTableIterator {
private:
	typedef TStringHashTable<TYPE, DATA, TRAITS, ALLOC> hash_type;
	typedef TStringHashTableNode<TYPE, DATA> node_type;

public:
	TStringHashTableIterator(const hash_type* self, node_type* node)
	{
		m_pSelf = self;
		m_pNode = node;
	}
	
	TStringHashTableIterator& operator++()
	{
		node_type* next = m_pNode->pNext;
		
		if (next != NULL)
		{
			m_pNode = next;
		}
		else
		{
			m_pNode = m_pSelf->ToNext(m_pNode);
		}
		
		return *this;
	}
	
	const TStringHashTableIterator operator++(int)
	{
		TStringHashTableIterator temp(*this);

		++(*this);
		return temp;
	}
	
	bool operator==(const TStringHashTableIterator& other) const
	{
		return m_pNode == other.m_pNode;
	}
	
	bool operator!=(const TStringHashTableIterator& other) const
	{
		return m_pNode != other.m_pNode;
	}

	const TYPE* GetKey() const
	{
		return m_pNode->sName;
	}
	
	DATA& GetData() const
	{
		return m_pNode->Data;
	}
	
	node_type* GetNode() const
	{
		return m_pNode;
	}
	
private:
	TStringHashTableIterator();
	
private:
	node_type* m_pNode;
	const hash_type* m_pSelf;
};

// TStringHashTable

template<typename TYPE, typename DATA, typename TRAITS, typename ALLOC>
class TStringHashTable
{
private:
	typedef TStringHashTable<TYPE, DATA, TRAITS, ALLOC> self_type;
	typedef TStringHashTableNode<TYPE, DATA> node_type;
	
public:
	typedef TStringHashTableIterator<TYPE, DATA, TRAITS, ALLOC> iterator;
	typedef TStringHashTableIterator<TYPE, DATA, TRAITS, ALLOC> const_iterator;
	
public:
	explicit TStringHashTable(size_t bucket_num = 0)
	{
		m_nBucketNum = bucket_num;
		m_nDataNum = 0;
		
		if (bucket_num > 0)
		{
			size_t size = sizeof(node_type*) * bucket_num;

			m_pBuckets = (node_type**)m_Alloc.Alloc(size);
			memset(m_pBuckets, 0, size);
		}
		else
		{
			m_pBuckets = NULL;
		}
	}

	TStringHashTable(const self_type& src)
	{
		const size_t bucket_num = src.m_nBucketNum;
		
		m_nDataNum = 0;
		m_nBucketNum = bucket_num;

		if (bucket_num > 0)
		{
			size_t size = sizeof(node_type*) * bucket_num;
			
			m_pBuckets = (node_type**)m_Alloc.Alloc(size);
			memset(m_pBuckets, 0, size);

			for (size_t i = 0; i < bucket_num; ++i)
			{
				node_type* p = src.m_pBuckets[i];
				
				while (p)
				{
					this->Add(p->sName, p->Data);
					p = p->pNext;
				}
			}
		} 
		else
		{
			m_pBuckets = NULL;
		}
	}

	~TStringHashTable()
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
		node_type** temp_buckets = src.m_pBuckets;
		size_t temp_bucket_num = src.m_nBucketNum;
		size_t temp_data_num = src.m_nDataNum;
		
		src.m_pBuckets = m_pBuckets;
		src.m_nBucketNum = m_nBucketNum;
		src.m_nDataNum = m_nDataNum;
		m_pBuckets = temp_buckets;
		m_nBucketNum = temp_bucket_num;
		m_nDataNum = temp_data_num;
	}

	void Clear()
	{
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			node_type* p = m_pBuckets[i];

			while (p) 
			{
				node_type* t = p->pNext;
				
				this->DeleteNode(p);
				p = t;
			}
			
			m_pBuckets[i] = NULL;
		}

		m_nDataNum = 0;
	}

	size_t GetCount() const
	{
		return m_nDataNum;
	}

	bool Add(const TYPE* name, const DATA& data)
	{
		Assert(name != NULL);
		
		if ((m_nDataNum * 3) >= (m_nBucketNum * 2))
		{
			this->Expand();
		}
		
		size_t hash = TRAITS::HashCode(name);
		size_t bucket = this->GetBucket(hash);
		node_type* p = this->NewNode(name);
		
		p->pNext = m_pBuckets[bucket];
		p->nHash = hash;
		p->Data = data;
		m_pBuckets[bucket] = p;
		m_nDataNum++;
		return true;
	}

	bool Remove(const TYPE* name)
	{
		Assert(name != NULL);

		if (0 == m_nBucketNum)
		{
			return false;
		}
		
		size_t hash = TRAITS::HashCode(name);
		size_t bucket = this->GetBucket(hash);
		node_type* p = m_pBuckets[bucket];

		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name))
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

	bool RemoveData(const TYPE* name, const DATA& data)
	{
		Assert(name != NULL);

		if (0 == m_nBucketNum)
		{
			return false;
		}
		
		size_t hash = TRAITS::HashCode(name);
		size_t bucket = this->GetBucket(hash);
		node_type* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name) && 
				(p->Data == data))
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

	bool Exists(const TYPE* name) const
	{
		return this->FindNode(name) != NULL;
	}

	const_iterator Find(const TYPE* name) const
	{
		return const_iterator(this, this->FindNode(name));
	}
	
	iterator Find(const TYPE* name)
	{
		return iterator(this, this->FindNode(name));
	}

	bool GetData(const TYPE* name, DATA* data) const
	{
		node_type* p = this->FindNode(name);

		if (NULL == p)
		{
			return false;
		}

		*data = p->Data;
		return true;
	}

	iterator Begin()
	{
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			if (m_pBuckets[i])
			{
				return iterator(this, m_pBuckets[i]);
			}
		}
		
		return this->End();
	}
	
	iterator End()
	{
		return iterator(this, NULL);
	}
	
	const_iterator Begin() const
	{
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			if (m_pBuckets[i])
			{
				return const_iterator(this, m_pBuckets[i]);
			}
		}
		
		return this->End();
	}
	
	const_iterator End() const
	{
		return const_iterator(this, NULL);
	}
	
	iterator Erase(iterator it)
	{
		iterator temp(it);
		
		++temp;
		
		node_type* p = it.GetNode();
		
		this->EraseNode(this->GetBucket(p->nHash), p);
		this->DeleteNode(p);
		m_nDataNum--;
		return temp;
	}
	
private:
	size_t GetBucket(size_t hash) const
	{
		return hash % m_nBucketNum;
	}

	node_type* NewNode(const TYPE* name)
	{
		const size_t len = TRAITS::Length(name);
		const size_t size = sizeof(node_type) + len * sizeof(TYPE);
		node_type* p = (node_type*)m_Alloc.Alloc(size);

		TRAITS::Copy(p->sName, name, len + 1);
		return p;
	}
	
	void DeleteNode(node_type* p)
	{
		m_Alloc.Free(p);
	}

	void EraseNode(size_t bucket, node_type* p)
	{
		Assert(bucket < m_nBucketNum);
		Assert(p != NULL);
		
		node_type* t = m_pBuckets[bucket];
		
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

	node_type* FindNode(const TYPE* name) const
	{
		Assert(name != NULL);
		
		if (0 == m_nBucketNum)
		{
			return NULL;
		}
		
		size_t hash = TRAITS::HashCode(name);
		size_t bucket = this->GetBucket(hash);
		node_type* p = m_pBuckets[bucket];
		
		while (p)
		{
			if ((p->nHash == hash) && TRAITS::Equal(p->sName, name))
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
		size_t new_size = sizeof(node_type*) * new_bucket_num;
		node_type** new_buckets = (node_type**)m_Alloc.Alloc(new_size);
		
		memset(new_buckets, 0, new_size); 
		
		for (size_t i = 0; i < m_nBucketNum; ++i)
		{
			node_type* p = m_pBuckets[i];
			
			while (p)
			{
				node_type* t = p->pNext;
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

	node_type* ToNext(node_type* node) const
	{
		for (size_t i = this->GetBucket(node->nHash) + 1; i < m_nBucketNum; ++i)
		{
			if (m_pBuckets[i])
			{
				return m_pBuckets[i];
			}
		}
		
		return NULL;
	}
	
private:
	ALLOC m_Alloc;
	node_type** m_pBuckets;
	size_t m_nBucketNum;
	size_t m_nDataNum;
	friend class TStringHashTableIterator<TYPE, DATA, TRAITS, ALLOC>;
};

#endif // __STRING_HASH_TABLE_H
