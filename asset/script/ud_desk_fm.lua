--Desk form script.
require("ud_widgets")

--Create designer desk form.
function create_desk_fm(ui, left, top, width, height)
    local ud = k_global("ud")
	--Create top dialog.
	local desk_fm = ui:Create("FlatForm")

	desk_fm.Name = "desk_fm"
	desk_fm.Left = left
	desk_fm.Top = top
	desk_fm.Width = width
	desk_fm.Height = height
	desk_fm:FitSafeArea()
	desk_fm:ShowDialog()
    ud.desk_fm = desk_fm

	--File menu.
	local file_mn = k_run_routine("ud_file_mn", "create_file_mn", ui)

	desk_fm.file_mn = file_mn
	k_run_routine("ud_file_mn", "clear_need_save")

	--File button.
	local file_bn = ui:Create("FlatButton")

	file_bn.Name = "file_bn"
	file_bn.Left = 0
	file_bn.Top = 0
	file_bn.Width = 80
	file_bn.Height = 30
	file_bn.Text = "File"
	desk_fm:AddChild(file_bn)
	desk_fm.file_bn = file_bn

	--Widget list view.
	local tool_width = 400
	local widget_lv = ui:Create("FlatListView")

	widget_lv.Name = "widget_lv"
	widget_lv.Left = 0
	widget_lv.Top = file_bn.Bottom
	widget_lv.Width = tool_width
	widget_lv.Height = 200
	widget_lv.ItemWidth = tool_width
	widget_lv.ItemHeight = 64
	widget_lv.GlyphWidth = 64
	widget_lv.GlyphHeight = 64
	widget_lv.Sorted = true
	widget_lv.RightText = true
	widget_lv.DragEnable = true
	widget_lv.DynamicSlide = true
	widget_lv.Hint = "@ud_hint@Drag and drop to target"
	desk_fm:AddChild(widget_lv)
	desk_fm.widget_lv = widget_lv

	--Add widget list.
	for i = 1, #widget_list do
		local wg = widget_list[i]

		if k_find_class(wg.widget) then
			widget_lv:AddItem(wg.name, wg.image)
			--local index = widget_lv:AddItem(wg.name, wg.image)
			--widget_lv:SetTag(index, wg.widget)
		end
	end

	--Hierarchy tree view.
	local hierarchy_tv = ui:Create("FlatTreeView")

	hierarchy_tv.Name = "hierarchy_tv"
	hierarchy_tv.Left = 0
	hierarchy_tv.Top = widget_lv.Bottom
	hierarchy_tv.Width = tool_width
	hierarchy_tv.Height = 200
	hierarchy_tv.ItemHeight = 24
	hierarchy_tv.HideRoot = false
	hierarchy_tv.Sorted = true
	hierarchy_tv.ShowPlusMinus = true
	hierarchy_tv.ScrollSize = 16
	hierarchy_tv.DynamicSlide = true
	hierarchy_tv:CreateRoot("root", true)
	hierarchy_tv:SelectNull()
	desk_fm:AddChild(hierarchy_tv)
	desk_fm.hierarchy_tv = hierarchy_tv

	--Property data grid.
	local property_dg = ui:Create("FlatDataGrid")

	property_dg.Name = "property_dg"
	property_dg.Left = 0
	property_dg.Top = hierarchy_tv.Bottom
	property_dg.Width = tool_width
	property_dg.Height = desk_fm.Height - property_dg.Top
	property_dg.ScrollSize = 16
	property_dg.DynamicSlide = true
	property_dg.RowHeadersVisible = false
    property_dg.ColumnHeadersVisible = true
	property_dg.TextAlign = "Center"
	property_dg.RowHeight = 30
	property_dg.ColumnWidth = 190
	property_dg.ColumnCount = 2
	property_dg.RowSelectable = true
	property_dg.ColumnSelectable = false
	property_dg:SetColumnHeaderText(0, "name")
	property_dg:SetColumnHeaderText(1, "value")
	desk_fm:AddChild(property_dg)
	desk_fm.property_dg = property_dg

	--Work panel.
	local work_pn = ui:Create("FlatPanel")

	work_pn.Name = "work_pn"
	work_pn.Left = tool_width
	work_pn.Top = file_bn.Bottom
	work_pn.Width = desk_fm.Width - work_pn.Left
	work_pn.Height = desk_fm.Height - work_pn.Top
	work_pn.Text = "Panel"
	work_pn.ScrollSize = 12
	work_pn.DynamicSlide = true
	work_pn:SetContentSize(1920, 1080)
	desk_fm:AddChild(work_pn)
	desk_fm.work_pn = work_pn

	--Desk form bind script.
	k_bind(desk_fm, "ud_desk_fm", "desk_fm_init")
	--Create new form.
	k_run_routine("ud_file_mn", "new_design_form")
	return 1
