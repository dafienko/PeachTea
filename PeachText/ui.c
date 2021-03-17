#include "ui.h"
#include "PeachTea.h"
#include "textEditorHandler.h"

PT_SCROLLFRAME* create_editor_scrollframe() {
	Instance* scrollFrameInstance = PT_SCROLLFRAME_new();
	PT_SCROLLFRAME* scrollframe = (PT_SCROLLFRAME*)scrollFrameInstance->subInstance;
	scrollframe->canvasSize = PT_REL_DIM_new(0.0f, 0, 0.0f, 1000);
	scrollframe->scrollBarThickness = 12;

	PT_GUI_OBJ* vTrack = scrollframe->vscrollTrack;
	vTrack->backgroundTransparency = 1;
	PT_GUI_OBJ* hTrack = scrollframe->hscrollTrack;
	hTrack->backgroundTransparency = vTrack->backgroundTransparency;

	PT_GUI_OBJ* vBar = scrollframe->vscrollBar;
	vBar->backgroundColor = colorTheme.borderColor;
	vBar->backgroundTransparency = 0.15f;
	vBar->blurred = 1;
	vBar->blurAlpha = .8f;
	vBar->blurRadius = 10;
	vBar->reactive = 1;
	vBar->activeBackgroundColor = colorTheme.textColor;
	vBar->activeBackgroundRange = (vec2f){ 10, 200 };
	PT_GUI_OBJ* hBar = scrollframe->hscrollBar;
	hBar->backgroundColor = vBar->backgroundColor;
	hBar->backgroundTransparency = vBar->backgroundTransparency;
	hBar->blurred = vBar->blurred;
	hBar->blurAlpha = vBar->blurAlpha;
	hBar->blurRadius = vBar->blurRadius;
	hBar->reactive = vBar->reactive;
	hBar->activeBackgroundColor = vBar->activeBackgroundColor;
	hBar->activeBackgroundRange = vBar->activeBackgroundRange;

	PT_GUI_OBJ* scrollObj = scrollframe->guiObj;
	scrollObj->backgroundTransparency = 1;
	scrollObj->position = PT_REL_DIM_new(0, SIDE_BAR_WIDTH, 0, 0);
	scrollObj->size = PT_REL_DIM_new(1.0f, -SIDE_BAR_WIDTH, 1.0f, -STATUS_BAR_HEIGHT);
	scrollObj->zIndex = 5;
	scrollObj->clipDescendants = 1;

	return scrollframe;
}

