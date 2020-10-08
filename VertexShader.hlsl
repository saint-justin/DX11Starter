// Creating cbuffer
cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	matrix world;
	matrix view;
	matrix proj;
}


// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
struct VertexShaderInput
{ 
	float3 position		: POSITION;     // XYZ position
	float4 normal		: NORMAL;       // Vertex Normal
	float2 uv			: TEXCOORD;     // Vertex UV info
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
struct VertexToPixel
{
	float4 position	: SV_POSITION;
	float4 color	: COLOR;
	float2 uv		: TEXCOORD;
	float3 normal	: NORMAL;
	float3 worldPos	: POSITION;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// Calculate and apply world view projection matrix
	matrix wvp = mul(proj, mul(view, world));
	output.position = mul(wvp, float4(input.position, 1.0f));
	output.normal = normalize(mul((float3x3)world, input.normal));			// TODO: Update the world position to be the inverse transpose when passing in
	
	// Calculate vert's world position
	output.worldPos = mul(world, float4(input.position, 1.0f)).xyz;

	// Pass the color and uv through 
	output.color = colorTint;
	output.uv = input.uv;

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}