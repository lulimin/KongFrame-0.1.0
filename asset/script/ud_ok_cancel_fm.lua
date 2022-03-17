--Ok cancel dialog script.

--When form load.
function ud_ok_cancel_fm_load(self)
    return 1
end

--When form show.
function ud_ok_cancel_fm_show(self)
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
    k_send_signal(form, "ok_cancel_signal", "ok")
    ui:Delete(form)
    return 1
end

--When cancel button clicked.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "ok_cancel_signal", "cancel")
    ui:Delete(form)
    return 1
end
