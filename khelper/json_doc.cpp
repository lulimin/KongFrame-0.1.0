// json_doc.cpp
// Created by lulimin on 2021/1/6.

#include "json_doc.h"
#include "../inc/json_writer.h"
#include "../inc/frame_file.h"
#include "../inc/auto_buffer.h"
//#pragma warning(disable:4200)
#define SAJSON_NO_STD_STRING
#include "../ext/sajson/sajson.h"

/// \file json_doc.cpp
/// \brief JSON document.

/// object: JsonDoc
/// \brief JSON document object.
DEF_OBJECT(JsonDoc, IObject);

/// function: bool LoadFile(string file_name)
/// \brief Load json from file.
/// \param file_name Json file name.
DEF_FUNC_1(bool, JsonDoc, LoadFile, const char*);
/// function: bool LoadRawFile(string file_name)
/// \brief Load json from raw file.
/// \param file_name Json file name.
DEF_FUNC_1(bool, JsonDoc, LoadRawFile, const char*);
/// function: bool LoadString(string json)
/// \brief Load json from string.
/// \param json Json string.
DEF_FUNC_1(bool, JsonDoc, LoadString, const char*);
/// function: bool SaveFile(string file_name)
/// \brief Save json to file.
/// \param file_name Json file name.
DEF_FUNC_1(bool, JsonDoc, SaveFile, const char*);
/// function: string SaveString()
/// \brief Save json to string.
DEF_FUNC_0(ArgString, JsonDoc, SaveString);
/// function: string SaveCompactString()
/// \brief Save json to unformatted string(no Tab Space Return).
DEF_FUNC_0(ArgString, JsonDoc, SaveCompactString);

/// function: bool CreateRoot()
/// \brief Create root node and set current node to it.
DEF_FUNC_0(bool, JsonDoc, CreateRoot);
/// function: bool AddNull(string key)
/// \brief Add null value node to current node.
/// \param key node key
DEF_FUNC_1(bool, JsonDoc, AddNull, const char*);
/// function: bool AddBoolean(string key, bool value)
/// \brief Add boolean value node to current node.
/// \param key Node key.
/// \param value Boolean value.
DEF_FUNC_2(bool, JsonDoc, AddBoolean, const char*, bool);
/// function: bool AddInt32(string key, int value)
/// \brief Add integer value node to current node.
/// \param key Node key.
/// \param value Integer value.
DEF_FUNC_2(bool, JsonDoc, AddInt32, const char*, int);
/// function: bool AddDouble(string key, double value)
/// \brief Add double value node to current node.
/// \param key Node key.
/// \param value Double value.
DEF_FUNC_2(bool, JsonDoc, AddDouble, const char*, double);
/// function: bool AddString(string key, string value)
/// \brief Add string value node to current node.
/// \param key Node key.
/// \param value String value.
DEF_FUNC_2(bool, JsonDoc, AddString, const char*, const char*);
/// function: bool AddArray(string key)
/// \brief Add array node to current node.
/// \param key Node key.
DEF_FUNC_1(bool, JsonDoc, AddArray, const char*);
/// function: bool AddObject(string key)
/// \brief Add object node to current node.
/// \param key Node key.
DEF_FUNC_1(bool, JsonDoc, AddObject, const char*);
/// function: bool ArrayAddNull()
/// \brief Add null value node to current array node.
DEF_FUNC_0(bool, JsonDoc, ArrayAddNull);
/// function: bool ArrayAddBoolean(bool value)
/// \brief Add boolean value node to current array node.
/// \param value Boolean value.
DEF_FUNC_1(bool, JsonDoc, ArrayAddBoolean, bool);
/// function: bool ArrayAddInt32(int value)
/// \brief Add integer value node to current array node.
/// \param value Integer value.
DEF_FUNC_1(bool, JsonDoc, ArrayAddInt32, int);
/// function: bool ArrayAddDouble(double value)
/// \brief Add double value node to current array node.
/// \param value Double value.
DEF_FUNC_1(bool, JsonDoc, ArrayAddDouble, double);
/// function: bool ArrayAddString(string value)
/// \brief Add string value node to current array node.
/// \param value String value.
DEF_FUNC_1(bool, JsonDoc, ArrayAddString, const char*);
/// function: bool ArrayAddArray()
/// \brief Add array node to current array node.
DEF_FUNC_0(bool, JsonDoc, ArrayAddArray);
/// function: bool ArrayAddObject()
/// \brief Add object node to current array node.
DEF_FUNC_0(bool, JsonDoc, ArrayAddObject);
/// function: bool RemoveKey(string key)
/// \brief Remove node by key from current object node.
/// \param key Node key.
DEF_FUNC_1(bool, JsonDoc, RemoveKey, const char*);
/// function: bool RemoveIndex(int index)
/// \brief Remove node by index from current array of object node
/// \param index Node index.
DEF_FUNC_1(bool, JsonDoc, RemoveIndex, int);
/// function: bool RemoveAll()
/// \brief Remove all node from current array of object node.
DEF_FUNC_0(bool, JsonDoc, RemoveAll);

