--Yes no cancel dialog script.

--When form load.
function ud_yes_no_cancel_fm_load(self)
    return 1
end

--When form show.
function ud_yes_no_cancel_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2
    return 1
end

--When yes button clicked.
function yes_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "yes_no_cancel_signal", "yes")
    ui:Delete(form)
    return 1
end

--When no button clicked.
function no_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "yes_no_cancel_signal", "no")
    ui:Delete(form)
    return 1
end

--When cancel button clicked.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "yes_no_cancel_signal", "cancel")
    ui:Delete(form)
    return 1
end
