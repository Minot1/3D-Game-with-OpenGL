# GraphicsRunner - 3D Game with OpenGL

A 3D Game made in C++ and OpenGL by Taner Dinçer and Murat Karaca.
This game inspired from mobile game "Temple Run". In the game, there are 3 corridors for dodging obstacles which come to the player and gaining scores. 
The game contains basic features of 3D game such as 3D viewing and objects, User Inputs like moving character as W/S/A/D keys and jumping as Space key, Lighting and Smooth Shading on objects and Texture Mapping on in-game models. Moreover, the game includes advanced features such as:

* **On-screen control panel**: implemented the score and player’s lives on the screen. The texts are loaded from .dds file which contains letters and numbers and we do not use any external libraries to render on screen. Also, the scores are increasing if the player has more than 1 live. If the player loses all 3 lives, the game over screen is rendered into the middle of the screen.
* **Collision detection**: used the Bounding Boxes method for our collision detection. Even though the monkey objects have a complex shape and lots of faces, we used a bounding box of 2x2x2 for a higher efficiency.
* **Simulated dynamics**: The player has speed, acceleration and gravity that are used to jump over obstacles and bounce from the wall. For applying efficient bouncing and jumping actions, we added friction force that limits the speed, acceleration and gravity.
* **Level of detail control**: The monkey objects have 2 different models which are 968 and 12 respectively. The complex one is rendered if the player is near 150 coordinates. Otherwise, they are rendered as a cube, which is not separable from distance. As a result, we get a higher efficiency in terms of model rendering.

* **External tutorials and resources**: openGL-tutorial, learn-opengl, combined some of the shaders, https://youtu.be/45MIykWJ-C4
* **Libraries**: GLFW, GLEW, GLM, GLAD

* **Pictures and more information**: https://docs.google.com/document/d/1P_fLhbd4mbMCwmuYFc5erL137seBsvL2tWnAuvcPRaU/edit?usp=sharing
