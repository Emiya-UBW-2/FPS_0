#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		struct CharaAnimeSet {
			CharaAnimeID	m_Down{ CharaAnimeID::Upper_Down1 };

			CharaAnimeID	m_Ready{ CharaAnimeID::Upper_Ready1 };

			CharaAnimeID	m_ADS{ CharaAnimeID::Upper_ADS1 };

			CharaAnimeID	m_Cocking{ CharaAnimeID::Upper_Cocking1 };

			CharaAnimeID	m_Reload{ CharaAnimeID::Upper_Reload1Start };
		};

		class Pendulum2D {
			float m_PendulumLength = 10.f;
			float m_PendulumMass = 2.f;
			float m_drag_coeff = 2.02f;

			float m_rad = deg2rad(12.f);
			float m_vel = 0.f;
		public:
			void Init(float Length, float N, float rad) {
				m_PendulumLength = Length;
				m_PendulumMass = N;
				m_rad = rad;
				m_vel = 0.f;
			}

			void Execute() {
				m_vel += (-9.8f / m_PendulumLength * std::sin(m_rad) - m_drag_coeff / m_PendulumMass * m_vel) / 60.f;
				m_rad += m_vel / 60.f;
			}

			const auto GetRad() const noexcept { return m_rad; }

		};

		enum class HitType {
			Head,
			Body,
			Leg,
		};
		class HitBox {
			VECTOR_ref	m_pos;
			float		m_radius{ 0.f };
			HitType		m_HitType{ HitType::Body };
		public:
			void	Execute(const VECTOR_ref&pos, float radius, HitType pHitType) {
				m_pos = pos;
				m_radius = radius;
				m_HitType = pHitType;
			}
			void	Draw() {
				unsigned int color;
				switch (m_HitType) {
				case FPS_n2::Sceneclass::HitType::Head:
					color = GetColor(255, 0, 0);
					break;
				case FPS_n2::Sceneclass::HitType::Body:
					color = GetColor(0, 255, 0);
					break;
				case FPS_n2::Sceneclass::HitType::Leg:
					color = GetColor(0, 0, 255);
					break;
				default:
					break;
				}
				DrawSphere_3D(m_pos, m_radius, color, color);
			}

			bool	Colcheck(const AmmoClass& pAmmo) {
				return (HitCheck_Sphere_Capsule(
					m_pos.get(), m_radius,
					pAmmo.GetMove().repos.get(), pAmmo.GetMove().pos.get(), pAmmo.GetCaliberSize()*Scale_Rate
				) == TRUE);
			}

			const auto GetColType()const noexcept { return m_HitType; }
		};
	};
};
