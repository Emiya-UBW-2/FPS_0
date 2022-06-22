#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class GateClass : public ObjectBaseClass {
			bool m_setstart;
			SoundHandle m_Open;
		public:
			GateClass(void) noexcept {
				this->m_objType = ObjType::Gate;
			}
			~GateClass(void) noexcept {}
		public:
			void Init(void) noexcept override {
				this->m_obj.get_anime(1).per = 1.f;
				this->m_obj.get_anime(1).time = 0.f;
				this->m_setstart = false;

				SetCreate3DSoundFlag(TRUE);
				this->m_Open = SoundHandle::Load("data/Sound/SE/GateOpen.wav");
				SetCreate3DSoundFlag(FALSE);

				this->m_Open.vol(128);
				Set3DPresetReverbParamSoundMem(DX_REVERB_PRESET_MOUNTAINS, this->m_Open.get());

			}
			void Execute(void) noexcept override {
				if (this->m_setstart) {
					this->m_obj.get_anime(1).time += 30.f / FPS;
				}
				if (this->m_obj.get_anime(1).TimeEnd()) {
					this->m_obj.get_anime(1).time = this->m_obj.get_anime(1).alltime;
				}

				this->m_obj.work_anime();
			}


			void Draw(void) noexcept override {
				this->m_obj.DrawModel();
			}

			void DrawShadow(void) noexcept override {
				this->m_obj.DrawModel();
			}


			void SetStart(void) noexcept {
				if (!this->m_setstart) {
					if (!this->m_Open.check()) {
						this->m_Open.play_3D(this->m_obj.GetMatrix().pos(), 12.5f * 50.f);
					}
				}
				this->m_setstart = true;
			}
		};
	};
};
