#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		template <class T>
		static T Leap(const T& A, const T& B, float Per) noexcept { return A + (T)((float)(B - A)*Per); }
		static VECTOR_ref GetVector(float Xrad, float Yrad) noexcept { return VECTOR_ref::vget(-cos(deg2rad(Xrad))*sin(-deg2rad(Yrad)), sin(deg2rad(Xrad)), -cos(deg2rad(Xrad))*cos(-deg2rad(Yrad))); }

		struct shaderparam {
			bool use;
			float param1;
			float param2;
			float param3;
			float param4;
		};
		//
		class TEMPSCENE {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//����`�F�b�N
			bool IsFirstLoop = true;
			//�J����
			cam_info camera_main;
			float fov_base = DX_PI_F / 2;
			//
			std::array<shaderparam, 2> shaderParam;
			//
			virtual void Set_EnvLight(VECTOR_ref Shadow_minpos_t, VECTOR_ref Shadow_maxpos_t, VECTOR_ref Light_vec_t, COLOR_F Light_color_t) noexcept {
				Shadow_minpos = Shadow_minpos_t;
				Shadow_maxpos = Shadow_maxpos_t;
				Light_vec = Light_vec_t;
				Light_color = Light_color_t;
			}
		public:
			scenes Next_scene{ scenes::NONE_SCENE };			//���݂̃V�[��
			std::shared_ptr<Sceneclass::TEMPSCENE> Next_ptr{ nullptr };

			TEMPSCENE(void) noexcept {
			}
			void Set_Next(const std::shared_ptr<Sceneclass::TEMPSCENE>& Next_scenes_ptr_t, scenes NEXT) noexcept {
				Next_ptr = Next_scenes_ptr_t;
				Next_scene = NEXT;
			}

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return Light_vec; }
			cam_info& Get_Camera(void) noexcept { return camera_main; }

			virtual void Set(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				fov_base = deg2rad(DrawParts->use_vr ? 120 : OPTION::Instance()->Get_Fov());	//fov
				SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
				if (DrawParts->use_vr) {
					camera_main.set_cam_info(fov_base, 0.001f, 100.f);//1P
				}
				else {
					camera_main.set_cam_info(fov_base, 0.05f, 200.f);//1P
				}

				DrawParts->Set_Light_Shadow(Shadow_maxpos, Shadow_minpos, Light_vec, [&] {Shadow_Draw_Far(); });
				SetGlobalAmbientLight(Light_color);

				IsFirstLoop = true;
			}
			virtual bool Update(void) noexcept {
				IsFirstLoop = false;
				return true;
			}
			virtual void Dispose(void) noexcept {}

			virtual void ReadyDraw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//���ʒu�w��
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//�e�p��
				auto NearShadow = std::min(camera_main.far_, 100.f);
				DrawParts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(NearShadow, 60.f, NearShadow), VECTOR_ref::vget(2000.f, 60.f, 2000.f));//MAIN_LOOP��near�͂��� (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_x, GetColor(192, 192, 192), TRUE);
			}
			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}

			const bool& is_lens(void) const noexcept { return shaderParam[0].use; }
			const float& xp_lens(void) const noexcept { return shaderParam[0].param1; }
			const float& yp_lens(void) const noexcept { return shaderParam[0].param2; }
			const float& size_lens(void) const noexcept { return shaderParam[0].param3; }
			const float& zoom_lens(void) const noexcept { return shaderParam[0].param4; }

			void Set_is_lens(bool value) noexcept { shaderParam[0].use = value; }
			void Set_xp_lens(float value) noexcept { shaderParam[0].param1 = value; }
			void Set_yp_lens(float value) noexcept { shaderParam[0].param2 = value; }
			void Set_size_lens(float value) noexcept { shaderParam[0].param3 = value; }
			void Set_zoom_lens(float value) noexcept { shaderParam[0].param4 = value; }

			const bool& is_Blackout(void) const noexcept { return shaderParam[1].use; }
			const float& Per_Blackout(void) const noexcept { return shaderParam[1].param1; }

			void Set_is_Blackout(bool value) noexcept { shaderParam[1].use = value; }
			void Set_Per_Blackout(float value) noexcept { shaderParam[1].param1 = value; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR(void) noexcept {}
			virtual void KeyOperation(void) noexcept {}
		};
		//�V�[���T���v��
		class SAMPLE : public TEMPSCENE {
			void Set(void) noexcept override {
			}
			bool Update(void) noexcept override {
				return false;
			}
			void Dispose(void) noexcept override {
			}
			//
			void UI_Draw(void) noexcept  override {
			}
			void BG_Draw(void) noexcept override {
			}
			void Shadow_Draw_NearFar(void) noexcept override {
			}
			void Shadow_Draw(void) noexcept override {
			}
			void Main_Draw(void) noexcept override {
			}
			void LAST_Draw(void) noexcept override {
			}
		};
		//
		class SceneControl {
			bool ending{ false };							//�I�������t���O
			scenes sel_scene{ scenes::NONE_SCENE };			//���݂̃V�[��
			std::shared_ptr<Sceneclass::TEMPSCENE> scenes_ptr{ nullptr };
			bool selend{ true };
			bool selpause{ true };
			LONGLONG Drawwaits, OLDwaits, waits;

			shaders::shader_Vertex Screen_vertex;					// ���_�f�[�^
			std::array<shaders, 2> shader2D;
		public:
			SceneControl(void) noexcept {
				//�V�F�[�_�[
				Screen_vertex.Set();																					// ���_�f�[�^�̏���
				shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
				shader2D[1].Init("DepthVS.vso", "DepthPS.pso");																//�����Y
			}
			~SceneControl(void) noexcept {
				if (scenes_ptr != nullptr) {
					scenes_ptr->Dispose();
				}
			}
			//
			const auto& isEnd()const noexcept { return ending; }
			const auto& isPause()const noexcept { return selpause; }
			//
			void ChangeScene(scenes select, std::shared_ptr<Sceneclass::TEMPSCENE>& ptr) {
				sel_scene = select;
				scenes_ptr = ptr;
			}
			//�J�n
			void StartScene(void) noexcept {
				scenes_ptr->Set();
				selend = false;
				selpause = false;
				OLDwaits = GetNowHiPerformanceCount();
			}
			//
			bool Execute(void) noexcept {
				OLDwaits = GetNowHiPerformanceCount() - waits;
				waits = GetNowHiPerformanceCount();

				auto* DrawParts = DXDraw::Instance();
#ifdef DEBUG
				auto DebugParts = DeBuG::Instance();					//�f�o�b�O
#endif // DEBUG
				//
				if (ProcessMessage() != 0) {
					this->ending = true;
					selend = true;
				}
				clsDx();

				FPS = GetFPS();
#ifdef DEBUG
				DebugParts->put_way();
#endif // DEBUG
				//�X�V
				selpause = false;
				selend |= !scenes_ptr->Update();
				//VR��ԂɓK�p
				DrawParts->Move_Player();
#ifdef DEBUG
				DebugParts->end_way();
#endif // DEBUG
				//�I������
				if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) {
					this->ending = true;
					return true;
				}
				if (selend) { return true; }
				return false;
			}
			//�`��
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PostPassParts = PostPassEffect::Instance();				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
#ifdef DEBUG
				auto DebugParts = DeBuG::Instance();					//�f�o�b�O
