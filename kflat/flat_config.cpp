// flat_config.cpp
// Created by lulimin on 2021/1/5.

#include "flat_config.h"
#include "flat_script.h"
#include "flat_widget.h"
#include "flat_ui.h"
#include "../inc/json_writer.h"
#include "../inc/frame_mem.h"
#include "../inc/frame_file.h"
#include "../inc/auto_buffer.h"
#include "../inc/i_data_table.h"
#include "../inc/string_hash_table.h"
//#pragma warning(disable:4200)
#define SAJSON_NO_STD_STRING
#include "../ext/sajson/sajson.h"

#define FLOAT_EQUAL(v1, v2, ep) ((v1) <= ((v2) + ep)) && ((v1) >= ((v2) - ep))

// Widget name container.
typedef TStringHashTable<char, int, TCharTraits<char>, 
	FrameAlloc> WidgetNameSet;
// Json writer.
typedef TJsonWriter<FrameAlloc, TJsonAutoBuffer<FrameAlloc> > FrameJsonWriter;

// Read configure string.
static inline const char* read_json_string(const sajson::value& node,
	const char* key)
{
	Assert(key != NULL);

	sajson::string sakey(key, strlen(key));

	if (node.find_object_key(sakey) == node.get_length())
	{
		return "";
	}

	const sajson::value& val = node.get_value_of_key(sakey);

	if (val.get_type() != sajson::TYPE_STRING)
	{
		return "";
	}

	return val.as_cstring();
}

// Set property value of widget.
static bool set_widget_prop(FlatWidget* pWidget, const char* name, 
	const char* value)
{
	Assert(pWidget != NULL);
	Assert(name != NULL);
	Assert(value != NULL);

	FrameArgData prop;

	if (!frame_get_property(pWidget, name, &prop))
	{
		return false;
	}

	switch (prop.GetType())
	{
	case DT_BOOLEAN:
		if (stricmp(value, "true") == 0)
		{
			prop.SetBoolean(true);
		}
		else
		{
			prop.SetBoolean(false);
		}
		break;
	case DT_INT32:
		prop.SetInt32(atoi(value));
		break;
	case DT_INT64:
		prop.SetInt64(atoi(value));
		break;
	case DT_FLOAT:
		prop.SetFloat((float)atof(value));
		break;
	case DT_DOUBLE:
		prop.SetDouble(atof(value));
		break;
	case DT_STRING:
		prop.SetString(value);
		break;
	default:
		return false;
	}

	return frame_set_property(pWidget, name, prop);
}

// Load part widget from json node.
static bool load_part_widget(FlatUI* pFlatUI, FlatWidget* pWidget,
	const sajson::value& node, bool design_mode)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);
	
	const char* widget_type = read_json_string(node, "widget");

	if (util_string_empty(widget_type))
	{
		K_LOG("(load_part_widget)No widget type.");
		return false;
	}

	if (strcmp(widget_type, pWidget->GetDesc()->GetObjectName()) != 0)
	{
		K_LOG("(load_part_widget)Widget type not match.");
		K_LOG(widget_type);
		return false;
	}

	// Load properties.
	sajson::string props_key("properties", 10);
	size_t props_index = node.find_object_key(props_key);

	if (props_index < node.get_length())
	{
		const sajson::value& props_node = node.get_object_value(props_index);

		if (props_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t prop_num = props_node.get_length();

			for (size_t i = 0; i < prop_num; ++i)
			{
				const sajson::string& prop_name = props_node.get_object_key(i);
				const sajson::value& prop = props_node.get_object_value(i);

				if (prop.get_type() != sajson::TYPE_STRING)
				{
					K_LOG("(load_part_widget)Property value error.");
					K_LOG(prop_name.data());
					continue;
				}

				const char* prop_value = prop.as_cstring();

				if (!set_widget_prop(pWidget, prop_name.data(), prop_value))
				{
					K_LOG("(load_part_widget)Set property error.");
					K_LOG(prop_name.data());
					return false;
				}
			}
		}
	}

	// Load part widgets.
	sajson::string parts_key("parts", 5);
	size_t parts_index = node.find_object_key(parts_key);

	if (parts_index < node.get_length())
	{
		const sajson::value& parts_node = node.get_object_value(parts_index);

		if (parts_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t part_num = parts_node.get_length();

			for (size_t i = 0; i < part_num; ++i)
			{
				const sajson::string& part_name = parts_node.get_object_key(i);
				FrameArgData part_value;

				if (!frame_get_property(pWidget, part_name.data(), &part_value))
				{
					K_LOG("(load_part_widget)Get part widget property error.");
					K_LOG(part_name.data());
					continue;
				}

				if (part_value.GetType() != DT_UNIQID)
				{
					K_LOG("(load_part_widget)Part widget property not id.");
					K_LOG(part_name.data());
					continue;
				}

				FlatWidget* pPartWidget = flat_get_widget(
					part_value.GetUniqid());

				if (NULL == pPartWidget)
				{
					K_LOG("(load_part_widget)Get part widget error.");
					K_LOG(part_name.data());
					continue;
				}

				const sajson::value& part = parts_node.get_object_value(i);

				if (!load_part_widget(pFlatUI, pPartWidget, part, design_mode))
				{
					K_LOG("(load_part_widget)Load part widget error.");
					K_LOG(part_name.data());
					return false;
				}
			}
		}
	}

	return true;
}

