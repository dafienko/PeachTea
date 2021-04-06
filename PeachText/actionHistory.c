#include "actionHistory.h"
#include "textCursor.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

TEXT_ACTION* TEXT_ACTION_new(char* data, int dataLen, int numRanges, TEXT_ACTION_TYPE type, TEXT_ACTION* lastAction, int* currentActionIndex) {
	TEXT_ACTION* action = calloc(1, sizeof(TEXT_ACTION));
	
	if (dataLen) {
		action->dataLen = dataLen;
		action->data = calloc(dataLen + 1, sizeof(char)); // + 1 for null terminator
		memcpy(action->data, data, dataLen * sizeof(char));
	}

	action->ranges = PT_EXPANDABLE_ARRAY_new(numRanges, sizeof(TEXT_ACTION));

	action->type = type;

	action->actionId = *currentActionIndex;
	*currentActionIndex = action->actionId + 1;

	return action;
}

vec2i TEXT_ACTION_undo(TEXT_ACTION* action) {
	vec2i newCursorPos = { 0 };
	
	switch (action->type) {
	case TA_INSERT: // to undo an insertion, remove the range of characters that insertion occupies
		for (int i = 0; i < action->ranges.numElements; i++) {
			ACTION_RANGE range = *(ACTION_RANGE*)PT_EXPANDABLE_ARRAY_get(&action->ranges, i);
			printf("undoing insertion %i from range (%i, %i) to (%i, %i)\n", action->actionId, range.start.x, range.start.y, range.end.x, range.end.y);
			remove_str_from_text_array(action->textLines, range.start, range.end);
			newCursorPos = range.start;
		}
		break;
	case TA_DELETE: // to undo a deletion, re-insert the removed characters at the deletion start
		for (int i = 0; i < action->ranges.numElements; i++) {
			ACTION_RANGE range = *(ACTION_RANGE*)PT_EXPANDABLE_ARRAY_get(&action->ranges, i);
			printf("undoing deletion %i from range (%i, %i) to (%i, %i)\n", action->actionId, range.start.x, range.start.y, range.end.x, range.end.y);
			insert_str_in_text_array(action->textLines, range.start, action->data, action->dataLen);
			newCursorPos = range.end;
		}
		break;
	}

	return newCursorPos;
}

vec2i TEXT_ACTION_do(TEXT_ACTION* action) {
	vec2i newCursorPos = { 0 };

	switch (action->type) {
	case TA_INSERT:
		for (int i = action->ranges.numElements; i >= 0; i--) {
			ACTION_RANGE range = *(ACTION_RANGE*)PT_EXPANDABLE_ARRAY_get(&action->ranges, i);
			printf("redoing insertion %i from range (%i, %i) to (%i, %i)\n", action->actionId, range.start.x, range.start.y, range.end.x, range.end.y);
			insert_str_in_text_array(action->textLines, range.start, action->data, action->dataLen);
			newCursorPos = range.end;
		}
		break;
	case TA_DELETE:
		for (int i = action->ranges.numElements; i >= 0; i--) {
			ACTION_RANGE range = *(ACTION_RANGE*)PT_EXPANDABLE_ARRAY_get(&action->ranges, i);
			printf("redoing deletion %i from range (%i, %i) to (%i, %i)\n", action->actionId, range.start.x, range.start.y, range.end.x, range.end.y);
			remove_str_from_text_array(action->textLines, range.start, range.end);
			newCursorPos = range.start;
		}
		break;
	}

	return newCursorPos;
}

void TEXT_ACTION_destroy(TEXT_ACTION* action) {
	if (action->data) {
		free(action->data);
		action->data = NULL;
	}

	PT_EXPANDABLE_ARRAY_destroy(&action->ranges);

	free(action);
}

void TEXT_ACTION_append(TEXT_ACTION** pFirstAction, TEXT_ACTION** pLastAction, TEXT_ACTION* thisAction, int* pCurrentActionIndex) {
	if (*pFirstAction) {
		int diff = thisAction->actionId - (*pFirstAction)->actionId;

		// if the old first action is more than n actions away from thisAction, remove it and retarget the new firstAction
		if (diff >= MAX_ACTION_CACHE_LENGTH) {
			TEXT_ACTION* nextAction = (*pFirstAction)->nextAction;
			TEXT_ACTION_destroy(*pFirstAction); // free the old first action
		}
	}

	*pFirstAction = thisAction;

	// free any actions that get overwritten by thisAction
	TEXT_ACTION* lastAction = *pLastAction;
	while (lastAction && lastAction->actionId >= thisAction->actionId) {
		TEXT_ACTION* newLastAction = lastAction->lastAction;
		TEXT_ACTION_destroy(lastAction);
		lastAction = newLastAction;
	}

	// link thisAction to the end of the chain
	thisAction->lastAction = lastAction;
	if (lastAction) {
		lastAction->nextAction = thisAction;
	}
	// retarget pLastAction to thisAction
	*pLastAction = thisAction;

	*pCurrentActionIndex = thisAction->actionId + 1;
}