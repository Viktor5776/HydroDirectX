#include "Camera.h"
#include "imgui/imgui.h"
#pragma once

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	const auto pos = DirectX::XMVector3Transform(
		DirectX::XMVectorSet( 0.0f,0.0f,-r,0.0f ),
		DirectX::XMMatrixRotationRollPitchYaw( phi,-theta,0.0f )
	);
	return DirectX::XMMatrixLookAtLH(
		pos,DirectX::XMVectorZero(),
		DirectX::XMVectorSet( 0.0f,1.0f,0.0f,0.0f )
	) * DirectX::XMMatrixRotationRollPitchYaw(
		pitch,-yaw,roll
	);
}

void Camera::SpawnControlWindow() noexcept
{
	if( ImGui::Begin( "Camera" ) )
	{
		ImGui::Text( "Posision" );
		ImGui::SliderFloat( "R",&r,0.0f,80.0f,"%.1f" );
		ImGui::SliderAngle( "Theta",&theta,-180.0f,180.0f );
		ImGui::SliderAngle( "Phi",&phi,-89.0f,89.0f );
		ImGui::Text( "Orientation" );
		ImGui::SliderAngle( "Roll",&roll,-180.0f,180.0f );
		ImGui::SliderAngle( "Pitch",&pitch,-180.0f,180.0f );
		ImGui::SliderAngle( "Yaw",&yaw,-180.0f,180.0f );
		if( ImGui::Button( "Reset" ) )
		{
			Reset();
		}
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	r = 20.0f;
	theta = 0.0f;
	phi = 0.0f;
	pitch = 0.0f;
	yaw = 0.0f;
	roll = 0.0f;
}
