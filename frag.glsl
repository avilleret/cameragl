precision mediump float; 
varying vec2 TexCoordOut;
uniform sampler2D Texture;
uniform float invert;
void main() 
{ 
  vec4 color = texture2D(Texture, TexCoordOut);
  color.rgb = color.rbg * invert + (1. - invert) * (vec3(1.,1.,1.) - color.rgb);
  //~color.rgb = vec3(1.,1.,1.) - color.rgb;
  gl_FragColor = color; 
}