/// function: bool SelectRoot()
/// \brief Select root node.
DEF_FUNC_0(bool, JsonDoc, SelectRoot);
/// function: bool SelectKey(string key)
/// \brief Select node by key in current node.
/// \param key Node key.
DEF_FUNC_1(bool, JsonDoc, SelectKey, const char*);
/// function: bool SelectIndex(int index)
/// \brief Select node by index in current node.
/// \param index Node index.
DEF_FUNC_1(bool, JsonDoc, SelectIndex, int);
/// function: bool SelectParent()
/// \brief Select parent node of current node.
DEF_FUNC_0(bool, JsonDoc, SelectParent);

/// function: string GetKey()
/// \brief Get key of current node.
DEF_FUNC_0(const char*, JsonDoc, GetKey);
/// function: string GetType()
/// \brief Get type of current node.
DEF_FUNC_0(const char*, JsonDoc, GetType);
/// function: bool GetBoolean()
/// \brief Get boolean value of current node.
DEF_FUNC_0(bool, JsonDoc, GetBoolean);
/// function: int GetInt32()
/// \brief Get integer value of current node.
DEF_FUNC_0(int, JsonDoc, GetInt32);
/// function: double GetDouble()
/// \brief Get double value of current node.
DEF_FUNC_0(double, JsonDoc, GetDouble);
/// function: string GetString()
/// \brief Get string value of current node.
DEF_FUNC_0(ArgString, JsonDoc, GetString);
/// function: int GetCount()
/// \brief Get child node number of current array or object node.
DEF_FUNC_0(int, JsonDoc, GetCount);
/// function: [key table] GetKeyList()
/// \brief Get key list of current object node.
DEF_FUNC_A(JsonDoc, GetKeyList);
/// function: bool FindKey(strng key)
/// \brief Find child node by key in current node.
/// \param key Node key.
DEF_FUNC_1(bool, JsonDoc, FindKey, const char*);
/// function: [var] ReadKey(string key, [[var] default_value])
/// \brief Read child node value by key in current node.
/// \param key Node key.
/// \param default_value Return default value when key not exists.
DEF_FUNC_M(JsonDoc, ReadKey);
/// function: bool ReadBoolean(string key, bool def)
/// \brief Read boolean value by key in current node.
/// \param key Node key.
/// \param def Return default value when key not exists.
DEF_FUNC_2(bool, JsonDoc, ReadBoolean, const char*, bool);
/// function: int ReadInt32(string key, int def)
/// \brief Read integer value by key in current node.
/// \param key Node key.
/// \param def Return default value when key not exists.
DEF_FUNC_2(int, JsonDoc, ReadInt32, const char*, int);
/// function: float ReadFloat(string key, float def)
/// \brief Read float value by key in current node.
/// \param key Node key.
/// \param def Return default value when key not exists.
DEF_FUNC_2(float, JsonDoc, ReadFloat, const char*, float);
/// function: double ReadDouble(string key, double def)
/// \brief Read double value by key in current node.
/// \param key Node key.
/// \param def Return default value when key not exists.
DEF_FUNC_2(double, JsonDoc, ReadDouble, const char*, double);
/// function: string ReadString(string key, string def)
/// \brief Read string value by key in current node.
/// \param key Node key.
/// \param def Return default value when key not exists.
DEF_FUNC_2(ArgString, JsonDoc, ReadString, const char*, const char*);

// Json writer.
typedef TJsonWriter<FrameAlloc, TJsonAutoBuffer<FrameAlloc> > FrameJsonWriter;

// JsonDoc

