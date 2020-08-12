#pragma once
#include <SFML/graphics.hpp>
#include <unordered_map>
#include <string>

using std::unordered_map;
using std::string;

class TextureManager
{
	// Since sf::Sprite only stores a pointer to its sf::Texture, this class stores the texture so it does not go out of scope.
	static unordered_map<string, sf::Texture> textures;
	
public:
	/* ================ Accessor ================ */
	//O(textures.size())		Returns the texture from the given key.
	static sf::Texture& GetTexture(string key);
	
	//O(image.getSize().x * image.getSize().y + textures.size())	
	/* ================ Mutator ================ */
	static void SetTexture(string textureName, sf::Image image);
};