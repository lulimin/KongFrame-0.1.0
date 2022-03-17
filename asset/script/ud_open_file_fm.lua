--User interface open file script.

--When form load.
function ud_open_file_fm_load(self)
    self.file_dir = ""
    self.file_root = "root"
    self.file_ext = "*.*"
    self.file_name = ""
    return 1
end

--When form show.
function ud_open_file_fm_show(self)
    local ui = self:GetUI()

    self.Left = (ui.Width - self.Width) / 2
    self.Top = (ui.Height - self.Height) / 2
    self.file_tx.Text = self.file_name

    --Show directory tree.
    local dir_finder = k_create("FileFinder")

    dir_finder:SearchDirectory(self.file_dir, "*.*")

    local dir_table = dir_finder:GetDirectoryList()
    local path_tv = self.path_tv

    path_tv:BeginUpdate()
    path_tv:CreateRoot(self.file_root, true)
    path_tv:SetTag(self.file_dir)

    for k = 1, #dir_table do
        local dir_name = dir_table[k]
        local index = path_tv:AddText(dir_name, false)

        path_tv:SelectIndex(index)
        path_tv:SetTag(self.file_dir .. dir_name .. "/")
        path_tv:SelectParent()
    end

    path_tv:EndUpdate()
    k_delete(dir_finder)

    --Show file list.
    local finder = k_create("FileFinder")

    finder:SearchFile(self.file_dir, self.file_ext)

    local file_table = finder:GetFileList()
    local file_lv = self.file_lv

    file_lv:BeginUpdate()

    for i = 1, #file_table do
        local file_name = file_table[i]

        file_lv:AddItem(file_name, "png/widget.png")
    end

    file_lv:EndUpdate()
    k_delete(finder)
    return 1
end

--Click ok button.
function ok_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()
    local file_name = form.file_tx.Text

    if file_name == "" then
        return 0
    end

    local dir = form.path_tv:GetTag()

    form:Close()
    k_send_signal(form, "open_file_signal", "ok", dir .. file_name)
    ui:Delete(form)
    return 1
end

--Click cancel button.
function cancel_bn_click(self)
    local ui = self:GetUI()
    local form = self:FindForm()

    form:Close()
    k_send_signal(form, "open_file_signal", "cancel")
    ui:Delete(form)
    return 1
end

--Select directory.
function path_tv_select(self)
    local form = self:FindForm()
    local text = self:GetText()

    if text == "" then
        return 0
    end

    local dir = self:GetTag()
    local finder = k_create("FileFinder")

    finder:SearchFile(dir, form.file_ext)

    local file_table = finder:GetFileList()
    local file_lv = form.file_lv

    file_lv:BeginUpdate()
    file_lv:ClearItems()

    for i = 1, #file_table do
        local file_name = file_table[i]

        file_lv:AddItem(file_name, "png/widget.png")
    end

    file_lv:EndUpdate()
    k_delete(finder)
    return 1
end

--Expand directory.
function path_tv_expand(self)
    local form = self:FindForm()
    local text = self:GetText()

    if text == "" then
        return 0
    end

    local dir = self:GetTag()
    local dir_finder = k_create("FileFinder")

    dir_finder:SearchDirectory(dir, "*.*")

    local dir_table = dir_finder:GetDirectoryList()

    self:BeginUpdate()

    for k = 1, #dir_table do
        local dir_name = dir_table[k]
        local index = self:AddText(dir_name, false)

        self:SelectIndex(index)
        self:SetTag(dir .. dir_name .. "/")
        self:SelectParent()
    end

    self:EndUpdate()
    k_delete(dir_finder)
    return 1
end

--Select file.
function file_lv_select(self)
    local form = self:FindForm()
    local select_index = self.SelectedIndex

    if select_index < 0 then
        return 0
    end

    local file_name = self:GetText(select_index)
    
    form.file_tx.Text = file_name
    return 1
end