end

--Initialize desk form.
function desk_fm_init(self)
    local file_bn = self.file_bn
    local widget_lv = self.widget_lv
    local hierarchy_tv = self.hierarchy_tv
    local property_dg = self.property_dg

    k_bind(file_bn, "ud_desk_fm")
    k_bind(widget_lv, "ud_desk_fm")
    k_bind(hierarchy_tv, "ud_desk_fm")
    k_bind(property_dg, "ud_desk_fm")
    k_event(file_bn, "click", "file_bn_click")
    k_event(widget_lv, "drag_begin", "widget_lv_drag_begin")
    k_event(widget_lv, "drag_move", "widget_lv_drag_move")
    k_event(widget_lv, "drag_end", "widget_lv_drag_end")
    k_event(hierarchy_tv, "select", "hierarchy_tv_select")
    k_event(property_dg, "select_row", "property_dg_select_row")
    return 1
end

--File button click.
function file_bn_click(self)
	local ud = k_global("ud")
    local ui = ud.ui
	local file_mn = ud.desk_fm.file_mn

	file_mn.Left = self.GlobalLeft
	file_mn.Top = self.GlobalTop + self.Height
	ui:AddFloating(file_mn)
	return 1
end

--Widget list view drag begin.
function widget_lv_drag_begin(self, x, y)
    local ud = k_global("ud")
    local ui = ud.ui

    if k_find_extra(ud, "drag_lb") then
        local old_lb = ud.drag_lb

        if k_exists(old_lb) then
            ui:Delete(old_lb)
        end
    end

	local index = self.SelectedIndex

	if index < 0 then
		return 0
	end

	local image = "png/widget.png"
	local name = self:GetText(index)

	for i = 1, #widget_list do
		local wg = widget_list[i]

		if wg.name == name then
			image = wg.image
			break
		end
	end

    local lb = ui:Create("FlatLabel")

    lb.Left = x
    lb.Top = y
    lb.Width = 64
    lb.Height = 64
    lb.Background = true
    lb.BackImage = image
    ui:AddFloating(lb)
    ud.drag_lb = lb
    return 1
end

--Widget list view drag moving.
function widget_lv_drag_move(self, x, y)
    local ud = k_global("ud")
    local lb = ud.drag_lb

    lb.Left = x
    lb.Top = y
    return 1
end

--Get available widget name.
function new_widget_name(fm, name)
    local i = 1
    local widget_name = name .. k_string(i)

    while k_find_extra(fm, widget_name) do
        i = i + 1
        widget_name = name .. k_string(i)
    end

    return widget_name
end

--Add widget to container.
function add_widget(container, widget, data_name, x, y)
	local ud = k_global("ud")
	local design_fm = ud.design_fm
	local widget_name = new_widget_name(design_fm, string.lower(data_name))

    widget.Left = x - container.GlobalLeft
    widget.Top = y - container.GlobalTop
    widget.Name = widget_name
    widget.DesignMode = true

    if k_find_property(widget, "Text") then
        widget.Text = widget_name
    end

    container:AddChild(widget)
	k_set_extra(design_fm, widget_name, widget)
	show_hierarchy()
	set_need_save()
	return 1
end

