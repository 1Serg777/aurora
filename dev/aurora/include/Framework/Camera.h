#pragma once

#include "Ray.h"

#include <cstdint>

namespace aurora
{
	struct CameraSettings
	{
		uint32_t resolution_x{ 1920 };
		uint32_t resolution_y{ 1080 };
		float fov_y{ 80.0f };
	};

	class Camera
	{
	public:

		Camera(uint32_t resolution_x, uint32_t resolution_y, float fov_y_deg);

		numa::Ray GenerateCameraRay(uint32_t x_coord, uint32_t y_coord) const;
		numa::Ray GenerateCameraRayJittered(uint32_t x_coord, uint32_t y_coord) const;

		void ResizeCamera(uint32_t resolution_x, uint32_t resolution_y);
		void ChangeCameraFOV_Y(float fov_y);

		uint32_t GetCameraResolution_X() const;
		uint32_t GetCameraResolution_Y() const;

	private:

		void ComputeCameraParameters();

		numa::Vec3 GeneratePixelPosition(uint32_t x_coord, uint32_t y_coord) const;
		numa::Vec3 GeneratePixelPositionJittered(uint32_t x_coord, uint32_t y_coord) const;

		uint32_t resolution_x{ 1 };
		uint32_t resolution_y{ 1 };

		float aspect_ratio{ 1.0f };

		float focal_length{ 1.0f };

		float h_over_2{ 1.0f };
		float w_over_2{ 1.0f };

		float fov_y_deg{ 0.0f }; // vertical fov (the one currently in use!)
		float fov_x_deg{ 0.0f }; // horizontal fov (not implemented yet!)
	};
}