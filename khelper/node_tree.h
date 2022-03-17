// node_tree.h
// Created by lulimin on 2019/2/21.

#ifndef __NODE_TREE_H
#define __NODE_TREE_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"
#include "../inc/pod_array.h"

#define CLASS_ID_NODE_TREE 0xF01

// Tree-style data node.

class NodeTree : public IObject
{
public:
	NodeTree();
	virtual ~NodeTree();

	// Object class id.
	virtual int GetClassID() { return CLASS_ID_NODE_TREE; }
	
	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Node name.
	void SetName(const char* value);
	const char* GetName() const;
	// Get hash value of name.
	unsigned int GetHash() const;

	// Create child node.
	uniqid_t CreateNode(const char* name);
	// Add exists child node.
	bool AddNode(const uniqid_t& id);
	// Insert exists child node.
	bool InsertNode(const uniqid_t& id, size_t index);
	// Insert new child node.
	uniqid_t InsertNewNode(const char* name, size_t index);
	// Delete child node.
	bool RemoveNode(const char* name);
	// Delete child node by index.
	bool RemoveNodeByIndex(size_t index);
	// Delete child node by object id.
	bool RemoveNodeByID(const uniqid_t& id);
	// Clear all child nodes.
	bool ClearNodes();
	// Get number of child nodes.
	int GetNodeCount() const;
	// Get child node list.
	void GetNodeList(const IArgList& args, IArgList* res);
	// Search child node.
	bool FindNode(const char* name) const;
	// Get child node by name.
	uniqid_t GetNode(const char* name) const;
	// Get child node by index.
	uniqid_t GetNodeByIndex(size_t index) const;
	// Get index of node.
	int GetNodeIndex(const char* name) const;

private:
	bool FindNodeIndex(const char* name, size_t* pIndex) const;

private:
	FrameString m_sName;
	unsigned int m_nHash;
	TPodArray<NodeTree*, 1, FrameAlloc> m_Nodes;
};

#endif // __NODE_TREE_H