--Widget list view drag end.
function widget_lv_drag_end(self, x, y, target)
    local ud = k_global("ud")
    local ui = ud.ui
    local lb = ud.drag_lb

    ui:RemoveFloating(lb)
    ui:Delete(lb)

    if not k_exists(target) then
        return 0
    end

    local target_host = target.Delegator

    if not k_exists(target_host) then
        target_host = target
    end

    if not target_host.DesignMode then
        return 0
    end

    if k_class_name(target_host) == "FlatPicker" then
		target_host = target_host.Target

		if target_host:IsContainer() then
			target_host = target_host:GetInRegion(x, y)
		end
    end

    local design_fm = ud.design_fm
    local container

    --if k_uniqid_equal(design_fm, target_host) then
    if design_fm == target_host then
        container = target_host
    else
        if target_host:IsContainer() then
            container = target_host
        else
            container = target:FindForm()
            
            --if not k_uniqid_equal(design_fm, container) then
            if design_fm ~= container then
                return 0
            end
        end
	end
	
	local index = self.SelectedIndex

	if index < 0 then
		return 0
	end

	local name = self:GetText(index)

	for i = 1, #widget_list do
		local wg = widget_list[i]

		if wg.name == name then
			local widget = ui:Create(wg.widget)
	
			widget.Width = wg.width
			widget.Height = wg.height
			add_widget(container, widget, wg.name, x, y)
			break
		end
	end

    return 1
end

--Show hierarchy of widget.
function show_widget_hierarchy(ui, hierarchy_tv, widget)
	local meta = ui:GetMeta(widget)
	local part_num = meta:GetPartCount()

	for i = 0, part_num - 1 do
		local part_name = meta:GetPartName(i)

		if k_find_property(widget, part_name) then
			local part_id = k_property(widget, part_name)

			if k_exists(part_id) then
				local ident = hierarchy_tv:GetIdentity()

				hierarchy_tv:AddText(part_name, true)	
				hierarchy_tv:SetTag("P" .. k_string(part_id))
				hierarchy_tv:SelectIdentity(ident)
			end
		end
	end

	if widget:IsContainer() then
		local child_list = widget:GetChildList()

		for i = 1, #child_list do
			local child = child_list[i]
			local ident = hierarchy_tv:GetIdentity()

			hierarchy_tv:AddText(child.Name, true)
			hierarchy_tv:SetTag("C" .. k_string(child))
			show_widget_hierarchy(ui, hierarchy_tv, child)
			hierarchy_tv:SelectIdentity(ident)
		end
	end

	return 1
end

--Show hierarchy of design form.
function show_hierarchy()
	local ud = k_global("ud")
	local ui = ud.ui
	local design_fm = ud.design_fm
	local hierarchy_tv = ud.desk_fm.hierarchy_tv

	hierarchy_tv:BeginUpdate()
	hierarchy_tv:Clear()
	hierarchy_tv:CreateRoot(design_fm.Name, true)
	hierarchy_tv:SetTag("C" .. k_string(design_fm))
	show_widget_hierarchy(ui, hierarchy_tv, design_fm)
	hierarchy_tv:SelectRoot()
	hierarchy_tv:EndUpdate()
	return 1
end

--Show editable property of part widget.
function show_part_property(part_widget)
	local ud = k_global("ud")
	local ui = ud.ui
	local meta = ui:GetMeta(part_widget)
	local prop_num = meta:GetPropertyCount()
	local part_prop_num = 0

	for i = 0, prop_num - 1 do
		if meta:GetPropertyPart(i) then
			part_prop_num = part_prop_num + 1
		end
	end

	edit_property_ended()

	local prop_dg = ud.desk_fm.property_dg

	prop_dg:BeginUpdate()
	prop_dg.RowCount = part_prop_num

	local row = 0

	for i = 0, prop_num - 1 do
		if meta:GetPropertyPart(i) then
			local prop_name = meta:GetPropertyName(i)
			local prop_type = meta:GetPropertyType(i)
			local prop_value = k_property(part_widget, prop_name)

			prop_dg:SetGridText(row, 0, prop_name)
			prop_dg:SetGridText(row, 1, k_string(prop_value))
			row = row + 1
		end
	end

	prop_dg:EndUpdate()
	prop_dg:SelectRow(-1)
	prop_dg.target = part_widget
	prop_dg.edit_row = -1
	return 1
