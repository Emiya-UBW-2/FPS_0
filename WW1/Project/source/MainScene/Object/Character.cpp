#pragma once
#include	"../../Header.hpp"


#include <math.h>

namespace FPS_n2 {
	namespace Sceneclass {
		//
		void			CharacterClass::ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos, PlayerID pID) noexcept {
			StaminaControl::InitStamina();
			LifeControl::InitLife();
			KeyControl::InitKey(pxRad, pyRad);
			ShapeControl::InitShape();
			//
			this->m_MyID = pID;
			//
			for (int i = 0; i < (int)CharaAnimeID::AnimeIDMax; i++) {
				GetAnimeBuf((CharaAnimeID)i) = 0.f;
			}
			//
			this->m_Speed = 0.f;
			this->m_Press_Shot = false;
			this->m_CharaSound = -1;			//サウンド
			//動作にかかわる操作
			ResetMove(MATRIX_ref::RotY(KeyControl::GetRad().y()), pPos);
			this->m_PosBuf = pPos;
			this->m_PrevPosBuf = this->m_PosBuf;
			//
			InitOverride(pPos);
			//
			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;
		}
		void			CharacterClass::SetInput(const InputControl& pInput, bool pReady) noexcept {
			KeyControl::InputKey(pInput, pReady, VECTOR_ref::zero());
			//AIM
			this->m_Press_Shot = pInput.GetAction5() && KeyControl::GetKeyActive();
		}
		void			CharacterClass::SetEyeVec() noexcept {
			Easing(&m_EyeVecR, m_CamEyeVec, 0.8f, EasingType::OutExpo);

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Head));
			auto v1 = (GetFrameWorldMat(CharaFrame::Head).GetRot() * GetCharaDir().Inverse()).zvec()*-1.f;
			auto v2 = Lerp(MATRIX_ref::Vtrans(m_EyeVecR.Norm(), GetCharaDir().Inverse()), v1, 0.f);

			auto radlimit = deg2rad(70);
			if (v1.dot(v2) <= cos(radlimit)) {
				v2 = v1 * cos(radlimit) + v1.cross(v1.cross(v2)) * (-sin(radlimit));
			}
			SetFrameLocalMat(CharaFrame::Head, MATRIX_ref::RotX(deg2rad(-10))*MATRIX_ref::RotVec2(v1, v2) * GetFrameLocalMat(CharaFrame::Head).GetRot());
		}
		//以前の状態保持														//
		void			CharacterClass::ExecuteSavePrev(void) noexcept {
			this->m_Speed = (this->m_PrevPosBuf - this->m_PosBuf).Length() / (60.f / FPS);
			this->m_PrevPosBuf = this->m_PosBuf;
			this->m_RadBufPrev = KeyControl::GetRad();
		}
		//操作
		void			CharacterClass::ExecuteInput(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			this->m_SendCamShake = false;
			//
			KeyControl::ExcuteKey();
			//
			if (this->m_MyID == 0) {
				SetEyeVec();
			}
			//
			auto vec = GetMatrix().zvec()*-1.f;
			vec += VECTOR_ref::vget(GetRandf(0.005f), GetRandf(0.005f), GetRandf(0.005f));
			vec = vec.Norm();
			if (m_ShotTimer[0] <= 0.f) {
				if (KeyControl::GetShotKey().press()) {
					m_ShotTimer[0] = 0.2f;
					m_ShotTimer[1] = 0.2f / 2.f;
					//射出
					{
						auto Pos = GetFrameWorldMat(CharaFrame::Gun1).pos();
						SE->Get((int)SoundEnum::Shot2).Play_3D(0, Pos, 250.f*Scale_Rate);													//サウンド
						EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire, Pos, vec, m_AmmoData->GetCaliber() / 0.1f * Scale_Rate, 4.f);	//銃発砲エフェクトのセット
						auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
						LastAmmo->Put(m_AmmoData, Pos, vec, this->m_MyID);
						Easing(&this->m_Yaw, this->m_Yaw + 1.f, 0.95f, EasingType::OutExpo);	//yrad
					}
				}
			}
			else {
				m_ShotTimer[0] -= 1.f / FPS;
			}
			//
			if (m_ShotTimer[1] <= 0.f) {
				if (KeyControl::GetShotKey().press()) {
					m_ShotTimer[1] = 0.2f;
					//射出
					{
						auto Pos = GetFrameWorldMat(CharaFrame::Gun2).pos();
						SE->Get((int)SoundEnum::Shot2).Play_3D(0, Pos, 250.f*Scale_Rate);													//サウンド
						EffectControl::SetOnce_Any(EffectResource::Effect::ef_fire, Pos, vec, m_AmmoData->GetCaliber() / 0.1f * Scale_Rate, 4.f);	//銃発砲エフェクトのセット
						auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
						LastAmmo->Put(m_AmmoData, Pos, vec, this->m_MyID);
						Easing(&this->m_Yaw, this->m_Yaw - 1.f, 0.95f, EasingType::OutExpo);	//yrad
					}
				}
			}
			else {
				m_ShotTimer[1] -= 1.f / FPS;
			}
			//
		}
		//SetMat指示
		void			CharacterClass::ExecuteAnim(void) noexcept {
			//auto SE = SoundPool::Instance();
			//アニメ演算
			{
				//SetAnimLoop(GetBottomWalkAnimSel(), 1.f);
			}
			//
			GetObj().work_anime();
		}
		//SetMat指示更新
		void			CharacterClass::ExecuteMatrix(void) noexcept {
			//auto SE = SoundPool::Instance();
			//auto* DrawParts = DXDraw::Instance();

			auto SpeedNormal = 51.39f*0.5f;//秒速換算 92.5km/hくらい

			if (KeyControl::GetRKey().press()) {
				this->m_Accel += 1.f*60.f / FPS;
			}
			else if (KeyControl::GetFKey().press()) {
				this->m_Accel -= 1.f*60.f / FPS;
			}
			else {
				Easing(&this->m_Accel, 0.f, 0.975f, EasingType::OutExpo);	//yrad
				if (std::abs(this->m_Accel) < 0.05f) {
					this->m_Accel = 0.f;
				}
			}
			this->m_Accel = std::clamp(this->m_Accel, -1.f, 1.f);

			float yp = (this->m_move.mat.zvec()*-1.f).y();
			if (yp > 0.f) {
				this->m_AccelAdd += -0.15f * 60.f / FPS * (std::powf(std::max(0.f, std::abs(yp) - 0.5f), 1.25f));
			}
			else {
				this->m_AccelAdd += 2.5f * 60.f / FPS * (std::powf(std::max(0.f, std::abs(yp) - 0.3f), 2.f));
			}
			if (this->m_Thrust / SpeedNormal > 1.5f) {
				if (std::abs(yp) <= sin(deg2rad(10))) {
					this->m_AccelAdd += -0.1f * 60.f / FPS;
				}
			}
			this->m_AccelAdd += -0.15f * 60.f / FPS * std::clamp((this->m_move.pos.y() - (600.f*Scale_Rate)) / (3000.f*Scale_Rate), 0.f, 1.f);

			this->m_AccelAdd = std::clamp(this->m_AccelAdd + this->m_Accel*0.05f * 60.f / FPS, -30.f, 30.f);

			//printfDx("[%f]\n", this->m_AccelAdd);

			this->m_Thrust = SpeedNormal + this->m_AccelAdd;

			auto RotScale = std::clamp(this->m_Thrust / SpeedNormal, 0.5f, 1.f);
			auto StallScale = std::abs((0.5f - this->m_Thrust / SpeedNormal)*4.f) - 2.f;
			StallScale = std::clamp(StallScale, 0.0f, 0.5f);
			//失速
			if ((this->m_move.vec.Length() > 0.01f) && (this->m_move.vec.Norm().dot(this->m_move.mat.zvec()*-1.f) < cos(deg2rad(30)))) {
				StallScale = 2.f;
			}

			//
			{
				auto PAdd = -(KeyControl::GetRadBuf() - this->m_RadBufPrev).y()*100.f;
				if (this->GetHP() == 0) {
					PAdd += 0.1f;
				}
				PAdd += (float)(KeyControl::GetQKey().press() - KeyControl::GetEKey().press())*0.5f;
				PAdd = std::clamp(PAdd, -1.f, 1.f);
				Easing(&this->m_Yaw, RotScale * 0.4f * (PAdd + GetRandf(2.f)*StallScale), 0.95f, EasingType::OutExpo);	//yrad
			}
			//
			if (KeyControl::GetPressLeft()) {
				Easing(&this->m_Roll, RotScale* 1.25f * (float)(1.f + GetRandf(2.f)*StallScale), 0.95f, EasingType::OutExpo);			//zrad
			}
			else if (KeyControl::GetPressRight()) {
				Easing(&this->m_Roll, RotScale* 1.25f * (float)(-1.f + GetRandf(2.f)*StallScale), 0.95f, EasingType::OutExpo);			//zrad
			}
			else {
				Easing(&this->m_Roll, RotScale* 1.25f * (float)(0.f + GetRandf(2.f)*StallScale), 0.975f, EasingType::OutExpo);			//zrad
			}
			//
			{
				auto PAdd = -(KeyControl::GetRadBuf() - this->m_RadBufPrev).x()*100.f;
				if (this->GetHP()==0) {
					PAdd += 0.1f;
				}
				if (KeyControl::GetPressFront()) {
					PAdd += 0.85f;
				}
				else if (KeyControl::GetPressRear()) {
					PAdd += -1.f;
				}
				PAdd = std::clamp(PAdd, -1.f, 0.85f);
				if (PAdd > 0.f) {
					Easing(&this->m_Pitch, RotScale* 0.7f * (float)(PAdd + GetRandf(2.f)*StallScale), 0.95f, EasingType::OutExpo);		//xrad
				}
				else if (PAdd == 0.f) {
					Easing(&this->m_Pitch, RotScale* 0.7f * (float)(PAdd + GetRandf(2.f)*StallScale), 0.975f, EasingType::OutExpo);		//xrad
				}
				else {
					Easing(&this->m_Pitch, RotScale* 0.5f * (float)(PAdd + GetRandf(2.f)*StallScale), 0.95f, EasingType::OutExpo);		//xrad
				}
			}
			this->m_move.mat = MATRIX_ref::RotY(-deg2rad(this->m_Yaw * 60.f / FPS)) * this->m_move.mat;
			this->m_move.mat = MATRIX_ref::RotZ(-deg2rad(this->m_Roll * 60.f / FPS)) * this->m_move.mat;
			this->m_move.mat = MATRIX_ref::RotX(-deg2rad(this->m_Pitch * 60.f / FPS)) * this->m_move.mat;

			//初回のみ更新する内容
			if (this->m_IsFirstLoop) {
				m_AddVecs.clear();
				m_AddVecs.emplace_back(VECTOR_ref::up() * (M_GR / (60.f*60.f) * 2.f));//重力
				m_AddVecs.emplace_back(VECTOR_ref::zero());//揚力
				m_AddVecs.emplace_back(VECTOR_ref::zero());//推力
			}

			//ベクトルの演算
			m_AddVecs.at(1) = (this->m_move.mat.yvec() * -(M_GR / (60.f*60.f) * 2.f) * std::clamp(this->m_Thrust / SpeedNormal, 0.1f, 1.f));//推力
			m_AddVecs.at(2) = (this->m_move.mat.zvec() * -1.f * this->m_Thrust / (Scale_Rate*(60.f / 5.f)));//推力
			//反映
			for (auto& v : m_AddVecs) {
				this->m_move.vec += v * 60.f / FPS;
			}
			//地面判定
			{
				float LocalUnder = -1.5f*Scale_Rate;
				auto HitResult = this->m_BackGround->GetGroundCol().CollCheck_Line(this->m_PrevPosBuf + VECTOR_ref::up() * 5.f*Scale_Rate, this->m_PrevPosBuf + VECTOR_ref::up() * (-1.f*Scale_Rate + LocalUnder));
				if (HitResult.HitFlag == TRUE) {
					//
					if (this->m_move.vec.dot(HitResult.Normal) < 0.f) {
						auto yPos = this->m_PosBuf.y();
						Easing(&yPos, HitResult.HitPosition.y - LocalUnder, 0.8f, EasingType::OutExpo);
						this->m_PosBuf.y(yPos);
					}
					//
					this->m_move.vec.y(std::max(this->m_move.vec.y(), 0.f));
					//
					if (this->GetHP() > 0) {
						if (this->m_move.mat.yvec().dot(HitResult.Normal) < std::cos(deg2rad(30))) {
							this->HitDamage(this->GetMatrix().pos(), this->GetHPMax());
							EffectControl::SetOnce_Any((EffectResource::Effect)11, HitResult.HitPosition, VECTOR_ref::front(), 3.f * Scale_Rate);
						}
					}
					if (this->GetHP() == 0) {
						auto Y = this->m_move.vec.y();
						Easing(&this->m_move.vec, VECTOR_ref::zero(), 0.5f, EasingType::OutExpo);
						this->m_move.vec.y(Y);
					}
				}
			}
			//摩擦
			Easing(&this->m_move.vec, VECTOR_ref::zero(), 0.95f, EasingType::OutExpo);
			this->m_PosBuf += this->m_move.vec;
			//壁判定
			{
				//std::vector<std::pair<MV1*, int>> cols;
				//cols.emplace_back(std::make_pair((MV1*)(&this->m_BackGround->GetGroundCol()), -1));
				//col_wall(this->m_PrevPosBuf, &this->m_PosBuf, cols);
			}
			{
				VECTOR_ref	PosBufOverRide;
				VECTOR_ref	VecBufOverRide;
				VECTOR_ref	RadOverRide;
				if (PutOverride(&PosBufOverRide, &VecBufOverRide, &RadOverRide)) {
					this->m_PosBuf = PosBufOverRide;
					this->m_move.vec = VecBufOverRide;
					KeyControl::SetRadBufXY(RadOverRide);
				}
			}
			this->m_move.repos = this->m_move.pos;
			Easing(&this->m_move.pos, this->m_PosBuf, 0.9f, EasingType::OutExpo);
			UpdateMove();
			{
				auto Mat = this->GetObj().GetMatrix().GetRot();
				VECTOR_ref Yvec = Mat.yvec();
				VECTOR_ref Zvec = Mat.zvec();
				Easing(&Yvec, this->m_move.mat.yvec(), 0.96f, EasingType::OutExpo);
				Easing(&Zvec, this->m_move.mat.zvec(), 0.96f, EasingType::OutExpo);
				this->GetObj().SetMatrix(MATRIX_ref::Axis1_YZ(Yvec, Zvec) * MATRIX_ref::Mtrans(this->m_move.pos));
			}
			//ヒットボックス
			{
				int ID = 0;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::Propeller).pos(), 1.0f*Scale_Rate, HitType::Head); ID++;

				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::Upper).pos(), 1.0f*Scale_Rate, HitType::Body); ID++;

				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::Yaw).pos(), 1.0f*Scale_Rate, HitType::Body); ID++;

				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RollLeft).pos(), 1.0f*Scale_Rate, HitType::Body); ID++;
				m_HitBox[ID].Execute(GetFrameWorldMat(CharaFrame::RollRight).pos(), 1.0f*Scale_Rate, HitType::Body); ID++;
			}
		}
		//顔
		void			CharacterClass::ExecuteShape(void) noexcept {
			ShapeControl::ExcuteShape();
			SetShapePer(CharaShape::Propeller, 1.f);

			auto SpeedNormal = 51.39f*0.5f;//秒速換算 92.5km/hくらい

			SetFrameLocalMat(CharaFrame::Propeller, MATRIX_ref::RotZ(deg2rad(-10.f*this->m_Thrust / SpeedNormal))*GetFrameLocalMat(CharaFrame::Propeller).GetRot());

			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Yaw));
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::RollLeft));
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::RollRight));
			GetObj().frame_Reset(GetCharaFrame(CharaFrame::Pitch));

			SetFrameLocalMat(CharaFrame::Yaw, MATRIX_ref::RotY(deg2rad(this->m_Yaw*30.f))*GetFrameLocalMat(CharaFrame::Yaw).GetRot());
			SetFrameLocalMat(CharaFrame::RollLeft, MATRIX_ref::RotX(-deg2rad(this->m_Roll*10.f))*GetFrameLocalMat(CharaFrame::RollLeft).GetRot());
			SetFrameLocalMat(CharaFrame::RollRight, MATRIX_ref::RotX(deg2rad(this->m_Roll*10.f))*GetFrameLocalMat(CharaFrame::RollRight).GetRot());
			SetFrameLocalMat(CharaFrame::Pitch, MATRIX_ref::RotX(deg2rad(this->m_Pitch*60.f))*GetFrameLocalMat(CharaFrame::Pitch).GetRot());
		}
	};
};
