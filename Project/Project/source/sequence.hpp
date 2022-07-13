#pragma once
#include"Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//
		struct shaderparam {
			bool use{ false };
			float param[4]{ 0,0,0,0 };
		};
		//
		class TEMPSCENE {
		private:
			VECTOR_ref Shadow_minpos;
			VECTOR_ref Shadow_maxpos;
			VECTOR_ref Light_vec;
			COLOR_F Light_color = GetColorF(0, 0, 0, 0);
		protected:
			//初回チェック
			bool IsFirstLoop{ true };
			//カメラ
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
			scenes Next_scene{ scenes::NONE_SCENE };			//現在のシーン
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
				SetUseMaskScreenFlag(FALSE);//←カスタム画面でエフェクトが出なくなるため入れる
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
				//音位置指定
				Set3DSoundListenerPosAndFrontPosAndUpVec(camera_main.campos.get(), camera_main.camvec.get(), camera_main.camup.get());
				//影用意
				auto NearShadow = std::min(camera_main.far_, 25.f*12.5f);
				DrawParts->Ready_Shadow(camera_main.campos, [&] { Shadow_Draw(); }, [&] { Shadow_Draw_NearFar(); }, VECTOR_ref::vget(NearShadow, 30.f, NearShadow), VECTOR_ref::vget(2000.f, 60.f, 2000.f));//MAIN_LOOPのnearはこれ (Get_Mine()->Damage.Get_alive()) ? VECTOR_ref::vget(2.f, 2.5f, 2.f) : VECTOR_ref::vget(10.f, 2.5f, 10.f)
			}
			virtual void UI_Draw(void) noexcept {}
			virtual void BG_Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_x, GetColor(192, 192, 192), TRUE);
			}

			virtual void Depth_Draw(void) noexcept {}

			virtual void Shadow_Draw_Far(void) noexcept {}
			virtual void Shadow_Draw_NearFar(void) noexcept {}
			virtual void Shadow_Draw(void) noexcept {}
			virtual void Main_Draw(void) noexcept {}
			virtual void Main_Draw2(void) noexcept {}

			const bool& is_lens(void) const noexcept { return shaderParam[0].use; }
			const float& xp_lens(void) const noexcept { return shaderParam[0].param[0]; }
			const float& yp_lens(void) const noexcept { return shaderParam[0].param[1]; }
			const float& size_lens(void) const noexcept { return shaderParam[0].param[2]; }
			const float& zoom_lens(void) const noexcept { return shaderParam[0].param[3]; }

			void Set_is_lens(bool value) noexcept { shaderParam[0].use = value; }
			void Set_xp_lens(float value) noexcept { shaderParam[0].param[0] = value; }
			void Set_yp_lens(float value) noexcept { shaderParam[0].param[1] = value; }
			void Set_size_lens(float value) noexcept { shaderParam[0].param[2] = value; }
			void Set_zoom_lens(float value) noexcept { shaderParam[0].param[3] = value; }

			const bool& is_Blackout(void) const noexcept { return shaderParam[1].use; }
			const float& Per_Blackout(void) const noexcept { return shaderParam[1].param[0]; }

			void Set_is_Blackout(bool value) noexcept { shaderParam[1].use = value; }
			void Set_Per_Blackout(float value) noexcept { shaderParam[1].param[0] = value; }

			virtual void Item_Draw(void) noexcept {}
			virtual void LAST_Draw(void) noexcept {}
			virtual void KeyOperation_VR(void) noexcept {}
			virtual void KeyOperation(void) noexcept {}
		};
		//シーンサンプル
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
			bool m_EndFlag{ false };						//終了処理フラグ
			bool m_SelEnd{ true };
			bool m_SelPause{ true };
			scenes m_SelScene{ scenes::NONE_SCENE };		//現在のシーン
			std::shared_ptr<Sceneclass::TEMPSCENE> m_ScenesPtr{ nullptr };
			LONGLONG m_DrawWait{ 0 }, m_OldWait{ 0 }, m_Wait{ 0 };
			shaders::shader_Vertex m_ScreenVertex;			// 頂点データ
			std::array<shaders, 2> m_Shader2D;
			//深度
			GraphHandle DepthScreen;						// 深度を取得するスクリーン
			shaders shader;									/*シェーダー*/
			shaders Depth;									/*シェーダー*/
		public:
			SceneControl(void) noexcept {
				// 深度を描画するテクスチャの作成( １チャンネル浮動小数点１６ビットテクスチャ )
				{
					SetCreateDrawValidGraphChannelNum(1);
					SetDrawValidFloatTypeGraphCreateFlag(TRUE);
					SetCreateGraphChannelBitDepth(16);
					DepthScreen = GraphHandle::Make(DXDraw::Instance()->disp_x, DXDraw::Instance()->disp_y, false);
					SetCreateDrawValidGraphChannelNum(0);
					SetDrawValidFloatTypeGraphCreateFlag(FALSE);
					SetCreateGraphChannelBitDepth(0);
				}
				Depth.Init("GetDepthVS.vso", "GetDepthPS.pso");
				shader.Init("NormalMesh_PointLightVS.vso", "NormalMesh_PointLightPS.pso");
				//シェーダー
				this->m_ScreenVertex.Set();																					// 頂点データの準備
				this->m_Shader2D[0].Init("VS_lens.vso", "PS_lens.pso");																//レンズ
				this->m_Shader2D[1].Init("DepthVS.vso", "DepthPS.pso");																//レンズ
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
			//開始
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
				auto* DebugParts = DebugClass::Instance();					//デバッグ
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
				//更新
				this->m_SelPause = false;
				this->m_SelEnd |= !this->m_ScenesPtr->Update();
				//VR空間に適用
				DrawParts->Move_Player();
				//終了判定
				if (CheckHitKeyWithCheck(KEY_INPUT_ESCAPE) != 0) {
					this->m_EndFlag = true;
					return true;
				}
				if (this->m_SelEnd) { return true; }
				return false;
			}
			//描画
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* PostPassParts = PostPassEffect::Instance();				//ホストパスエフェクト(VR、フルスクリーン共用)
#ifdef DEBUG
				auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//共通の描画前用意
				this->m_ScenesPtr->ReadyDraw();
				//
				{
					cam_info tmp_cam = this->m_ScenesPtr->Get_Camera();
					{
						DepthScreen.SetDraw_Screen(tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_);
						{
							Depth.Draw_lamda(
								[&] {
								SetUseTextureToShader(0, -1);
								this->m_ScenesPtr->Depth_Draw();
							}
							);
						}
					}
				}
				//UI書き込み
				PostPassParts->Set_UI_Draw([&] { this->m_ScenesPtr->UI_Draw(); });
				//VRに移す
				DrawParts->Draw([&] {
					auto tmp = GetDrawScreen();
					cam_info tmp_cam = this->m_ScenesPtr->Get_Camera();
					tmp_cam.campos = GetCameraPosition();
					tmp_cam.camvec = GetCameraTarget();
					{
						//被写体深度描画
						PostPassParts->BUF_Draw(
							[&] { this->m_ScenesPtr->BG_Draw(); },
							[&] { DrawParts->Draw_by_Shadow(
								[&] {
									this->m_ScenesPtr->Main_Draw();

									//this->m_ScenesPtr->Main_Draw2();

									shader.Set_param(3.f*12.5f, 0, 0, 0);
									shader.Draw_lamda([&] {
										SetUseTextureToShader(1, DepthScreen.get());
										this->m_ScenesPtr->Main_Draw2();
									});
								}
								);
							}, tmp_cam, effectControl.Update_effect_f);
						//最終描画
						PostPassParts->Set_MAIN_Draw();
					}
					GraphHandle::SetDraw_Screen(tmp);
					{
						SetUseTextureToShader(0, PostPassParts->Get_MAIN_Screen().get());	//使用するテクスチャをセット
						if (this->m_ScenesPtr->is_lens()) {
							//レンズ描画
							this->m_Shader2D[0].Set_dispsize();
							this->m_Shader2D[0].Set_param(this->m_ScenesPtr->xp_lens(), this->m_ScenesPtr->yp_lens(), this->m_ScenesPtr->size_lens(), this->m_ScenesPtr->zoom_lens());
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								this->m_Shader2D[0].Draw(this->m_ScreenVertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						if (this->m_ScenesPtr->is_Blackout()) {
							//描画
							this->m_Shader2D[1].Set_dispsize();
							this->m_Shader2D[1].Set_param(this->m_ScenesPtr->Per_Blackout(), 0, 0, 0);
							PostPassParts->Get_BUF_Screen().SetDraw_Screen();
							{
								this->m_Shader2D[1].Draw(this->m_ScreenVertex);
							}
							PostPassParts->Set_MAIN_Draw_nohost();
						}
						SetUseTextureToShader(0, -1);	//使用するテクスチャをセット
					}
					GraphHandle::SetDraw_Screen(tmp, tmp_cam.campos, tmp_cam.camvec, tmp_cam.camup, tmp_cam.fov, tmp_cam.near_, tmp_cam.far_, false);
					{
						PostPassParts->MAIN_Draw();											//デフォ描画
						PostPassParts->DrawUI(&this->m_ScenesPtr->Get_Camera(), DrawParts->use_vr);	//UI1
						this->m_ScenesPtr->Item_Draw();											//UI2
					}
				}, this->m_ScenesPtr->Get_Camera());
				//ディスプレイ描画
				GraphHandle::SetDraw_Screen((int32_t)(DX_SCREEN_BACK), true);
				{
					//描画
					if (DrawParts->use_vr) {
						DrawBox(0, 0, DrawParts->disp_x, DrawParts->disp_y, GetColor(255, 255, 255), TRUE);
						DrawParts->outScreen[0].DrawRotaGraph(DrawParts->disp_x / 2, DrawParts->disp_y / 2, 0.5f, 0, false);
					}
					else {
						DrawParts->outScreen[0].DrawGraph(0, 0, false);
					}
					//上に書く
					this->m_ScenesPtr->LAST_Draw();
					//デバッグ
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
			//垂直同期
			void Vsync(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				//画面の反映
				DrawParts->Screen_Flip();
				this->m_DrawWait = GetNowHiPerformanceCount() - this->m_Wait;
				//while ((GetNowHiPerformanceCount() - this->m_Wait) <= (1000 * 1000 / 60)) {}
			}
			//
			void NextScene(void) noexcept {
				//解放
				this->m_ScenesPtr->Dispose();
				//遷移
				this->m_SelScene = this->m_ScenesPtr->Next_scene;
				this->m_ScenesPtr = this->m_ScenesPtr->Next_ptr;
			}
			//
		};
	};
};
