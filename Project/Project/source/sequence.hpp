#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		class TEMPSCENE {
		private:
			VECTOR_ref		m_ShadowMaxSize;
			VECTOR_ref		m_ShadowMinSize;
			VECTOR_ref		m_LightVec;
			COLOR_F			m_LightColorF{ GetColorF(0, 0, 0, 0) };
		protected:
			//����`�F�b�N
			bool			m_IsFirstLoop{ true };
			//�J����
			Camera3DInfo	m_MainCamera;
			float			fov_base{ DX_PI_F / 2 };
			//
			std::array<shaderparam, 2> m_ShaderParam;
		protected:
			virtual void SetAmbientShadow(const VECTOR_ref& ShadowMinSize, const VECTOR_ref& ShadowMaxSize, const VECTOR_ref& Lightvec, const COLOR_F& LightColor) noexcept {
				m_ShadowMaxSize = ShadowMaxSize;
				m_ShadowMinSize = ShadowMinSize;
				m_LightVec = Lightvec;
				m_LightColorF = LightColor;//GetColorF(0.12f, 0.11f, 0.10f, 0.0f)
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

			const VECTOR_ref& Get_Light_vec(void) const noexcept { return m_LightVec; }
			Camera3DInfo& GetMainCamera(void) noexcept { return m_MainCamera; }

			virtual void Set(void) noexcept {
				auto* OptionParts = OPTION::Instance();
				auto* DrawParts = DXDraw::Instance();
				SetUseMaskScreenFlag(FALSE);//���J�X�^����ʂŃG�t�F�N�g���o�Ȃ��Ȃ邽�ߓ����
				fov_base = deg2rad(OptionParts->Get_useVR() ? 120 : OptionParts->Get_Fov());	//fov
				m_MainCamera.SetCamInfo(fov_base, 0.05f, 200.f);//1P
				DrawParts->SetAmbientShadow(m_ShadowMinSize, m_ShadowMaxSize, m_LightVec, m_LightColorF, [&] {Shadow_Draw_Far(); });
				m_IsFirstLoop = true;
			}
			virtual bool Update(void) noexcept {
				m_IsFirstLoop = false;
				return true;
			}
			virtual void Dispose(void) noexcept {}

			virtual void ReadyDraw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//���ʒu�w��
				Set3DSoundListenerPosAndFrontPosAndUpVec(m_MainCamera.GetCamPos().get(), m_MainCamera.GetCamVec().get(), m_MainCamera.GetCamUp().get());
				//�e�p��
				auto NearShadow = std::min(m_MainCamera.GetCamFar(), 25.f*12.5f);
				DrawParts->Ready_Shadow(m_MainCamera.GetCamPos(), [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(NearShadow, 30.f, NearShadow), VECTOR_ref::vget(2000.f, 60.f, 2000.f));//MAIN_LOOP��near�͂��� (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispXSize, GetColor(192, 192, 192), TRUE);
			}

			virtual void Depth_Draw(void) noexcept {}

			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}
			virtual void Main_Draw2(void) noexcept {}

			const bool& is_lens(void) const noexcept { return m_ShaderParam[0].use; }
			const float& xp_lens(void) const noexcept { return m_ShaderParam[0].param[0]; }
			const float& yp_lens(void) const noexcept { return m_ShaderParam[0].param[1]; }
			const float& size_lens(void) const noexcept { return m_ShaderParam[0].param[2]; }
			const float& zoom_lens(void) const noexcept { return m_ShaderParam[0].param[3]; }

			void Set_is_lens(bool value) noexcept { m_ShaderParam[0].use = value; }
			void Set_xp_lens(float value) noexcept { m_ShaderParam[0].param[0] = value; }
			void Set_yp_lens(float value) noexcept { m_ShaderParam[0].param[1] = value; }
			void Set_size_lens(float value) noexcept { m_ShaderParam[0].param[2] = value; }
			void Set_zoom_lens(float value) noexcept { m_ShaderParam[0].param[3] = value; }

			const bool& is_Blackout(void) const noexcept { return m_ShaderParam[1].use; }
			const float& Per_Blackout(void) const noexcept { return m_ShaderParam[1].param[0]; }

			void Set_is_Blackout(bool value) noexcept { m_ShaderParam[1].use = value; }
			void Set_Per_Blackout(float value) noexcept { m_ShaderParam[1].param[0] = value; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR(void) noexcept {}
			virtual void KeyOperation(void) noexcept {}
		};
		//�V�[���T���v��
		class SAMPLE : public TEMPSCENE {
			void Set(void) noexcept override {}
			bool Update(void) noexcept override { return false; }
			void Dispose(void) noexcept override {}
			//
			void UI_Draw(void) noexcept  override {}
			void BG_Draw(void) noexcept override {}
			void Shadow_Draw_NearFar(void) noexcept override {}
			void Shadow_Draw(void) noexcept override {}
			void Main_Draw(void) noexcept override {}
			void LAST_Draw(void) noexcept override {}
		};
		//
		class SceneControl {
			bool					m_EndFlag{ false };						//�I�������t���O
			bool					m_SelEnd{ true };
			bool					m_SelPause{ true };
			scenes					m_SelScene{ scenes::NONE_SCENE };		//���݂̃V�[��
			std::shared_ptr<Sceneclass::TEMPSCENE> m_ScenesPtr{ nullptr };
			LONGLONG				m_DrawWait{ 0 }, m_OldWait{ 0 }, m_Wait{ 0 };
			shaders::shader_Vertex	m_ScreenVertex;			// ���_�f�[�^
			std::array<shaders, 2>	m_Shader2D;
			//�[�x
			GraphHandle				DepthScreen;						// �[�x���擾����X�N���[��
			shaders					shader;									/*�V�F�[�_�[*/
			shaders					Depth;									/*�V�F�[�_�[*/
		public:
			SceneControl(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				// �[�x��`�悷��e�N�X�`���̍쐬( �P�`�����l�����������_�P�U�r�b�g�e�N�X�`�� )
				{
					SetCreateDrawValidGraphChannelNum(1);
					SetDrawValidFloatTypeGraphCreateFlag(TRUE);
					SetCreateGraphChannelBitDepth(16);
					DepthScreen = GraphHandle::Make(DXDraw::Instance()->m_DispXSize, DXDraw::Instance()->m_DispYSize, false);
					SetCreateDrawValidGraphChannelNum(0);
					SetDrawValidFloatTypeGraphCreateFlag(FALSE);
					SetCreateGraphChannelBitDepth(0);
				}
				Depth.Init("GetDepthVS.vso", "GetDepthPS.pso");
				shader.Init("NormalMesh_PointLightVS.vso", "NormalMesh_PointLightPS.pso");
				//�V�F�[�_�[
				this->m_ScreenVertex.SetScreenVertex(DrawParts->m_DispXSize, DrawParts->m_DispYSize);																				// ���_�f�[�^�̏���
				this->m_Shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//�����Y
				this->m_Shader2D[1].Init("DepthVS.vso", "DepthPS.pso");																//�����Y
			}
			~SceneControl(void) noexcept {
				if (this->m_ScenesPtr != nullptr) {
					this->m_ScenesPtr->Dispose();
				}
			}
			//
			const auto& isEnd(void) const noexcept { return this->m_EndFlag; }
			const auto& isPause(void) const noexcept { return this->m_SelPause; }
			//
			void ChangeScene(scenes select, std::shared_ptr<Sceneclass::TEMPSCENE>& ptr) {
				this->m_SelScene = select;
				this->m_ScenesPtr = ptr;
			}
			//�J�n
			void StartScene(void) noexcept {
				this->m_ScenesPtr->Set();
				this->m_SelEnd = false;
				this->m_SelPause = false;
				this->m_OldWait = GetNowHiPerformanceCount();
			}
			//
			bool Execute(void) noexcept {
				this->m_OldWait = GetNowHiPerformanceCount() - this->m_Wait;
				this->m_Wait = GetNowHiPerformanceCount();

				auto* DrawParts = DXDraw::Instance();
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//�f�o�b�O
				clsDx();
#endif // DEBUG
				if (ProcessMessage() != 0) {
					this->m_EndFlag = true;
					this->m_SelEnd = true;
				}
				FPS = GetFPS();
#ifdef DEBUG
				DebugParts->put_way();
#endif // DEBUG
				//�X�V
				this->m_SelPause = false;
				this->m_SelEnd |= !this->m_ScenesPtr->Update();
				//VR��ԂɓK�p
				DrawParts->Move_Player();
				//�I������
				if (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0) {
					this->m_EndFlag = true;
					return true;
				}
				if (this->m_SelEnd) { return true; }
				return false;
			}
			//�`��
			void Draw(void) noexcept {
				auto* OptionParts = OPTION::Instance();
				auto* EffectUseControl = EffectResource::Instance();
				auto* DrawParts = DXDraw::Instance();
				auto* PostPassParts = PostPassEffect::Instance();				//�z�X�g�p�X�G�t�F�N�g(VR�A�t���X�N���[�����p)
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//�f�o�b�O
#endif // DEBUG
				//���ʂ̕`��O�p��
				this->m_ScenesPtr->ReadyDraw();
				//
				{
					Camera3DInfo tmp_cam = this->m_ScenesPtr->GetMainCamera();
					DepthScreen.SetDraw_Screen(tmp_cam.GetCamPos(), tmp_cam.GetCamVec(), tmp_cam.GetCamUp(), tmp_cam.GetCamFov(), tmp_cam.GetCamNear(), tmp_cam.GetCamFar());
					{
						Depth.Draw_lamda(
							[&] {
							SetUseTextureToShader(0, -1);
							this->m_ScenesPtr->Depth_Draw();
						}
						);
					}
				}
				//UI��������
				PostPassParts->Set_UI_Draw([&] { this->m_ScenesPtr->UI_Draw(); });
				//VR�Ɉڂ�
				DrawParts->Draw([&] {
					auto tmp = GetDrawScreen();
					Camera3DInfo tmp_cam = this->m_ScenesPtr->GetMainCamera();
					tmp_cam.SetNowCamPos();
					{
						//��ʑ̐[�x�`��
						PostPassParts->BUF_Draw(
							[&] { this->m_ScenesPtr->BG_Draw(); },
							[&] { DrawParts->Draw_by_Shadow(
								[&] {
									this->m_ScenesPtr->Main_Draw();

									//this->m_ScenesPtr->Main_Draw2();

									shader.SetPixelParam(3.f*12.5f, 0, 0, 0);
									shader.Draw_lamda([&] {
										SetUseTextureToShader(1, DepthScreen.get());
										this->m_ScenesPtr->Main_Draw2();
									});
								}
								);
							}, tmp_cam, EffectUseControl->Update_effect_f);
						//�ŏI�`��
						PostPassParts->Set_MAIN_Draw();
					}
					GraphHandle::SetDraw_Screen(tmp);
					{
						SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//�g�p����e�N�X�`�����Z�b�g
						if (this->m_ScenesPtr->is_lens()) {
							//�����Y�`��
							this->m_Shader2D[0].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
							this->m_Shader2D[0].SetPixelParam(this->m_ScenesPtr->xp_lens(), this->m_ScenesPtr->yp_lens(), this->m_ScenesPtr->size_lens(), this->m_ScenesPtr->zoom_lens());
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								this->m_Shader2D[0].Draw(this->m_ScreenVertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						if (this->m_ScenesPtr->is_Blackout()) {
							//�`��
							this->m_Shader2D[1].SetPixelDispSize(DrawParts->m_DispXSize, DrawParts->m_DispYSize);
							this->m_Shader2D[1].SetPixelParam(this->m_ScenesPtr->Per_Blackout(), 0, 0, 0);
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								this->m_Shader2D[1].Draw(this->m_ScreenVertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						SetUseTextureToShader(0, -1);	//�g�p����e�N�X�`�����Z�b�g
					}
					GraphHandle::SetDraw_Screen(tmp, tmp_cam, false);
					{
						PostPassParts->MAIN_Draw();											//�f�t�H�`��
						PostPassParts->DrawUI(&this->m_ScenesPtr->GetMainCamera(), OptionParts->Get_useVR());	//UI1
						this->m_ScenesPtr->Item_Draw();											//UI2
					}
				}, this->m_ScenesPtr->GetMainCamera());
				//�f�B�X�v���C�`��
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				{
					//�`��
					if (OptionParts->Get_useVR()) {
						DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(255, 255, 255), TRUE);
						DrawParts->GetOutScreen().DrawRotaGraph(DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize / 2, 0.5f, 0, false);
					}
					else {
						DrawParts->GetOutScreen().DrawGraph(0, 0, false);
					}
					//��ɏ���
					this->m_ScenesPtr->LAST_Draw();
					//�f�o�b�O
#ifdef DEBUG
					DebugParts->end_way();
					DebugParts->debug(1920 - 300, 50, float(GetNowHiPerformanceCount() - this->m_Wait) / 1000.f);
					printfDx("AsyncCount :%d\n", GetASyncLoadNum());
					printfDx("Drawcall   :%d\n", GetDrawCallCount());
					printfDx("FPS        :%5.2f fps\n", FPS);
					printfDx("AllTime    :%5.2f ms\n", float(this->m_OldWait) / 1000.f);
					printfDx("DrawTime   :%5.2f ms\n", float(this->m_DrawWait) / 1000.f);
					printfDx("All-Draw   :%5.2f ms\n", float(this->m_OldWait - this->m_DrawWait) / 1000.f);
#endif // DEBUG

					//DepthScreen.DrawExtendGraph(0, 0, 960, 540, true);
				}
			}
			//��������
			void Vsync(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				DrawParts->Screen_Flip();	//��ʂ̔��f
				this->m_DrawWait = GetNowHiPerformanceCount() - this->m_Wait;
				//while ((GetNowHiPerformanceCount() - this->m_Wait) <= (1000 * 1000 / 60)) {}
			}
			//
			void NextScene(void) noexcept {
				//���
				this->m_ScenesPtr->Dispose();
				//�J��
				this->m_SelScene = this->m_ScenesPtr->Next_scene;
				this->m_ScenesPtr = this->m_ScenesPtr->Next_ptr;
			}
			//
		};
	};
};
