GL_Particles
============

Particles using compute shaders done in OpenGL 4.3


Disclaimer
----------
This is my first OpenGL work so I wouldn't take it as great learning material.

I might have done some noob-ish mistakes. If I did please tell me and I will correct them.

Be sure you have the latest drivers.

Movement
--------

W - move forward

S - move backward

Hold Right Mouse Button in order to rotate the camera.


How it works
------------

Nothing fancy here, I just wanted to learn some OpenGL and this is what happened.

All the collision detection and physics is done using compute shaders.
Collision detection simply checks if we intersect a circle ( or the planes ) and adjusts the velocity and speed of the particles.

I use 2 shader storage buffer objects: one for speed and one for velocity. Each of them holds some additional data too.
This is their structure:

Velocity: 'VelocityX, VelocityY, VelocityZ, RemainingTime'(in seconds)
Position: 'PositionX, PositionY, PositionZ, Speed'

Remaining time is not used. For now I just re-spawn particles in a random location after they touch the "ground".


Third Party Libs
----------------

GLUT, GLEW, SDL and GLM are just awesome :)


Future work
-----------

I plan to add models in the scene and collide with a distance field representation of them.

I can't promise anything :).
