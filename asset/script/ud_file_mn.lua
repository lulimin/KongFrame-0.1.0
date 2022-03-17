--File menu script.
require("ud_notify_fm")

--Create file menu.
function create_file_mn(ui)
	local file_mn = ui:Create("FlatMenu")

	file_mn.Name = "file"
	file_mn.Left = 0
	file_mn.Top = 0
	file_mn.Width = 150
	file_mn.Height = 50
	file_mn.StripHeight = 30
	file_mn.ForeColor = "50,50,50,255"
	file_mn.BackColor = "220,220,220,255"
	file_mn:AddStrip("new", "New")
	file_mn:AddStrip("open", "Open")
	file_mn:AddStrip("save", "Save")
	file_mn:AddStrip("save_as", "Save as")
    file_mn:AddStrip("-", "-")
    file_mn:AddStrip("test", "Test form")

    if k_find_class("WinTerminal") then
        file_mn:AddStrip("-", "-")
        file_mn:AddStrip("terminal", "Terminal")
    end

    file_mn:AddStrip("-", "-")
    file_mn:AddStrip("exit", "Exit")
    k_bind(file_mn, "ud_file_mn", "file_mn_init")
    return file_mn
end

--Initialize file menu.
function file_mn_init(self)
    k_event(self, "new", "file_mn_new")
    k_event(self, "open", "file_mn_open")
    k_event(self, "save", "file_mn_save")
    k_event(self, "save_as", "file_mn_save_as")
    k_event(self, "test", "file_mn_test")
    k_event(self, "terminal", "file_mn_terminal")
    k_event(self, "exit", "file_mn_exit")
    return 1
end

--Clear need save flag.
function clear_need_save()
	local ud = k_global("ud")

    ud.need_save = false
    ud.desk_fm.file_mn:DisableStrip("save")
	return 1
end

--Save designing form.
function save_design_form()
    local ud = k_global("ud")
    local ui = ud.ui

    if ud.file_name == "" then
        local file_dialog = ui:LoadForm("ud_open_file_fm.json")

        file_dialog.file_dir = ui.ConfigPath
        file_dialog.file_root = "widget"
        file_dialog.file_ext = "*.json"
        file_dialog.file_name = ud.design_fm.Name .. ".json"
        file_dialog:ShowDialog()

        local file_result, file_name = k_wait_signal(-1, file_dialog, "open_file_signal")

        if file_result == "cancel" then
            return false
        end

        ud.file_name = file_name
    end

    if not ui:SaveForm(ud.design_fm, ud.file_name) then
        show_error(ui, "Save form to file '" .. ud.file_name .. "' failed!")
        return false
    end

    clear_need_save()
    return true
end

--Create new form.
function new_design_form()
    local ud = k_global("ud")
    local ui = ud.ui
    local fm = ui:Create("FlatForm")

    fm.Name = "design_fm"
    fm.Left = 100
    fm.Top = 100
    fm.Width = 400
    fm.Height = 300
    fm.DesignMode = true
    fm.StepSize = 8
    ud.desk_fm.work_pn:AddChild(fm)
    ud.design_fm = fm
    k_run_routine("ud_desk_fm", "show_hierarchy")
    return 1
end

--New file.
function file_mn_new(self)
    local ud = k_global("ud")
    local ui = ud.ui

    if ud.need_save then
        local dialog = ui:LoadForm("ud_yes_no_cancel_fm.json")
		
		dialog.notify_lb.Text = "Work form changed, do you want save it?"
		dialog:ShowDialog()

		local result = k_wait_signal(-1, dialog, "yes_no_cancel_signal")

		if result == "cancel" then
			return 0
		end

		if result == "yes" then
            save_design_form()
        end
    end

    new_design_form()
    --Picker in design form.
    k_run_routine("ud_desk_fm", "pick_widget", ud.design_fm)
    return 1
end

