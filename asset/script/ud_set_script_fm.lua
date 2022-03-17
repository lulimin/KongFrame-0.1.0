--User interface set script.
require("ud_notify_fm")

--When form load.
function ud_set_script_fm_load(self)
    self.target = k_uniqid()
    return 1
end

--When form show.
function ud_set_script_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2

    local target = self.target

    if not k_exists(target) then
        return 0
    end

    local meta = ui:GetMeta(target)
    local event_num = meta:GetEventCount()

    for i = 0, event_num - 1 do
        local event_name = meta:GetEventName(i)

        self.event_cx.DropList:AddText(event_name)
    end

    self.event_cx.InputBox.ReadOnly = true
    self.script_tx.Text = k_extra(target, "__script")
    self.loader_tx.Text = k_extra(target, "__loader")

    local event_dg = self.event_dg
    local row = 0

    event_dg:BeginUpdate()
    event_dg.RowCount = 0

    local extra_list = k_extra_list(target)

    for i = 1, #extra_list do
        local extra_name = extra_list[i]

        if string.len(extra_name) > 8 and string.sub(extra_name, 1, 8) == "__event_" then
            local event_name = string.sub(extra_name, 9)
            local func = k_extra(target, extra_name)

            event_dg.RowCount = row + 1
            event_dg:SetGridText(row, 0, event_name)
            event_dg:SetGridText(row, 1, func)
            row = row + 1
        end
    end

    event_dg:EndUpdate()
    return 1
end

--Click ok button.
function ok_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local target = form.target
    
    k_set_extra(target, "__script", form.script_tx.Text)
    k_set_extra(target, "__loader", form.loader_tx.Text)

    local extra_list = k_extra_list(target)

    --Clear event processors.
    for i = 1, #extra_list do
        local extra_name = extra_list[i]

        if string.len(extra_name) > 8 and string.sub(extra_name, 1, 8) == "__event_" then
            k_remove_extra(target, extra_name)
        end
    end

    --Add event processors.
    local event_dg = form.event_dg
    local row_num = event_dg.RowCount

    for row = 0, row_num - 1 do
        local event_name = event_dg:GetGridText(row, 0)
        local func = event_dg:GetGridText(row, 1)

        k_set_extra(target, "__event_" .. event_name, func)
    end

    form:Close()
    k_send_signal(form, "set_script_signal", "ok")
    ui:Delete(form)
    return 1
end

--Click cancel button.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "set_script_signal", "cancel")
    ui:Delete(form)
    return 1
end

--Select event name.
function event_cx_select(self)
    local form = self:FindForm()
    local target = form.target
    local event_name = self.Text

    form.func_tx.Text = target.Name .. "_" .. event_name
    return 1
end

--Add event processor.
function add_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local event_name = form.event_cx.Text
    local func = form.func_tx.Text

    if event_name == "" or func == "" then
        return 0
    end

    local event_dg = form.event_dg
    local row_num = event_dg.RowCount

    --Check event name repeat.
    for i = 0, row_num - 1 do
        local ev_name = event_dg:GetGridText(i, 0)

        if ev_name == event_name then
            show_error(ui, "Event '" .. event_name .. "' already exists!")
            return 0
        end
    end
    
    event_dg.RowCount = row_num + 1
    event_dg:SetGridText(row_num, 0, event_name)
    event_dg:SetGridText(row_num, 1, func)
    form.event_cx.Text = ""
    form.func_tx.Text = ""
    return 1
end

--Remove event processor.
function remove_bn_click(self)
    local form = self:FindForm()
    local event_dg = form.event_dg
    local cur_row = event_dg.CurrentRow

    if cur_row < 0 then
        return 0
    end

    event_dg:RemoveRow(cur_row)
    return 1
end

--Open script file.
function open_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local script = form.script_tx.Text
    local script_file = k_path("script") .. script .. ".lua"
    local fm = k_single("FrameMisc")

    if not fm:FindFile(script_file) then
        show_error(ui, "Script file " .. script_file .. " not found!")
        return 0
    end

    if not ui:OpenFile(script_file) then
        show_error(ui, "Open script file " .. script_file .. " failed!")
        return 0
    end
    
    return 1
end

--Write widget to script.
function write_script(fp, target)
    if k_find_extra(target, "__loader") then
        local loader = target.__loader

        if loader ~= "" then
            fp:write("function " .. loader .. "(self)\n")
            fp:write("\treturn 1\n")
            fp:write("end\n")
            fp:write("\n")
        end
    end

    local extra_list = k_extra_list(target)

    for i = 1, #extra_list do
        local extra_name = extra_list[i]

        if string.len(extra_name) > 8 and string.sub(extra_name, 1, 8) == "__event_" then
            local event_name = string.sub(extra_name, 9)
            local func = k_extra(target, extra_name)

            fp:write("function " .. func .. "(self)\n")
            fp:write("\treturn 1\n")
            fp:write("end\n")
            fp:write("\n")
        end
    end

    if target:IsContainer() then
        local child_list = target:GetChildList()

        for k = 1, #child_list do
            write_script(fp, child_list[k])
        end
    end
    
    return true
end

--Generate script file.
function generate_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local script = form.script_tx.Text
    
    if script == "" then
        show_error(ui, "No script file.")
        return 0
    end
    
    local script_file = k_path("script") .. script .. ".lua"
    local fm = k_single("FrameMisc")

    if fm:FindFile(script_file) then
        show_error(ui, "Script file " .. script .. ".lua already exists!")
        return 0
    end

    local target = form.target

    if not k_exists(target) then
        return 0
    end

    if k_class_name(target) ~= "FlatForm" then
        return 0
    end

    local fp = io.open(script_file, "w+")

    fp:write("--" .. target.Name .. " script\n")
    fp:write("\n")
    write_script(fp, target)
    fp:close()
    show_error(ui, "Generate script file " .. script .. ".lua succeed!")
    return 1
end
