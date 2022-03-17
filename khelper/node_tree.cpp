// node_tree.cpp
// Created by lulimin on 2019/2/21.

#include "node_tree.h"
#include "../inc/utility.h"
#include "../inc/arg_list.h"

/// \file node_tree.cpp
/// \brief Tree-style data node.

/// object: NodeTree
/// \brief Tree-style data node object.
DEF_OBJECT(NodeTree, IObject);

/// property: string Name
/// \brief Node name.
DEF_PROP(const char*, NodeTree, Name);

/// function: object CreateNode(string name)
/// \brief Create child node.
/// \param name Child node name.
DEF_FUNC_1(uniqid_t, NodeTree, CreateNode, const char*);
/// function: bool AddNode(object obj)
/// \brief Add exists child node.
/// \param obj Object id.
DEF_FUNC_1(bool, NodeTree, AddNode, const uniqid_t&);
/// function: bool InsertNode(object obj, size_t index)
/// \brief Insert exists child node.
/// \param obj Object id.
/// \param index Position index.
DEF_FUNC_2(bool, NodeTree, InsertNode, const uniqid_t&, size_t);
/// function: object InsertNewNode(string name, size_t index)
/// \brief Insert new child node.
/// \param name New child node name.
/// \param index Position index.
DEF_FUNC_2(uniqid_t, NodeTree, InsertNewNode, const char*, size_t);
/// function: bool RemoveNode(string name)
/// \brief Delete child node.
/// \param name Child node name.
DEF_FUNC_1(bool, NodeTree, RemoveNode, const char*);
/// function: bool RemoveNodeByIndex(int index)
/// \brief Delete child node by index.
/// \param index Position index.
DEF_FUNC_1(bool, NodeTree, RemoveNodeByIndex, size_t);
/// function: bool RemoveNodeByID(int index)
/// \brief Delete child node by object id.
/// \param id Object id.
DEF_FUNC_1(bool, NodeTree, RemoveNodeByID, const uniqid_t&);
/// function: bool ClearNodes()
/// \brief Clear all child nodes.
DEF_FUNC_0(bool, NodeTree, ClearNodes);
/// function: int GetNodeCount()
/// \brief Get number of child nodes.
DEF_FUNC_0(int, NodeTree, GetNodeCount);
/// function: table GetNodeList()
/// \brief Get child node list.
DEF_FUNC_A(NodeTree, GetNodeList);
/// function: bool FindNode(string name)
/// \brief Search child node by name.
/// \param name Child node name.
DEF_FUNC_1(bool, NodeTree, FindNode, const char*);
/// function: object GetNode(string name)
/// \brief Get child node by name.
/// \param name Child node name.
DEF_FUNC_1(uniqid_t, NodeTree, GetNode, const char*);
/// function: object GetNodeByIndex(int index)
/// \brief Get child node by index.
/// \param index Position index.
DEF_FUNC_1(uniqid_t, NodeTree, GetNodeByIndex, size_t);
/// function: int GetNodeIndex(string name)
/// \brief Get child node index.
/// \param name Child node name.
DEF_FUNC_1(int, NodeTree, GetNodeIndex, const char*);

// NodeTree

NodeTree::NodeTree()
{
	m_nHash = 0;
}

NodeTree::~NodeTree()
{
}

bool NodeTree::Startup(const IArgList& args)
{
	if (args.GetType(0) == DT_STRING)
	{
		this->SetName(args.GetString(0));
	}
	
	return true;
}

bool NodeTree::Shutdown()
{
	this->ClearNodes();
	return true;
}

void NodeTree::SetName(const char* value)
{
	Assert(value != NULL);

	m_sName = value;
	m_nHash = util_string_hash(value);
}

const char* NodeTree::GetName() const
{
	return m_sName.CString();
}

unsigned int NodeTree::GetHash() const
{
	return m_nHash;
}

bool NodeTree::FindNodeIndex(const char* name, size_t* pIndex) const
{
	Assert(name != NULL);
	Assert(pIndex != NULL);

	unsigned int hash = util_string_hash(name);
	const size_t child_num = m_Nodes.Size();

	for (size_t i = 0; i < child_num; ++i)
	{
		NodeTree* p = m_Nodes[i];

		if ((p->GetHash() == hash) && (strcmp(p->GetName(), name) == 0))
		{
			*pIndex = i;
			return true;
		}
	}

	return false;
}