// Load widget from json node.
static FlatWidget* load_widget(FlatUI* pFlatUI, FlatWidget* pParent, 
	const sajson::value& node, WidgetNameSet* pNames, bool design_mode)
{
	Assert(pFlatUI != NULL);
	Assert(pNames != NULL);

	const char* widget_type = read_json_string(node, "widget");

	if (util_string_empty(widget_type))
	{
		K_LOG("(load_widget)No widget type.");
		return NULL;
	}

	const char* name = read_json_string(node, "name");

	if (pNames->Exists(name))
	{
		K_LOG("(load_widget)Widget name exists.");
		K_LOG(name);
		return NULL;
	}

	FlatWidget* pWidget = pFlatUI->CreateWidget(widget_type);

	if (NULL == pWidget)
	{
		K_LOG("(load_widget)Create widget error.");
		K_LOG(widget_type);
		return NULL;
	}

	pWidget->SetName(name);
	pWidget->SetDesignMode(design_mode);
	// Add to widget name list.
	pNames->Add(name, 0);

	// Script file name.
	const char* script = read_json_string(node, "script");

	if (util_string_empty(script))
	{
		if ((pParent != NULL) && (pParent->GetHook() != NULL))
		{
			script = pParent->GetHook()->GetName();
		}
	}

	if (!design_mode)
	{
		if (!util_string_empty(script))
		{
			// Bind script.
			FlatScript* pScript = FlatScript::CreateInstance();

			pScript->SetScriptFile(script);
			pWidget->SetHook(pScript);
		}
	}

	// Load properties.
	sajson::string props_key("properties", 10);
	size_t props_index = node.find_object_key(props_key);
	
	if (props_index < node.get_length()) 
	{
		const sajson::value& props_node = node.get_object_value(props_index);

		if (props_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t prop_num = props_node.get_length();

			for (size_t i = 0; i < prop_num; ++i)
			{
				const sajson::value& prop = props_node.get_object_value(i);
				
				if (prop.get_type() != sajson::TYPE_STRING)
				{
					continue;
				}

				const sajson::string& prop_name = props_node.get_object_key(i);
				const char* prop_value = prop.as_cstring();

				if (!set_widget_prop(pWidget, prop_name.data(), prop_value))
				{
					K_LOG("(load_widget)Set property error.");
					K_LOG(prop_name.data());
				}
			}
		}
	}

	// Load event processors.
	sajson::string events_key("events", 6);
	size_t events_index = node.find_object_key(events_key);

	if (events_index < node.get_length())
	{
		const sajson::value& events_node = node.get_object_value(events_index);

		if (events_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t event_num = events_node.get_length();

			for (size_t i = 0; i < event_num; ++i)
			{
				const sajson::string& ev_key = events_node.get_object_key(i);
				const char* ev_name = ev_key.data();
				const sajson::value& ev_val = events_node.get_object_value(i);

				if (ev_val.get_type() != sajson::TYPE_STRING)
				{
					K_LOG("(load_widget)Event processor not string.");
					K_LOG(ev_name);
					continue;
				}

				const char* proc = ev_val.as_cstring();

				if (util_string_empty(ev_name) || util_string_empty(proc))
				{
					K_LOG("(load_widget)Event data error.");
					K_LOG(ev_name);
					continue;
				}

				if (design_mode)
				{
					char extra_name[256];

					plat_safe_sprintf(extra_name, sizeof(extra_name),
						"__event_%s", ev_name);

					IDataTable* pExtra = pWidget->GetExtra();

					if (NULL == pExtra)
					{
						frame_create_extra(pWidget);
						pExtra = pWidget->GetExtra();
					}

					pExtra->Assign(extra_name, FrameArgData(DT_STRING, proc));
				}
				else
				{
					IObjectHook* pHook = pWidget->GetHook();

					if (pHook != NULL)
					{
						pHook->AddEventProcessor(ev_name, "", proc);
					}
				}
			}
		}
	}

	// Initialize script function.
	const char* loader = read_json_string(node, "loader");

	if (design_mode)
	{
		IDataTable* pExtra = pWidget->GetExtra();

		if (NULL == pExtra)
		{
			frame_create_extra(pWidget);
			pExtra = pWidget->GetExtra();
		}

		pExtra->Assign("__script", FrameArgData(DT_STRING, script));
		pExtra->Assign("__loader", FrameArgData(DT_STRING, loader));
	}
	else
	{
		if (!util_string_empty(script) && !util_string_empty(loader))
		{
			uniqid_t id = pWidget->GetUID();

			frame_run_routine(script, loader, id, FrameArgList(), NULL);
		}
	}

	// Add to container.
	if (pParent)
	{
		flat_container_t* pContainer = pParent->GetContainer();

		if (pContainer)
		{
			flat_container_add(pContainer, pWidget);
		}
	}

	// Load part widgets.
	sajson::string parts_key("parts", 5);
	size_t parts_index = node.find_object_key(parts_key);

	if (parts_index < node.get_length())
	{
		const sajson::value& parts_node = node.get_object_value(parts_index);

		if (parts_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t part_num = parts_node.get_length();

			for (size_t i = 0; i < part_num; ++i)
			{
				const sajson::string& part_name = parts_node.get_object_key(i);
				FrameArgData part_val;

				if (!frame_get_property(pWidget, part_name.data(), &part_val))
				{
					K_LOG("(load_widget)Get part widget property error.");
					K_LOG(part_name.data());
					continue;
				}

				if (part_val.GetType() != DT_UNIQID)
				{
					K_LOG("(load_widget)Part widget property not id.");
					K_LOG(part_name.data());
					continue;
				}

				FlatWidget* pPartWidget = flat_get_widget(part_val.GetUniqid());

				if (NULL == pPartWidget)
				{
					K_LOG("(load_widget)Get part widget error.");
					K_LOG(part_name.data());
					continue;
				}

				const sajson::value& part = parts_node.get_object_value(i);
				
				if (!load_part_widget(pFlatUI, pPartWidget, part, design_mode))
				{
					K_LOG("(load_widget)Load part widget error.");
					K_LOG(part_name.data());
				}
			}
		}
	}

	// Load child widgets.
	sajson::string children_key("children", 8);
	size_t children_index = node.find_object_key(children_key);

	if (children_index < node.get_length())
	{
		const sajson::value& children_node = node.get_object_value(
			children_index);

		if (children_node.get_type() == sajson::TYPE_OBJECT)
		{
			size_t child_num = children_node.get_length();

			for (size_t i = 0; i < child_num; ++i)
			{
				const sajson::value& child = children_node.get_object_value(i);
				FlatWidget* pChild = load_widget(pFlatUI, pWidget, child, 
					pNames, design_mode);

				if (NULL == pChild)
				{
					K_LOG("(load_widget)Load child widget error.");
					continue;
				}

				// Record this widget to root.
				FlatWidget* pRootWidget = pChild->GetRootWidget();
				IDataTable* pExtra = pRootWidget->GetExtra();

				if (NULL == pExtra)
				{
					frame_create_extra(pRootWidget);
					pExtra = pRootWidget->GetExtra();
				}

				pExtra->Assign(pChild->GetName(), 
					FrameArgData(DT_UNIQID, pChild->GetUID()));
			}
		}
	}

	return pWidget;
}

