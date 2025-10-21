#pragma once

#include <cmath> // For sqrtf

struct Vector3
{
	float x;
	float y;
	float z;

	/**
	 * \brief Calculates the magnitude (length) of the vector.
	 * \return The length of the vector.
	 */
	float Length() const
	{
		return sqrtf(x * x + y * y + z * z);
	}
	/**
	 * \brief Subtracts another Vector3 from this one.
	 */
	Vector3 operator-(const Vector3& other) const
	{
		return { x - other.x, y - other.y, z - other.z };
	}

	/**
	 * \brief Adds another Vector3 to this one.
	 */
	Vector3 operator+(const Vector3& other) const
	{
		return { x + other.x, y + other.y, z + other.z };
	}

	/**
	 * \brief Adds another Vector3 to this one (in-place).
	 */
	void operator+=(const Vector3& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
	}

	/**
	 * \brief Multiplies the vector by a scalar.
	 */
	Vector3 operator*(float scalar) const
	{
		return { x * scalar, y * scalar, z * scalar };
	}
	/**
	 * \brief Calculates the squared magnitude of the vector.
	 * \return The squared length (avoids a costly sqrt operation).
	 */
	float LengthSquared() const
	{
		return (x * x + y * y + z * z);
	}

	/**
	 * \brief Scales the vector to have a length of 1 (a unit vector).
	 * Modifies the vector in-place.
	 */
	void Normalize()
	{
		float len = Length();
		if (len > 0.0f) // Avoid division by zero
		{
			// Use multiplication by inverse length, as it's often faster
			float invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
			z *= invLen;
		}
	}
};

struct Vector2
{
	float x;
	float y;

	/**
	 * \brief Calculates the magnitude (length) of the vector.
	 * \return The length of the vector.
	 */
	float Length() const
	{
		return sqrtf(x * x + y * y);
	}

	/**
	 * \brief Calculates the squared magnitude of the vector.
	 * \return The squared length (avoids a costly sqrt operation).
	 */
	float LengthSquared() const
	{
		return (x * x + y * y);
	}

	/**
	 * \brief Scales the vector to have a length of 1 (a unit vector).
	 * Modifies the vector in-place.
	 */
	void Normalize()
	{
		float len = Length();
		if (len > 0.0f) // Avoid division by zero
		{
			float invLen = 1.0f / len;
			x *= invLen;
			y *= invLen;
		}
	}
};