uniqid_t NodeTree::CreateNode(const char* name)
{
	Assert(name != NULL);

	NodeTree* pNodeTree = (NodeTree*)frame_create_object("NodeTree");

	if (NULL == pNodeTree)
	{
		return uniqid_t();
	}

	pNodeTree->SetName(name);
	m_Nodes.PushBack(pNodeTree);
	return pNodeTree->GetUID();
}

bool NodeTree::AddNode(const uniqid_t& id)
{
	NodeTree* pNodeTree = (NodeTree*)frame_get_object(id);

	if (NULL == pNodeTree)
	{
		return false;
	}

	if (pNodeTree->GetClassID() != CLASS_ID_NODE_TREE)
	{
		return false;
	}

	m_Nodes.PushBack(pNodeTree);
	return true;
}

bool NodeTree::InsertNode(const uniqid_t& id, size_t index)
{
	if (index > m_Nodes.Size())
	{
		return false;
	}

	NodeTree* pNodeTree = (NodeTree*)frame_get_object(id);

	if (NULL == pNodeTree)
	{
		return false;
	}

	if (pNodeTree->GetClassID() != CLASS_ID_NODE_TREE)
	{
		return false;
	}

	m_Nodes.Insert(index, pNodeTree);
	return true;
}

uniqid_t NodeTree::InsertNewNode(const char* name, size_t index)
{
	Assert(name != NULL);

	if (index > m_Nodes.Size())
	{
		return uniqid_t();
	}

	NodeTree* pNodeTree = (NodeTree*)frame_create_object("NodeTree");

	if (NULL == pNodeTree)
	{
		return uniqid_t();
	}

	pNodeTree->SetName(name);
	m_Nodes.Insert(index, pNodeTree);
	return pNodeTree->GetUID();
}

bool NodeTree::RemoveNode(const char* name)
{
	size_t index;

	if (!this->FindNodeIndex(name, &index))
	{
		return false;
	}

	return this->RemoveNodeByIndex(index);
}

bool NodeTree::RemoveNodeByIndex(size_t index)
{
	if (index >= m_Nodes.Size())
	{
		return false;
	}
	
	SAFE_RELEASE(m_Nodes[index]);
	m_Nodes.Remove(index);
	return true;
}

bool NodeTree::RemoveNodeByID(const uniqid_t& id)
{
	for (size_t i = 0; i < m_Nodes.Size(); ++i)
	{
		if (uniqid_equal(id, m_Nodes[i]->GetUID()))
		{
			return this->RemoveNodeByIndex(i);
		}
	}

	return false;
}

bool NodeTree::ClearNodes()
{
	for (size_t i = 0; i < m_Nodes.Size(); ++i)
	{
		SAFE_RELEASE(m_Nodes[i]);
	}

	m_Nodes.Clear();
	return true;
}

int NodeTree::GetNodeCount() const
{
	return (int)m_Nodes.Size();
}

void NodeTree::GetNodeList(const IArgList& args, IArgList* res)
{
	res->Clear();

	for (size_t i = 0; i < m_Nodes.Size(); ++i)
	{
		res->AddUniqid(m_Nodes[i]->GetUID());
	}
}

bool NodeTree::FindNode(const char* name) const
{
	size_t index;
	
	return this->FindNodeIndex(name, &index);
}

uniqid_t NodeTree::GetNode(const char* name) const
{
	size_t index;
	
	if (!this->FindNodeIndex(name, &index))
	{
		return uniqid_t();
	}
	
	return m_Nodes[index]->GetUID();
}

uniqid_t NodeTree::GetNodeByIndex(size_t index) const
{
	if (index >= m_Nodes.Size())
	{
		return uniqid_t();
	}

	return m_Nodes[index]->GetUID();
}

int NodeTree::GetNodeIndex(const char* name) const
{
	size_t index;

	if (!this->FindNodeIndex(name, &index))
	{
		return -1;
	}

	return (int)index;
}
