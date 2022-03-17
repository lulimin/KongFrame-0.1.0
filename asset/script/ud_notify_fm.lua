--Notify dialog script.

--When form load.
function ud_notify_fm_load(self)
    return 1
end

--When form show.
function ud_notify_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2
    return 1
end

--When ok button clicked.
function ok_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "notify_signal", "ok")
    ui:Delete(form)
    return 1
end

--Show error information.
function show_error(ui, err)
    local dialog = ui:LoadForm("ud_notify_fm.json")

    dialog.notify_lb.Text = k_string(err)
    dialog.notify_lb.ForeColor = "255,0,0,255"
    dialog.ok_bn.ForeColor = "255,0,0,255"
    dialog.ForeColor = "255,0,0,255"
    dialog:ShowDialog()

    local result = k_wait_signal(-1, dialog, "notify_signal")

    return 1
end

--Show notify information.
function show_nofity(ui, info)
    local dialog = ui:LoadForm("ud_notify_fm.json")

    dialog.notify_lb.Text = k_string(info)
    dialog:ShowDialog()

    local result = k_wait_signal(-1, dialog, "notify_signal")

    return 1
end
