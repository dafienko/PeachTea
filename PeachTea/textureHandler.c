#include "imageLoader.h"
#include "glExtensions.h"

PT_IMAGE PT_IMAGE_from_image_bit_data(image_bit_data* ibd) {
	PT_IMAGE img = { 0 };
	
	GLuint tex;
	glGenTextures(1, &tex);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		ibd->width, ibd->height,
		0,
		GL_BGRA,
		GL_UNSIGNED_BYTE,
		ibd->lpBits
	);

	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	img.bottomRight = (vec2i){ ibd->width, ibd->height };
	img.topLeft = (vec2i){ 0, 0 };
	img.texId = tex;
	img.totalImageSize = img.bottomRight;

	return img;
}

PT_IMAGE PT_IMAGE_from_png(const char* filename) {
	image_bit_data ibd = read_png_file_simple(filename);

	PT_IMAGE img = PT_IMAGE_from_image_bit_data(&ibd);

	free_image_bit_data(&ibd);

	return img;
}