cameragl
===============

This is a real-time video processing application for the Raspbery Pi.

It opens the Raspi camera and pass it through a fragment shader.
All the shader's uniform variables are automatically exposed via OSC.

# building
This repo is intended to be cloned in the `/opt/vc/src/hello_pi` folder present on Rasbian distribution.
You should follow the `/opt/vc/src/hello_pi/README` first.
Then you need to install the `liblo` library :
`sudo apt-get install liblo-dev`
And then run `make`

# using
Modify the frag.glsl shader just as you like.
But remember that the Raspberry Pi GPU driver is not fully compliant with OpenGL.
For example, I found no way to intialize uniform variables.
Moreover, the GLSL compiler is quite sensitive, and may fail because of a typo or removing the last 2 empty lines...
All compilation errors are reported to console.