end

--Show editable property of widget.
function show_property(widget)
	local ud = k_global("ud")
	local ui = ud.ui
	local meta = ui:GetMeta(widget)
	local prop_num = meta:GetPropertyCount()
	local prop_dg = ud.desk_fm.property_dg

	edit_property_ended()

	prop_dg:BeginUpdate()
	prop_dg.RowCount = prop_num + 1
	prop_dg:SetGridText(0, 0, "Name")
	prop_dg:SetGridText(0, 1, widget.Name)

	for i = 0, prop_num - 1 do
		local prop_name = meta:GetPropertyName(i)
		local prop_type = meta:GetPropertyType(i)
		local prop_value = k_property(widget, prop_name)

		prop_dg:SetGridText(i + 1, 0, prop_name)
		prop_dg:SetGridText(i + 1, 1, k_string(prop_value))
	end

	prop_dg:EndUpdate()
	prop_dg:SelectRow(-1)
	prop_dg.target = widget
	prop_dg.edit_row = -1
	return 1
end

--Pick widget.
function pick_widget(widget)
	local ud = k_global("ud")
	local ui = ud.ui
	local picker = ud.picker

	if not k_exists(picker) then
		picker = k_run_routine("ud_picker", "create_picker", ui)
		ud.desk_fm.work_pn:AddChild(picker)
		ud.picker = picker
	end

	picker.GlobalLeft = widget.GlobalLeft
	picker.GlobalTop = widget.GlobalTop
	picker.Width = widget.Width
	picker.Height = widget.Height
	picker.Target = widget
	picker:BringToFront()

	local hierarchy_tv = ud.desk_fm.hierarchy_tv
	local ident = hierarchy_tv:FindTag("C" .. k_string(widget))

	if ident >= 0 then
		hierarchy_tv:SelectIdentity(ident)
	end

	show_property(widget)
	return 1
end

--Hierarchy tree view select node.
function hierarchy_tv_select(self, old_identity)
	local tag = self:GetTag()
	local widget_type = string.sub(tag, 1, 1)
	local widget_id = string.sub(tag, 2)
	local widget = k_uniqid(widget_id)

	if not k_exists(widget) then
		return 0
	end

	if widget_type == "C" then
		--show_property(widget)
		pick_widget(widget)
	elseif widget_type == "P" then
		show_part_property(widget)
	end

	return 1
end

--Edit property ended.
function edit_property_ended()
	local ud = k_global("ud")
	local ui = ud.ui
	local prop_dg = ud.desk_fm.property_dg

	if not k_find_extra(prop_dg, "target") then
		return 0
	end

	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row

	if edit_row < 0 then
		return 0
	end

	local prop_name = prop_dg:GetGridText(edit_row, 0)
	local prop_value = prop_dg:GetGridText(edit_row, 1)
	local edit_wg = prop_dg:GetGridWidget(edit_row, 1)

	if k_exists(edit_wg) then
		prop_value = edit_wg.Text
		ui:Delete(edit_wg)
	end

	set_widget_property(target, prop_name, prop_value)
	prop_dg:SetGridText(edit_row, 1, k_string(k_property(target, prop_name)))
	prop_dg.edit_row = -1
	return 1
end

