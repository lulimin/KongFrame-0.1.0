--User interface designer main script.

--Entry point of script.
function main()
	local window = k_main_object()
	local init_width = window:GetInitialWidth()
	local init_height = window:GetInitialHeight()

	window.Width = init_width
	window.Height = init_height
	window:StartRender()

	--Create ui object.
	local ui = k_create("FlatUI")

	k_bind(ui, "ud_main", "flat_ui_init")
	window:SetUIObject(ui)

	--Create defult font.
	local font = k_create("FlatFont")

	font.Name = "default"
	--font.Resource = "font/simsun.ttc"
	font.FixedWidth = false
	--font.Resource = "font/GenJyuuGothic-Monospace-Regular-2.ttf"
	font.Resource = "font/objectivity.regular.otf"
	font.CharHeight = 18
	font.Interval = 2
	ui:AddFont(font)
	ui:SetCurrentFont("default")

	--Default cursor.
	local cur = k_create("FlatCursor")

	cur.Name = "default"
	cur.Resource = "IDC_ARROW"
	cur:Load()
	ui:AddCursor(cur)
	ui:SetCurrentCursor("default")

	--Default hint label.
	local hl = ui:Create("FlatLabel")

	hl.Name = "default"
	hl.Font = "default"
	hl.Left = 0
	hl.Top = 0
	hl.Width = 200
	hl.Height = 30
	hl.Text = "hint"
	hl.Background = true
	hl.Border = true
	hl.ForeColor = "50,50,50,255"
	hl.BackColor = "220,220,220,255"
	ui:AddHintLabel(hl)

	--TextBox context menu.
	local cm = ui:Create("FlatMenu")

	cm.Name = "TextBox"
	cm.Left = 0
	cm.Top = 0
	cm.Width = 100
	cm.Height = 50
	cm.StripHeight = 30
	cm.ForeColor = "50,50,50,255"
	cm.BackColor = "220,220,220,255"
	cm:AddStrip("cut", "Cut")
	cm:AddStrip("copy", "Copy")
	cm:AddStrip("paste", "Paste")
	k_bind(cm, "ud_main", "cm_text_box_init")
	ui:AddContextMenu(cm)

	--Create designer.
	create_ui_designer(ui, 0, 0, window.Width, window.Height)
	return 1
end

--Create user interface designer.
function create_ui_designer(ui, left, top, width, height)
	--User interface designer global value.
	local ud = k_create("NodeTree")

	k_set_global("ud", ud)
	ud.ui = ui
	ud.picker = k_uniqid()
	ud.clone_source = k_uniqid()
	ud.file_name = ""
	ud.need_save = false

	--User interface designer font.
	local ud_font = k_create("FlatFont")

	ud_font.Name = "ud_font"
	--ud_font.Resource = "font/simsun.ttc"
	ud_font.FixedWidth = true
	ud_font.Resource = "font/objectivity.regular.otf"
	ud_font.CharHeight = 16
	ud_font.Interval = 2
	ui:AddFont(ud_font)

	--User interface designer cursor.
	local ud_cursor = k_create("FlatCursor")

	ud_cursor.Name = "ud_cursor"
	ud_cursor.Resource = "IDC_ARROW"
	ud_cursor:Load()
	ui:AddCursor(ud_cursor)

	--Size cursor.
	local cur_size_all = k_create("FlatCursor")

	cur_size_all.Name = "ud_size_all"
	cur_size_all.Resource = "IDC_SIZEALL"
	cur_size_all:Load()
	ui:AddCursor(cur_size_all)

	local cur_size_we = k_create("FlatCursor")

	cur_size_we.Name = "ud_size_we"
	cur_size_we.Resource = "IDC_SIZEWE"
	cur_size_we:Load()
	ui:AddCursor(cur_size_we)

	local cur_size_ns = k_create("FlatCursor")

	cur_size_ns.Name = "ud_size_ns"
	cur_size_ns.Resource = "IDC_SIZENS"
	cur_size_ns:Load()
	ui:AddCursor(cur_size_ns)

	local cur_size_nwse = k_create("FlatCursor")

	cur_size_nwse.Name = "ud_size_nwse"
	cur_size_nwse.Resource = "IDC_SIZENWSE"
	cur_size_nwse:Load()
	ui:AddCursor(cur_size_nwse)

	local cur_size_nesw = k_create("FlatCursor")

	cur_size_nesw.Name = "ud_size_nesw"
	cur_size_nesw.Resource = "IDC_SIZENESW"
	cur_size_nesw:Load()
	ui:AddCursor(cur_size_nesw)

	--User interface designer hint label.
	local ud_hint = ui:Create("FlatLabel")

	ud_hint.Name = "ud_hint"
	ud_hint.Font = "ud_font"
	ud_hint.Left = 0
	ud_hint.Top = 0
	ud_hint.Width = 200
	ud_hint.Height = 30
	ud_hint.Text = "hint"
	ud_hint.Background = true
	ud_hint.Border = true
	ud_hint.ForeColor = "50,50,50,255"
	ud_hint.BackColor = "220,220,220,255"
	ui:AddHintLabel(ud_hint)

	--User interface designer TextBox context menu.
	local text_box_cm = ui:Create("FlatMenu")

	text_box_cm.Name = "ud_text_box"
	text_box_cm.Left = 0
	text_box_cm.Top = 0
	text_box_cm.Width = 100
	text_box_cm.Height = 50
	text_box_cm.StripHeight = 30
	text_box_cm.ForeColor = "50,50,50,255"
	text_box_cm.BackColor = "220,220,220,255"
	text_box_cm:AddStrip("cut", "Cut")
	text_box_cm:AddStrip("copy", "Copy")
	text_box_cm:AddStrip("paste", "Paste")
	k_bind(text_box_cm, "ud_main", "cm_text_box_init")
	ui:AddContextMenu(text_box_cm)

	--Create context menu of picker.
	k_run_routine("ud_picker", "create_picker_cm", ui)
	--Create desk form of designer.
	k_run_routine("ud_desk_fm", "create_desk_fm", ui, left, top, width, height)
	return 1
end

--Initialize flat ui.
function flat_ui_init(self)
	k_event(self, "design_pick", "flat_ui_design_pick")
	k_event(self, "close", "flat_ui_close")
	return 1
end

--Design mode pick.
function flat_ui_design_pick(self, x, y, pick_id, ctrl, shift)
	if not k_exists(pick_id) then
		return 0
	end

	local dest = pick_id

	if k_class_name(dest) == "FlatPicker" then
		local target = dest.Target

		if target:IsContainer() then
			dest = target:GetInRegion(x, y)
		else
			return 0
		end
	end

	k_run_routine("ud_desk_fm", "pick_widget", dest)
	return 1
end

--Window closing.
function flat_ui_close(self)
	k_run_routine("ud_file_mn", "exit_designer")
	return 1
end

--Initialize TextBox context menu.
function cm_text_box_init(self)
	k_event(self, "cut", "cm_text_box_cut")
	k_event(self, "copy", "cm_text_box_copy")
	k_event(self, "paste", "cm_text_box_paste")
	return 1
end

--TextBox context menu cut.
function cm_text_box_cut(self)
	self.Owner:Cut()
	return 1
end

--TextBox context menu copy.
function cm_text_box_copy(self)
	self.Owner:Copy()
	return 1
end

--TextBox context menu paste.
function cm_text_box_paste(self)
	self.Owner:Paste()
	return 1
end