// Load widget configuration.
FlatWidget* flat_load_config(FlatUI* pFlatUI, const char* file_name, 
	bool design_mode)
{
	Assert(pFlatUI != NULL);
	Assert(file_name != NULL);

	void* fp = frame_fopen(file_name, "rb");

	if (NULL == fp)
	{
		K_LOG("(flat_load_config)Open file error.");
		K_LOG(file_name);
		return NULL;
	}

	frame_fseek(fp, 0, IFileAccess::_SEEK_END);
	size_t file_size = frame_ftell(fp);
	frame_fseek(fp, 0, IFileAccess::_SEEK_SET);

	TAutoBuffer<char, 2048, FrameAlloc> auto_buf(file_size);
	char* file_data = auto_buf.GetBuffer();

	if (frame_fread(file_data, 1, file_size, fp) != file_size)
	{
		frame_fclose(fp);
		K_LOG("(flat_load_config)Read file error.");
		K_LOG(file_name);
		return NULL;
	}

	frame_fclose(fp);
	
	TAutoBuffer<size_t, 1024> auto_buf1(file_size);
	size_t* pJsonMem = auto_buf1.GetBuffer();
	const sajson::document& doc = sajson::parse(
		sajson::bounded_allocation(pJsonMem, file_size),
		sajson::mutable_string_view(file_size, file_data));

	if (!doc.is_valid())
	{
		K_LOG("(flat_load_config)Parse json document error.");
		K_LOG(file_name);
		return NULL;
	}

	const sajson::value& root = doc.get_root();
	WidgetNameSet names;
	FlatWidget* pWidget = load_widget(pFlatUI, NULL, root, &names, design_mode);

	if (NULL == pWidget)
	{
		K_LOG("(flat_load_config)Load widget error.");
		K_LOG(file_name);
		return NULL;
	}

	return pWidget;
}

