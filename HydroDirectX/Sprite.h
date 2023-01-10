#pragma once
#include "DrawableBase.h"

class Sprite : public DrawableBase<Sprite>
{
public:
	Sprite( Graphics& gfx, float x, float y);
	void Update( float dt ) noexcept override;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	//Positional
	float x;
	float y;
};