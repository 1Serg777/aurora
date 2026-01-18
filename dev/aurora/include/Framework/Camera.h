#pragma once

#include "Actor.h"
#include "Ray.h"

#include <cstdint>

namespace aurora {

	enum class FilmFitType {
		FILL,
		OVERSCAN
	};

	enum class FovType {
		VERTICAL,
		HORIZONTAL
	};

	struct CameraProperties {
		uint32_t resolution_x{1920};
		uint32_t resolution_y{1080};
		float nearPlane{0.1f};
		float farPlane{10000.0f};
		// 1. Physical camera properties.
		//    What you have exposed in Maya.
		float filmWidth{1.417f}; // in inches. (multiply by 25.4 to convert to millimeters) (35.9918 mm.)
		float filmHeight{0.945f}; // in inches. (multiply by 25.4 to convert to millimeters) (24.003 mm.)
		float focalLength{35.0f}; // in millimeters.
		FilmFitType filmFitType{FilmFitType::FILL};
		// 2. Non-physical camera properties.
		//    What game engines usually expose to the player.
		float fov_y{90.0f};
		float fov_x{120.0f};
		FovType fovType{FovType::VERTICAL};
		bool usePhysicalCamera{true};
	};

	class Camera : public Actor {
	public:
		Camera(uint32_t resolution_x, uint32_t resolution_y, FovType fovType, float fovDeg);

		numa::Ray GenerateCameraRay(uint32_t x_coord, uint32_t y_coord) const;
		numa::Ray GenerateCameraRayJittered(uint32_t x_coord, uint32_t y_coord) const;

		void ResizeCamera(uint32_t resolution_x, uint32_t resolution_y);
		void ChangeCameraFOV_Y(float fov_y);

		uint32_t GetCameraResolution_X() const;
		uint32_t GetCameraResolution_Y() const;

	private:
		void ComputeCameraParameters();

		numa::Vec3 GeneratePixelPosition(float x_raster_coord, float y_raster_coord) const;

		// numa::Vec3 GeneratePixelPosition(uint32_t x_coord, uint32_t y_coord) const;
		// numa::Vec3 GeneratePixelPositionJittered(uint32_t x_coord, uint32_t y_coord) const;

		uint32_t resolution_x{1};
		uint32_t resolution_y{1};

		float aspect_ratio{1.0f};

		float nearPlane{0.01f};
		float farPlane{1000.0f};
		float focal_length{1.0f};

		float half_height{1.0f};
		float half_width{1.0f};

		float fov_y_deg{0.0f}; // vertical fov
		float fov_x_deg{0.0f}; // horizontal fov

		FovType fovType{FovType::VERTICAL};
	};

}