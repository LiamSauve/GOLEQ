#version 330 core
in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D lifeTexture; // R = alive (0..1), G = age (0..1 or more)
uniform float uTime;           // optional; 0 if you don't set it

// Simple HSV->RGB for nice age-based hues
vec3 hsv2rgb(vec3 c){
    vec3 p = abs(fract(c.x + vec3(0.0, 1.0/3.0, 2.0/3.0))*6.0 - 3.0);
    vec3 rgb = c.z * mix(vec3(1.0), clamp(p - 1.0, 0.0, 1.0), c.y);
    return rgb;
}

void main()
{
  vec4 data = texture(lifeTexture, texCoord);
  float alive = data.r;
  float age   = data.g;

  // Texel size for neighbor sampling
  vec2 texel = 1.0 / vec2(textureSize(lifeTexture, 0));

  // Sobel on the alive channel for glowing edges
  float tl = texture(lifeTexture, texCoord + vec2(-texel.x, -texel.y)).r;
  float  t = texture(lifeTexture, texCoord + vec2( 0.0    , -texel.y)).r;
  float tr = texture(lifeTexture, texCoord + vec2( texel.x, -texel.y)).r;
  float l  = texture(lifeTexture, texCoord + vec2(-texel.x,  0.0     )).r;
  float c  = alive;
  float r  = texture(lifeTexture, texCoord + vec2( texel.x,  0.0    )).r;
  float bl = texture(lifeTexture, texCoord + vec2(-texel.x,  texel.y)).r;
  float  b = texture(lifeTexture, texCoord + vec2( 0.0    ,  texel.y)).r;
  float br = texture(lifeTexture, texCoord + vec2( texel.x,  texel.y)).r;

  float gx = -tl - 2.0*l - bl + tr + 2.0*r + br;
  float gy = -tl - 2.0*t - tr + bl + 2.0*b + br;
  float edge = clamp(length(vec2(gx, gy)), 0.0, 3.0);

  // Perpendicular to gradient = local flow direction
  vec2 flowDir = normalize(vec2(-gy, gx) + 1e-5);
  float flowMag = (0.75 + 0.25 * sin(uTime * 0.8 + age * 6.28318)); // age modulates motion
  vec2 flow = flowDir * flowMag * 1.5 * texel;

  // Look-ahead sample: treats patterns (e.g. Testerino) like streaks
  float advAlive = texture(lifeTexture, texCoord + flow).r;
  float activity = max(alive, advAlive);

  // Age-driven hue; brighter where more active, edges get neon bloom
  float hue = mix(0.55, 0.95, clamp(age, 0.0, 1.0)); // cyan→magenta band
  vec3 base = hsv2rgb(vec3(hue, 0.85, mix(0.15, 1.0, activity)));

  // Neon edge glow
  float glow = smoothstep(0.25, 1.2, edge);
  vec3 color = base + glow * vec3(0.20, 0.90, 1.00);

  // Subtle scanline shimmer
  color *= 0.96 + 0.04 * sin((texCoord.y + uTime * 0.2) * float(textureSize(lifeTexture, 0).y) * 3.14159);

  // Gentle gamma lift
  fragColor = vec4(pow(color, vec3(0.9)), 1.0);
}