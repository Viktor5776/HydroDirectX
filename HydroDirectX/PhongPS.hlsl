cbuffer LightCBuf
{
	float3 lightPos;
};

static const float3 materialColor = { 0.7f,0.7f,0.9f };
static const float3 ambient = { 0.15f,0.15f,0.15f };
static const float3 diffuseColor = { 1.0f,1.0f,1.0f };
static const float diffuseIntensity = 1.0f;
static const float attConst = 1.0f;
static const float attLin = 1.0f;
static const float attQuad = 1.0f;


float4 main( float3 worldPos : Position, float3 n : Normal ) : SV_Target
{
	//Fragment to Light Vector Data
    const float3 vTol = lightPos - worldPos;
    const float distTol = length( vTol );
    const float3 dirToL = vTol / distTol;
    //Diffuse Attenuation
    const float att = attConst + attLin * distTol + attQuad * (distTol * distTol);
    //Diffuse Intensity
    const float3 diffuse = diffuseColor * diffuseIntensity * att * max( 0.0f, dot( dirToL, n ) );
    //Final Color
    return float4( saturate( diffuse + ambient ), 1.0f );
	
}