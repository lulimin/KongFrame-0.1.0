--User interface designer terminal script.

--Initialize terminal.
function terminal_init(self)
	k_event(self, "input_close", "terminal_input_close")
	k_event(self, "input_exit", "terminal_input_exit")
	k_event(self, "input_getp", "terminal_input_getp")
	k_event(self, "input_setp", "terminal_input_setp")
	k_event(self, "input_gete", "terminal_input_gete")
	k_event(self, "input_sete", "terminal_input_sete")
	k_event(self, "input_call", "terminal_input_runm")
	k_event(self, "input_allp", "terminal_input_allp")
	k_event(self, "input_alle", "terminal_input_alle")
	k_event(self, "input_allm", "terminal_input_allm")
	k_event(self, "input_allg", "terminal_input_allg")
	k_event(self, "input_dump", "terminal_input_dump")
	k_event(self, "input_clear_comments", "terminal_input_clear_comments")
	return 1
end

--Close terminal.
function terminal_input_close(self)
	k_delete(self)
	k_remove_global("terminal")
	return 1
end

--Exit program.
function terminal_input_exit(self)
	k_exit()
	return 1
end

--Display response information.
function terminal_response(self, info)
	self:Response(info, "white")
	return true
end

--Search object.
function get_object(obj_name)
	local id = k_global(obj_name)
	
	if id ~= nil then
		return id
	end
	
	id = k_find_object(obj_name)
	
	--if not k_uniqid_null(id) then
	if id ~= k_uniqid() then
		return id
	end
	
	id = k_uniqid(obj_name)
	
	if k_exists(id) then
		return id
	end
	
	return nil
end

--Convert to suitable type.
function get_new_value(old_v, value)
	local type = k_type(old_v)
	
	if type == "boolean" then
		return k_boolean(value)
	elseif type == "number" then
		return k_number(value)
	elseif type == "string" then
		return k_string(value)
	elseif type == "int32" then
		return k_int32(value)
	elseif type == "int64" then
		return k_int64(value)
	elseif type == "float" then
		return k_float(value)
	elseif type == "double" then
		return k_double(value)
	elseif type == "uniqid" then
		return k_uniqid(value)
	end
		
	return nil	
end

--Get internal property of object.
function terminal_input_getp(self, obj_name, prop)
	if obj_name == nil or prop == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local value = k_property(obj, k_string(prop))
	
	terminal_response(self, "object " .. k_class_name(obj) .. " property " 
		.. k_string(prop) .. " value is " .. k_string(value))
	return 1
end

--Set internal property of object.
function terminal_input_setp(self, obj_name, prop, value)
	if k_global("exiting") == true then
		--In exiting progress, can't process this command.
		return 0
	end
	
	if obj_name == nil or prop == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local old_v = k_property(obj, k_string(prop))
	local new_v = get_new_value(old_v, value)
	
	if not k_set_property(obj, k_string(prop), new_v) then
		terminal_response(self, "set property failed")
		return 0
	end
	
	new_v = k_property(obj, k_string(prop))
	terminal_response(self, "set object " .. k_class_name(obj) .. " property " 
		.. k_string(prop) .. " value " .. k_string(new_v))
	return 1
end

--Get extra property of object.
function terminal_input_gete(self, obj_name, prop)
	if obj_name == nil or prop == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local value = k_extra(obj, k_string(prop))
	
	terminal_response(self, "object " .. k_class_name(obj) .. " custom " 
		.. k_string(prop) .. " value is " .. k_string(value))
	return 1
end

--Set extra property of object.
function terminal_input_sete(self, obj_name, prop, value)
	if k_global("exiting") == true then
		--In exiting progress, can't process this command.
		return 0
	end
	
	if obj_name == nil or prop == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local old_v = k_extra(obj, k_string(prop))
	local new_v = get_new_value(old_v, value)
	
	if not k_set_extra(obj, k_string(prop), new_v) then
		terminal_response(self, "set extra failed")
		return 0
	end
	
	new_v = k_extra(obj, k_string(prop))
	terminal_response(self, "set object " .. k_class_name(obj) .. " extra " 
		.. k_string(prop) .. " value " .. k_string(new_v))
	return 1
end

