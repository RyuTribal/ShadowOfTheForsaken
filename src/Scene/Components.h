#pragma once
#include <glm/gtx/quaternion.hpp>
#include "Renderer/Texture.h"

namespace SOF {

	struct TagComponent {
		std::string Tag = "";

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& new_tag) : Tag(new_tag) {}
	};

	struct TransformComponent {
		glm::vec3 Translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 Scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& new_translation) : Translation(new_translation) {}

		glm::mat4 CreateMat4x4() {
			glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

			return glm::translate(glm::mat4(1.0f), Translation)
				* rotation
				* glm::scale(glm::mat4(1.0f), Scale);
		}

		glm::quat RotationVecToQuat() const
		{
			return glm::quat(Rotation);
		}
	};

	struct SpriteComponent {
		glm::vec4 Color = { 1.f, 1.f, 1.f, 1.f };
		std::shared_ptr<Texture> Texture = nullptr;

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;
		SpriteComponent(const glm::vec4& new_color) : Color(new_color){}
	};
}