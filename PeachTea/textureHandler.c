#include "imageLoader.h"
#include "glExtensions.h"

PT_IMAGE PT_IMAGE_index_spritemap(PT_IMAGE spritemap, vec2i position, vec2i size, int unitSizePx) {
	vec2i topLeft = (vec2i){ position.x * unitSizePx, position.y * unitSizePx };
	vec2i subImageSize = (vec2i){ size.x * unitSizePx, size.y * unitSizePx };
	vec2i bottomRight = vector_add_2i(topLeft, subImageSize);

	PT_IMAGE image = { 0 };
	image.texId = spritemap.texId;
	image.topLeft = topLeft;
	image.bottomRight = bottomRight;
	image.totalImageSize = spritemap.totalImageSize;

	return image;
}

PT_IMAGE PT_IMAGE_from_image_bit_data(image_bit_data* ibd) {
	PT_IMAGE img = { 0 };
	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA,
		ibd->width, ibd->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		ibd->lpBits
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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