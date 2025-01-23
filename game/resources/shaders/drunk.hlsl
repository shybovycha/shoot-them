float time;
float4x4 mWorldViewProj;  // World * View * Projection transformation
// float4x4 mInvWorld;       // Inverted world matrix
// float4x4 mTransWorld;     // Transposed world matrix

// Vertex shader output structure
struct VS_OUTPUT
{
  float4 Position   : POSITION;   // vertex position
  float2 TexCoord   : TEXCOORD0;  // tex coords
};

VS_OUTPUT vertexMain( in float4 vPosition : POSITION,
                      in float3 vNormal   : NORMAL,
                      float2 texCoord     : TEXCOORD0 )
{
  VS_OUTPUT Output;

  // transform position to clip space 
  Output.Position = mul(vPosition, mWorldViewProj);

  // transform normal 
  // float3 normal = mul(vNormal, mInvWorld);

  // renormalize normal 
  // normal = normalize(normal);

  // position in world coodinates
  // float3 worldpos = mul(mTransWorld, vPosition);

  Output.TexCoord = texCoord;

  return Output;
}

// Pixel shader output structure
struct PS_OUTPUT
{
    float4 RGBColor : COLOR0;  // Pixel color
};

sampler2D tex;

PS_OUTPUT pixelMain( float2 TexCoord : TEXCOORD0,
                     float4 Position : POSITION)
{ 
  PS_OUTPUT Output;

  float maxStrength = 0.0125;
  float minStrength = 0.0050;

  float st = sin(time);
  float ct = cos(time);

  // Animation strength
  maxStrength = clamp(st, minStrength, maxStrength) * 2.0;

  // Shift UVs
  float2 uv = TexCoord;
  float d1 = sin(time * 5.0);
  float d2 = sin(time * 7.0);
  float d3 = sin(time * 2.0);

  float2 uv1 = { uv.x + d1, uv.y - d2 };
  float2 uv2 = { uv.x - d1, uv.y + d3 };
  float2 uv3 = { uv.x - d3, uv.y - d1 };

  uv1 *= maxStrength;
  uv2 *= maxStrength;
  uv3 *= maxStrength;

  float x1 = ct; // abs(st) < abs(ct) ? ct * sign(time) : st;
  float strongComponent = x1 * 2.0;

  // float alpha = clamp(x1, 0.0, 1.0);
  float alpha = x1;

  float3 c1 = tex2D(tex, uv1);
  float3 c2 = tex2D(tex, uv2);
  float3 c3 = tex2D(tex, uv3);

  // Transform the fragment
  Output.RGBColor = float4(c1.b, c2.r, c3.g, alpha);

  return Output;
}