JsonDoc::JsonDoc()
{
	m_pRoot = NULL;
	m_pCurrent = NULL;
}

JsonDoc::~JsonDoc()
{
	if (m_pRoot)
	{
		this->DeleteNode(m_pRoot);
	}
}

bool JsonDoc::Startup(const IArgList& args)
{
	return true;
}

bool JsonDoc::Shutdown()
{
	return true;
}

bool JsonDoc::LoadFile(const char* file_name)
{
	Assert(file_name != NULL);

	void* fp = frame_fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	size_t file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	TAutoBuffer<char, 1024, FrameAlloc> auto_buf(file_size);
	char* file_data = auto_buf.GetBuffer();

	if (frame_fread(file_data, 1, file_size, fp) != file_size)
	{
		frame_fclose(fp);
		return false;
	}

	frame_fclose(fp);
	return this->LoadData(file_data, file_size);
}

bool JsonDoc::LoadRawFile(const char* file_name)
{
	Assert(file_name != NULL);

	FILE* fp = fopen(file_name, "rb");

	if (NULL == fp)
	{
		return false;
	}

	fseek(fp, 0, SEEK_END);
	size_t file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	TAutoBuffer<char, 1024, FrameAlloc> auto_buf(file_size);
	char* file_data = auto_buf.GetBuffer();

	if (fread(file_data, 1, file_size, fp) != file_size)
	{
		fclose(fp);
		return false;
	}

	fclose(fp);
	return this->LoadData(file_data, file_size);
}

bool JsonDoc::LoadString(const char* json)
{
	Assert(json != NULL);

	size_t len = strlen(json);
	TAutoBuffer<char, 1024, FrameAlloc> auto_buf(len + 1);
	char* data = auto_buf.GetBuffer();

	memcpy(data, json, len + 1);
	return this->LoadData(data, len);
}

// Load json node.
static bool load_node(JsonDoc* pDoc, JsonDoc::jnode_t* pNode, 
	const sajson::value& sa_node)
{
	Assert(pDoc != NULL);
	Assert(pNode != NULL);

	if (sa_node.get_type() == sajson::TYPE_ARRAY)
	{
		size_t arr_num = sa_node.get_length();

		for (size_t i = 0; i < arr_num; ++i)
		{
			const sajson::value& arr_node = sa_node.get_array_element(i);
			JsonDoc::jnode_t* pChild = NULL;

			switch (arr_node.get_type())
			{
			case sajson::TYPE_NULL:
				pChild = pDoc->NewNullNode();
				break;
			case sajson::TYPE_TRUE:
				pChild = pDoc->NewBooleanNode(true);
				break;
			case sajson::TYPE_FALSE:
				pChild = pDoc->NewBooleanNode(false);
				break;
			case sajson::TYPE_INTEGER:
				pChild = pDoc->NewInt32Node(arr_node.get_integer_value());
				break;
			case sajson::TYPE_DOUBLE:
				pChild = pDoc->NewDoubleNode(arr_node.get_double_value());
				break;
			case sajson::TYPE_STRING:
				pChild = pDoc->NewStringNode(arr_node.as_cstring());
				break;
			case sajson::TYPE_ARRAY:
				pChild = pDoc->NewArrayNode();
				break;
			case sajson::TYPE_OBJECT:
				pChild = pDoc->NewObjectNode();
				break;
			default:
				Assert(0);
				break;
			}

			pDoc->AddNode(pNode, pChild);

			if ((arr_node.get_type() == sajson::TYPE_ARRAY) ||
				(arr_node.get_type() == sajson::TYPE_OBJECT))
			{
				if (!load_node(pDoc, pChild, arr_node))
				{
					return false;
				}
			}
		}
	}
	else if (sa_node.get_type() == sajson::TYPE_OBJECT)
	{
		size_t key_num = sa_node.get_length();

		for (size_t i = 0; i < key_num; ++i)
		{
			const sajson::string& key_name = sa_node.get_object_key(i);
			const sajson::value& key_node = sa_node.get_object_value(i);
			JsonDoc::jnode_t* pChild = NULL;

			switch (key_node.get_type())
			{
			case sajson::TYPE_NULL:
				pChild = pDoc->NewNullNode();
				break;
			case sajson::TYPE_TRUE:
				pChild = pDoc->NewBooleanNode(true);
				break;
			case sajson::TYPE_FALSE:
				pChild = pDoc->NewBooleanNode(false);
				break;
			case sajson::TYPE_INTEGER:
				pChild = pDoc->NewInt32Node(key_node.get_integer_value());
				break;
			case sajson::TYPE_DOUBLE:
				pChild = pDoc->NewDoubleNode(key_node.get_double_value());
				break;
			case sajson::TYPE_STRING:
				pChild = pDoc->NewStringNode(key_node.as_cstring());
				break;
			case sajson::TYPE_ARRAY:
				pChild = pDoc->NewArrayNode();
				break;
			case sajson::TYPE_OBJECT:
				pChild = pDoc->NewObjectNode();
				break;
			default:
				Assert(0);
				break;
			}

			pDoc->SetNodeKey(pChild, key_name.data());
			pDoc->AddNode(pNode, pChild);

			if ((key_node.get_type() == sajson::TYPE_ARRAY) ||
				(key_node.get_type() == sajson::TYPE_OBJECT))
			{
				if (!load_node(pDoc, pChild, key_node))
				{
					return false;
				}
			}
		}
	}
	
	return true;
}

