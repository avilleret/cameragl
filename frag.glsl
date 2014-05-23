precision mediump float; 
varying vec2 TexCoordOut;
uniform sampler2D Texture;
uniform float invert; // unfortunately, Raspberry Pi driver doesn't seems to support uniform initializer...
uniform float alpha;
uniform float gain;
uniform float offset_r,offset_g,offset_b;
uniform float gain_r,gain_g,gain_b;
uniform float hue, saturation;

vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() 
{ 
  vec4 color = texture2D(Texture, TexCoordOut);
  color.rgb = color.rgb * (1. - invert) + invert * (vec3(1.,1.,1.) - color.rgb);
  color.rgb += vec3(offset_r,offset_g,offset_b);
  color.rgb *= vec3(gain_r, gain_g, gain_b);
  color.rgb*=gain;
  
  vec3 hsv = rgb2hsv(color.rgb);
  hsv.xy*=vec2(hue,saturation);
  color.rgb=hsv2rgb(hsv);
  
  color.a=alpha;
  gl_FragColor = color; 
}