#endif // DEBUG
					//���ʂ̕`��O�p��
				scenes_ptr->ReadyDraw();
				//UI��������
				PostPassParts->Set_UI_Draw([&] { scenes_ptr->UI_Draw(); });
				//VR�Ɉڂ�
				DrawParts->Draw_VR([&] {
					auto tmp = GetDrawScreen();
					cam_info tmp_cam = scenes_ptr->Get_Camera();
					tmp_cam.campos = GetCameraPosition();
					tmp_cam.camvec = GetCameraTarget();
					{
						//��ʑ̐[�x�`��
						PostPassParts->BUF_Draw([&] { scenes_ptr->BG_Draw(); }, [&] { DrawParts->Draw_by_Shadow([&] { scenes_ptr->Main_Draw(); }); }, tmp_cam, effectControl.Update_effect_f);
						//�ŏI�`��
						PostPassParts->Set_MAIN_Draw();
					}
					//*
					GraphHandle::SetDraw_Screen(tmp);
					{
						SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
						if (scenes_ptr->is_lens()) {
							//�����Y�`��
							shader2D[0].Set_dispsize();
							shader2D[0].Set_param(scenes_ptr->xp_lens(), scenes_ptr->yp_lens(), scenes_ptr->size_lens(), scenes_ptr->zoom_lens());
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								shader2D[0].Draw(Screen_vertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						if (scenes_ptr->is_Blackout()) {
							//�`��
							shader2D[1].Set_dispsize();
							shader2D[1].Set_param(scenes_ptr->Per_Blackout(), 0, 0, 0);
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								shader2D[1].Draw(Screen_vertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
					}
					//*/
					GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
					{
						PostPassParts->MAIN_Draw();											//�f�t�H�`��
						PostPassParts->DrawUI(&scenes_ptr->Get_Camera(), DrawParts->use_vr);	//UI1
						scenes_ptr->Item_Draw();											//UI2
					}
				}, scenes_ptr->Get_Camera());
#ifdef DEBUG
				//DebugParts->end_way();
#endif // DEBUG
				//�f�B�X�v���C�`��
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				{
					//�`��
					if (DrawParts->use_vr) {
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
						DrawParts->outScreen[0].DrawRotaGraph(DrawParts->disp_x / 2, DrawParts->disp_y / 2, 0.5f, 0, false);
					}
					else {
						DrawParts->outScreen[0].DrawGraph(0, 0, false);
					}
					//��ɏ���
					scenes_ptr->LAST_Draw();
					//MAPPTs->DepthScreen.DrawExtendGraph(0, 0, 960, 540, false);
					//�f�o�b�O
#ifdef DEBUG
					DebugParts->end_way();
					DebugParts->debug(1920 - 300, 50, float(GetNowHiPerformanceCount() - waits) / 1000.f);
					printfDx("AsyncCount :%d\n", GetASyncLoadNum());
					printfDx("Drawcall   :%d\n", GetDrawCallCount());
					printfDx("FPS        :%5.2f fps\n", FPS);
					printfDx("AllTime    :%5.2f ms\n", float(OLDwaits) / 1000.f);
					printfDx("DrawTime   :%5.2f ms\n", float(Drawwaits) / 1000.f);
					printfDx("All-Draw   :%5.2f ms\n", float(OLDwaits - Drawwaits) / 1000.f);
					printfDx("GameSpeed  :%3.1f\n", GameSpeed);
#endif // DEBUG
				}
			}
			//��������
			void Vsync(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//��ʂ̔��f
				DrawParts->Screen_Flip();
				Drawwaits = GetNowHiPerformanceCount() - waits;
				while ((GetNowHiPerformanceCount() - waits) <= (1000 * 1000 / 60)) {}
			}
			//
			void NextScene(void) noexcept {
				//���
				scenes_ptr->Dispose();
				//�J��
				{
					sel_scene = scenes_ptr->Next_scene;
					scenes_ptr = scenes_ptr->Next_ptr;
				}
			}
			//
		};
	};
};