bool JsonDoc::LoadData(char* data, size_t size)
{
	Assert(data != NULL);
	
	TAutoBuffer<size_t, 1024, FrameAlloc> auto_buf1(size);
	size_t* pJsonMem = auto_buf1.GetBuffer();
	const sajson::document& doc = sajson::parse(
		sajson::bounded_allocation(pJsonMem, size),
		sajson::mutable_string_view(size, data));

	if (!doc.is_valid())
	{
		return false;
	}

	if (!this->CreateRoot())
	{
		return false;
	}

	return load_node(this, m_pRoot, doc.get_root());
}

// Save json node.
static bool save_node(FrameJsonWriter* jw, json_node_t* jw_node, 
	const JsonDoc::jnode_t* pNode)
{
	Assert(jw != NULL);
	Assert(jw_node != NULL);
	Assert(pNode != NULL);

	if ((pNode->nType == JsonDoc::JTYPE_ARRAY) || 
		(pNode->nType == JsonDoc::JTYPE_OBJECT))
	{
		json_node_t* child_node = NULL;
		JsonDoc::jarray_t* pArray = pNode->pArray;

		for (size_t i = 0; i < pArray->nCount; ++i)
		{
			JsonDoc::jnode_t* pChild = pArray->pNodes[i];
			const char* key = pChild->szKey;

			if (NULL == key)
			{
				key = "";
			}

			json_node_t* new_node = NULL;

			switch (pChild->nType)
			{
			case JsonDoc::JTYPE_NULL:
				new_node = jw->NewNull(key);
				break;
			case JsonDoc::JTYPE_TRUE:
				new_node = jw->NewBoolean(key, true);
				break;
			case JsonDoc::JTYPE_FALSE:
				new_node = jw->NewBoolean(key, false);
				break;
			case JsonDoc::JTYPE_INT32:
				new_node = jw->NewInt32(key, pChild->nValue);
				break;
			case JsonDoc::JTYPE_DOUBLE:
				new_node = jw->NewDouble(key, pChild->dValue);
				break;
			case JsonDoc::JTYPE_STRING:
				new_node = jw->NewString(key, pChild->szValue);
				break;
			case JsonDoc::JTYPE_ARRAY:
				new_node = jw->NewArray(key);
				break;
			case JsonDoc::JTYPE_OBJECT:
				new_node = jw->NewObject(key);
				break;
			default:
				Assert(0);
				break;
			}

			if (NULL == child_node)
			{
				child_node = jw->AddFirstChild(jw_node, new_node);
			}
			else
			{
				child_node = jw->AddSibling(child_node, new_node);
			}

			if ((pChild->nType == JsonDoc::JTYPE_ARRAY) ||
				(pChild->nType == JsonDoc::JTYPE_OBJECT))
			{
				if (!save_node(jw, child_node, pChild))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool JsonDoc::SaveFile(const char* file_name)
{
	Assert(file_name != NULL);

	if (NULL == m_pRoot)
	{
		return false;
	}
	
	FrameJsonWriter jw;

	if (!save_node(&jw, jw.CreateRoot(), m_pRoot))
	{
		return false;
	}

	char buffer[1024];
	TJsonOutput<FrameAlloc> jo(buffer, sizeof(buffer));

	if (!jw.Write(&jo, true))
	{
		return false;
	}

	void* fp = frame_fopen(file_name, "wb");

	if (NULL == fp)
	{
		return false;
	}

	if (frame_fwrite(jo.GetData(), 1, jo.GetLength(), fp) != jo.GetLength())
	{
		frame_fclose(fp);
		return false;
	}

	frame_fclose(fp);
	return true;
}

ArgString JsonDoc::SaveString()
{
	if (NULL == m_pRoot)
	{
		return "";
	}

	FrameJsonWriter jw;

	if (!save_node(&jw, jw.CreateRoot(), m_pRoot))
	{
		return "";
	}

	char buffer[1024];
	TJsonOutput<FrameAlloc> jo(buffer, sizeof(buffer));

	if (!jw.Write(&jo, true))
	{
		return "";
	}

	return ArgString(jo.GetData(), jo.GetLength());
}

ArgString JsonDoc::SaveCompactString()
{
	if (NULL == m_pRoot)
	{
		return "";
	}

	FrameJsonWriter jw;

	if (!save_node(&jw, jw.CreateRoot(), m_pRoot))
	{
		return "";
	}

	char buffer[1024];
	TJsonOutput<FrameAlloc> jo(buffer, sizeof(buffer));

	if (!jw.Write(&jo, false))
	{
		return "";
	}

	return ArgString(jo.GetData(), jo.GetLength());
}

bool JsonDoc::CreateRoot()
{
	if (m_pRoot != NULL)
	{
		return false;
	}

	m_pRoot = this->NewObjectNode();
	// Set current node to root.
	m_pCurrent = m_pRoot;
	return true;
}

bool JsonDoc::AddNull(const char* key)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewNullNode();

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddBoolean(const char* key, bool value)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewBooleanNode(value);

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddInt32(const char* key, int value)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewInt32Node(value);

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddDouble(const char* key, double value)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewDoubleNode(value);

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddString(const char* key, const char* value)
{
	Assert(key != NULL);
	Assert(value != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewStringNode(value);

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddArray(const char* key)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewArrayNode();

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::AddObject(const char* key)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jnode_t* pNode = this->NewObjectNode();

	this->SetNodeKey(pNode, key);
	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddNull()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewNullNode();

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddBoolean(bool value)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewBooleanNode(value);

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddInt32(int value)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewInt32Node(value);

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddDouble(double value)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewDoubleNode(value);

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddString(const char* value)
{
	Assert(value != NULL);
	
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewStringNode(value);

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddArray()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewArrayNode();

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::ArrayAddObject()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_ARRAY)
	{
		return false;
	}

	jnode_t* pNode = this->NewObjectNode();

	this->AddNode(m_pCurrent, pNode);
	return true;
}

bool JsonDoc::RemoveKey(const char* key)
{
	Assert(key != NULL);
	
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (strcmp(pNode->szKey, key) == 0)
		{
			this->RemoveNode(pArray, i);
			return true;
		}
	}

	return false;
}

bool JsonDoc::RemoveIndex(int index)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if ((m_pCurrent->nType != JTYPE_ARRAY) &&
		(m_pCurrent->nType != JTYPE_OBJECT))
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	if ((size_t)index >= pArray->nCount)
	{
		return false;
	}

	this->RemoveNode(pArray, index);
	return true;
}

bool JsonDoc::RemoveAll()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if ((m_pCurrent->nType != JTYPE_ARRAY) &&
		(m_pCurrent->nType != JTYPE_OBJECT))
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		this->DeleteNode(pArray->pNodes[i]);
		pArray->pNodes[i] = NULL;
	}

	pArray->nCount = 0;
	return true;
}

bool JsonDoc::SelectRoot()
{
	if (NULL == m_pRoot)
	{
		return false;
	}

	m_pCurrent = m_pRoot;
	return true;
}

bool JsonDoc::SelectKey(const char* key)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}
	
	if ((m_pCurrent->nType != JTYPE_ARRAY) &&
		(m_pCurrent->nType != JTYPE_OBJECT))
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		if (NULL == pNode->szKey)
		{
			continue;
		}
		
		if (strcmp(pNode->szKey, key) == 0)
		{
			m_pCurrent = pNode;
			return true;
		}
	}

	return false;
}

