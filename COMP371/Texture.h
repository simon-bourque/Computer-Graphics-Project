#pragma once

#include <GL/glew.h>

class Texture {
	friend class TextureCache;
public:
	enum Type : GLenum {
		TEXTURE_2D = GL_TEXTURE_2D,
		TEXTURE_2D_ARRAY = GL_TEXTURE_2D_ARRAY
	};

	enum Unit : GLenum {
		UNIT_0 = GL_TEXTURE0,
		UNIT_1 = GL_TEXTURE1,
		UNIT_2 = GL_TEXTURE2,
		UNIT_3 = GL_TEXTURE3,
		UNIT_4 = GL_TEXTURE4
	};
private:
	GLuint m_texture;
	Type m_type;

	Texture(GLuint texture, Type type);
	virtual ~Texture();
public:
	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void bind(Unit unit);
};

