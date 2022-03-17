--Client main script.

--Entry point of script.
function main()
	local window = k_main_object()
	local init_width = window:GetInitialWidth()
	local init_height = window:GetInitialHeight()

	window.Width = init_width
	window.Height = init_height
	window:StartRender()

	--Create ui object.
	local ui = k_create("FlatUI")

	k_bind(ui, k_script_file(), "flat_ui_init")
	window:SetUIObject(ui)

	--Create defult font.
	local font = k_create("FlatFont")

	font.Name = "default"
	font.Resource = "font/simsun.ttc"
	font.CharHeight = 24
	font.Interval = 8
	ui:AddFont(font)
	ui:SetCurrentFont("default")

	--Create top dialog.
	local form = ui:Create("FlatForm")

	--form.Transparent = true
	form.Name = "test_form"
	form.Left = 0
	form.Top = 0
	form.Width = init_width
	form.Height = init_height
	form.BackImage = "png/tools.png"
	form:ShowDialog()

	--Test button.
	local btn = ui:Create("FlatButton")

	btn.Name = "test_button"
	btn.Left = 20
	btn.Top = 10
	btn.Width = 200
	btn.Height = 40
	btn.Text = "Button"
	btn.ForeColor = "200,200,200,255"
	btn.BackColor = "120,120,120,255"
	btn.Hint = "Click button"
	form:AddChild(btn)

	--Test label.
	local lbl = ui:Create("FlatLabel")

	lbl.Name = "test_label"
	lbl.Left = 20
	lbl.Top = 100
	lbl.Width = 200
	lbl.Height = 50
	lbl.Text = "Label"
	lbl.Align = "Center"
	lbl.Background = true
	lbl.Border = true
	lbl.ForeColor = "255,255,0,255"
	lbl.BackColor = "120,120,120,255"
	form:AddChild(lbl)

	--Test progress bar.
	local pb = ui:Create("FlatProgressBar")

	pb.Name = "test_progress_bar"
	pb.Left = 20
	pb.Top = 200
	pb.Width = 200
	pb.Height = 8
	pb.Minimum = 0
	pb.Maximum = 100
	pb.Value = 50
	form:AddChild(pb)

	--Test check box.
	local cb = ui:Create("FlatCheckBox")

	cb.Name = "test_check_box"
	cb.Left = 20
	cb.Top = 300
	cb.Width = 200
	cb.Height = 30
	cb.Text = "check box"
	form:AddChild(cb)

	--Test radio button.
	local rb1 = ui:Create("FlatRadioButton")
	local rb2 = ui:Create("FlatRadioButton")

	rb1.Name = "test_radio_button_1"
	rb1.Left = 20
	rb1.Top = 400
	rb1.Width = 100
	rb1.Height = 30
	rb1.Text = "rb1"
	form:AddChild(rb1)
	rb2.Name = "test_radio_button_2"
	rb2.Left = 120
	rb2.Top = 400
	rb2.Width = 100
	rb2.Height = 30
	rb2.Text = "rb2"
	form:AddChild(rb2)

	--Test track bar.
	local track = ui:Create("FlatTrackBar")

	track.Name = "test_track_bar"
	track.Left = 20
	track.Top = 500
	track.Width = 200
	track.Height = 80
	track.Minimum = 0
	track.Maximum = 100
	track.Value = 50
	track.VerticalMinimum = 0
	track.VerticalMaximum = 50
	track.VerticalValue = 20
	form:AddChild(track)

	--Test text box.
	local tb = ui:Create("FlatTextBox")

	tb.Name = "test_text_box"
	tb.Left = 20
	tb.Top = 600
	tb.Width = 200
	tb.Height = 40
	tb.Text = "text"
	tb.MaxLength = 20
	tb.TabIndex = 1
	tb.ContextMenu = "TextBox"
	--tb.PasswordChar = "*"
	form:AddChild(tb)

	--Test numeric.
	local nm = ui:Create("FlatNumeric")

	nm.Name = "test_numeric"
	nm.Left = 20
	nm.Top = 700
	nm.Width = 200
	nm.Height = 40
	nm.Text = "100.0"
	nm.MaxLength = 10
	nm.TabIndex = 2
	nm.SmallStep = 1
	nm.LargeStep = 10
	form:AddChild(nm)

	--Test group box.
	local gb = ui:Create("FlatGroupBox")

	gb.Name = "test_group_box"
	gb.Left = 300
	gb.Top = 20
	gb.Width = 200
	gb.Height = 200
	gb.Text = "GB"
	gb.Hint = "Group Box"
	form:AddChild(gb)

	--Test horizontal scroll bar.
	local hsb = ui:Create("FlatHScrollBar")

	hsb.Name = "test_h_scroll_bar"
	hsb.Left = 300
	hsb.Top = 300
	hsb.Width = 200
	hsb.Height = 20
	form:AddChild(hsb)

	--Test list box.
	local lb = ui:Create("FlatListBox")

	lb.Name = "test_list_box"
	lb.Left = 300
	lb.Top = 350
	lb.Width = 200
	lb.Height = 200
	lb.Sorted = true
	lb.DynamicSlide = true
	lb.ItemHeight = 24
	lb.HScrollExists = true
	lb.VScrollBar.UpButton.BackImage = "png/up24x24_0.png"
	lb.VScrollBar.UpButton.CaptureImage = "png/up24x24_1.png"
	lb.VScrollBar.UpButton.PressImage = "png/up24x24_2.png"
	--lb.VScrollBar.__dont_save = true
	--lb.CrossLabel.__dont_save = true
	lb:AddText("list item 1")
	lb:AddText("list ddfff 2")
	lb:AddText("sssss ddfff 2")
	lb:AddText("sdASDas ddfff 2")
	lb:AddText("lxxcccxf 2")
	lb:AddText("list ddfff 2")
	lb:AddText("xcZxcZXcZxff 2")
	lb:AddText("xZxxwwfff")
	lb:AddText("gggghsasdfa")
	lb:AddText("ttasdfaeeeeee")
	form:AddChild(lb)

	--Test combo box.
	local cbx = ui:Create("FlatComboBox")

	cbx.Name = "test_combo_box"
	cbx.Left = 300
	cbx.Top = 600
	cbx.Width = 200
	cbx.Height = 40
	cbx.DropList.ItemHeight = 32
	cbx.DropList:AddText("select0")
	cbx.DropList:AddText("select1")
	cbx.DropList:AddText("select2")
	cbx.DropList:AddText("select3")
	form:AddChild(cbx)

	--Test panel.
	local panel = ui:Create("FlatPanel")

	panel.Name = "test_panel"
	panel.Left = 600
	panel.Top = 20
	panel.Width = 200
	panel.Height = 200
	panel.Text = "Panel"
	panel.ScrollSize = 12
	panel.DynamicSlide = true
	panel:SetContentSize(300, 300)
	form:AddChild(panel)

	--Test tree view.
	local tv = ui:Create("FlatTreeView")

	tv.Name = "test_tree_view"
	tv.Left = 600
	tv.Top = 300
	tv.Width = 200
	tv.Height = 200
	tv.ItemHeight = 24
	--tv.HideRoot = true
	tv.Sorted = true
	tv.ShowPlusMinus = true
	tv.ScrollSize = 16
	tv.DynamicSlide = true
	tv:CreateRoot("root", true)
	tv:AddText("child1dd", true)
	tv:AddText("childffd1sdd", true)
	tv:AddText("childddfd1daad", true)
	tv:AddText("child1asdfaddsdd", true)
	tv:AddText("child1ddfasd", true)
	tv:AddText("chilasdaafasdd1dd", true)
	tv:AddText("chasdasild1dd", true)
	tv:AddText("chddild1ddfffffd", true)
	tv:AddText("chdasdfd1dd", true)
	tv:AddText("adsdfasdd1dd", true)
	tv:AddText("cwasdfafqhiladd1dd", true)
	tv:AddText("ddggggggggcd", true)
	tv:SelectNull()
	form:AddChild(tv)

	--Test data grid.
	local dg = ui:Create("FlatDataGrid")

	dg.Name = "test_data_grid"
	dg.Left = 600
	dg.Top = 520
	dg.Width = 200
	dg.Height = 200
	dg.ScrollSize = 16
	dg.DynamicSlide = true
	dg.RowHeadersVisible = true
	dg.ColumnHeadersVisible = true
	dg.TextAlign = "Center"
	dg.RowHeight = 30
	dg.ColumnWidth = 80
	dg.ColumnCount = 8
	dg.RowCount = 10
	dg.RowSelectable = true
	dg.ColumnSelectable = true
	dg:SetRowHeaderText(0, "r0")
	dg:SetRowHeaderText(1, "r1")
	dg:SetRowHeaderText(2, "r2")
	dg:SetRowHeaderText(3, "r3")
	dg:SetRowHeaderText(4, "r4")
	dg:SetRowHeaderText(5, "r5")
	dg:SetRowHeaderText(6, "r6")
	dg:SetRowHeaderText(7, "r7")
	dg:SetRowHeaderText(8, "r8")
	dg:SetRowHeaderText(9, "r9")
	dg:SetColumnHeaderText(0, "c0")
	dg:SetColumnHeaderText(1, "c1")
	dg:SetColumnHeaderText(2, "c2")
	dg:SetColumnHeaderText(3, "c3")
	dg:SetColumnHeaderText(4, "c4")
	dg:SetColumnHeaderText(5, "c5")
	dg:SetColumnHeaderText(6, "c6")
	dg:SetColumnHeaderText(7, "c7")
	dg:SetGridText(0, 0, "g00")
	dg:SetGridText(1, 1, "g11")
	--dg:SelectGrid(0, 0)
	form:AddChild(dg)

	--Test list view.
	local lv = ui:Create("FlatListView")

	lv.Name = "test_list_view"
	lv.Left = 900
	lv.Top = 50
	lv.Width = 300
	lv.Height = 300
	lv.ItemWidth = 100
	lv.ItemHeight = 100
	lv.GlyphWidth = 64
	lv.GlyphHeight = 64
	lv.Sorted = true
	lv.DynamicSlide = true
	lv:AddItem("listem", "png/tools.png")
	lv:AddItem("adaim", "png/tools.png")
	lv:AddItem("lissam", "png/tools.png")
	lv:AddItem("asddem", "png/tools.png")
	lv:AddItem("lasdem", "png/tools.png")
	lv:AddItem("liaem", "png/tools.png")
	lv:AddItem("feeee", "png/tools.png")
	lv:AddItem("feee", "png/tools.png")
	lv:AddItem("fe", "png/tools.png")
	lv:AddItem("ase", "png/tools.png")
	lv:AddItem("fse", "png/tools.png")
	form:AddChild(lv)

	--Test main menu.
	local mm = ui:Create("FlatMainMenu")

	mm.Name = "test_main_menu"
	mm.Left = 900
	mm.Top = 400
	mm.Width = 200
	mm.Height = 30
	
	local fm = mm:CreateMenu("file", "File", 80)

	fm:AddStrip("open", "Open")
	fm:AddStrip("s1", "-")
	fm:AddStrip("close", "Close")

	local em = mm:CreateMenu("edit", "Edit", 80)

	em:AddStrip("copy", "Copy")
	
	form:AddChild(mm)

	--Test cursor.
	local cur = k_create("FlatCursor")

	cur.Name = "default"
	cur.Resource = "IDC_ARROW"
	cur:Load()
	ui:AddCursor(cur)
	ui:SetCurrentCursor("default")
	--ui:HideCurrentCursor()

	--Test hint label.
	local hl = ui:Create("FlatLabel")

	hl.Name = "hint_label"
	hl.Font = "default"
	hl.Left = 0
	hl.Top = 0
	hl.Width = 200
	hl.Height = 30
	hl.Text = "hint"
	hl.Background = true
	hl.Border = true
	hl.ForeColor = "50,50,50,255"
	hl.BackColor = "220,220,220,255"
	ui:AddHintLabel(hl)

	--Test menu.
	local cm = ui:Create("FlatMenu")

	cm.Name = "TextBox"
	cm.Left = 0
	cm.Top = 0
	cm.Width = 100
	cm.Height = 50
	cm.StripHeight = 30
	cm.ForeColor = "50,50,50,255"
	cm.BackColor = "220,220,220,255"
	cm:AddStrip("cut", "Cut")
	cm:AddStrip("s1", "-")
	cm:AddStrip("copy", "Copy")
	cm:AddStrip("s2", "-")
	cm:AddStrip("paste", "Paste")
	k_bind(cm, k_script_file(), "cm_text_box_init")
	ui:AddContextMenu(cm)
	--ui:AddFloating(cm)

	ui:SaveForm(form, "form.json")

	local form1 = ui:LoadDesignForm("form.json")

	local jd = k_create("JsonDoc")

	jd:LoadFile("test.json")

	if not jd:FindKey("thisjj") then
		jd:AddDouble("thisjj", 3444.98921)
	end

	jd:SaveFile("test1.json")
	--k_message(jd:SaveCompactString())
	--k_message(jd:ReadKey("t2221", "def_value"))
	k_delete(jd)
	return 1