bool JsonDoc::SelectIndex(int index)
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if ((m_pCurrent->nType != JTYPE_ARRAY) &&
		(m_pCurrent->nType != JTYPE_OBJECT))
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	if ((size_t)index >= pArray->nCount)
	{
		return false;
	}

	m_pCurrent = pArray->pNodes[index];
	return true;
}

bool JsonDoc::SelectParent()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	jnode_t* pParent = m_pCurrent->pParent;

	if (NULL == pParent)
	{
		return false;
	}
	
	m_pCurrent = pParent;
	return true;
}

const char* JsonDoc::GetKey()
{
	if (NULL == m_pCurrent)
	{
		return "";
	}

	if (NULL == m_pCurrent->szKey)
	{
		return "";
	}

	return m_pCurrent->szKey;
}

const char* JsonDoc::GetType()
{
	if (NULL == m_pCurrent)
	{
		return "";
	}

	switch (m_pCurrent->nType)
	{
	case JTYPE_NULL:
		return "null";
	case JTYPE_TRUE:
		return "boolean";
	case JTYPE_FALSE:
		return "boolean";
	case JTYPE_INT32:
		return "int32";
	case JTYPE_DOUBLE:
		return "double";
	case JTYPE_STRING:
		return "string";
	case JTYPE_ARRAY:
		return "array";
	case JTYPE_OBJECT:
		return "object";
	default:
		break;
	}

	return "";
}

