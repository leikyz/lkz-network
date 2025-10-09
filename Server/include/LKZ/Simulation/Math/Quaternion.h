#pragma once
#include <cmath>
#include <algorithm>
#include <limits> // For std::numeric_limits
#include <numbers>


struct Quaternion
{
    float w, x, y, z;

    Quaternion() : w(1), x(0), y(0), z(0) {}
    Quaternion(float _w, float _x, float _y, float _z)
        : w(_w), x(_x), y(_y), z(_z) {
    }

    Quaternion operator*(const Quaternion& other) const {
        Quaternion result;
        result.w = w * other.w - x * other.x - y * other.y - z * other.z;
        result.x = w * other.x + x * other.w + y * other.z - z * other.y;
        result.y = w * other.y - x * other.z + y * other.w + z * other.x;
        result.z = w * other.z + x * other.y - y * other.x + z * other.w;
        return result;
    }

    static Quaternion FromEuler(float pitch, float yaw, float roll)
    {
        Quaternion qPitch(cos(pitch * 0.5f), sin(pitch * 0.5f), 0.0f, 0.0f);
        Quaternion qYaw(cos(yaw * 0.5f), 0.0f, sin(yaw * 0.5f), 0.0f);
        Quaternion qRoll(cos(roll * 0.5f), 0.0f, 0.0f, sin(roll * 0.5f));

        return qRoll * qPitch * qYaw;
    }

    static Vector3 QuaternionToEuler(const Quaternion& q)
    {
        Vector3 euler;

        // --- Pitch (X) ---
        float sinp = 2.0f * (q.w * q.x - q.z * q.y);
        if (std::abs(sinp) >= 1.0f)
            euler.x = std::copysign(std::numbers::pi_v<float> / 2.0f, sinp);
        else
            euler.x = std::asin(sinp);

        // --- Yaw (Y) ---
        euler.y = std::atan2(
            2.0f * (q.w * q.y + q.x * q.z),
            1.0f - 2.0f * (q.y * q.y + q.x * q.x)
        );

        // --- Roll (Z) ---
        euler.z = std::atan2(
            2.0f * (q.w * q.z + q.y * q.x),
            1.0f - 2.0f * (q.z * q.z + q.x * q.x)
        );

        return euler;
    }

};
