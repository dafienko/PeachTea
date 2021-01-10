#pragma once
#ifndef PT_SHADERS_H 
#define PT_SHADERS_H

unsigned int* qVAO;
unsigned int* qVBO;

float DEFAULT_QUAD_CORNERS[8];
unsigned int PTS_text, PTS_guiObj, PTS_img, PTS_blur, PTS_tex, PTS_rect;

void PT_SHADERS_init(); // init core shaders

#endif
