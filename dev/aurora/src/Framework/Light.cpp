#include "Framework/Light.h"

#include "Framework/Actor.h"
#include "Framework/Components/Geometry.h"
#include "Framework/Components/Transform.h"

#include "Numa.h"
#include "Random.h"

namespace aurora {

	// Light Sample

	void LightSampleBundle::AddLightSample(const LightSampleData& lightSample) {
		bundle.push_back(lightSample);
	}

	// Light base class

	Light::Light(LightType type)
		: Component(ComponentType::LIGHT), type(type) {
	}

	LightType Light::GetLightType() const {
		return type;
	}

	// Directional light

	DirectionalLight::DirectionalLight(const numa::Vec3& lightColor, float lightStrength)
		: Light(LightType::DIRECTIONAL), color(lightColor), strength(lightStrength) {
	}

	void DirectionalLight::Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) {
		data.wi = Wi();
		data.pos = P();
		data.Li = Li();
		data.pdf = pdf();
		data.lightPtr = this;
	}

	numa::Vec3 DirectionalLight::P() const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f};
		return transform->GetWorldPosition();
	}

	numa::Vec3 DirectionalLight::Wi() const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f};
		numa::Mat4 world = transform->GetWorldMatrix();
		// Lights are oriented in the same way as cameras are, i.e. along the negative z-axis.
		// Light source shines in the direction '-world.forward', however 'wi' variable
		// is the opposite of that, directed toward the light source.
		// So we return world.forward, the direction toward the light source.
		return numa::Vec3{world.forward};
	}
	numa::Vec3 DirectionalLight::Li() const {
		return color * strength;
	}

	float DirectionalLight::pdf() const {
		// Directional light source is a "delta" light source, meaning it can't exist in the real world.
		// Instead, it is defined with the help of the delta function $\sigma$. That delta function
		// "cancels" the integral, so the Monte Carlo estimator is not needed. In other words,
		// we directly evaluate the integrand. To be consistent with the interface we've created, we set pdf = 1.0f.
		return 1.0f;
	}

	// Point light

	PointLight::PointLight(const numa::Vec3& lightColor, float lightIntensity)
		: Light(LightType::POINT), color(lightColor), intensity(lightIntensity) {
	}

	void PointLight::Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) {
		data.wi = Wi(p);
		data.pos = P();
		data.Li = Li(numa::Length(p - data.pos));
		data.pdf = pdf();
		data.lightPtr = this;
	}

	numa::Vec3 PointLight::P() const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f};
		return transform->GetWorldPosition();
	}

	numa::Vec3 PointLight::Wi(const numa::Vec3& p) const {
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f, 1.0f, 0.0f};
		const numa::Vec3& lightPos = transform->GetWorldPosition();
		return numa::Normalize(lightPos - p);
	}
	numa::Vec3 PointLight::Li(float d) const {
		// To avoid division by zero!
		static constexpr float bias{0.00001f};

		// 1) is it
		// return (color * intensity) / (4.0f * numa::Pi<float>() * d + bias);

		// 2) or
		// return (color * intensity) / (d + bias);

		// 3) or
		return (color * intensity) / (d * d + bias);
	}

	float PointLight::pdf() const {
		// Point light is also a "delta" light source. We return pdf = 1.0f.
		// See the explanation in DirectionLight's pdf() method.
		return 1.0f;
	}

	// Area light

	AreaLight::AreaLight(const numa::Vec3& lightColor, float lightIntensity)
		: Light(LightType::AREA), color(lightColor), intensity(lightIntensity) {
	}

	void AreaLight::Sample(const numa::Vec3& p, const numa::Vec3& N, LightSampleData& data) {
		data.pos = P();
		numa::Vec3 dP = data.pos - p; // Vector from the hit point to the area light sample
		data.wi = numa::Normalize(dP);
		data.Li = Li();
		data.pdf = pdf(data.wi, N, numa::Length(dP));
		data.lightPtr = this;
	}

	numa::Vec3 AreaLight::P() const {
		static constexpr float bias{0.00001f};

		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return numa::Vec3{0.0f};
		numa::Vec3 right = transform->GetRightAxis();
		numa::Vec3 up = transform->GetUpAxis();
		numa::Vec3 forward = transform->GetForwardAxis();

		std::shared_ptr<Geometry> lightGeometry = ownerActor.lock()->GetComponent<Geometry>();
		if (!lightGeometry) return numa::Vec3{0.0f};
		numa::Vec3 sample{0.0f};
		switch (lightGeometry->GetGeometryType()) {
			case GeometryType::CIRCLE: {
				// TODO
				sample = transform->GetWorldPosition();
			} break;
			case GeometryType::PLANE: {
				const Plane* planeGeometry = static_cast<const Plane*>(lightGeometry.get());
				const numa::Vec2& planeDimensions = planeGeometry->GetDimensions();
				float w = numa::RandomFloat(-planeDimensions.x / 2.0f, planeDimensions.x / 2.0f);
				float h = numa::RandomFloat(-planeDimensions.y / 2.0f, planeDimensions.y / 2.0f);
				sample = transform->GetWorldPosition() + w * right + h * up + bias * forward;
			} break;
			case GeometryType::SPHERE: {
				// TODO
				sample = transform->GetWorldPosition();
			} break;
		}
		return sample;
	}

	numa::Vec3 AreaLight::Li() const {
		return intensity * color;
	}

	float AreaLight::pdf(const numa::Vec3& wi, const numa::Vec3& N, float r) const {
		// TODO: provide the formula and short description.
		std::shared_ptr<Transform> transform = ownerActor.lock()->GetComponent<Transform>();
		if (!transform) return 1.0f;
		std::shared_ptr<Geometry> lightGeometry = ownerActor.lock()->GetComponent<Geometry>();
		switch (lightGeometry->GetGeometryType()) {
			case GeometryType::CIRCLE: {
				// TODO
				return 1.0f;
			}
			case GeometryType::PLANE: {
				numa::Vec3& planeNormal = transform->GetForwardAxis();
				const Plane* planeGeometry = static_cast<const Plane*>(lightGeometry.get());

				const numa::Vec2& planeDimensions = planeGeometry->GetDimensions();
				float planeArea = planeDimensions.x * planeDimensions.y; // TODO: handle 'inf' properly!

				float cosTheta = std::clamp(numa::Dot(-wi, planeNormal), 0.0f, 1.0f);
				// float cosTheta = std::clamp(numa::Dot(wi, N), 0.0f, 1.0f);
				return (r * r) / (cosTheta);
			}
			case GeometryType::SPHERE: {
				// TODO
				return 1.0f;
			}
			default: {
				// TODO
				return 1.0f;
			}
		}
	}
}