void realize_color_theme(EDITOR_COLOR_THEME theme) {
	colorTheme = theme;

	PT_EXPANDABLE_ARRAY editors = get_open_editors();
	// editor list elmeents;
	for (int i = 0; i < editors.numElements; i++) {
		PT_COLOR SELECTED_COLOR = accentColor;
		PT_COLOR SELECTED_ACTIVE_COLOR = PT_COLOR_lerp(accentColor, colorTheme.borderColor, .3f);
		PT_COLOR DESELECTED_COLOR = colorTheme.sidebarColor;
		PT_COLOR DESELECTED_ACTIVE_COLOR = PT_COLOR_lerp(colorTheme.sidebarColor, colorTheme.borderColor, .14f);
		
		TEXT_EDITOR* editor = *(TEXT_EDITOR**)PT_EXPANDABLE_ARRAY_get(&editors, i);

		PT_SCROLLFRAME* scrollframe = editor->scrollFrame;
		PT_GUI_OBJ* vBar = scrollframe->vscrollBar;
		vBar->backgroundColor = colorTheme.borderColor;
		vBar->activeBackgroundColor = colorTheme.textColor;
		PT_GUI_OBJ* hBar = scrollframe->hscrollBar;
		hBar->backgroundColor = colorTheme.borderColor;
		hBar->activeBackgroundColor = colorTheme.textColor;

		PT_GUI_OBJ* main = editor->listElement.main;
		main->backgroundColor = get_current_text_editor() == editor ? SELECTED_COLOR : DESELECTED_COLOR;
		main->activeBackgroundColor = get_current_text_editor() == editor ? SELECTED_ACTIVE_COLOR : DESELECTED_ACTIVE_COLOR;

		PT_COLOR fringeColor = PT_TWEEN_PT_COLOR(colorTheme.sidebarColor, colorTheme.backgroundColor, sideFrameTransparency, PT_LINEAR, PT_IN);
		fringeColor = PT_TWEEN_PT_COLOR(main->backgroundColor, fringeColor, main->backgroundTransparency, PT_LINEAR, PT_IN);

		EDITOR_LIST_ELEMENT listElement = editor->listElement;
		listElement.header->textColor = colorTheme.textColor;
		listElement.header->fringeColor = fringeColor;

		listElement.desc->textColor = PT_COLOR_lerp(colorTheme.borderColor, colorTheme.accentColor, .2);
		listElement.desc->fringeColor = fringeColor;

		listElement.removeButton->imageTint = colorTheme.borderColor;
		listElement.removeButton->guiObj->backgroundColor = colorTheme.sidebarColor;

		PT_EXPANDABLE_ARRAY selectTweens = listElement.selectTweens;
		PT_EXPANDABLE_ARRAY deselectTweens = listElement.deselectTweens;


		TWEEN_CONFIG selectConfig = { 0 };
		selectConfig.direction = PT_OUT;
		selectConfig.type = PT_CUBIC;
		selectConfig.duration = .15f;

		TWEEN_CONFIG deselectConfig = { 0 };
		deselectConfig.direction = PT_IN;
		deselectConfig.type = PT_CUBIC;
		deselectConfig.duration = .15f;

		// update select tweens
		PT_TWEEN* tween = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&selectTweens, 0);
		PT_TWEEN_destroy(tween);
		tween = PT_TWEEN_PT_COLOR_new(SELECTED_COLOR, &main->backgroundColor, selectConfig);
		PT_EXPANDABLE_ARRAY_set(&selectTweens, 0, &tween);

		tween = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&selectTweens, 1);
		PT_TWEEN_destroy(tween);
		tween = PT_TWEEN_PT_COLOR_new(SELECTED_ACTIVE_COLOR, &main->activeBackgroundColor, selectConfig);
		PT_EXPANDABLE_ARRAY_set(&selectTweens, 1, &tween);


		// update deselect tweens
		tween = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&deselectTweens, 0);
		PT_TWEEN_destroy(tween);
		tween = PT_TWEEN_PT_COLOR_new(DESELECTED_COLOR, &main->backgroundColor, deselectConfig);
		PT_EXPANDABLE_ARRAY_set(&deselectTweens, 0, &tween);

		tween = *(PT_TWEEN**)PT_EXPANDABLE_ARRAY_get(&deselectTweens, 1);
		PT_TWEEN_destroy(tween);
		tween = PT_TWEEN_PT_COLOR_new(DESELECTED_ACTIVE_COLOR, &main->activeBackgroundColor, deselectConfig);
		PT_EXPANDABLE_ARRAY_set(&deselectTweens, 1, &tween);
	}


	sideBarObj->backgroundColor = colorTheme.sidebarColor;

	sidebarHeader->textColor = colorTheme.textColor;
	sidebarHeader->fringeColor = colorTheme.sidebarColor;

	statusBarLabel->textColor = colorTheme.textColor;
	statusBarLabel->fringeColor = colorTheme.sidebarColor;
	statusBarLabel->guiObj->backgroundColor = colorTheme.sidebarColor;

	menuButton->guiObj->backgroundColor = colorTheme.sidebarColor;
	saveButton->guiObj->backgroundColor = colorTheme.sidebarColor;
	saveAsButton->guiObj->backgroundColor = colorTheme.sidebarColor;
	wordWrapButton->guiObj->backgroundColor = colorTheme.sidebarColor;
	themeButton->guiObj->backgroundColor = colorTheme.sidebarColor;
	newFileButton->guiObj->backgroundColor = colorTheme.sidebarColor;

	menuButton->imageTint = colorTheme.borderColor;
	saveButton->imageTint = colorTheme.borderColor;
	saveAsButton->imageTint = colorTheme.borderColor;
	wordWrapButton->imageTint = colorTheme.borderColor;
	themeButton->imageTint = colorTheme.borderColor;
	newFileButton->imageTint = colorTheme.borderColor;

	backgroundObj->backgroundColor = colorTheme.backgroundColor;
}
