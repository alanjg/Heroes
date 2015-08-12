//--------------------------------------------------------------------------------------
// File: Tutorial07.fx
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------
// Constant Buffer Variables
//--------------------------------------------------------------------------------------
Texture2D txDiffuse : register(t0);
SamplerState samLinear : register(s0);

cbuffer cbViewTransform : register(b0)
{
	matrix View;
};

cbuffer cbProjectionTransform : register(b1)
{
	matrix Projection;
};

cbuffer cbWorldTransform : register(b2)
{
	matrix World;
};

cbuffer cbSkinnedModelMatrices : register(b3)
{
	matrix SkinnedMatrices[184];
}


//--------------------------------------------------------------------------------------
struct VS_INPUT
{
	float4 Pos : POSITION;
	float4 Weights : WEIGHTS;
	int4 MatrixIndices : MATRIXINDICES;
	float3 Norm : NORMAL;
	float2 Tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD0;
};


//--------------------------------------------------------------------------------------
// Vertex Shader
//--------------------------------------------------------------------------------------
PS_INPUT VS(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT) 0;
	int NumBones = 4;
	float4      Pos = 0.0f;
	float4      Normal = 0.0f;
	float       LastWeight = 0.0f;

	// calculate the pos/normal using the "normal" weights 
	//        and accumulate the weights to calculate the last weight
	for (int iBone = 0; iBone < NumBones; iBone++)
	{
		LastWeight = LastWeight + input.Weights[iBone];

		Pos += mul(input.Pos, SkinnedMatrices[input.MatrixIndices[iBone]]) * input.Weights[iBone];
		Normal += mul(input.Norm, SkinnedMatrices[input.MatrixIndices[iBone]]) * input.Weights[iBone];
	}
	LastWeight = 1.0f - LastWeight;

	// Now that we have the calculated weight, add in the final influence
//	Pos += (mul(input.Pos, SkinnedMatrices[input.MatrixIndices[NumBones - 1]]) * LastWeight);
//	Normal += (mul(input.Norm, SkinnedMatrices[input.MatrixIndices[NumBones - 1]]) * LastWeight);


	output.Pos = mul(Pos, World);
	//output.Pos = mul(input.Pos, World) +0.0001f * Pos;
	output.Pos = mul(output.Pos, View);
	output.Pos = mul(output.Pos, Projection);
	output.Tex = input.Tex;

	return output;
}


//--------------------------------------------------------------------------------------
// Pixel Shader
//--------------------------------------------------------------------------------------
float4 PS(PS_INPUT input) : SV_Target
{
	return txDiffuse.Sample(samLinear, input.Tex);
}
