#ifndef ACTION_HISTORY_H
#define ACTION_HISTORY_H

#include "PeachTea.h"
#include "textEditorHandler.h"

typedef struct {
	vec2i start;
	vec2i end;
} ACTION_RANGE;

#define MAX_ACTION_CACHE_LENGTH 100

TEXT_ACTION* TEXT_ACTION_new(char* data, int dataLen, int numRanges, TEXT_ACTION_TYPE type, TEXT_ACTION* lastAction, int* currentActionIndex);
vec2i TEXT_ACTION_undo(TEXT_ACTION* action);
vec2i TEXT_ACTION_do(TEXT_ACTION* action);
void TEXT_ACTION_destroy(TEXT_ACTION* action);

void TEXT_ACTION_append(TEXT_ACTION** pFirstAction, TEXT_ACTION** pLastAction, TEXT_ACTION* thisAction, int* pCurrentActionIndex);

#endif