// Test property equalize.
static bool property_equal(const IArgData& p1, const IArgData& p2)
{
	if (p1.GetType() != p2.GetType())
	{
		return false;
	}

	switch (p1.GetType())
	{
	case DT_BOOLEAN:
		return p1.GetBoolean() == p2.GetBoolean();
	case DT_INT32:
		return p1.GetInt32() == p2.GetInt32();
	case DT_INT64:
		return p1.GetInt64() == p2.GetInt64();
	case DT_FLOAT:
		return FLOAT_EQUAL(p1.GetFloat(), p2.GetFloat(), FLT_EPSILON);
	case DT_DOUBLE:
		return FLOAT_EQUAL(p1.GetDouble(), p2.GetDouble(), DBL_EPSILON);
	case DT_STRING:
		return strcmp(p1.GetString(), p2.GetString()) == 0;
	default:
		break;
	}

	return false;
}

// Creat property node.
static json_node_t* create_prop_node(FrameJsonWriter* jw, const char* name,
	const IArgData& value)
{
	Assert(jw != NULL);
	Assert(name != NULL);

	json_node_t* pPropNode = NULL;

	switch (value.GetType())
	{
	case DT_BOOLEAN:
		if (value.GetBoolean())
		{
			pPropNode = jw->NewString(name, "true");
		}
		else
		{
			pPropNode = jw->NewString(name, "false");
		}
		break;
	case DT_INT32:
	{
		char buffer[32];

		plat_safe_sprintf(buffer, sizeof(buffer), "%d", value.GetInt32());
		pPropNode = jw->NewString(name, buffer);
		break;
	}
	case DT_INT64:
	{
		char buffer[64];

		//plat_safe_sprintf(buffer, sizeof(buffer), "%lld", value.GetInt64());
		plat_int64_to_string(buffer, sizeof(buffer), value.GetInt64());
		pPropNode = jw->NewString(name, buffer);
		break;
	}
	case DT_FLOAT:
	{
		char buffer[64];

		plat_safe_sprintf(buffer, sizeof(buffer), "%f", 
			(double)value.GetFloat());
		pPropNode = jw->NewString(name, buffer);
		break;
	}
	case DT_DOUBLE:
	{
		char buffer[64];

		plat_safe_sprintf(buffer, sizeof(buffer), "%f", value.GetDouble());
		pPropNode = jw->NewString(name, buffer);
		break;
	}
	case DT_STRING:
		pPropNode = jw->NewString(name, value.GetString());
		break;
	default:
		break;
	}
	
	return pPropNode;
}