--Run method of object.
function terminal_input_runm(self, obj_name, method, ...)
	if k_global("exiting") == true then
		--In exiting progress, can't process this command.
		return 0
	end
	
	if obj_name == nil or method == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local res = k_method(obj, k_string(method), unpack(arg))
	
	if type(res) == "table" then
		terminal_response(self, "call object " .. k_class_name(obj) .. " method " 
			.. k_string(method) .. " return table")
		
		for i = 1, #res do
			terminal_response(self, "table value " .. k_string(i) .. ": " .. k_string(res[i]))
		end
	else
		terminal_response(self, "call object " .. k_class_name(obj) .. " method " 
			.. k_string(method) .. " return " .. k_string(res))
	end
	
	return 1
end

--List all internal properties of object.
function terminal_input_allp(self, obj_name)
	if obj_name == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local prop_table = k_property_list(obj)
	local num = #prop_table
	
	terminal_response(self, "object " .. k_class_name(obj) .. " property number:" .. k_string(num))
	
	for i = 1, num do
		local name = prop_table[i]
		local value = k_property(obj, name)

		terminal_response(self, k_string(name) .. "[" .. k_type(value) .. "]:" .. k_string(value))
	end
	
	return 1
end

--List all extra properties of object.
function terminal_input_alle(self, obj_name)
	if obj_name == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local extra_table = k_extra_list(obj)
	local num = #extra_table
	
	terminal_response(self, "object " .. k_class_name(obj) .. " extra number:" .. k_string(num))
	
	for i = 1, num do
		local name = extra_table[i]
		local value = k_extra(obj, name)

		terminal_response(self, k_string(name) .. "[" .. k_type(value) .. "]:" .. k_string(value))
	end
	
	return 1
end

--List all methods of object.
function terminal_input_allm(self, obj_name)
	if obj_name == nil then
		return 0
	end
	
	local obj = get_object(k_string(obj_name))
	
	if obj == nil then
		return 0
	end
	
	local method_table = k_method_list(obj)
	local num = #method_table
	
	terminal_response(self, "object " .. k_class_name(obj) .. " method number:" .. k_string(num))
	
	for i = 1, num do
		terminal_response(self, k_string(method_table[i]))
	end
	
	return 1
end

--List all global variables.
function terminal_input_allg(self)
	local global_table = k_global_list()
	local global_num = #global_table
	
	terminal_response(self, "global number is " .. k_string(global_num))
	
	for i = 1, global_num do
		local name = global_table[i]
		local value = k_global(name)
		
		terminal_response(self, k_string(name) .. "[" .. k_type(value) .. "]:" .. k_string(value))
	end
	
	return 1
end

--Dump informations.
function terminal_input_dump(self, dump_type, file_name)
	if dump_type == nil then
		return 0
	end

	if file_name == nil  then
		file_name = dump_type .. ".txt"
	end

	local fm = k_single("FrameMisc")

	if fm:DumpInformation(dump_type, file_name) then
		terminal_response(self, "dump to file:" .. file_name)
	else
		terminal_response(self, "dump failed")
	end

	return 1
end

--Clear c/c++ comments.
function terminal_input_clear_comments(self, src_name, dst_name)
	local finder = k_create("FileFinder")
	
	if dst_name == nil then
		local file_count = 0
		local dir_name = k_string(src_name) .. "/"

		finder:SearchFile(dir_name, "*.h")

		local header_table = finder:GetFileList()

		for i = 1, #header_table do
			local header = dir_name .. header_table[i]
			local count = finder:ClearComments(header, header)
	
			terminal_response(self, "clear comments:" .. header .. " return:" .. k_string(count))
			file_count = file_count + 1
		end

		finder:SearchFile(dir_name, "*.cpp")

		local source_table = finder:GetFileList()

		for i = 1, #source_table do
			local source = dir_name .. source_table[i]
			local count = finder:ClearComments(source, source)
	
			terminal_response(self, "clear comments:" .. source .. " return:" .. k_string(count))
			file_count = file_count + 1
		end

		terminal_response(self, "total " .. k_string(file_count) .. " files processed")
	else
		local count = finder:ClearComments(k_string(src_name), k_string(dst_name))
	
		terminal_response(self, "clear comments:" .. src_name .. " return:" .. k_string(count))
	end

	k_delete(finder)
	return 1
end
