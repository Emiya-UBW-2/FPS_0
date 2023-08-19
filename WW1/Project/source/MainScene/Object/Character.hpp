#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {


		class CharacterClass :
			public ObjectBaseClass,
			public StaminaControl,
			public LifeControl,
			public KeyControl,
			public ShapeControl,
			public OverrideControl,

			public EffectControl
		{
		private://キャラパラメーター
			const float											SpeedLimit{ 4.5f };
		private:
			CharaTypeID											m_CharaType;
			//
			float												m_AccelAdd{ 0.f };
			float												m_Accel{ 0.f };
			float												m_Thrust{ 1.f };
			float												m_Speed{ 1.f };
			//
			std::array<float, (int)CharaAnimeID::AnimeIDMax>	m_AnimPerBuf{ 0 };
			VECTOR_ref											m_PrevPosBuf;
			VECTOR_ref											m_PosBuf;
			//入力
			bool												m_Press_Shot{ false };
			std::array<float,2>									m_ShotTimer{ 0.f, 0.f };	//yrad
			//体力
			std::vector<HitBox>									m_HitBox;
			DamageEvent											m_DamageEvent;									//
			unsigned long long									m_DamageSwitch{ 0 };							//
			unsigned long long									m_DamageSwitchRec{ 0 };							//
			//サウンド
			int													m_CharaSound{ -1 };
			//
			bool												m_SendCamShake{ false };
			//
			VECTOR_ref											m_EyeVecR;
			VECTOR_ref											m_CamEyeVec;
			VECTOR_ref											m_RadBufPrev;

			//物理
			std::vector<VECTOR_ref>								m_AddVecs;

			float												m_Yaw{ 0.f };	//yrad
			float												m_Roll{ 0.f };	//zrad
			float												m_Pitch{ 0.f };	//xrad

			std::shared_ptr<AmmoData>							m_AmmoData;

			int													m_EngineSESel{ -1 };
			int													m_PropellerSESel{ -1 };

			float												m_EffectAlphaPer1{ 0.f };
		public:
			bool m_NearAmmo{ false };//至近弾(被弾含む)
			int aim_cnt{ 0 };
		public://ゲッター(ラッパー)
			const auto		GetCharaFrame(CharaFrame frame) const noexcept { return m_Frames[(int)frame].first; }
		public://ゲッター
			auto&			GetAnimeBuf(CharaAnimeID anim) noexcept { return this->m_AnimPerBuf[(int)anim]; }
			auto&			GetAnime(CharaAnimeID anim) noexcept { return this->GetObj().get_anime((int)anim); }
			const auto&		GetReticleRad(void) const noexcept { return 0.f; }
			const auto&		GetSendCamShake(void) const noexcept { return this->m_SendCamShake; }
			const auto&		GetDamageEvent(void) const noexcept { return this->m_DamageEvent; }
			const auto&		GetDamageSwitch(void) const noexcept { return this->m_DamageSwitch; }
			const auto&		GetDamageSwitchRec(void) const noexcept { return this->m_DamageSwitchRec; }
			const auto		GetSpeed(void) const noexcept { return this->m_Speed * 28.f; }
			const auto&		GetCharaType(void) const noexcept { return this->m_CharaType; }
			const auto&		GetAccel(void) const noexcept { return this->m_Accel; }
			const auto&		GetYaw(void) const noexcept { return this->m_Yaw; }
			const auto&		GetRoll(void) const noexcept { return this->m_Roll; }
			const auto&		GetPitch(void) const noexcept { return this->m_Pitch; }
		public://セッター
			void			SetCamEyeVec(const VECTOR_ref& value) noexcept { this->m_CamEyeVec = value; }
			void			SetDamageSwitchRec(unsigned long long value) noexcept { this->m_DamageSwitchRec = value; }
			void			ResetFrameLocalMat(CharaFrame frame) noexcept { GetObj().frame_Reset(GetCharaFrame(frame)); }
			bool			SetDamageEvent(const DamageEvent& value) noexcept {
				if (this->m_MyID == value.ID && this->m_objType == value.CharaType) {
					auto prev = GetHP();
					SubHP(value.Damage, value.rad);
					if ((GetHP() <= GetHPMax() * 3 / 10) && (prev > GetHPMax() * 3 / 10)) {
						EffectControl::SetLoop((EffectResource::Effect)12, this->GetMatrix().pos());
					}
					return true;
				}
				return false;
			}
			void			SetAnimLoop(CharaAnimeID ID, float speed) { ObjectBaseClass::SetAnimLoop((int)ID, speed); }
			void			SetFrameLocalMat(CharaFrame frame, const MATRIX_ref&value) noexcept { GetObj().SetFrameLocalMatrix(GetCharaFrame(frame), value * this->m_Frames[(int)frame].second); }
			void			SetShapePer(CharaShape pShape, float Per) noexcept { this->m_Shapes[(int)pShape].second = Per; }
			void			SetCharaType(CharaTypeID value) noexcept { this->m_CharaType = value; }
		public://ゲッター
			const auto		GetFrameLocalMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalMatrix(GetCharaFrame(frame)); }
			const auto		GetFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix(GetCharaFrame(frame)); }
			const auto		GetParentFrameWorldMat(CharaFrame frame) const noexcept { return this->GetObj_const().GetFrameLocalWorldMatrix((int)this->GetObj_const().frame_parent(GetCharaFrame(frame))); }
			const auto		GetCharaDir(void) const noexcept { return this->m_move.mat; }
			const auto		GetCharaVecX(void) const noexcept { return GetCharaDir().xvec(); }
			const auto		GetCharaVecY(void) const noexcept { return GetCharaDir().yvec(); }
			const auto		GetCharaVector(void) const noexcept { return GetCharaDir().zvec() * -1.f; }

			const auto		GetEyeVecMat(void) const noexcept {
				auto tmpUpperMatrix = MATRIX_ref::RotX(KeyControl::GetRad().x()) * MATRIX_ref::RotY(KeyControl::GetRad().y());
				return tmpUpperMatrix * this->m_move.mat;
			}
			const auto		GetEyeVecX(void) const noexcept { return GetEyeVecMat().xvec(); }
			const auto		GetEyeVecY(void) const noexcept { return GetEyeVecMat().yvec(); }
			const auto		GetEyeVector(void) const noexcept { return GetEyeVecMat().zvec() * -1.f; }
			const auto		GetEyePosition(void) const noexcept { return GetFrameWorldMat(CharaFrame::Head).pos() + MATRIX_ref::Vtrans(VECTOR_ref::vget(0.f, 0.f, -0.5f), GetEyeVecMat()); }
		private:
			//被弾チェック
			const auto		CheckLineHited(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				bool is_Hit = false;
				for (auto& h : this->m_HitBox) {
					is_Hit |= h.Colcheck(StartPos, pEndPos);
				}
				return is_Hit;
			}
		public:
			const auto		CheckLineHit(const VECTOR_ref& StartPos, VECTOR_ref* pEndPos) const noexcept {
				if (GetMinLenSegmentToPoint(StartPos, *pEndPos, m_move.pos) <= 10.0f*Scale_Rate) {
					if (this->CheckLineHited(StartPos, pEndPos)) {									//とりあえず当たったかどうか探す
						return true;
					}
				}
				return false;
			}
			void			HitDamage(const VECTOR_ref& ShooterPos, HitPoint Damage) {
				//ダメージ計算
				auto v1 = GetCharaVector();
				auto v2 = (ShooterPos - this->m_move.pos).Norm(); v2.y(0);
				this->m_DamageEvent.SetEvent(this->m_MyID, m_objType, Damage, atan2f(v1.cross(v2).y(), v1.dot(v2)));
				++this->m_DamageSwitch;// %= 255;//
			}
			//
			void			ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID) noexcept;
			void			SetInput(const InputControl& pInput, bool pReady) noexcept;
			void			SetEyeVec() noexcept;
		private: //更新関連
			void			ExecuteSavePrev(void) noexcept;			//以前の状態保持														//
			void			ExecuteInput(void) noexcept;			//操作																	//0.01ms
			void			ExecuteAnim(void) noexcept;				//SetMat指示															//0.03ms
			void			ExecuteMatrix(void) noexcept;			//SetMat指示更新														//0.03ms
			void			ExecuteShape(void) noexcept;			//顔																	//0.01ms
		public: //コンストラクタ、デストラクタ
			CharacterClass(void) noexcept {
				this->m_objType = ObjType::Plane;
				this->m_IsDraw = true;
			}
			~CharacterClass(void) noexcept {}
		public: //継承
			//
			void			Init(void) noexcept override {
				ObjectBaseClass::Init();
				//
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					//getparams::_str(mdata);
					FileRead_close(mdata);
				}
				//
				{
					m_AmmoData = std::make_shared<AmmoData>();
					m_AmmoData->Set("data/ammo/M2_AP/");
				}
				//
				m_HitBox.resize(10);
			}
			void			FirstExecute(void) noexcept override {
				auto* SE = SoundPool::Instance();
				//初回のみ更新する内容
				if (this->m_IsFirstLoop) {
					for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) { GetAnime((CharaAnimeID)i).per = GetAnimeBuf((CharaAnimeID)i); }
					GetObj().work_anime();

					if (this->m_MyID == 0) {
						EffectControl::SetLoop((EffectResource::Effect)3, GetMatrix().pos());
						EffectControl::SetLoop((EffectResource::Effect)4, GetMatrix().pos());
					}

					m_EngineSESel = SE->Get((int)SoundEnum::Engine).Play_3D(0, GetMatrix().pos(), 250.f*Scale_Rate,-1, DX_PLAYTYPE_LOOP);										//サウンド
					m_PropellerSESel = SE->Get((int)SoundEnum::Propeller).Play_3D(0, GetMatrix().pos(), 250.f*Scale_Rate, -1, DX_PLAYTYPE_LOOP);										//サウンド
				}
				else {
					SE->Get((int)SoundEnum::Engine).SetPos(0, m_EngineSESel, GetMatrix().pos());
					SE->Get((int)SoundEnum::Propeller).SetPos(0, m_PropellerSESel, GetMatrix().pos());
				}
				if (this->m_MyID == 0) {
					EffectControl::Update_LoopEffect((EffectResource::Effect)3, GetMatrix().pos() + GetMatrix().xvec()*Scale_Rate*5.0f, GetMatrix().zvec()*-1.f, 0.36f);
					EffectControl::Update_LoopEffect((EffectResource::Effect)4, GetMatrix().pos() + GetMatrix().xvec()*Scale_Rate*-5.0f, GetMatrix().zvec()*-1.f, 0.36f);
					//*
					Easing(&this->m_EffectAlphaPer1, (this->m_Pitch < -0.5f) ? 1.f : 0.f, 0.95f, EasingType::OutExpo);
					int Alpha = std::clamp((int)(255.f* this->m_EffectAlphaPer1), 0, 255);
					EffectControl::SetEffectColor((EffectResource::Effect)3, 255, 255, 255, Alpha);
					EffectControl::SetEffectColor((EffectResource::Effect)4, 255, 255, 255, Alpha);
					//*/
				}
				if (GetHP() <= GetHPMax() * 3 / 10) {
					EffectControl::Update_LoopEffect((EffectResource::Effect)12, GetMatrix().pos(), GetMatrix().zvec()*-1.f, 6.f * Scale_Rate);
				}

				ExecuteSavePrev();			//以前の状態保持
				ExecuteInput();				//操作//0.01ms
				ExecuteAnim();				//AnimUpdte//0.03ms
				ExecuteMatrix();			//SetMat指示//0.03ms
				ExecuteShape();				//顔//スコープ内0.01ms
				EffectControl::Execute();
			}
			void			Draw(bool isDrawSemiTrans) noexcept override {
				int fog_enable;
				int fog_mode;
				int fog_r, fog_g, fog_b;
				float fog_start, fog_end;
				float fog_density;

				fog_enable = GetFogEnable();													// フォグが有効かどうかを取得する( TRUE:有効  FALSE:無効 )
				fog_mode = GetFogMode();														// フォグモードを取得する
				GetFogColor(&fog_r, &fog_g, &fog_b);											// フォグカラーを取得する
				GetFogStartEnd(&fog_start, &fog_end);											// フォグが始まる距離と終了する距離を取得する( 0.0f ～ 1.0f )
				fog_density = GetFogDensity();													// フォグの密度を取得する( 0.0f ～ 1.0f )

				SetFogEnable(TRUE);
				if (GetCharaType() == CharaTypeID::Enemy) {
					SetFogColor(0, 0, 0);
					SetFogStartEnd(Scale_Rate*100.f, Scale_Rate*400.f);
				}
				else {
					SetFogColor(64, 64, 64);
					SetFogStartEnd(Scale_Rate*100.f, Scale_Rate*400.f);
				}
				//SetFogEnable(FALSE);

				//
				if (this->m_IsActive && this->m_IsDraw) {
					if (CheckCameraViewClip_Box(
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(-30.f*Scale_Rate, -30.f*Scale_Rate, -30.f*Scale_Rate)).get(),
						(this->GetObj().GetMatrix().pos() + VECTOR_ref::vget(30.f*Scale_Rate, 30.f*Scale_Rate, 30.f*Scale_Rate)).get()) == FALSE
						) {
						auto* DrawParts = DXDraw::Instance();
						DrawParts->SetUseFarShadowDraw(false);
						for (int i = 0; i < this->GetObj().mesh_num(); i++) {
							if ((MV1GetMeshSemiTransState(this->GetObj().get(), i) == TRUE) == isDrawSemiTrans) {
								this->GetObj().DrawMesh(i);
							}
						}
						DrawParts->SetUseFarShadowDraw(true);
					}
				}
				//hitbox描画
				if (false)
				{
					//this->GetObj().SetOpacityRate(0.5f);
					SetFogEnable(FALSE);
					SetUseLighting(FALSE);
					SetUseZBuffer3D(FALSE);

					for (auto& h : this->m_HitBox) {
						h.Draw();
					}

					SetUseZBuffer3D(TRUE);
					SetUseLighting(TRUE);
				}

				SetFogEnable(fog_enable);
				SetFogMode(fog_mode);
				SetFogColor(fog_r, fog_g, fog_b);
				SetFogStartEnd(fog_start, fog_end);
				SetFogDensity(fog_density);
			}
			void			DrawShadow(void) noexcept override {
				if (this->m_IsActive) {
					this->GetObj().DrawModel();
				}
			}
			//
			void			Dispose(void) noexcept override {
				this->m_BackGround.reset();
				this->GetObj().Dispose();
				this->m_col.Dispose();
				this->m_move.vec.clear();
				EffectControl::Dispose();
				this->m_AmmoData.reset();
			}
		};
	};
};