// Save part widget to json node.
static bool save_part_widget(FlatUI* pFlatUI, FlatWidget* pWidget,
	FrameJsonWriter* jw, json_node_t* pNode)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);
	Assert(jw != NULL);
	Assert(pNode != NULL);
	
	json_node_t* pWidgetType = jw->AddFirstChild(pNode,
		jw->NewString("widget", pWidget->GetDesc()->GetObjectName()));
	//json_node_t* pName = jw->AddSibling(pWidgetType,
	//	jw->NewString("name", pWidget->GetName()));
	flat_metadata_t* pMD = pFlatUI->GetMetadata(pWidget);
	
	Assert(pMD != NULL);

	// Save properties.
	//json_node_t* pProps = jw->AddSibling(pName, jw->NewObject("properties"));
	json_node_t* pProps = jw->AddSibling(pWidgetType, 
		jw->NewObject("properties"));
	json_node_t* pPropNode = NULL;
	size_t prop_num = flat_metadata_prop_count(pMD);

	for (size_t i = 0; i < prop_num; ++i)
	{
		const char* prop_name = flat_metadata_prop_name(pMD, i);
		FrameArgData prop_value;

		if (!frame_get_property(pWidget, prop_name, &prop_value))
		{
			K_LOG("(save_part_widget)Get property error.");
			K_LOG(prop_name);
			continue;
		}

		int prop_flags = flat_metadata_prop_flags(pMD, i);
		
		if ((prop_flags & FLAT_METAFLAG_PART_SAVE) == 0)
		{
			continue;
		}
		
		const IArgData* prop_def = flat_metadata_prop_default(pMD, i);

		if (prop_def)
		{
			if (property_equal(prop_value, *prop_def))
			{
				continue;
			}
		}

		json_node_t* pProp = create_prop_node(jw, prop_name, prop_value);

		if (NULL == pProp)
		{
			K_LOG("(save_part_widget)Create property node error.");
			K_LOG(prop_name);
			continue;
		}

		if (NULL == pPropNode)
		{
			pPropNode = jw->AddFirstChild(pProps, pProp);
		}
		else
		{
			pPropNode = jw->AddSibling(pPropNode, pProp);
		}
	}

	// Save part widgets.
	json_node_t* pParts = NULL;
	json_node_t* pPartNode = NULL;
	size_t part_num = flat_metadata_part_count(pMD);

	if (part_num > 0)
	{
		pParts = jw->AddSibling(pProps, jw->NewObject("parts"));
	}

	for (size_t k = 0; k < part_num; ++k)
	{
		const char* part_name = flat_metadata_part_name(pMD, k);
		FrameArgData prop_value;

		if (!frame_get_property(pWidget, part_name, &prop_value))
		{
			K_LOG("(save_part_widget)Get part widget property error.");
			K_LOG(part_name);
			continue;
		}

		if (prop_value.GetType() != DT_UNIQID)
		{
			K_LOG("(save_part_widget)Part widget property not id.");
			K_LOG(part_name);
			continue;
		}

		FlatWidget* pPartWidget = (FlatWidget*)frame_get_object(
			prop_value.GetUniqid());

		if (NULL == pPartWidget)
		{
			continue;
		}

		if (!pPartWidget->GetDesc()->DeriveFrom("FlatWidget"))
		{
			K_LOG("(save_part_widget)Part widget type error.");
			K_LOG(part_name);
			continue;
		}

		IDataTable* pExtra = pPartWidget->GetExtra();

		if (pExtra)
		{
			if (pExtra->Exists("__ignore"))
			{
				// Donot save with ignore flag.
				continue;
			}
		}

		json_node_t* pPart = jw->NewObject(part_name);

		if (NULL == pPartNode)
		{
			pPartNode = jw->AddFirstChild(pParts, pPart);
		}
		else
		{
			pPartNode = jw->AddSibling(pPartNode, pPart);
		}

		if (!save_part_widget(pFlatUI, pPartWidget, jw, pPart))
		{
			K_LOG("(save_part_widget)Save part widget error.");
			K_LOG(part_name);
			return false;
		}
	}

	return true;
}

