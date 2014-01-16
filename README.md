GL_Particles
============

Particles using compute shaders done in OpenGL 4.3
Disclaimer
----------
This is my first OpenGL work so I wouldn't take it as great learning material.
I might have done some noob-ish mistakes.


Movement
--------

W - move forward
S - move backward
Hold Right Mouse Button in order to rotate the camera.


How it works
------------

Nothing fancy here, I just wanted to learn some OpenGL and this is what happened.

All the collision detection and physics is done using compute shaders.
Collision detection simply checks if we intersect a circle ( or the planes).

The main idea was to have 2 texture pairs of Position/Velocity.
One pair represents the input, and the other the output.
I swap between them after each shader program invocation.
The output from the previous frame becomes input for the next one and viceversa.

The speed and velocity vectors look like this:
Velocity: 'VelocityX, VelocityY, VelocityZ, RemainingTime'(in seconds)
Position: 'PositionX, PositionY, PositionZ, Speed'

I gave up the idea of 'RemainingTime' for each particle because I got bore fast trying to make it look right.

Third Party Libs
----------------
GLUT, GLEW, SDL and GLM are just awesome :)

Future work
-----------

If I get some free time and I am not distracted by other projects maybe in the future I will make collision detection work with a distance field represented as a 3D texture.
I can't promise anything :).