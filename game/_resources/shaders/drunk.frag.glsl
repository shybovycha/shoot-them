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
  vec2 uv1 = vec2(uv.x + sin(time * 5.0) * maxStrength, uv.y - sin(time * 7.0) * maxStrength);
  vec2 uv2 = vec2(uv.x - sin(time * 4.0) * maxStrength, uv.y + sin(time * 2.0) * maxStrength);
  vec2 uv3 = vec2(uv.x - sin(time * 3.0) * maxStrength, uv.y - sin(time * 5.0) * maxStrength);

  float strongComponent = abs(sin(time)) < abs(cos(time)) ? cos(time) * sign(time) : sin(time) * 2.0;

  float alpha = clamp(abs(sin(time)) < abs(cos(time)) ? cos(time) * sign(time) : sin(time), 0.0, 1.0);

  // Transform the fragment
  gl_FragColor = vec4(texture(tex, uv1).b, texture(tex, uv2).r, texture(tex, uv3).g, alpha);
}