--Property data grid select row.
function property_dg_select_row(self, old_row)
	local ud = k_global("ud")
	local ui = ud.ui
	local target = self.target
	local cur_row = self.CurrentRow

	if not k_exists(target) then
		return 0
	end

	edit_property_ended()

	if cur_row < 0 then
		return 0
	end

	local prop_name = self:GetGridText(cur_row, 0)
	local prop_text = self:GetGridText(cur_row, 1)

	if prop_name == "Name" then
		local widget = ui:Create("FlatTextBox")

		widget.Text = prop_text
		self:SetGridWidget(cur_row, 1, widget)
		self.edit_row = cur_row
		ui:FocusTo(widget)
		return 1
	end

	local meta = ui:GetMeta(target)
	local prop_type = meta:FindPropertyType(prop_name)

	if prop_type == "" then
		return 0
	end

	local widget = k_uniqid()

	if prop_name == "ImageLayout" then
		widget = ui:Create("FlatComboBox")
		widget.Text = prop_text
		widget.InputBox.ReadOnly = true
		widget.DropList:AddText("fit")
		widget.DropList:AddText("9patch")
		widget.DropList:AddText("v3patch")
		widget.DropList:AddText("h3patch")
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "select", "edit_string_cx_select")
	elseif prop_name == "TextAlign" then
		widget = ui:Create("FlatComboBox")
		widget.Text = prop_text
		widget.InputBox.ReadOnly = true
		widget.DropList:AddText("Left")
		widget.DropList:AddText("Center")
		widget.DropList:AddText("Right")
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "select", "edit_string_cx_select")
	elseif prop_type == "boolean" then
		widget = ui:Create("FlatComboBox")
		widget.Text = prop_text
		widget.InputBox.ReadOnly = true
		widget.DropList:AddText("true")
		widget.DropList:AddText("false")
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "select", "edit_boolean_cx_select")
	elseif prop_type == "int32" or prop_type == "int64" or prop_type == "float" or prop_type == "double" then
		widget = ui:Create("FlatNumeric")
		widget.Text = prop_text
	elseif prop_type == "string" then
		widget = ui:Create("FlatTextBox")
		widget.Text = prop_text
	elseif prop_type == "color" then
		widget = ui:Create("FlatButton")
		widget.Text = prop_text
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "click", "set_color_bn_click")
	elseif prop_type == "image" then
		widget = ui:Create("FlatButton")
		widget.Text = prop_text
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "click", "set_image_bn_click")
	elseif prop_type == "cursor" then
		widget = ui:Create("FlatComboBox")
		widget.Text = prop_text
		widget.InputBox.ReadOnly = true

		local cursor_list = ui:GetCursorList()

		for i = 1, #cursor_list do
			widget.DropList:AddText(cursor_list[i])
		end

		k_bind(widget, "ud_desk_fm")
		k_event(widget, "select", "edit_string_cx_select")
	elseif prop_type == "font" then
		widget = ui:Create("FlatComboBox")
		widget.Text = prop_text
		widget.InputBox.ReadOnly = true

		local font_list = ui:GetFontList()

		for i = 1, #font_list do
			widget.DropList:AddText(font_list[i])
		end

		k_bind(widget, "ud_desk_fm")
		k_event(widget, "select", "edit_string_cx_select")
	elseif prop_type == "file" then
		widget = ui:Create("FlatButton")
		widget.Text = prop_text
		k_bind(widget, "ud_desk_fm")
		k_event(widget, "click", "set_file_bn_click")
	else
		widget = ui:Create("FlatTextBox")
		widget.Text = prop_text
	end

	self:SetGridWidget(cur_row, 1, widget)
	self.edit_row = cur_row
	ui:FocusTo(widget)
	return 1
end

--Set need save flag.
function set_need_save()
	local ud = k_global("ud")

	ud.need_save = true
    ud.desk_fm.file_mn:EnableStrip("save")
	return 1
end

--Set property value of widget.
function set_widget_property(widget, prop_name, prop_value)
	local ud = k_global("ud")
	local ui = ud.ui

	if prop_name == "Name" then
		local design_fm = ud.design_fm

		--if k_uniqid_equal(design_fm, widget) then
		if design_fm == widget then
			k_set_property(widget, "Name", k_string(prop_value))
		else
			local old_name = k_property(widget, "Name")

			k_remove_extra(design_fm, old_name)
			k_set_property(widget, "Name", k_string(prop_value))
			k_set_extra(design_fm, prop_name, widget)
		end

		--Update widget name in hierarchy tree.
		show_hierarchy()
		
		local hierarchy_tv = ud.desk_fm.hierarchy_tv
		local ident = hierarchy_tv:FindTag("C" .. k_string(widget))

		if ident >= 0 then
			hierarchy_tv.Enabled = false
			hierarchy_tv:SelectIdentity(ident)
			hierarchy_tv.Enabled = true
		end
		
		set_need_save()
		return 1
	end

	local meta = ui:GetMeta(widget)
	local prop_type = meta:FindPropertyType(prop_name)

	if prop_type == "" then
		return 0
	end

	if prop_type == "boolean" then
		prop_value = k_boolean(prop_value)
	elseif prop_type == "int32" or prop_type == "int64" or prop_type == "float" or prop_type == "double" then
		prop_value = k_number(prop_value)
	end

	if not k_set_property(widget, prop_name, prop_value) then
		return 0
	end

	if prop_name == "Left" or prop_name == "Top" or prop_name == "Width" or prop_name == "Height" then
		local picker = ud.picker

		--if k_uniqid_equal(design_fm, widget) then
		if design_fm == widget then
			if prop_name == "Left" then
				widget.Left = prop_value
				picker.GlobalLeft = widget.GlobalLeft
			elseif prop_name == "Top" then
				widget.Top = prop_value
				picker.GlobalTop = widget.GlobalTop
			elseif prop_name == "Width" then
				widget.Width = prop_value
				picker.Width = widget.Width
			elseif prop_name == "Height" then
				widget.Height = prop_value
				picker.Height = widget.Height
			end
		end
	end
	
	set_need_save()
	return 1