bool JsonDoc::GetBoolean()
{
	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType == JTYPE_TRUE)
	{
		return true;
	}

	return false;
}

int JsonDoc::GetInt32()
{
	if (NULL == m_pCurrent)
	{
		return 0;
	}

	switch (m_pCurrent->nType)
	{
	case JTYPE_TRUE:
		return 1;
	case JTYPE_INT32:
		return m_pCurrent->nValue;
	case JTYPE_DOUBLE:
		return (int)m_pCurrent->dValue;
	case JTYPE_STRING:
		return atoi(m_pCurrent->szValue);
	default:
		break;
	}

	return 0;
}

double JsonDoc::GetDouble()
{
	if (NULL == m_pCurrent)
	{
		return 0.0;
	}

	switch (m_pCurrent->nType)
	{
	case JTYPE_TRUE:
		return 1.0;
	case JTYPE_INT32:
		return (double)m_pCurrent->nValue;
	case JTYPE_DOUBLE:
		return m_pCurrent->dValue;
	case JTYPE_STRING:
		return atof(m_pCurrent->szValue);
	default:
		break;
	}

	return 0.0;
}

ArgString JsonDoc::GetString()
{
	if (NULL == m_pCurrent)
	{
		return "";
	}

	switch (m_pCurrent->nType)
	{
	case JTYPE_TRUE:
		return "true";
	case JTYPE_FALSE:
		return "false";
	case JTYPE_INT32:
	{
		char buffer[32];

		plat_safe_sprintf(buffer, sizeof(buffer), "%d", m_pCurrent->nValue);
		return ArgString(buffer);
	}
	case JTYPE_DOUBLE:
	{
		char buffer[64];

		plat_safe_sprintf(buffer, sizeof(buffer), "%f", m_pCurrent->dValue);
		return ArgString(buffer);
	}
	case JTYPE_STRING:
		return m_pCurrent->szValue;
	default:
		break;
	}

	return "";
}

int JsonDoc::GetCount()
{
	if (NULL == m_pCurrent)
	{
		return 0;
	}

	if ((m_pCurrent->nType != JTYPE_ARRAY) &&
		(m_pCurrent->nType != JTYPE_OBJECT))
	{
		return 0;
	}

	return (int)m_pCurrent->pArray->nCount;
}

void JsonDoc::GetKeyList(const IArgList& args, IArgList* res)
{
	Assert(res != NULL);

	res->Clear();
	
	if (NULL == m_pCurrent)
	{
		return;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		res->AddString(pNode->szKey);
	}
}

bool JsonDoc::FindKey(const char* key)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return false;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return false;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (strcmp(pNode->szKey, key) == 0)
		{
			return true;
		}
	}

	return false;
}