// Save widget to json node.
static bool save_widget(FlatUI* pFlatUI, FlatWidget* pWidget,
	FrameJsonWriter* jw, json_node_t* pNode)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);
	Assert(jw != NULL);
	Assert(pNode != NULL);

	json_node_t* pWidgetType = jw->AddFirstChild(pNode, 
		jw->NewString("widget", pWidget->GetDesc()->GetObjectName()));
	json_node_t* pName = jw->AddSibling(pWidgetType,
		jw->NewString("name", pWidget->GetName()));

	const char* script = "";
	const char* loader = "";
	IDataTable* pExtra = pWidget->GetExtra();

	if (pExtra)
	{
		if (pExtra->Exists("__script"))
		{
			script = pExtra->GetValue("__script")->GetString();
		}

		if (pExtra->Exists("__loader"))
		{
			loader = pExtra->GetValue("__loader")->GetString();
		}
	}

	json_node_t* pScript = jw->AddSibling(pName, 
		jw->NewString("script", script));
	json_node_t* pLoader = jw->AddSibling(pScript, 
		jw->NewString("loader", loader));
	flat_metadata_t* pMD = pFlatUI->GetMetadata(pWidget);

	Assert(pMD != NULL);

	// Save properties.
	json_node_t* pProps = jw->AddSibling(pLoader, jw->NewObject("properties"));
	json_node_t* pPropNode = NULL;
	size_t prop_num = flat_metadata_prop_count(pMD);

	for (size_t i = 0; i < prop_num; ++i)
	{
		const char* prop_name = flat_metadata_prop_name(pMD, i);
		FrameArgData prop_value;

		if (!frame_get_property(pWidget, prop_name, &prop_value))
		{
			K_LOG("(save_widget)Get property error.");
			K_LOG(prop_name);
			continue;
		}

		int prop_flags = flat_metadata_prop_flags(pMD, i);

		if ((prop_flags & FLAT_METAFLAG_SAVE) == 0)
		{
			continue;
		}

		const IArgData* prop_def = flat_metadata_prop_default(pMD, i);

		if (prop_def)
		{
			if (property_equal(prop_value, *prop_def))
			{
				continue;
			}
		}

		json_node_t* pProp = create_prop_node(jw, prop_name, prop_value);

		if (NULL == pProp)
		{
			K_LOG("(save_widget)Create property node error.");
			K_LOG(prop_name);
			continue;
		}

		if (NULL == pPropNode)
		{
			pPropNode = jw->AddFirstChild(pProps, pProp);
		}
		else
		{
			pPropNode = jw->AddSibling(pPropNode, pProp);
		}
	}

	// Save event processors.
	json_node_t* pEvents = jw->AddSibling(pProps, jw->NewObject("events"));
	json_node_t* pEventNode = NULL;
	size_t event_num = flat_metadata_event_count(pMD);

	for (size_t j = 0; j < event_num; ++j)
	{
		const char* event_name = flat_metadata_event_name(pMD, j);
		char extra_name[256];

		plat_safe_sprintf(extra_name, sizeof(extra_name), "__event_%s",
			event_name);

		IDataTable* pExtra = pWidget->GetExtra();

		if (NULL == pExtra)
		{
			continue;
		}

		if (!pExtra->Exists(extra_name))
		{
			continue;
		}

		const IArgData* pValue = pExtra->GetValue(extra_name);
		const char* func = pValue->GetString();

		if (util_string_empty(func))
		{
			continue;
		}

		json_node_t* pEvent = jw->NewString(event_name, func);

		if (NULL == pEventNode)
		{
			pEventNode = jw->AddFirstChild(pEvents, pEvent);
		}
		else
		{
			pEventNode = jw->AddSibling(pEventNode, pEvent);
		}
	}

	// Save part widgets.
	json_node_t* pParts = NULL;
	json_node_t* pPartNode = NULL;
	size_t part_num = flat_metadata_part_count(pMD);

	if (part_num > 0)
	{
		pParts = jw->AddSibling(pEvents, jw->NewObject("parts"));
	}

	for (size_t k = 0; k < part_num; ++k)
	{
		const char* part_name = flat_metadata_part_name(pMD, k);
		FrameArgData prop_value;

		if (!frame_get_property(pWidget, part_name, &prop_value))
		{
			K_LOG("(save_widget)Get part widget property error.");
			K_LOG(part_name);
			continue;
		}

		if (prop_value.GetType() != DT_UNIQID)
		{
			K_LOG("(save_widget)Part widget property not id.");
			K_LOG(part_name);
			continue;
		}

		FlatWidget* pPartWidget = (FlatWidget*)frame_get_object(
			prop_value.GetUniqid());

		if (NULL == pPartWidget)
		{
			continue;
		}

		if (!pPartWidget->GetDesc()->DeriveFrom("FlatWidget"))
		{
			K_LOG("(save_widget)Part widget type error.");
			K_LOG(part_name);
			continue;
		}

		IDataTable* pExtra = pPartWidget->GetExtra();

		if (pExtra)
		{
			if (pExtra->Exists("__ignore"))
			{
				// Donot save with ignore flag.
				continue;
			}
		}

		json_node_t* pPart = jw->NewObject(part_name);

		if (NULL == pPartNode)
		{
			pPartNode = jw->AddFirstChild(pParts, pPart);
		}
		else
		{
			pPartNode = jw->AddSibling(pPartNode, pPart);
		}

		if (!save_part_widget(pFlatUI, pPartWidget, jw, pPart))
		{
			K_LOG("(save_widget)Save part widget error.");
			K_LOG(part_name);
			return false;
		}
	}

	// Save child widgets.
	flat_container_t* pContainer = pWidget->GetContainer();

	if (pContainer)
	{
		json_node_t* pChildren;

		if (pParts)
		{
			pChildren = jw->AddSibling(pParts, jw->NewObject("children"));
		}
		else
		{
			pChildren = jw->AddSibling(pEvents, jw->NewObject("children"));
		}

		size_t child_size = flat_container_get_size(pContainer);
		FlatWidget** child_data = flat_container_get_data(pContainer);
		json_node_t* pChildNode = NULL;

		for (size_t i = 0; i < child_size; ++i)
		{
			FlatWidget* pChild = child_data[i];

			if (NULL == pChild)
			{
				continue;
			}

			IDataTable* pExtra = pChild->GetExtra();
			
			if (pExtra)
			{
				if (pExtra->Exists("__ignore"))
				{
					// Donot save with ignore flag.
					continue;
				}
			}

			if (NULL == pChildNode)
			{
				pChildNode = jw->AddFirstChild(pChildren, 
					jw->NewObject(pChild->GetName()));
			}
			else
			{
				pChildNode = jw->AddSibling(pChildNode, 
					jw->NewObject(pChild->GetName()));
			}

			if (!save_widget(pFlatUI, pChild, jw, pChildNode))
			{
				K_LOG("(save_widget)Save child widget error.");
				K_LOG(pChild->GetName());
				return false;
			}
		}
	}

	return true;
}

