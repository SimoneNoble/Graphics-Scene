#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light() {} // Default constructor, we'll be needing this later!
	Light(const Matrix4& position, const Vector4& colour, float radius) {
		this->position	= position;
		this->colour	= colour;
		this->radius	= radius;
	}

	~Light(void) {};

	Matrix4		GetPosition() const					{ return position; }
	void		SetPosition(const Matrix4& val)		{ position = val; }

	float		GetRadius() const					{ return radius; }
	void		SetRadius(float val)				{ radius = val; }

	Vector4		GetColour() const					{ return colour; }
	void		SetColour(const Vector4& val)		{ colour = val; }

protected:
	Matrix4		position;
	float		radius;
	Vector4		colour;
	float		lightCycle; 

};