void JsonDoc::ReadKey(const IArgList& args, IArgList* res)
{
	if (NULL == m_pCurrent)
	{
		return;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return;
	}

	const char* key = args.GetString(0);
	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				res->AddBoolean(true);
				break;
			case JTYPE_FALSE:
				res->AddBoolean(false);
				break;
			case JTYPE_INT32:
				res->AddInt32(pNode->nValue);
				break;
			case JTYPE_DOUBLE:
				res->AddDouble(pNode->dValue);
				break;
			case JTYPE_STRING:
				res->AddString(pNode->szValue);
				break;
			default:
				break;
			}
			
			return;
		}
	}

	// Return default value.
	if (args.GetCount() >= 2)
	{
		switch (args.GetType(1))
		{
		case DT_BOOLEAN:
			res->AddBoolean(args.GetBoolean(1));
			break;
		case DT_INT32:
			res->AddInt32(args.GetInt32(1));
			break;
		case DT_INT64:
			res->AddInt64(args.GetInt64(1));
			break;
		case DT_FLOAT:
			res->AddFloat(args.GetFloat(1));
			break;
		case DT_DOUBLE:
			res->AddDouble(args.GetDouble(1));
			break;
		case DT_STRING:
			res->AddString(args.GetString(1));
			break;
		default:
			break;
		}
	}
}

bool JsonDoc::ReadBoolean(const char* key, bool def)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return def;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return def;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				return true;
			case JTYPE_FALSE:
				return false;
			case JTYPE_INT32:
				return pNode->nValue != 0;
			case JTYPE_DOUBLE:
				return pNode->dValue != 0.0;
			case JTYPE_STRING:
				return stricmp(pNode->szValue, "true") == 0;
			default:
				return def;
			}
		}
	}

	return def;
}

int JsonDoc::ReadInt32(const char* key, int def)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return def;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return def;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				return 1;
			case JTYPE_FALSE:
				return 0;
			case JTYPE_INT32:
				return pNode->nValue;
			case JTYPE_DOUBLE:
				return (int)pNode->dValue;
			case JTYPE_STRING:
				return atoi(pNode->szValue);
			default:
				return def;
			}
		}
	}

	return def;
}

float JsonDoc::ReadFloat(const char* key, float def)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return def;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return def;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				return 1.0F;
			case JTYPE_FALSE:
				return 0.0F;
			case JTYPE_INT32:
				return (float)pNode->nValue;
			case JTYPE_DOUBLE:
				return (float)pNode->dValue;
			case JTYPE_STRING:
				return (float)atof(pNode->szValue);
			default:
				return def;
			}
		}
	}

	return def;
}

double JsonDoc::ReadDouble(const char* key, double def)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return def;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return def;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				return 1.0;
			case JTYPE_FALSE:
				return 0.0;
			case JTYPE_INT32:
				return (double)pNode->nValue;
			case JTYPE_DOUBLE:
				return pNode->dValue;
			case JTYPE_STRING:
				return atof(pNode->szValue);
			default:
				return def;
			}
		}
	}

	return def;
}

ArgString JsonDoc::ReadString(const char* key, const char* def)
{
	Assert(key != NULL);

	if (NULL == m_pCurrent)
	{
		return def;
	}

	if (m_pCurrent->nType != JTYPE_OBJECT)
	{
		return def;
	}

	jarray_t* pArray = m_pCurrent->pArray;

	for (size_t i = 0; i < pArray->nCount; ++i)
	{
		jnode_t* pNode = pArray->pNodes[i];

		Assert(pNode->szKey != NULL);

		if (stricmp(pNode->szKey, key) == 0)
		{
			switch (pNode->nType)
			{
			case JTYPE_TRUE:
				return "true";
			case JTYPE_FALSE:
				return "false";
			case JTYPE_INT32:
			{
				char buffer[32];

				plat_safe_sprintf(buffer, sizeof(buffer), "%d", pNode->nValue);
				return ArgString(buffer);
			}
			case JTYPE_DOUBLE:
			{
				char buffer[64];

				plat_safe_sprintf(buffer, sizeof(buffer), "%f",
					pNode->dValue);
				return ArgString(buffer);
			}
			case JTYPE_STRING:
				return pNode->szValue;
			default:
				return def;
			}
		}
	}

	return def;
}

