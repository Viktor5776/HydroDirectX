#include "Box.h"
#include "BindableBase.h"
#include "GraphicsThrowMacros.h"
#include "Cube.h"
#include "imgui/imgui.h"

Box::Box( Graphics& gfx,
		  std::mt19937& rng,
		  std::uniform_real_distribution<float>& adist,
		  std::uniform_real_distribution<float>& ddist,
		  std::uniform_real_distribution<float>& odist,
		  std::uniform_real_distribution<float>& rdist,
		  std::uniform_real_distribution<float>& bdist,
		  DirectX::XMFLOAT3 material )
	:
	TestObject( gfx,rng,adist,ddist,odist,rdist )
{
	if( !IsStaticInitialized() )
	{
		struct Vertex
		{
			DirectX::XMFLOAT3 pos;
			DirectX::XMFLOAT3 n;
		};
		auto model = Cube::MakeIndependent<Vertex>();
		model.SetNormalsIndependentFlat();

		AddStaticBind( std::make_unique<VertexBuffer>( gfx,model.vertices ) );

		auto pvs = std::make_unique<VertexShader>( gfx,L"PhongVS.cso" );
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind( std::move( pvs ) );

		AddStaticBind( std::make_unique<PixelShader>( gfx,L"PhongPS.cso" ) );

		AddStaticIndexBuffer( std::make_unique<IndexBuffer>( gfx,model.indices ) );

		const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
		{
			{ "Position",0,DXGI_FORMAT_R32G32B32_FLOAT,0,0,D3D11_INPUT_PER_VERTEX_DATA,0 },
			{ "Normal",0,DXGI_FORMAT_R32G32B32_FLOAT,0,12,D3D11_INPUT_PER_VERTEX_DATA,0 },
		};
		AddStaticBind( std::make_unique<InputLayout>( gfx,ied,pvsbc ) );

		AddStaticBind( std::make_unique<Topology>( gfx,D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST ) );
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind( std::make_unique<TransformCbuf>( gfx,*this ) );

	materialConstants.color = material;
	AddBind( std::make_unique<MaterialCbuf>( gfx, materialConstants, 1u ) );

	struct PSMatrialConstant
	{
		DirectX::XMFLOAT3 color;
		float specularIntensity = 0.6f;
		float specularPower = 30.0f;
		float padding[3];
	} colorConst;
	colorConst.color = material;
	AddBind( std::make_unique<PixelConstantBuffer<PSMatrialConstant>>( gfx, colorConst, 1u ) );

	//Model Deformation Transform
	DirectX::XMStoreFloat3x3(
		&mt,
		DirectX::XMMatrixScaling( 1.0f,1.0f,bdist( rng ) )
	);
}


DirectX::XMMATRIX Box::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat3x3( &mt ) * TestObject::GetTransformXM();
}

void Box::SpawnControlWindow( int id, Graphics& gfx ) noexcept
{
	using namespace std::string_literals;

	bool dirty = false;
	if( ImGui::Begin( ("Box "s + std::to_string( id )).c_str() ) )
	{
		dirty = dirty || ImGui::ColorEdit3( "Material Color", &materialConstants.color.x );
		dirty = dirty || ImGui::SliderFloat( "Specular Intensity", &materialConstants.specularIntensity, 0.05f, 4.0f, "%.2f", 2 );
		dirty = dirty || ImGui::SliderFloat( "Specular Power", &materialConstants.specularPower, 1.0f, 200.0f, "%.2f", 2 );
	}
	ImGui::End();

	if( dirty )
	{
		SyncMaterial( gfx );
	}
}

void Box::SyncMaterial( Graphics& gfx ) noexcept(!IS_DEBUG)
{
	auto pConstPS = QueryBindable<MaterialCbuf>();
	assert( pConstPS != nullptr );
	pConstPS->Update( gfx, materialConstants );
}