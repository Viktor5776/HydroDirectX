#include "Sprite.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Plane.h"
#include "Surface.h"
#include "Texture.h"
#include "Sampler.h"


Sprite::Sprite( Graphics& gfx, float x, float y )
	:
	x(x),
	y(y)
{

	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			struct
			{
				float u;
				float v;
			} tex;
		};
		auto model = Plane::Make<Vertex>();
		model.vertices[0].tex = { 0.0f,0.0f };
		model.vertices[1].tex = { 1.0f,0.0f };
		model.vertices[2].tex = { 0.0f,1.0f };
		model.vertices[3].tex = { 1.0f,1.0f };

		AddStaticBind( std::make_unique<Texture>( gfx,Surface::FromFile( "Images\\Dog.png" ) ) );

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

		AddStaticBind( std::make_unique<Sampler>( gfx ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"TextureVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"TexturePS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "TexCoord",0,DXGI_FORMAT_R32G32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );
}

void Sprite::Update( float dt ) noexcept
{
	return;
}

DirectX::XMMATRIX Sprite::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationZ(PI) * DirectX::XMMatrixScaling(0.15f,0.15f,1.0f) * DirectX::XMMatrixTranslation(x,y,0.5f);
}