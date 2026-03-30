#pragma once

#include "Framework/Components/Component.h"

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
		float pdf{1.0f};
		Light* lightPtr{nullptr};
	};

	struct LightSampleBundle {
		void AddLightSample(const LightSampleData& lightSample);
		std::vector<LightSampleData> bundle;
	};

	class Light : public Component {
	public:
		static constexpr ComponentType COMPONENT_TYPE = ComponentType::LIGHT;

		Light(LightType type);

		virtual void Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) = 0;

		LightType GetLightType() const;

	private:
		LightType type{};
	};

	class DirectionalLight : public Light {
	public:
		DirectionalLight(const numa::Vec3& lightColor, float lightStrength);

		void Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) override;

		numa::Vec3 P() const;

		numa::Vec3 Wi() const;
		numa::Vec3 Li() const;

		float pdf() const;

	private:
		numa::Vec3 color{1.0f, 1.0f, 1.0f};
		float strength{1.0f};
	};

	class PointLight : public Light {
	public:
		PointLight(const numa::Vec3& lightColor, float lightIntensity);

		void Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) override;

		numa::Vec3 P() const;

		numa::Vec3 Wi(const numa::Vec3& p) const;
		numa::Vec3 Li(float d) const;

		float pdf() const;

	private:
		numa::Vec3 color{1.0f, 1.0f, 1.0f};
		float intensity{1.0f};
	};

	class AreaLight : public Light {
	public:
		AreaLight(const numa::Vec3& lightColor, float lightIntensity);

		void Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) override;

		numa::Vec3 P() const;

		numa::Vec3 Li() const;

		float pdf(const numa::Vec3& wi, const numa::Vec3& N, float r) const;

	private:
		numa::Vec3 color{1.0f, 1.0f, 1.0f};
		float intensity{1.0f};
	};

}