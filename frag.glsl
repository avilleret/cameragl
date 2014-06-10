precision mediump float; // nécessaire pour la Pi ?
varying vec2 TexCoordOut;
uniform sampler2D Texture;
uniform float invert; // unfortunately, Raspberry Pi driver doesn't seems to support uniform initializer...
uniform float white;
uniform float gamma;

void main() 
{ 
  vec4 color = texture2D(Texture, TexCoordOut); //  version RPi
  //~vec4 color = texture2D(Texture, (gl_TextureMatrix[0] * gl_TexCoord[0]).st); // version Gem
  color.a  = 1.;
  
  // expansion de gamma pour la conversion en niveau de gris
  vec3 color_gamma = pow(color.rgb,vec3( gamma ));

  // conversion en niveau de gris inversé
  float ibw = 1. - (0.299 * color_gamma.r + 0.587 * color_gamma.g + 0.114 * color_gamma.b); // conversion en 

  color.rgb = mix(color.rgb, vec3(ibw,ibw,ibw), invert);
  color.rgb = mix(color.rgb, vec3(1.,1.,1.), white);

  gl_FragColor = color; 
}

