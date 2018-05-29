PS_CONSTANTBUFFER SceneCB : register(b0)
{
  row_major float4x4 model;
  row_major float4x4 view;
  row_major float4x4 projection;
}

struct VS_OUTPUT
{
  float4 position : PS_VSOUTPUT;
  float3 position_VS : VIEWPOS;
  float3 position_WS : WORLDPOS;
  float3 normal_VS : VIEWNORMAL;
  float3 normal_WS : WORLDNORMAL;
  float2 uv : TEXCOORD0;
};

struct VS_INPUT
{
  float3 position : POSITION;
  float3 normal : NORMAL;
  float2 uv : TEXCOORD;
};

VS_OUTPUT main(VS_INPUT input)
{
  VS_OUTPUT output;

  output.position = float4(input.position, 1.0f);
	float4 pos = mul(output.position, mul(model, view));
	output.position_VS = pos.xyz / pos.w;
  output.position_WS = mul(float4(input.position, 1.0f), model).xyz / mul(float4(input.position, 1.0f), model).w;
	output.position = mul(pos, projection);
  output.normal_VS = mul(float4(input.normal, 0.0f), mul(model, view)).xyz;
  output.normal_WS = mul(float4(input.normal, 0.0f), model).xyz;
  output.uv = input.uv;
    
  return output;
}