#pragma once
#ifndef PT_SHADERS_H 
#define PT_SHADERS_H

unsigned int* qVAO;
unsigned int* qVBO;

unsigned int PTS_text, PTS_guiObj, PTS_img, PTS_blur, PTS_tex, PTS_rect, PTS_textSpritemapGenerator;

void PT_SHADERS_init(); // init core shaders

#endif
