#include "ComponentMaterial.h"

ComponentMaterial::ComponentMaterial(aiMaterial * mat, GameObject * par)
{
	// Default Material Color
	aiColor3D color;
	mat->Get(AI_MATKEY_COLOR_DIFFUSE, color);
	default_print = { color.r, color.g, color.b, 1 };

	// For future property things
	this->NumProperties = mat->mNumProperties;
	this->NumDiffTextures = mat->GetTextureCount(aiTextureType_DIFFUSE);

	for (int i = 0; i < this->NumDiffTextures; i++)
	{
		aiString path;
		aiReturn err = mat->GetTexture(aiTextureType_DIFFUSE, 0, &path);
		Texture* diff = new Texture;
		LoadTexture(path.C_Str(), diff);
	}
}

void ComponentMaterial::LoadTexture(const char * path, Texture * tex)
{



}