end

--Edit property boolean value.
function edit_boolean_cx_select(self)
	local prop_dg = self.Parent
	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row
	local prop_name = prop_dg:GetGridText(edit_row, 0)
	local prop_value = self.InputBox.Text

	set_widget_property(target, prop_name, prop_value)
	return 1
end

--Edit property string value.
function edit_string_cx_select(self)
	local prop_dg = self.Parent
	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row
	local prop_name = prop_dg:GetGridText(edit_row, 0)
	local prop_value = self.InputBox.Text

	set_widget_property(target, prop_name, prop_value)
	return 1
end

--Edit color button click.
function set_color_bn_click(self)
	local ui = self:GetUI()
	local dialog = ui:LoadForm("ud_set_color_fm.json")

	dialog.color = self.Text
	dialog:ShowDialog()

	local result, color = k_wait_signal(-1, dialog, "set_color_signal")

	if result == "cancel" then
		return 0
	end

	local prop_dg = self.Parent
	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row
	local prop_name = prop_dg:GetGridText(edit_row, 0)

	prop_dg:SetGridText(edit_row, 1, color)
	self.Text = file_name
	set_widget_property(target, prop_name, color)
	return 1
end

--Edit image button click.
function set_image_bn_click(self)
	local ui = self:GetUI()
	local dialog = ui:LoadForm("ud_open_file_fm.json")

	dialog.file_dir = k_path("asset") .. "png/"
	dialog.file_root = "png"
	dialog.file_ext = "*.png"
	dialog.file_name = self.Text
	dialog:ShowDialog()

	local result, file_name = k_wait_signal(-1, dialog, "open_file_signal")

	if result == "cancel" then
		return 0
	end

	--Cut asset path.
	file_name = string.sub(file_name, string.len(k_path("asset")) + 1)

	local prop_dg = self.Parent
	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row
	local prop_name = prop_dg:GetGridText(edit_row, 0)

	prop_dg:SetGridText(edit_row, 1, file_name)
	self.Text = file_name
	set_widget_property(target, prop_name, file_name)
	return 1
end

--Edit file button click.
function set_file_bn_click(self)
	local ui = self:GetUI()
	local dialog = ui:LoadForm("ud_open_file_fm.json")

	dialog.file_dir = k_path("asset")
	dialog.file_root = "asset"
	dialog.file_ext = "*.*"
	dialog.file_name = self.Text
	dialog:ShowDialog()

	local result, file_name = k_wait_signal(-1, dialog, "open_file_signal")

	if result == "cancel" then
		return 0
	end

	--Cut asset path.
	file_name = string.sub(file_name, string.len(k_path("asset")) + 1)

	local prop_dg = self.Parent
	local target = prop_dg.target

	if not k_exists(target) then
		return 0
	end

	local edit_row = prop_dg.edit_row
	local prop_name = prop_dg:GetGridText(edit_row, 0)

	prop_dg:SetGridText(edit_row, 1, file_name)
	self.Text = file_name
	set_widget_property(target, prop_name, file_name)
	return 1
end
