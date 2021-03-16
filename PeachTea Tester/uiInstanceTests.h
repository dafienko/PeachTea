#ifndef UI_INSTANCE_TESTS_H
#define UI_INSTANCE_TESTS_H


#include "PeachTea.h"

typedef struct {
	Instance** instances;
	int numInstances;
	vec2i size;
	Instance* parent;
} TEST_GRID;

TEST_GRID generate_test_grid(int w, int h, Instance* parent);

void TEST_GRID_update();

void TEST_GRID_free(TEST_GRID grid);

void test_ui_instances();

#endif