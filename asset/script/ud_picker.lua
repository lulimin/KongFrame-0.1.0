--Picker script.

--Create picker.
function create_picker(ui)
	local picker = ui:Create("FlatPicker")
	local color = "200,0,0,255"
	local edge_color = "200,0,0,255"

	picker.Left = 0
	picker.Top = 0
	picker.Width = 100
	picker.Height = 100
	picker.Actived = true
	picker.DesignMode = true
	picker.ContextMenu = "ud_picker"
	picker.ForeColor = color
	picker.Cursor = "ud_size_all"
	picker.LeftEdge.Cursor = "ud_size_we"
	picker.RightEdge.Cursor = "ud_size_we"
	picker.UpEdge.Cursor = "ud_size_ns"
	picker.DownEdge.Cursor = "ud_size_ns"
	picker.LeftUpEdge.Cursor = "ud_size_nwse"
	picker.RightDownEdge.Cursor = "ud_size_nwse"
	picker.LeftDownEdge.Cursor = "ud_size_nesw"
	picker.RightUpEdge.Cursor = "ud_size_nesw"
	picker.LeftEdge.ForeColor = edge_color
	picker.RightEdge.ForeColor = edge_color
	picker.UpEdge.ForeColor = edge_color
	picker.DownEdge.ForeColor = edge_color
	picker.LeftUpEdge.ForeColor = edge_color
	picker.LeftDownEdge.ForeColor = edge_color
	picker.RightUpEdge.ForeColor = edge_color
	picker.RightDownEdge.ForeColor = edge_color
	k_bind(picker, "ud_picker", "picker_init")
	return picker
end

--Initialize picker.
function picker_init(self)
    k_event(self, "changed", "picker_changed")
    return 1
end

--Picker changed.
function picker_changed(self)
    local ud = k_global("ud")
    local prop_dg = ud.desk_fm.property_dg
    local target = self.Target

    if not k_exists(target) then
        return 0
    end

    --if not k_uniqid_equal(target, prop_dg.target) then
	if target ~= prop_dg.target then
		return 0
    end

    prop_dg:SetGridText(1, 1, k_string(target.Left))
    prop_dg:SetGridText(2, 1, k_string(target.Top))
    prop_dg:SetGridText(3, 1, k_string(target.Width))
	prop_dg:SetGridText(4, 1, k_string(target.Height))
	k_run_routine("ud_desk_fm", "set_need_save")
    return 1
end

--Create picker context menu.
function create_picker_cm(ui)
	local picker_cm = ui:Create("FlatMenu")

	picker_cm.Name = "ud_picker"
	picker_cm.Left = 0
	picker_cm.Top = 0
	picker_cm.Width = 200
	picker_cm.Height = 50
	picker_cm.StripHeight = 30
	picker_cm.ForeColor = "50,50,50,255"
	picker_cm.BackColor = "200,200,200,255"
	picker_cm:AddStrip("back", "To Back")
	picker_cm:AddStrip("front", "To Front")
	picker_cm:AddStrip("-", "-")
	picker_cm:AddStrip("script", "Script")
	picker_cm:AddStrip("-", "-")
	picker_cm:AddStrip("copy", "Copy")
	picker_cm:AddStrip("paste", "Paste")
	picker_cm:AddStrip("delete", "Delete")
	k_bind(picker_cm, "ud_picker", "picker_cm_init")
    ui:AddContextMenu(picker_cm)
    return 1
end

--Initialize picker context menu.
function picker_cm_init(self)
	k_event(self, "popup", "picker_cm_popup")
	k_event(self, "back", "picker_cm_back")
    k_event(self, "front", "picker_cm_front")
    k_event(self, "script", "picker_cm_script")
    k_event(self, "copy", "picker_cm_copy")
    k_event(self, "paste", "picker_cm_paste")
	k_event(self, "delete", "picker_cm_delete")
	return 1
end

--When context menu popup.
function picker_cm_popup(self)
	local ud = k_global("ud")
	local picker = self.Owner
	local target = picker.Target
	
	--if k_uniqid_equal(ud.design_fm, target) then
	if target == ud.design_fm then
		self:DisableStrip("back")
		self:DisableStrip("front")
		self:DisableStrip("delete")
	else
		self:EnableStrip("back")
		self:EnableStrip("front")

		if self:HasChildren() then
			--Can't be deleted if has any child.
			self:DisableStrip("delete")
		else
			self:EnableStrip("delete")
		end
	end

	if target:IsContainer() then
		self:DisableStrip("copy")

		if k_exists(ud.clone_source) then
			self:EnableStrip("paste")
		else
			self:DisableStrip("paste")
		end
	else
		self:EnableStrip("copy")
		self:DisableStrip("paste")
	end
	
	return 1
end

--Send widget to background.
function picker_cm_back(self)
	local picker = self.Owner
	local target = picker.Target

	target:SendToBack()
	return 1
end

--Bring widget to foreground.
function picker_cm_front(self)
	local ud = k_global("ud")
	local picker = self.Owner
	local target = picker.Target

	--if not k_uniqid_equal(ud.design_fm, target) then
	if target ~= ud.design_fm then
		target:BringToFront()
	end

	return 1
end

--Set script.
function picker_cm_script(self)
	local picker = self.Owner
	local target = picker.Target
	local ud = k_global("ud")
	local ui = ud.ui
	local dialog = ui:LoadForm("ud_set_script_fm.json")

	dialog.target = target
	dialog:ShowDialog()

	local result = k_wait_signal(-1, dialog, "set_script_signal")

	if result == "cancel" then
		return 0
	end
    
	k_run_routine("ud_desk_fm", "set_need_save")
	return 1
end

--Copy widget.
function picker_cm_copy(self)
	local ud = k_global("ud")
	local picker = self.Owner
	local target = picker.Target
	
	ud.clone_source = target
    return 1
end

--Paste widget.
function picker_cm_paste(self)
	local ud = k_global("ud")
	local ui = ud.ui
	local picker = self.Owner
	local target = picker.Target

	if not target:IsContainer() then
		return 0
	end

	local source = ud.clone_source

	if not k_exists(source) then
		return 0
	end

	local widget = ui:Clone(source)

	if not k_exists(widget) then
		return 0
	end

	local data_name = k_class_name(widget)

	if string.find(data_name, "Flat", 1, 4) ~= nil then
		data_name = string.sub(data_name, 5)
	end

	local x = self.GlobalLeft
	local y = self.GlobalTop

	k_run_routine("ud_desk_fm", "add_widget", target, widget, data_name, x, y)
	return 1
end

--Delete widget.
function picker_cm_delete(self)
    local ud = k_global("ud")
    local ui = ud.ui
    local picker = self.Owner
    local target = picker.Target
    local design_fm = ud.design_fm

    --if k_uniqid_equal(design_fm, target) then
    if target == design_fm then
        return 0
    end
    
    k_remove_extra(design_fm, target.Name)
    design_fm:RemoveChild(target)
    ui:Delete(target)
    --Update widget hierarchy.
    k_run_routine("ud_desk_fm", "show_hierarchy")
    --Picker in design form.
    k_run_routine("ud_desk_fm", "pick_widget", design_fm)
	--Set need save form.
	k_run_routine("ud_desk_fm", "set_need_save")
    return 1
end
