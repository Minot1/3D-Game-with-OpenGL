#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/text2D.hpp>
#include <common/heart.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>


using namespace glm;

GLFWwindow* window;

int main(void)
{
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		getchar();
		return -1;
	}

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(1024, 768, "GraphicsRunner", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window, 1024 / 2, 768 / 2);

	// sky is the limit
	glClearColor(0.2f, 0.5f, 0.5f, 0.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	float xpos = 0.0f, ypos = 0.0f, zpos = 0.0f, speed = 3.0f;
	glm::vec3 cameraPos = glm::vec3(0, 4, 16);
	float cameraSpeed = 11.0f;
	float mouseSpeed = 0.001f;
	float horizontalAngle = 3.14f;
	float verticalAngle = 0.0f;
	float initialFoV = 45.0f;
	float player_score = 0;
	int player_hearts = 3;
	bool isGameOver = false;

	int tmp = 0;
	std::vector<glm::vec3> cube_positions;
	for (float i = 20.0f; i > -520.0f; i -= 2.0f)
	{
		cube_positions.push_back(glm::vec3(-5.0f, 0.0f, i));
		cube_positions.push_back(glm::vec3(5.0f, 0.0f, i));
		cube_positions.push_back(glm::vec3(-5.0f, 2.0f, i));
		cube_positions.push_back(glm::vec3(5.0f, 2.0f, i));
		cube_positions.push_back(glm::vec3(-5.0f, 4.0f, i));
		cube_positions.push_back(glm::vec3(5.0f, 4.0f, i));
		if (tmp % 7 == 0)
		{
			cube_positions.push_back(glm::vec3(3.0f, 0.0f, i));
			cube_positions.push_back(glm::vec3(-3.0f, 0.0f, i));
		}
		tmp += 1;
	}

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);


	GLuint programID = LoadShaders("StandardShading.vertexshader", "StandardShading.fragmentshader");
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	GLuint Texture = loadBMP_custom("brickwall.bmp");
	GLuint TextureID = glGetUniformLocation(programID, "myTextureSampler");

	// Read our .obj file
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);

	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), &uvs[0], GL_STATIC_DRAW);

	GLuint normalbuffer;
	glGenBuffers(1, &normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(glm::vec3), &normals[0], GL_STATIC_DRAW);

	initHeart2D("heart-png-38789.bmp");

	initText2D("Holstein.DDS");

	// Get a handle for our "LightPosition" uniform
	glUseProgram(programID);
	GLuint LightID = glGetUniformLocation(programID, "LightPosition_worldspace");

	// main loop
	do {
		static double lastTime = glfwGetTime();
		double currentTime = glfwGetTime();
		float deltaTime = float(currentTime - lastTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programID);
		GLuint loc = glGetUniformLocation(programID, "MVP");

		double mouseX, mouseY;
		glfwGetCursorPos(window, &mouseX, &mouseY);
		glfwSetCursorPos(window, 1024 / 2, 768 / 2);

		horizontalAngle += mouseSpeed * float(1024 / 2 - mouseX);
		verticalAngle += mouseSpeed * float(768 / 2 - mouseY);

		glm::vec3 direction = glm::vec3(
			cos(verticalAngle) * sin(horizontalAngle),
			sin(verticalAngle),
			cos(verticalAngle) * cos(horizontalAngle)
		);

		glm::vec3 right = glm::vec3(
			sin(horizontalAngle - 3.14f / 2.0f),
			0,
			cos(horizontalAngle - 3.14f / 2.0f)
		);

		glm::vec3 up = glm::cross(right, direction);

		glm::mat4 mdl = glm::mat4(1.0f); //ModelMatrix
		mdl = glm::translate(mdl, glm::vec3(xpos, ypos, zpos));

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
			zpos -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
			xpos -= speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
			zpos += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
			xpos += speed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			cameraPos += direction * cameraSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			cameraPos -= right * cameraSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			cameraPos -= direction * cameraSpeed * deltaTime;
		}
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			cameraPos += right * cameraSpeed * deltaTime;
		}

		glm::mat4 vw = glm::lookAt( //ViewMatrix
			cameraPos,
			cameraPos + direction,
			up
		);

		glm::mat4 prjction = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 300.0f);

		glm::mat4 movp = prjction * vw * mdl;

		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &movp[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &mdl[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &vw[0][0]);

		//Light stuff
		glm::vec3 lightPos = glm::vec3(0, 10, 10);
		glUniform3f(LightID, lightPos.x, lightPos.y, lightPos.z);

		// Bind our texture in Texture Unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		// Set our "myTextureSampler" sampler to use Texture Unit 0
		glUniform1i(TextureID, 0);

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			2,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// 3rd attribute buffer : normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, normalbuffer);
		glVertexAttribPointer(
			2,                                // attribute
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, vertices.size());

		for (size_t i = 0; i < cube_positions.size(); i++)
		{
			glm::vec3 tmpvec = cube_positions[i];
			if (tmpvec.x == 3.0f)
			{
				tmpvec = glm::vec3(tmpvec.x, tmpvec.y, tmpvec.z + glfwGetTime() * 4);
				glm::mat4 second_model = glm::mat4(1.0f);
				second_model = glm::translate(second_model, tmpvec);
				glm::mat4 second_mvp = prjction * vw * second_model;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &second_mvp[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &second_model[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &vw[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
			else if (tmpvec.x == -3.0f)
			{
				tmpvec = glm::vec3(tmpvec.x, tmpvec.y, tmpvec.z + glfwGetTime() * 4 + 7);
				glm::mat4 second_model = glm::mat4(1.0f);
				second_model = glm::translate(second_model, tmpvec);
				glm::mat4 second_mvp = prjction * vw * second_model;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &second_mvp[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &second_model[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &vw[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
			else
			{
				glm::mat4 second_model = glm::mat4(1.0f);
				second_model = glm::translate(second_model, tmpvec);
				glm::mat4 second_mvp = prjction * vw * second_model;
				glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &second_mvp[0][0]);
				glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &second_model[0][0]);
				glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &vw[0][0]);
				glDrawArrays(GL_TRIANGLES, 0, vertices.size());
			}
		}


		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);

		if (!isGameOver)
		{
			player_score += 100 * deltaTime;
			char score_text[256];
			sprintf(score_text, "Score: %.0f", player_score);
			printText2D(score_text, 10, 570, 20);

			char lives_text[256];
			sprintf(lives_text, "Lives:");
			printText2D(lives_text, 550, 570, 20);
		}

		if (player_hearts >= 1)
		{
			printHeart2D(675, 560, 30);
		}
		if (player_hearts >= 2)
		{
			printHeart2D(715, 560, 30);
		}
		if (player_hearts >= 3)
		{
			printHeart2D(755, 560, 30);
		}

		// Collision
		for (size_t i = 0; i < cube_positions.size(); i++)
		{
			glm::vec3 tmpvec = cube_positions[i];
			if (tmpvec.x == 3.0f)
			{
				tmpvec = glm::vec3(tmpvec.x, tmpvec.y, tmpvec.z + glfwGetTime() * 4);
			}
			else if (tmpvec.x == -3.0f)
			{
				tmpvec = glm::vec3(tmpvec.x, tmpvec.y, tmpvec.z + glfwGetTime() * 4 + 7);
			}
			if (abs(xpos - tmpvec.x) < 2 && abs(zpos - tmpvec.z) < 2)
			{
				player_hearts -= 1;
				xpos = 0;
				ypos = 0;
				zpos = 0;
				break;
			}
		}

		if (player_hearts == 0)
		{
			isGameOver = true;
			xpos = 0;
			ypos = 0;
			zpos = 1000;
			char gameOverText[256];
			sprintf(gameOverText, "Game Over!");
			printText2D(gameOverText, 265, 300, 30);
			char score_text[256];
			sprintf(score_text, "Score: %.0f", player_score);
			printText2D(score_text, 265, 270, 30);
			cameraPos = glm::vec3(cameraPos.x, cameraPos.y + 0.003f, cameraPos.z + 0.003f);
		}

		printf("X,Y,Z: %f %f %f\n", xpos, ypos, zpos);
		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		lastTime = currentTime;
	} // Check if the ESC key was pressed or the window was closed
	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS &&
		glfwWindowShouldClose(window) == 0);

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &uvbuffer);
	glDeleteBuffers(1, &normalbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1, &Texture);
	glDeleteVertexArrays(1, &VertexArrayID);

	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

