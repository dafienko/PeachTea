#include "uiInstanceTests.h"
#include <stdio.h>
#include <stdlib.h>


PT_SCREEN_UI* screenUI;
TEST_GRID currentGrid;
vec2i expandSize;

void event_callback1(void* args) {
	printf("callback 1\n");
}

void event_callback2(void* args) {
	printf("callback 2\n");
}

void event_callback3(void* args) {
	printf("callback 3\n");
}

void on_activate(void* args) {
	PT_GUI_OBJ* pushedObj = (PT_GUI_OBJ*)args;

	for (int x = 0; x < currentGrid.size.x; x++) {
		for (int y = 0; y < currentGrid.size.y; y++) {
			Instance* instance = *(currentGrid.instances + y * currentGrid.size.x + x);
			if (pushedObj == instance->subInstance) {
				expandSize = (vec2i){
					x + 1,
					y + 1
				};
			}
		}
	}
}

void TEST_GRID_update() {
	if (expandSize.x > 0 || expandSize.y > 0) {
		//vec2i newSize = vector_add_2i(currentGrid.size, expandSize);
		vec2i newSize = currentGrid.size;
		Instance* parent = currentGrid.parent;
		TEST_GRID_free(currentGrid);

		printf("regenerating grid\n");

		generate_test_grid(newSize.x, newSize.y, parent);

		PT_SCREEN_UI_update_rendertree((PT_SCREEN_UI*)parent->subInstance);
		
		expandSize = (vec2i){ 0 };
	}
}

TEST_GRID generate_test_grid(int w, int h, Instance* parent) {
	TEST_GRID grid = { 0 };

	Instance** instances = calloc(w * h, sizeof(Instance*));

	for (int x = 0; x < w; x++) {
		for (int y = 0; y < h; y++) {
			Instance* instance = PT_GUI_OBJ_new();
			PT_GUI_OBJ* obj = (PT_GUI_OBJ*)instance->subInstance;
			obj->size = PT_REL_DIM_new(.9f / w, 0, .9f / h, 0);
			obj->anchorPosition = (vec2f){ .5, .5 };
			obj->position = PT_REL_DIM_new(
				x / (float)w + .5f / w, 0,
				y / (float)h + .5f / h, 0
			);

			obj->backgroundColor = PT_COLOR_fromHSV(0, 0, .7);
			obj->borderWidth = 1;
			obj->borderColor = PT_COLOR_fromHSV(0, 0, 1);

			PT_BINDABLE_EVENT_bind(&obj->e_obj_activated, on_activate);

			set_instance_parent(instance, parent);

			*(instances + y * w + x) = instance;
		}
	}

	grid.instances = instances;
	grid.numInstances = w * h;
	grid.size = (vec2i){ w, h };
	grid.parent = parent;

	currentGrid = grid;
	return grid;
}

void TEST_GRID_free(TEST_GRID grid) {
	for (int i = 0; i < grid.numInstances; i++) {
		Instance* instance = *(grid.instances + i);

		destroy_instance(instance);
	}

	if (grid.instances) {
		free(grid.instances);
	}

	currentGrid = (TEST_GRID){ 0 };
}

void on_update(float dt) {
	TEST_GRID_update();
}

void on_render() {
	PT_SCREEN_UI_render(screenUI);
}

void on_close() {
	TEST_GRID_free(currentGrid);
}

void test_ui_instances() {
	expandSize = (vec2i){ 0 };
	currentGrid = (TEST_GRID){ 0 };

	PT_CREATE_MAIN_WND((vec2i) { 1100, 800 }, "PeachText", NULL);

	screenUI = PT_SCREEN_UI_new()->subInstance;

	generate_test_grid(50, 50, screenUI->instance);

	PT_SCREEN_UI_update_rendertree(screenUI);

	PT_RUN(on_update, on_render);

	on_close();
}