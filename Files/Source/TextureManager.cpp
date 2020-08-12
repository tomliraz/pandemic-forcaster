#include "TextureManager.h"

unordered_map<string, sf::Texture> TextureManager::textures;

/* ================ Accessor ================ */
sf::Texture& TextureManager::GetTexture(string key)
{
	if (textures.find(key) == textures.end())
		throw std::invalid_argument("invalid key");
	return textures[key];
}

/* ================ Mutator ================ */
void TextureManager::SetTexture(string textureName, sf::Image image)
{
	textures[textureName].loadFromImage(image);
}