JsonDoc::jnode_t* JsonDoc::NewNullNode()
{
	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_NULL;
	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->pArray = NULL;
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewBooleanNode(bool value)
{
	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	if (value)
	{
		pNode->nType = JTYPE_TRUE;
	}
	else
	{
		pNode->nType = JTYPE_FALSE;
	}

	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->pArray = NULL;
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewInt32Node(int value)
{
	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_INT32;
	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->nValue = value;
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewDoubleNode(double value)
{
	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_DOUBLE;
	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->dValue = value;
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewStringNode(const char* value)
{
	Assert(value != NULL);

	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_STRING;
	pNode->szKey = NULL;
	pNode->pParent = NULL;

	size_t value_size = strlen(value) + 1;
	
	pNode->szValue = (char*)K_ALLOC(value_size);
	memcpy(pNode->szValue, value, value_size);
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewArrayNode()
{
	jarray_t* pArray = (jarray_t*)K_ALLOC(sizeof(jarray_t));

	pArray->pNodes = NULL;
	pArray->nSize = 0;
	pArray->nCount = 0;
	
	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_ARRAY;
	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->pArray = pArray;
	return pNode;
}

JsonDoc::jnode_t* JsonDoc::NewObjectNode()
{
	jarray_t* pArray = (jarray_t*)K_ALLOC(sizeof(jarray_t));

	pArray->pNodes = NULL;
	pArray->nSize = 0;
	pArray->nCount = 0;

	jnode_t* pNode = (jnode_t*)K_ALLOC(sizeof(jnode_t));

	pNode->nType = JTYPE_OBJECT;
	pNode->szKey = NULL;
	pNode->pParent = NULL;
	pNode->pArray = pArray;
	return pNode;
}

void JsonDoc::SetNodeKey(jnode_t* pNode, const char* key)
{
	Assert(pNode != NULL);
	Assert(key != NULL);

	size_t key_size = strlen(key) + 1;
	char* pNewKey = (char*)K_ALLOC(key_size);

	memcpy(pNewKey, key, key_size);

	if (pNode->szKey)
	{
		K_FREE(pNode->szKey);
	}

	pNode->szKey = pNewKey;
}

void JsonDoc::AddNode(jnode_t* pParent, jnode_t* pNode)
{
	Assert(pParent != NULL);
	Assert(pNode != NULL);

	jarray_t* pArray = pParent->pArray;

	Assert(pArray != NULL);
	Assert(pArray->nCount <= pArray->nSize);

	size_t count = pArray->nCount;

	if (count == pArray->nSize)
	{
		size_t new_size = pArray->nSize * 2;

		if (0 == new_size)
		{
			new_size = 8;
		}

		jnode_t** pNodes = (jnode_t**)K_ALLOC(sizeof(jnode_t*) * new_size);

		if (pArray->pNodes)
		{
			memcpy(pNodes, pArray->pNodes, sizeof(jnode_t*) * count);
			K_FREE(pArray->pNodes);
		}

		pArray->pNodes = pNodes;
		pArray->nSize = new_size;
	}

	pNode->pParent = pParent;
	pArray->pNodes[count] = pNode;
	++pArray->nCount;
}

void JsonDoc::RemoveNode(jarray_t* pArray, size_t index)
{
	Assert(pArray != NULL);
	Assert(pArray->nCount > 0);
	Assert(index < pArray->nCount);

	jnode_t* pNode = pArray->pNodes[index];

	Assert(pNode != NULL);

	this->DeleteNode(pNode);
	memmove(pArray->pNodes + index, pArray->pNodes + index + 1,
		sizeof(jnode_t*) * (pArray->nCount - index - 1));
	--pArray->nCount;
}

void JsonDoc::DeleteNode(jnode_t* pNode)
{
	Assert(pNode != NULL);

	if (pNode->szKey)
	{
		K_FREE(pNode->szKey);
	}

	if (pNode->nType == JTYPE_STRING)
	{
		K_FREE(pNode->szValue);
	}
	else if ((pNode->nType == JTYPE_ARRAY) || (pNode->nType == JTYPE_OBJECT))
	{
		jarray_t* pArray = pNode->pArray;

		if (pArray->pNodes)
		{
			for (size_t k = 0; k < pArray->nCount; ++k)
			{
				this->DeleteNode(pArray->pNodes[k]);
			}

			K_FREE(pArray->pNodes);
		}

		K_FREE(pArray);
	}

	K_FREE(pNode);
}
