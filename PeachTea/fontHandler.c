#include "fontHandler.h"

typedef struct {
	PT_FONT font;
	int fontSize;

	char_set* cs;
} PT_LOADED_FONT;

int initialized;

PT_LOADED_FONT** loadedFonts;
int numLoadedFonts;
int loadedFontsSize;


void init_fontHandler() {
	loadedFontsSize = 10;
	numLoadedFonts = 0;
	loadedFonts = calloc(10, sizeof(PT_LOADED_FONT*));

	initialized = 1;
}

char_set* get_char_set(PT_FONT font, int fontSize) {
	if (!initialized) {
		init_fontHandler();
	}

	// check to see if the font is already loaded
	for (int i = 0; i < numLoadedFonts; i++) {
		PT_LOADED_FONT* f = *(loadedFonts + i);
		if (f->font == font && f->fontSize == fontSize) {
			return f->cs;
		}
	}

	// if we haven't returned by now, the font doesn't exist and it needs to be created

	// make sure there is room to store the created font
	if (numLoadedFonts + 1 > loadedFontsSize) {
		loadedFontsSize *= 2;
		loadedFonts = realloc(loadedFonts, loadedFontsSize * sizeof(PT_LOADED_FONT*));
	}

	char* fontPath = "";
	switch (font) {
	case PT_FONT_COMIC: 
		fontPath = "assets\\fonts\\comic.ttf";
		break;
	case PT_FONT_CONSOLA:
		fontPath = "assets\\fonts\\consola.ttf";
		break;
	case PT_FONT_CONSOLA_B:
		fontPath = "assets\\fonts\\consolab.ttf";
		break;
	case PT_FONT_TIMES:
		fontPath = "assets\\fonts\\times.ttf";
		break;
	}

	// create a new char_set
	char_set* charset = calloc(1, sizeof(char_set));
	char_set cs = create_char_set(fontPath, fontSize);
	memcpy(charset, &cs, sizeof(char_set));
	

	PT_LOADED_FONT* loadedFont = calloc(1, sizeof(PT_LOADED_FONT));
	loadedFont->cs = charset;
	loadedFont->fontSize = fontSize;
	loadedFont->font = font;

	// cache new font
	*(loadedFonts + numLoadedFonts) = loadedFont;

	numLoadedFonts += 1;

	return charset;
}