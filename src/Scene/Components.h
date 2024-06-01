#pragma once
#include <glm/gtx/quaternion.hpp>

namespace SOF {

	struct TagComponent {
		std::string Tag = "";

		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& new_tag) : Tag(new_tag) {}
	};

	struct TransformComponent {
		glm::vec3 translation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& new_translation) : translation(new_translation) {}

		glm::mat4 CreateMat4x4() {
			glm::mat4 Rotation = glm::toMat4(glm::quat(rotation));

			return glm::translate(glm::mat4(1.0f), translation)
				* Rotation
				* glm::scale(glm::mat4(1.0f), scale);
		}

		glm::quat RotationVecToQuat() const
		{
			return glm::quat(rotation);
		}
	};
}