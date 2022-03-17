--Set color dialog script.

--When form load.
function ud_set_color_fm_load(self)
    self.color = ""
    return 1
end

--When form show.
function ud_set_color_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2

    local color = self.color
    local red = 0
    local green = 0
    local blue = 0
    local alpha = 0
    local comma1 = string.find(color, ",")

    if comma1 ~= nil then
        local comma2 = string.find(color, ",", comma1 + 1)

        if comma2 ~= nil then
            local comma3 = string.find(color, ",", comma2 + 1)

            if comma3 ~= nil then
                red = k_number(string.sub(color, 1, comma1))
                green = k_number(string.sub(color, comma1 + 1, comma2))
                blue = k_number(string.sub(color, comma2 + 1, comma3))
                alpha = k_number(string.sub(color, comma3 + 1))
            end
        end
    end

    self.red_nm.Text = k_string(red)
    self.green_nm.Text = k_string(green)
    self.blue_nm.Text = k_string(blue)
    self.alpha_nm.Text = k_string(alpha)
    self.color_lb.BackColor = color
    return 1
end

--When ok button clicked.
function ok_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local color = form.color

    form:Close()
    k_send_signal(form, "set_color_signal", "ok", color)
    ui:Delete(form)
    return 1
end

--When cancel button clicked.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "set_color_signal", "cancel")
    ui:Delete(form)
    return 1
end

--Update color value.
function update_color(form)
    local red = k_number(form.red_nm.Text)
    local green = k_number(form.green_nm.Text)
    local blue = k_number(form.blue_nm.Text)
    local alpha = k_number(form.alpha_nm.Text)

    if red < 0 or red > 255 then
        return 0
    end

    if green < 0 or green > 255 then
        return 0
    end

    if blue < 0 or blue > 255 then
        return 0
    end

    if alpha < 0 or alpha > 255 then
        return 0
    end

    local color = k_string(red) .. "," .. k_string(green) .. "," .. k_string(blue) .. "," .. k_string(alpha)

    form.color_lb.BackColor = color
    form.color = color
    return 1
end

--Edit red changed.
function red_nm_changed(self)
    local form = self:FindForm()

    update_color(form)
    return 1
end

--Edit green changed.
function green_nm_changed(self)
    local form = self:FindForm()

    update_color(form)
    return 1
end

--Edit blue changed.
function blue_nm_changed(self)
    local form = self:FindForm()

    update_color(form)
    return 1
end

--Edit alpha changed.
function alpha_nm_changed(self)
    local form = self:FindForm()

    update_color(form)
    return 1
end
