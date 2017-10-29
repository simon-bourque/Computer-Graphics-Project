#include "Texture.h"


Texture::Texture(GLuint texture, Type type) : m_texture(texture), m_type(type) {}

Texture::~Texture() {}

void Texture::bind(Unit unit) {
	glActiveTexture(unit);
	glBindTexture(m_type, m_texture);
}