--Open file.
function file_mn_open(self)
    local ud = k_global("ud")
    local ui = ud.ui

    if ud.need_save then
        local dialog = ui:LoadForm("ud_yes_no_cancel_fm.json")
		
		dialog.notify_lb.Text = "Work form changed, do you want save it?"
		dialog:ShowDialog()

		local result = k_wait_signal(-1, dialog, "yes_no_cancel_signal")

		if result == "cancel" then
			return 0
		end

		if result == "yes" then
            save_design_form()
        end
    end

    local file_dialog = ui:LoadForm("ud_open_file_fm.json")

    file_dialog.file_dir = ui.ConfigPath
    file_dialog.file_root = "widget"
    file_dialog.file_ext = "*.json"
    file_dialog:ShowDialog()

    local file_result, file_name = k_wait_signal(-1, file_dialog, "open_file_signal")

    if file_result == "cancel" then
        return 0
    end

    local new_fm = ui:LoadDesignForm(file_name)

    if not k_exists(new_fm) then
        show_error(ui, "Load form from file '" .. file_name .. "' failed!")
        return 0
    end

    local old_fm = ud.design_fm

    ud.desk_fm.work_pn:RemoveChild(old_fm)
    ui:Delete(old_fm)
    ud.desk_fm.work_pn:AddChild(new_fm)
    ud.design_fm = new_fm
    ud.file_name = file_name
    --Display helper grid.
    new_fm.StepSize = 8
    --Display widget hierarchy.
    k_run_routine("ud_desk_fm", "show_hierarchy")
    --Picker in design form.
    k_run_routine("ud_desk_fm", "pick_widget", ud.design_fm)
    return 1
end

--Save file.
function file_mn_save(self)
    save_design_form()
    return 1
end

--Save file as.
function file_mn_save_as(self)
    local ud = k_global("ud")
    local ui = ud.ui
    local file_dialog = ui:LoadForm("ud_open_file_fm.json")

    file_dialog.file_dir = ui.ConfigPath
    file_dialog.file_root = "widget"
    file_dialog.file_ext = "*.json"
    file_dialog.file_name = ud.file_name
    file_dialog:ShowDialog()

    local file_result, file_name = k_wait_signal(-1, file_dialog, "open_file_signal")

    if file_result == "cancel" then
        return 0
    end

    local fm = k_single("FrameMisc")

    if fm:FindFile(ui.ConfigPath .. file_name) then
        local dialog = ui:LoadForm("ud_ok_cancel_fm.json")

        dialog.notify_lb.Text = "File '" .. file_name .. "'exists, cover it?"
        dialog:ShowDialog()

        local result = k_wait_signal(-1, dialog, "ok_cancel_signal")

        if result == "cancel" then
            return 0
        end
    end

    ud.file_name = file_name

    if not ui:SaveForm(ud.design_fm, file_name) then
        show_error(ui, "Save form to file '" .. file_name .. "' failed!")
        return 0
    end

    clear_need_save()
    return 1
end

--Test form in designer.
function file_mn_test(self)
    local ud = k_global("ud")
    local ui = ud.ui
    local design_fm = ud.design_fm
    local script = design_fm.__script
    local script_file = k_path("script") .. k_string(script) .. ".lua"
    local fm = k_single("FrameMisc")

    if not fm:FindFile(script_file) then
        show_error(ui, "Script file " .. script_file " not found!")
        return 0
    end

    local file_name = ud.file_name

    --if not fm:FindFile(ui.ConfigPath .. file_name) then
    if not fm:FindFile(file_name) then
        show_error(ui, "Config file " .. file_name .. " not found!")
        return 0
    end

    local test_fm = ui:LoadForm(file_name)

    if not k_exists(test_fm) then
        show_error(ui, "Load form " .. file_name .. " failed!")
        return 0
    end

    test_fm:Show()
    return 1
end

--Open terminal.
function file_mn_terminal(self)
	if not k_find_class("WinTerminal") then
		return 0
    end
    
    if k_exists(k_global("terminal")) then
        return 0
    end

	local terminal = k_create("WinTerminal")

	k_set_global("terminal", terminal)
	k_bind(terminal, "ud_terminal", "terminal_init")
    return 1
end

--Exit user interface designer.
function exit_designer()
    local ud = k_global("ud")
    local ui = ud.ui

	if ud.need_save then
		local dialog = ui:LoadForm("ud_yes_no_cancel_fm.json")
		
		dialog.notify_lb.Text = "Work form changed, do you want save it?"
		dialog:ShowDialog()

		local result = k_wait_signal(-1, dialog, "yes_no_cancel_signal")

		if result == "cancel" then
			return 0
		end

		if result == "yes" then
            if not save_design_form() then
                return 0
            end
		end
	end

	k_exit()
    return 1
end

--Exit designer.
function file_mn_exit(self)
    exit_designer()
    return 1
end