// Save widget configuration.
bool flat_save_config(FlatUI* pFlatUI, FlatWidget* pWidget,
	const char* file_name)
{
	Assert(pFlatUI != NULL);
	Assert(pWidget != NULL);
	Assert(file_name != NULL);

	FrameJsonWriter jw;
	json_node_t* pRoot = jw.CreateRoot();

	if (!save_widget(pFlatUI, pWidget, &jw, pRoot))
	{
		K_LOG("(flat_save_config)Save widget error.");
		K_LOG(file_name);
		return false;
	}

	char buffer[2048];
	TJsonOutput<FrameAlloc> jo(buffer, sizeof(buffer));
	
	if (!jw.Write(&jo, true))
	{
		K_LOG("(flat_save_config)Write output error.");
		K_LOG(file_name);
		return false;
	}

	void* fp = frame_fopen(file_name, "wb");

	if (NULL == fp)
	{
		K_LOG("(flat_save_config)Open file error.");
		K_LOG(file_name);
		return false;
	}

	if (frame_fwrite(jo.GetData(), 1, jo.GetLength(), fp) != jo.GetLength())
	{
		frame_fclose(fp);
		K_LOG("(flat_save_config)Write file error.");
		K_LOG(file_name);
		return false;
	}

	frame_fclose(fp);
	return true;
}
