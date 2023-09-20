uniform sampler2D tex;
uniform float time;

varying vec2 uv;

#define PI 3.14159265359

mat2 rotate(float angle) {
  return mat2(cos(angle), -sin(angle), sin(angle), cos(angle));
}

void main() {
  float maxStrength = 0.0125;
  float minStrength = 0.0050;

  // Animation strength
  maxStrength = clamp(sin(time), minStrength, maxStrength) * 2.0;

  // Shift UVs
  vec2 newUV = vec2(uv.x + sin(time * 5.0) * maxStrength, uv.y + sin(time * 20.0) * maxStrength);

  float strongComponent = abs(sin(time)) < abs(cos(time)) ? cos(time) * sign(time) : sin(time) * 2.0;

  newUV = rotate(sin(time * 2.5) * PI * maxStrength * strongComponent) * newUV;

  // Transform the fragment
  vec4 background = texture(tex, newUV);

  // Shift each of the fragment's color channels separately; optional
  // background.r = texture(tex, vec2(newUV.x + maxStrength, newUV.y + maxStrength)).r;
  // background.g = texture(tex, vec2(newUV.x - maxStrength, newUV.y + maxStrength)).g;
  // background.b = texture(tex, vec2(newUV.x - maxStrength, newUV.y - maxStrength)).b;

  gl_FragColor = background;
}