end

--Initialize flat user interface.
function flat_ui_init(self)
	k_event(self, "close", "flat_ui_close")
	return 1
end

--Window close.
function flat_ui_close(self)
	k_exit()
	return 1
end

--Initialize text-box context menu.
function cm_text_box_init(self)
	k_event(self, "cut", "cm_text_box_cut")
	k_event(self, "copy", "cm_text_box_copy")
	k_event(self, "paste", "cm_text_box_paste")
	return 1
end

--Text-box context menu cut.
function cm_text_box_cut(self)
	self.Owner:Cut()
	return 1
end

--Text-box context menu copy.
function cm_text_box_copy(self)
	self.Owner:Copy()
	return 1
end

--Text-box context menu paste.
function cm_text_box_paste(self)
	self.Owner:Paste()
	return 1
end

--Open terminal.
function open_terminal()
	if not k_find_class("WinTerminal") then
		return false
	end

	local terminal = k_create("WinTerminal")

	k_set_global("terminal", terminal)
	k_bind(terminal, k_script_file(), "terminal_init")
	return true
end

--Close terminal.
function close_terminal()
	local terminal = k_global("terminal")

	k_delete(terminal)
	k_remove_global("terminal")
	return true
end

--Initialize terminal.
function terminal_init(self)
	k_event(self, "input_close", "terminal_input_close")
	k_event(self, "input_exit", "terminal_input_exit")
	k_event(self, "input_getp", "terminal_input_getp")
	k_event(self, "input_setp", "terminal_input_setp")
	k_event(self, "input_gete", "terminal_input_gete")
	k_event(self, "input_sete", "terminal_input_sete")
	k_event(self, "input_method", "terminal_input_runm")
	k_event(self, "input_listp", "terminal_input_allp")
	k_event(self, "input_liste", "terminal_input_alle")
	k_event(self, "input_listm", "terminal_input_allm")
	k_event(self, "input_listg", "terminal_input_allg")
	return 1
end

--Close terminal.
function terminal_input_close(self)
	close_terminal()
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
		terminal_response(self, "run object " .. k_class_name(obj) .. " method " 
			.. k_string(method) .. " return table")
		
		for i = 1, #res do
			terminal_response(self, "table value " .. k_string(i) .. ": " .. k_string(res[i]))
		end
	else
		terminal_response(self, "run object " .. k_class_name(obj) .. " method " 
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
	
	terminal_response(self, "object " .. k_class_name(obj) .. " property number is " .. k_string(num))
	
	for i = 1, num do
		terminal_response(self, k_string(prop_table[i]))
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
	
	terminal_response(self, "object " .. k_class_name(obj) .. " extra number is " .. k_string(num))
	
	for i = 1, num do
		terminal_response(self, k_string(extra_table[i]))
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
	
	terminal_response(self, "object " .. k_class_name(obj) .. " method number is " .. k_string(num))
	
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
		
		terminal_response(self, k_string(name) .. "[" .. k_type(value) .. "]: " .. k_string(value))
	end
	
	return 1
end
