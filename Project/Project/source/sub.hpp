#pragma once
#include"Header.hpp"

//Box2D�g��
namespace std {
	template <>
	struct default_delete<b2Body> {
		void operator()(b2Body* body) const {
			body->GetWorld()->DestroyBody(body);
		}
	};
}; // namespace std
//
namespace FPS_n2 {
};