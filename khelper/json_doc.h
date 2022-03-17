// json_doc.h
// Created by lulimin on 2021/1/6.

#ifndef __JSON_DOC_H
#define __JSON_DOC_H

#include "../inc/frame_def.h"
#include "../inc/frame_types.h"

// JSON document.

class JsonDoc : public IObject 
{
public:
	// Node types.
	enum
	{
		JTYPE_NULL,
		JTYPE_TRUE,
		JTYPE_FALSE,
		JTYPE_INT32,
		JTYPE_DOUBLE,
		JTYPE_STRING,
		JTYPE_ARRAY,
		JTYPE_OBJECT,
	};

	struct jnode_t;

	// Node array.
	struct jarray_t
	{
		jnode_t** pNodes;
		size_t nSize;
		size_t nCount;
	};

	// Json node.
	struct jnode_t
	{
		int nType;
		char* szKey;
		jnode_t* pParent;
		union
		{
			int nValue;
			double dValue;
			char* szValue;
			jarray_t* pArray;
		};
	};

public:
	JsonDoc();
	virtual ~JsonDoc();

	virtual bool Startup(const IArgList& args);
	virtual bool Shutdown();

	// Load from file.
	bool LoadFile(const char* file_name);
	// Load from raw file.
	bool LoadRawFile(const char* file_name);
	// Load from string.
	bool LoadString(const char* json);
	// Save to file.
	bool SaveFile(const char* file_name);
	// Save to string.
	ArgString SaveString();
	// Save to unformatted string.
	ArgString SaveCompactString();

	// Creat root node.
	bool CreateRoot();
	// Add node to current object node.
	bool AddNull(const char* key);
	bool AddBoolean(const char* key, bool value);
	bool AddInt32(const char* key, int value);
	bool AddDouble(const char* key, double value);
	bool AddString(const char* key, const char* value);
	bool AddArray(const char* key);
	bool AddObject(const char* key);
	// Add node to current array node.
	bool ArrayAddNull();
	bool ArrayAddBoolean(bool value);
	bool ArrayAddInt32(int value);
	bool ArrayAddDouble(double value);
	bool ArrayAddString(const char* value);
	bool ArrayAddArray();
	bool ArrayAddObject();
	// Remove child node by key.
	bool RemoveKey(const char* key);
	// Remove child node by index.
	bool RemoveIndex(int index);
	// Remove all child node.
	bool RemoveAll();

	// Select root node.
	bool SelectRoot();
	// Select node by key.
	bool SelectKey(const char* key);
	// Select node by index.
	bool SelectIndex(int index);
	// Select parent of current node.
	bool SelectParent();

	// Get key of current node.
	const char* GetKey();
	// Get type of current node.
	const char* GetType();
	// Get boolean value of current node.
	bool GetBoolean();
	// Get integer value of current node.
	int GetInt32();
	// Get double value of current node.
	double GetDouble();
	// Get string value of current node.
	ArgString GetString();
	// Get child node number of current node.
	int GetCount();
	// Get child key list of current node.
	void GetKeyList(const IArgList& args, IArgList* res);
	// Find child node by key in current node.
	bool FindKey(const char* key);
	// Read child node value by key in current node.
	void ReadKey(const IArgList& args, IArgList* res);
	bool ReadBoolean(const char* key, bool def);
	int ReadInt32(const char* key, int def);
	float ReadFloat(const char* key, float def);
	double ReadDouble(const char* key, double def);
	ArgString ReadString(const char* key, const char* def);

	// New node.
	jnode_t* NewNullNode();
	jnode_t* NewBooleanNode(bool value);
	jnode_t* NewInt32Node(int value);
	jnode_t* NewDoubleNode(double value);
	jnode_t* NewStringNode(const char* value);
	jnode_t* NewArrayNode();
	jnode_t* NewObjectNode();
	// Set key of node.
	void SetNodeKey(jnode_t* pNode, const char* key);
	// Add node to array.
	void AddNode(jnode_t* pParent, jnode_t* pNode);

private:
	// Load json fram data.
	bool LoadData(char* data, size_t size);
	// Remove node from array.
	void RemoveNode(jarray_t* pArray, size_t index);
	// Delete node.
	void DeleteNode(jnode_t* pNode);

private:
	jnode_t* m_pRoot;
	jnode_t* m_pCurrent;
};

#endif // __JSON_DOC_H
