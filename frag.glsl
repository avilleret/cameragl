precision mediump float; 
varying vec2 TexCoordOut;
uniform sampler2D Texture;
uniform float invert; // unfortunately, Raspberry Pi driver doesn't seems to support uniform initializer...
uniform float alpha;
uniform float gain;
void main() 
{ 
  vec4 color = texture2D(Texture, TexCoordOut);
  color.rgb = color.rgb * (1. - invert) + invert * (vec3(1.,1.,1.) - color.rgb);
  color.rgb*=gain;
  color.a=alpha;
  gl_FragColor = color; 
}

