#include <vector>
#include <cstring>

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace glm;

#include "shader.hpp"
#include "texture.hpp"

#include "heart.hpp"

unsigned int Heart2DTextureID;
unsigned int Heart2DVertexBufferID;
unsigned int Heart2DUVBufferID;
unsigned int Heart2DShaderID;
unsigned int Heart2DUniformID;

void initHeart2D(const char* texturePath) {

	// Initialize texture
	//Heart2DTextureID = loadDDS(texturePath);
	Heart2DTextureID = loadBMP_custom("toppng.com-heart-pixelart-game-retro-red-minecraft-life-pixel-corazon-de-8-bits-1024x865.bmp");

	// Initialize VBO
	glGenBuffers(1, &Heart2DVertexBufferID);
	glGenBuffers(1, &Heart2DUVBufferID);

	// Initialize Shader
	Heart2DShaderID = LoadShaders("imageshader.imageshader", "imagefragment.fragmentshader");

	// Initialize uniforms' IDs
	Heart2DUniformID = glGetUniformLocation(Heart2DShaderID, "myTextureSampler");

}

void printHeart2D(int x, int y, int size) {

	// Fill buffers
	std::vector<glm::vec2> vertices;
	std::vector<glm::vec2> UVs;

	glm::vec2 vertex_up_left = glm::vec2(x, y + size);
	glm::vec2 vertex_up_right = glm::vec2(x + size, y + size);
	glm::vec2 vertex_down_right = glm::vec2(x + size, y);
	glm::vec2 vertex_down_left = glm::vec2(x, y);

	vertices.push_back(vertex_up_left);
	vertices.push_back(vertex_down_left);
	vertices.push_back(vertex_up_right);

	vertices.push_back(vertex_down_right);
	vertices.push_back(vertex_up_right);
	vertices.push_back(vertex_down_left);

	glm::vec2 uv_up_left = glm::vec2(0.0f, 1.0f);
	glm::vec2 uv_up_right = glm::vec2(1.0f, 1.0f);
	glm::vec2 uv_down_right = glm::vec2(1.0f, 0.0f);
	glm::vec2 uv_down_left = glm::vec2(0.0f, 0.0f);

	UVs.push_back(uv_up_left);
	UVs.push_back(uv_down_left);
	UVs.push_back(uv_up_right);

	UVs.push_back(uv_down_right);
	UVs.push_back(uv_up_right);
	UVs.push_back(uv_down_left);

	glBindBuffer(GL_ARRAY_BUFFER, Heart2DVertexBufferID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec2), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, Heart2DUVBufferID);
	glBufferData(GL_ARRAY_BUFFER, UVs.size() * sizeof(glm::vec2), &UVs[0], GL_STATIC_DRAW);

	// Bind shader
	glUseProgram(Heart2DShaderID);

	// Bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, Heart2DTextureID);
	// Set our "myTextureSampler" sampler to use Texture Unit 0
	glUniform1i(Heart2DUniformID, 0);

	// 1rst attribute buffer : vertices
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, Heart2DVertexBufferID);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	// 2nd attribute buffer : UVs
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, Heart2DUVBufferID);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Draw call
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());

	glDisable(GL_BLEND);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

}

void cleanupHeart2D() {

	// Delete buffers
	glDeleteBuffers(1, &Heart2DVertexBufferID);
	glDeleteBuffers(1, &Heart2DUVBufferID);

	// Delete texture
	glDeleteTextures(1, &Heart2DTextureID);

	// Delete shader
	glDeleteProgram(Heart2DShaderID);
}
