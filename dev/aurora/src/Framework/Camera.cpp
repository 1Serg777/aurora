#include "Framework/Camera.h"

#include "Core/Utility.h"

#include "Numa.h"
#include "Random.h"

namespace aurora {

	Camera::Camera(uint32_t resolution_x, uint32_t resolution_y, FovType fovType, float fovDeg)
		: Actor(GenerateUniqueName<Camera>("Camera")),
		resolution_x(resolution_x), resolution_y(resolution_y), fovType(fovType) {
		if (fovType == FovType::VERTICAL) {
			fov_y_deg = fovDeg;
		} else /* if (fovType == FovType::HORIZONTAL) */ {
			fov_x_deg = fovDeg;
		}
		ComputeCameraParameters();
	}

	numa::Ray Camera::GenerateCameraRay(uint32_t x_coord, uint32_t y_coord) const {
		float raster_coord_x = static_cast<float>(x_coord) + 0.5f;
		float raster_coord_y = static_cast<float>(y_coord) + 0.5f;

		// Generate a camera ray using raster coordinates provided
		// 1. First we create a camera space ray

		numa::Vec3 rayOrigin{0.0f};
		numa::Vec3 rayDirection = numa::Normalize(GeneratePixelPosition(raster_coord_x, raster_coord_y));

		// 2. We transform this ray to the world space

		// WARNING!!!
		// This is a potential bottleneck when I decide to switch to multithreading,
		// because of "cash coherency" (is this the right name?).
		// std::shared_ptr<TransformComponent> cameraTransform = GetTransformComponent();
		// WARNING!!!

		/*
		Transform* cameraTransform = GetTransform();

		glm::vec3 rayOrigin = cameraTransform->GetWorldPosition();

		glm::vec3 rayDirection = glm::vec3(camera_space_x, camera_space_y, -1.0f);
		rayDirection = glm::normalize(rayDirection);
		rayDirection = cameraTransform->GetWorldMatrix() * glm::vec4(rayDirection, 0.0f);

		return numa::Ray{ rayOrigin, rayDirection };
		*/

		Transform* cameraTransform = GetTransform();
		if (cameraTransform) {
			rayOrigin = cameraTransform->GetWorldPosition();
			rayDirection = cameraTransform->GetRotationMatrix() * rayDirection;
		}

		numa::Ray cameraRay{rayOrigin, rayDirection};
		return cameraRay;
	}
	numa::Ray Camera::GenerateCameraRayJittered(uint32_t x_coord, uint32_t y_coord) const {
		float raster_coord_x = static_cast<float>(x_coord) + 0.5f;
		float raster_coord_y = static_cast<float>(y_coord) + 0.5f;

		// 1.
		numa::Vec2 shift{numa::RandomInSquare(0.5f)};
		numa::Vec3 pixelPosition = GeneratePixelPosition(raster_coord_x + shift.x, raster_coord_y + shift.y);

		// 2.
		/*
		numa::Vec3 pixelPosition = GeneratePixelPosition(raster_coord_x, raster_coord_y);
		numa::Vec3 shift{ numa::RandomInSquare(0.5f) };
		// numa::Vec3 shiftScale{ 1.0f / resolution_x, 1.0f / resolution_y, 0.0f }; // wrong!
		
		// We divide the width of the screen (2.0 * half_width) by the horizontal (x) resolution,
		// and the height of the screen (2.0 * half_height) by the vertical (y) resolution.
		numa::Vec3 shiftScale{ (2.0f * half_width) / resolution_x, (2.0f * half_height) / resolution_y, 0.0f };

		shift *= shiftScale;
		pixelPosition += shift;
		*/

		numa::Vec3 rayOrigin{0.0f};
		numa::Vec3 rayDirection = numa::Normalize(pixelPosition);

		Transform* cameraTransform = GetTransform();
		if (cameraTransform) {
			rayOrigin = cameraTransform->GetWorldPosition();
			rayDirection = cameraTransform->GetRotationMatrix() * rayDirection;
		}

		numa::Ray jitteredCameraRay{rayOrigin, rayDirection};
		return jitteredCameraRay;
	}

	void Camera::ResizeCamera(uint32_t resolution_x, uint32_t resolution_y) {
		this->resolution_x = resolution_x;
		this->resolution_y = resolution_y;
		ComputeCameraParameters();
	}
	void Camera::ChangeCameraFOV_Y(float fov_y_deg) {
		this->fov_y_deg = fov_y_deg;
		ComputeCameraParameters();
	}

	uint32_t Camera::GetCameraResolution_X() const {
		return resolution_x;
	}
	uint32_t Camera::GetCameraResolution_Y() const {
		return resolution_y;
	}

	void Camera::ComputeCameraParameters() {
		aspect_ratio = static_cast<float>(resolution_x) / resolution_y;
		if (fovType == FovType::VERTICAL) {
			half_height = focal_length * tanf(numa::Rad(fov_y_deg) * 0.5f);
			half_width = aspect_ratio * half_height;
		} else /* if (fovType == FovType::HORIZONTAL) */ {
			half_width = tanf(numa::Rad(fov_x_deg) * 0.5f) * focal_length;
			half_height = half_width / aspect_ratio;
		}
	}

	numa::Vec3 Camera::GeneratePixelPosition(float x_raster_coord, float y_raster_coord) const {
		float x_ndc = x_raster_coord / resolution_x;
		float y_ndc = y_raster_coord / resolution_y;

		x_ndc = x_ndc * 2.0f - 1.0f;
		y_ndc = 1.0f - y_ndc * 2.0f; // flip 'y' coordinate

		float camera_space_x = x_ndc * half_width;
		float camera_space_y = y_ndc * half_height;

		numa::Vec3 pixelPosition{camera_space_x, camera_space_y, -focal_length};
		return pixelPosition;
	}

	/*
	numa::Vec3 Camera::GeneratePixelPosition(uint32_t x_coord, uint32_t y_coord) const {
		float x_raster = static_cast<float>(x_coord) + 0.5f;
		float y_raster = static_cast<float>(y_coord) + 0.5f;

		float x_ndc = x_raster / resolution_x;
		float y_ndc = y_raster / resolution_y;

		x_ndc = x_ndc * 2.0f - 1.0f;
		y_ndc = 1.0f - y_ndc * 2.0f; // flip 'y' coordinate

		float camera_space_x = x_ndc * half_width;
		float camera_space_y = y_ndc * half_height;

		numa::Vec3 pixelPosition{ camera_space_x, camera_space_y, -1.0f };
		return pixelPosition;
	}
	numa::Vec3 Camera::GeneratePixelPositionJittered(uint32_t x_coord, uint32_t y_coord) const {
		float x_raster = static_cast<float>(x_coord) + 0.5f;
		float y_raster = static_cast<float>(y_coord) + 0.5f;

		numa::Vec2 shift = numa::RandomInSquare(0.5f);
		x_raster += shift.x;
		y_raster += shift.y;

		float x_ndc = x_raster / resolution_x;
		float y_ndc = y_raster / resolution_y;

		x_ndc = x_ndc * 2.0f - 1.0f;
		y_ndc = 1.0f - y_ndc * 2.0f; // flip 'y' coordinate

		// float aspect_ratio = static_cast<float>(resolution_x) / resolution_y;
		// float half_height = focal_length * tanf(fov_y * 0.5f);
		// float half_width = aspect_ratio * half_height;

		float camera_space_x = x_ndc * half_width;
		float camera_space_y = y_ndc * half_height;

		numa::Vec3 pixelPosition{ camera_space_x, camera_space_y, -1.0f };
		return pixelPosition;
	}
	*/

}