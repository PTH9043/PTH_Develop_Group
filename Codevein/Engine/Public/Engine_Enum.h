#pragma once

typedef enum ETextureType
{
	TextureType_NONE = 0,
	TextureType_DIFFUSE = 1,
	TextureType_SPECULAR = 2,
	TextureType_AMBIENT = 3,
	TextureType_EMISSIVE = 4,
	TextureType_HEIGHT = 5,
	TextureType_NORMALS = 6,
	TextureType_SHINNESS = 7,
	TextureType_OPACITY = 8,
	TextureType_DISPLACEMENT = 9,
	TextureType_LIGHTMAP = 10,
	TextureType_REFLECTION = 11,
	TextureType_BASE_COLOR = 12,
	TextureType_NORMAL_CAMERA = 13,
	TextureType_EIMISSION_COLOR = 14,
	TextureType_METALNESS = 15,
	TextureType_DIFFUSE_ROUGHTNESS = 16,
	TextureType_AMBIENT_OCCLUSION = 17,
	TextureType_UNKNOWN = 18
}TEXTYPE;