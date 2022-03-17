--Test scene script.

--When form load.
function test_scene_fm_load(self)
    return 1
end

--When form show.
function test_scene_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2

    local scene = self.test_sn
    local sprite = scene:CreateSprite("FlatSprite")

    sprite:SetPosition(50, 50)
    sprite.Image = "png/sun.png"
    scene:AddSprite(sprite)
    k_run_routine("example/test_scene_fm", "rotate_sprite", sprite)

    --if sprite ~= k_uniqid() then
    --end

    return 1
end

--Click ok button.
function ok_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "test_scene_signal", "ok")
    ui:Delete(form)
    return 1
end

--Click cancel button.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "test_scene_signal", "cancel")
    ui:Delete(form)
    return 1
end

function rotate_sprite(sprite)
    while true do
        local seconds = k_wait(0.0)

        if not k_exists(sprite) then
            break
        end

        local old_angle = sprite.Rotation
        local new_angle = old_angle + 1.0 * seconds

        sprite:SetRotation(new_angle)
    end

    return 1
end
