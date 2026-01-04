#pragma once

#include "Framework/Actor.h"

#include "Vec.hpp"

#include <vector>

namespace aurora {

	enum class LightType {
		DIRECTIONAL,
		POINT,
		AREA
	};

	class Light;

	struct LightSampleData {
		numa::Vec3 wi{};
		numa::Vec3 pos{};
		numa::Vec3 Li{};
		Light* lightPtr{ nullptr };
	};

	struct LightSampleBundle {
		void AddLightSample(const LightSampleData& lightSample);

		std::vector<LightSampleData> bundle;
	};

	class Light : public Actor {
	public:
		Light(LightType type, std::string_view lightName);

		virtual void Sample(const numa::Vec3& p, LightSampleData& data) = 0;

		LightType GetLightType() const;

	private:
		LightType type;
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(std::string_view lightName, const numa::Vec3& lightColor, float lightStrength);

		void Sample(const numa::Vec3& p, LightSampleData& data) override;

		numa::Vec3 Pos() const;

		numa::Vec3 Wi() const;
		numa::Vec3 Li() const;

	private:
		numa::Vec3 color{1.0f, 1.0f, 1.0f};
		float strength{1.0f};
	};

	class PointLight : public Light {
	public:
		PointLight(std::string_view lightName, const numa::Vec3& lightColor, float lightIntensity);

		void Sample(const numa::Vec3& p, LightSampleData& data) override;

		numa::Vec3 Pos() const;

		numa::Vec3 Wi(const numa::Vec3& p) const;
		numa::Vec3 Li(float d) const;

	private:
		numa::Vec3 color{1.0f, 1.0f, 1.0f};
		float intensity{1.0f};
	};

	class AreaLight : public Light {
	public:
		AreaLight(std::string_view lightName);

		void Sample(const numa::Vec3& p, LightSampleData& data) override;
	};

}