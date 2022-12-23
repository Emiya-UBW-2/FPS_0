#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//----------------------------------------------------------
		//初期化関連
		//----------------------------------------------------------

		//
		void			VehicleClass::ValueInit(const VhehicleData* pVeh_data, const MV1& hit_pic, const std::shared_ptr<b2World>& pB2World, PlayerID pID) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			this->m_MyID = pID;
			this->m_VecData = pVeh_data;
			this->m_Hit_active.Init(hit_pic);														//弾痕

			ObjMngr->LoadModel(this, ("data/tank/" + this->m_VecData->GetName() + "/").c_str());

			GetObj().material_AlphaTestAll(true, DX_CMP_GREATER, 128);								//本体
			this->hitres.resize(this->m_col.mesh_num());											//モジュールごとの当たり判定結果を確保
			this->hitssort.resize(this->m_col.mesh_num());											//モジュールごとの当たり判定順序を確保
			b2PolygonShape dynamicBox = this->m_VecData->GetDynamicBox();
			this->m_b2mine.Set(CreateB2Body((std::shared_ptr<b2World>&)pB2World, b2_dynamicBody, 0.f, 0.f, 0.f), &dynamicBox);	//シェイプをボディに追加
			for (auto& f : this->m_b2Foot) {
				f.Init(&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj());
			}
			this->m_wheel_frameYpos.resize(this->m_VecData->Get_wheelframe().size());
			//砲
			this->m_Gun.resize(this->m_VecData->Get_gunframe().size());
			this->m_view_rad.resize(this->m_Gun.size());
			for (const auto& g : this->m_VecData->Get_gunframe()) {
				this->m_Gun[&g - &this->m_VecData->Get_gunframe().front()].Init(&g);
			}
		}
		//
		void			VehicleClass::ValueSet(float pxRad, float pyRad, const VECTOR_ref& pPos) noexcept {
			this->m_move.mat = MATRIX_ref::RotX(pxRad) * MATRIX_ref::RotY(pyRad);
			this->m_move.pos = pPos;
			this->m_move.vec.clear();
			for (auto& w : this->m_wheel_frameYpos) { w = 0.f; }
			//砲
			//ヒットポイント
			{
				this->m_HP = this->m_VecData->GetMaxHP();
				this->m_HP_parts.resize(this->m_col.mesh_num());
				for (auto& h : this->m_HP_parts) { h = this->m_VecData->GetMaxHP(); }//モジュール耐久
			}
			//戦車スポーン
			this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), Get_body_yrad());
			this->m_PosBufOverRideFlag = false;
			this->m_DamageSwitch = 0;
			this->m_DamageSwitchRec = this->m_DamageSwitch;

			this->m_MouseVec = this->m_move.mat.Inverse();

			this->m_speed = 0.f;
			this->m_speed_add = 0.f;
			this->m_speed_sub = 0.f;
			this->m_wheel_Left = 0.f;
			this->m_wheel_Right = 0.f;
			this->m_Tilt.clear();
			this->m_radAdd.clear();
			this->m_yradadd_left = 0.f;
			this->m_yradadd_right = 0.f;
		}
		//
		void			VehicleClass::SetInput(const InputControl& pInput, bool pReady, bool isOverrideView) noexcept {
			this->m_ReadySwitch = (this->m_KeyActive != pReady);
			this->m_KeyActive = pReady;
			//エイム
			auto y_mouse = std::atan2f(this->m_MouseVec.zvec().x(), this->m_MouseVec.zvec().z());
			auto x_mouse = std::atan2f(-this->m_MouseVec.zvec().y(), std::hypotf(this->m_MouseVec.zvec().x(), this->m_MouseVec.zvec().z()));
			this->m_MouseVec = MATRIX_ref::RotX(std::clamp(x_mouse + pInput.GetAddxRad(), -deg2rad(40.f), deg2rad(40.f))) * MATRIX_ref::RotY(y_mouse + pInput.GetAddyRad());
			//
			if (isOverrideView) {
				this->m_view_override = true;
				this->m_view_rad[0].x(pInput.GetxRad());
				this->m_view_rad[0].y(pInput.GetyRad());
			}
			this->m_key[0] = pInput.GetAction5() && pReady && this->Get_alive();			//射撃
			this->m_key[1] = pInput.GetAction1() && pReady && this->Get_alive();			//マシンガン
			this->m_key[2] = pInput.GetGoFrontPress() && pReady && this->Get_alive();		//前進
			this->m_key[3] = pInput.GetGoBackPress() && pReady && this->Get_alive();		//後退
			this->m_key[4] = pInput.GetGoRightPress() && pReady && this->Get_alive();		//右
			this->m_key[5] = pInput.GetGoLeftPress() && pReady && this->Get_alive();		//左
			this->m_key[6] = pInput.GetAction6() && pReady && this->Get_alive();			//左
		}
		//カメラ設定出力
		void			VehicleClass::Setcamera(Camera3DInfo& m_MainCamera, const float fov_base) noexcept {
			auto fov_t = m_MainCamera.GetCamFov();
			auto near_t = m_MainCamera.GetCamNear();
			auto far_t = m_MainCamera.GetCamFar();

			VECTOR_ref eyepos = Get_EyePos_Base();
			auto OLD = this->m_range;
			if (is_ADS()) {
				this->m_ratio = std::clamp(this->m_ratio + float(GetMouseWheelRotVolWithCheck()) * 2.0f, 0.0f, 30.f);
				if (this->m_ratio == 0.f) {
					this->m_range = 0.f + this->m_range_change;
					this->m_range_r = this->m_range;
				}
				else {
					Easing(&fov_t, fov_base / this->m_ratio, 0.9f, EasingType::OutExpo);
				}
			}
			else {
				eyepos += this->m_MouseVec.zvec() * this->m_range_r*Scale_Rate;
				VECTOR_ref eyepos2 = eyepos + this->m_MouseVec.zvec() * -1.f * (this->m_range_r * Scale_Rate);
				if (this->m_BackGround->CheckLinetoMap(eyepos, &eyepos2, true)) { eyepos = eyepos2; }

				this->m_ratio = 2.0f;
				this->m_range = std::clamp(this->m_range - float(GetMouseWheelRotVolWithCheck()) * this->m_range_change, 0.f, 9.f);
				Easing(&this->m_range_r, this->m_range, 0.8f, EasingType::OutExpo);

				Easing(&fov_t, fov_base, 0.9f, EasingType::OutExpo);
			}
			VECTOR_ref eyetgt = Get_EyePos_Base() + this->m_MouseVec.zvec() * -1.f * std::max(this->m_range_r*Scale_Rate, 1.f);

			this->m_changeview = ((this->m_range != OLD) && (this->m_range == 0.f || OLD == 0.f));
			m_MainCamera.SetCamPos(eyepos, eyetgt, this->m_move.mat.yvec());

			Easing(&near_t, 1.f + 2.f*((is_ADS()) ? this->m_ratio : 0.f), 0.9f, EasingType::OutExpo);
			Easing(&far_t, std::max(this->m_AimingDistance, Scale_Rate * 20.f) + Scale_Rate * 20.f, 0.9f, EasingType::OutExpo);
			m_MainCamera.SetCamInfo(fov_t, near_t, far_t);
		}

		//----------------------------------------------------------
		//更新関連
		//----------------------------------------------------------

		//被弾チェック
		const auto		VehicleClass::CheckAmmoHited(const AmmoClass& pAmmo) noexcept {
			auto HitCheck_Tank = [&](int m, const VECTOR_ref& ray_repos, const MV1_COLL_RESULT_POLY& hitres) {
				this->hitres[m] = hitres;
				if (this->hitres[m].HitFlag == TRUE) {
					this->hitssort[m].Set(m, (ray_repos - this->hitres[m].HitPosition).size());
					return true;
				}
				else {
					this->hitssort[m].Set(m);
					return false;
				}
			};
			bool is_Hit = false;
			for (auto& m : this->m_VecData->Get_module_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, pAmmo.GetMove().pos, m)); }//モジュール
			for (auto& m : this->m_VecData->Get_space_mesh()) { is_Hit |= HitCheck_Tank(m, pAmmo.GetMove().repos, GetColLine(pAmmo.GetMove().repos, pAmmo.GetMove().pos, m)); }				//空間装甲
			//装甲(一番近い位置のものに限定する)
			int t = -1;
			MV1_COLL_RESULT_POLY colres; colres.HitFlag = FALSE;
			VECTOR_ref EndPos = pAmmo.GetMove().pos;
			for (const auto& m : this->m_VecData->Get_armer_mesh()) { HitCheck_Tank(m.first, pAmmo.GetMove().repos, colres); }//全リセット
			for (const auto& m : this->m_VecData->Get_armer_mesh()) {
				auto colres_t = GetColLine(pAmmo.GetMove().repos, EndPos, m.first);
				if (colres_t.HitFlag == TRUE) {
					t = m.first;
					colres = colres_t;
					EndPos = colres_t.HitPosition;
				}
			}
			if (t != -1) {
				is_Hit |= HitCheck_Tank(t, pAmmo.GetMove().repos, colres);
			}
			return is_Hit;
		}
		//被弾処理
		const auto		VehicleClass::CalcAmmoHited(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			auto SE = SoundPool::Instance();
			std::sort(this->hitssort.begin(), this->hitssort.end(), [](const HitSortInfo& x, const HitSortInfo& y) { return x < y; });	//当たり判定を近い順にソート
			bool isDamage = false;
			//ダメージ面に届くまで判定
			for (const auto& tt : this->hitssort) {
				if (tt.IsHit()) {
					VECTOR_ref HitPos = this->hitres[tt.GetHitMesh()].HitPosition;
					VECTOR_ref HitNormal = this->hitres[tt.GetHitMesh()].Normal;
					HitNormal = HitNormal.Norm();

					for (auto& mesh : this->m_VecData->Get_space_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//空間装甲に当たったのでモジュールに30ダメ
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							//this->SubHP_Parts(30, (HitPoint)tt.GetHitMesh());
						}
					}
					for (auto& mesh : this->m_VecData->Get_module_mesh()) {
						if (tt.GetHitMesh() == mesh) {
							//モジュールに当たったのでモジュールに30ダメ
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate);
							//this->SubHP_Parts(30, (HitPoint)tt.GetHitMesh());
						}
					}
					//ダメージ面に当たった
					for (auto& mesh : this->m_VecData->Get_armer_mesh()) {
						if (tt.GetHitMesh() == mesh.first) {
							//弾がダメージ層に届いた
							if (pAmmo->PenetrationCheck(mesh.second, HitNormal) && this->Get_alive()) {						//ダメージ面に当たった時に装甲値に勝てるかどうか
								pAmmo->Penetrate();	//貫通
								SE->Get((int)SoundEnum::Tank_Damage).Play_3D(GetRand(1), HitPos, 100.f*Scale_Rate, 216);
								//ダメージ計算
								auto v1 = MATRIX_ref::RotY(Get_body_yrad()).zvec();
								auto v2 = (pShooterPos - this->m_move.pos).Norm(); v2.y(0);
								this->m_DamageEvent.SetEvent(this->m_MyID, this->m_objType, pAmmo->GetDamage(), atan2f(v1.cross(v2).y(), v1.dot(v2)));
								++this->m_DamageSwitch;// %= 255;//
								//this->SubHP_Parts(pAmmo->GetDamage(), (HitPoint)tt.GetHitMesh());
								if (!this->Get_alive()) {
									//撃破
									EffectControl::SetOnce(EffectResource::Effect::ef_greexp2, this->m_move.pos, this->m_move.mat.zvec(), Scale_Rate*2.f);
								}
								isDamage = true;
							}
							else {
								pAmmo->Ricochet(HitPos, HitNormal);	//跳弾
								SE->Get((int)SoundEnum::Tank_Ricochet).Play_3D(GetRand(16), HitPos, 100.f*Scale_Rate, 216);
							}
							//エフェクトセット
							EffectControl::SetOnce(EffectResource::Effect::ef_reco, HitPos, HitNormal, pAmmo->GetEffectSize()*Scale_Rate*10.f);
							this->m_Hit_active.Set(this->m_move, HitPos, HitNormal, pAmmo->GetMove().vec.Norm(), pAmmo->GetCaliberSize()*Scale_Rate, !pAmmo->IsActive());
							break;
						}
					}
				}
				else {
					break;
				}
			}
			return isDamage;
		}

		//以前の状態保持
		void			VehicleClass::ExecuteSavePrev(void) noexcept {
			this->m_spd_rec = this->m_speed;
			this->m_ColActive = false;
		}
		//その他
		void			VehicleClass::ExecuteElse(void) noexcept {
			auto SE = SoundPool::Instance();
			//エンジン音
			{
				if (this->m_engine_time == 0.f) {
					SE->Get((int)SoundEnum::Tank_engine).Play_3D(0, this->m_move.pos, 50.f*Scale_Rate, 64);//, DX_PLAYTYPE_LOOP
					this->m_engine_time = 1.f;
				}
				else {
					this->m_engine_time -= 1.f / FPS;
					if (this->m_engine_time <= 0.f) {
						this->m_engine_time = 0.f;
					}
				}
			}
		}
		//操作
		void			VehicleClass::ExecuteInput(void) noexcept {
			if (!this->m_view_override) {
				float	view_YradAdd{ 0.f };							//
				float	view_XradAdd{ 0.f };							//
				for (int i = 0; i < this->m_view_rad.size(); i++) {
					if (this->Get_alive()) {
						if (this->m_key[6]) { //砲塔ロック
							view_XradAdd = 0.f;
							view_YradAdd = 0.f;
						}
						else {
							//狙い
							VECTOR_ref MuzPos = GetGunMuzzlePos(i);
							VECTOR_ref EndPos = MuzPos + (this->m_MouseVec.zvec() * -1.f).Norm() * (500.f*Scale_Rate);
							this->m_BackGround->CheckLinetoMap(MuzPos, &EndPos, true);

							VECTOR_ref BasePos = GetGunMuzzleBase(i);
							//反映
							auto vec_a = (EndPos - BasePos).Norm();
							auto vec_z = (MuzPos - BasePos).Norm();
							float a_hyp = std::hypotf(vec_a.x(), vec_a.z());
							float z_hyp = std::hypotf(vec_z.x(), vec_z.z());
							{
								float cost = vec_z.cross(vec_a).y() / z_hyp;
								float sint = sqrtf(std::abs(1.f - cost * cost));
								view_YradAdd = (atan2f(cost, sint)) / 5.f;
							}
							view_XradAdd = (atan2f(vec_a.y(), a_hyp) - atan2f(vec_z.y(), z_hyp)) / 1.f;
							if (i == 0) {
								//printfDx("%.2f\n", rad2deg(view_XradAdd));
							}
						}
					}
					else {
						view_XradAdd = 0.1f / FPS;
						view_YradAdd = 0.f;
					}

					float limit = this->m_VecData->GetMaxTurretRad() / FPS;
					if (i > 0) {
						limit *= 2.f;
					}
					this->m_view_rad[i].x(std::clamp(this->m_view_rad[i].x() + std::clamp(view_XradAdd, -limit / 5.f, limit / 5.f), -deg2rad(10.f), deg2rad(40.f)));
					this->m_view_rad[i].yadd(std::clamp(view_YradAdd, -limit, limit));
				}
			}
			this->m_view_override = false;
		}
		//フレーム操作
		void			VehicleClass::ExecuteFrame(void) noexcept {
			//砲塔旋回
			for (auto& g : this->m_Gun) {
				auto ID = &g - &this->m_Gun.front();
				if (ID == 0) {
					g.ExecuteGunFrame(this->m_view_rad[ID].x(), this->m_view_rad[ID].y(), &GetObj(), &this->m_col);
				}
				else {
					g.ExecuteGunFrame(this->m_view_rad[ID].x(), this->m_view_rad[ID].y() - this->m_view_rad[0].y(), &GetObj(), &this->m_col);
				}
			}
			//転輪
			auto y_vec = GetObj().GetMatrix().yvec();
			for (auto& f : this->m_VecData->Get_wheelframe()) {
				if (f.GetFrameID() >= 0) {
					auto ID = &f - &this->m_VecData->Get_wheelframe().front();
					GetObj().frame_Reset(f.GetFrameID());

					auto startpos = GetObj().frame(f.GetFrameID());
					auto pos_t1 = startpos + y_vec * ((-f.GetFrameWorldPosition().y()) + 2.f*Scale_Rate);
					auto pos_t2 = startpos + y_vec * ((-f.GetFrameWorldPosition().y()) - 0.3f*Scale_Rate);
					auto ColRes = this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true);
					Easing(&this->m_wheel_frameYpos[ID], (ColRes) ? (pos_t2.y() + y_vec.y() * f.GetFrameWorldPosition().y() - startpos.y()) : -0.3f*Scale_Rate, 0.9f, EasingType::OutExpo);
					GetObj().SetFrameLocalMatrix(f.GetFrameID(),
						MATRIX_ref::RotX((f.GetFrameWorldPosition().x() >= 0) ? this->m_wheel_Left : this->m_wheel_Right) *
						MATRIX_ref::Mtrans(VECTOR_ref::up() * this->m_wheel_frameYpos[ID]) *
						MATRIX_ref::Mtrans(f.GetFrameWorldPosition())
					);
				}
			}
			for (const auto& f : this->m_VecData->Get_wheelframe_nospring()) {
				if (f.GetFrameID() >= 0) {
					GetObj().SetFrameLocalMatrix(f.GetFrameID(), MATRIX_ref::RotX((f.GetFrameWorldPosition().x() >= 0) ? this->m_wheel_Left : this->m_wheel_Right) * MATRIX_ref::Mtrans(f.GetFrameWorldPosition()));
				}
			}
		}
		//操作共通
		void			VehicleClass::ExecuteMove(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			bool isfloat = this->m_VecData->GetIsFloat() && (this->m_move.pos.y() == -this->m_VecData->GetDownInWater());
			//傾きの取得
			{
				const auto bNormal = (isfloat) ? VECTOR_ref::up() : this->m_BodyNormal;
				float yradBody = 0.f;
				if (this->m_PosBufOverRideFlag) {
					yradBody = this->m_RadOverRide.y();
				}
				else {
					auto pp = (this->m_move.mat * MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal).Inverse()).zvec() * -1.f;
					yradBody = atan2f(pp.x(), pp.z());
				}
				this->m_move.mat = MATRIX_ref::Axis1_YZ(bNormal, (MATRIX_ref::RotY(yradBody)*MATRIX_ref::RotVec2(VECTOR_ref::up(), bNormal)).zvec() * -1.f);
				if (this->m_PosBufOverRideFlag) {
					//一旦その場で地形判定
					this->m_move.pos = this->m_PosBufOverRide;
					GetObj().SetMatrix(this->m_move.MatIn());
				}
				Easing(&this->m_BodyNormal, ((GetObj().frame(this->m_VecData->Get_square(0)) - GetObj().frame(this->m_VecData->Get_square(3))).cross(GetObj().frame(this->m_VecData->Get_square(1)) - GetObj().frame(this->m_VecData->Get_square(2)))).Norm(), 0.8f, EasingType::OutExpo);
			}
			//地面判定
			{
				auto pos_ytmp = this->m_move.pos.y();
				float hight_t = 0.f;
				int cnt_t = 0;
				//履帯
				for (auto& f : this->m_b2Foot) {
					f.FirstExecute(&GetObj(), m_BackGround);
					for (const auto& t : f.Getdownsideframe()) {
						if (t.GetColResult_Y() != (std::numeric_limits<float>::max)()) {
							hight_t += t.GetColResult_Y();
							cnt_t++;
						}
					}
				}
				int cnt_sq = cnt_t;
				for (const auto& s : this->m_VecData->Get_square()) {
					auto p_t = GetObj().frame(s);
					auto pos_t1 = p_t + (VECTOR_ref::up() * 3.f*Scale_Rate);
					auto pos_t2 = p_t + (VECTOR_ref::up() * -0.5f*Scale_Rate);
					if (this->m_BackGround->CheckLinetoMap(pos_t1, &pos_t2, true)) {
						hight_t += pos_t2.y();
						cnt_t++;
					}
				}
				if (cnt_t > 0) {
					Easing(&pos_ytmp, (hight_t / cnt_t), 0.9f, EasingType::OutExpo);
				}
				this->m_move.pos.y(pos_ytmp);
				//地面or水面にいるかどうか
				if ((cnt_t > 0) || isfloat) {
					//前進後退
					{
						const auto spdold = this->m_speed_add + this->m_speed_sub;
						this->m_speed_add = (this->m_key[2]) ? std::min(this->m_speed_add + (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxFrontSpeed() / 3.6f)) : std::max(this->m_speed_add - (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed_sub = (this->m_key[3]) ? std::max(this->m_speed_sub - (0.15f / 3.6f) * (60.f / FPS), (this->m_VecData->GetMaxBackSpeed() / 3.6f)) : std::min(this->m_speed_sub + (0.7f / 3.6f) * (60.f / FPS), 0.f);
						this->m_speed = (spdold + ((this->m_speed_add + this->m_speed_sub) - spdold) * 0.1f) / FPS;
						auto vec = this->m_move.mat.zvec() * -1.f * (this->m_speed*Scale_Rate);
						this->m_move.vec.x(vec.x());
						if ((cnt_t - cnt_sq) >= 3) {
							this->m_move.vec.y(vec.y());
						}
						else {
							this->m_move.vec.yadd(M_GR / (FPS * FPS));
						}
						this->m_move.vec.z(vec.z());
					}
					//旋回
					{
						const auto radold = this->m_radAdd;
						this->m_yradadd_left = (this->m_key[4]) ? std::max(this->m_yradadd_left - deg2rad(1.f * (60.f / FPS)), deg2rad(-this->m_VecData->GetMaxBodyRad())) : std::min(this->m_yradadd_left + deg2rad(2.f * (60.f / FPS)), 0.f);
						this->m_yradadd_right = (this->m_key[5]) ? std::min(this->m_yradadd_right + deg2rad(1.f * (60.f / FPS)), deg2rad(this->m_VecData->GetMaxBodyRad())) : std::max(this->m_yradadd_right - deg2rad(2.f * (60.f / FPS)), 0.f);
						this->m_radAdd.y((this->m_yradadd_left + this->m_yradadd_right) / FPS);
						//慣性
						this->m_radAdd.x(deg2rad(-(this->m_speed / (60.f / FPS)) / (0.1f / 3.6f) * 50.f));
						this->m_radAdd.z(deg2rad(-this->m_radAdd.y() / (deg2rad(5.f) / FPS) * 5.f));
						Easing(&this->m_Tilt, VECTOR_ref::vget(std::clamp(this->m_radAdd.x() - radold.x(), deg2rad(-30.f), deg2rad(30.f)), 0.f, std::clamp(this->m_radAdd.z() - radold.z(), deg2rad(-15.f), deg2rad(15.f))), 0.95f, EasingType::OutExpo);
						this->m_move.mat *= MATRIX_ref::RotAxis(this->m_move.mat.xvec(), -this->m_Tilt.x()) * MATRIX_ref::RotAxis(this->m_move.mat.zvec(), this->m_Tilt.z());
					}
					//
				}
				else {
					this->m_move.vec.yadd(M_GR / (FPS * FPS));
				}
			}
			//射撃反動
			for (auto& cg : this->m_Gun) {
				auto index = &cg - &this->m_Gun.front();
				cg.FireReaction(&this->m_move.mat);
				//射撃
				if (cg.Execute(this->m_key[(index == 0) ? 0 : 1], this->m_CharaType == CharaTypeID::Mine)) {
					SE->Get((int)SoundEnum::Tank_Shot).Play_3D(cg.GetShotSound(), this->m_move.pos, 250.f*Scale_Rate);													//サウンド
					EffectControl::SetOnce(EffectResource::Effect::ef_fire, GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), cg.GetCaliberSize() / 0.1f * Scale_Rate);	//銃発砲エフェクトのセット
					auto& LastAmmo = (std::shared_ptr<AmmoClass>&)(*ObjMngr->AddObject(ObjType::Ammo));
					LastAmmo->Put(cg.GetAmmoSpec(), GetGunMuzzlePos((int)index), GetGunMuzzleVec((int)index), this->m_MyID);
				}
			}
			//移動
			if (this->m_PosBufOverRideFlag) {
				this->m_move.pos = this->m_PosBufOverRide;
				this->m_move.vec = this->m_VecBufOverRide;
			}
			else {
				this->m_move.pos += this->m_move.vec;
				//浮く
				if (this->m_VecData->GetIsFloat()) {
					this->m_move.pos.y(std::max(this->m_move.pos.y(), -this->m_VecData->GetDownInWater()));
				}
			}
			//転輪
			this->m_wheel_Left += (-(this->m_speed * 2.f - this->m_radAdd.y() * 5.f));
			this->m_wheel_Right += (-(this->m_speed * 2.f + this->m_radAdd.y() * 5.f));
			//戦車壁判定
			if (this->m_PosBufOverRideFlag) {
				this->m_b2mine.SetTransform(b2Vec2(this->m_move.pos.x(), this->m_move.pos.z()), Get_body_yrad());
			}
			else {
				this->m_b2mine.Execute(this->m_move.vec, this->m_radAdd.y());
			}
		}
		//SetMat指示更新
		void			VehicleClass::ExecuteMatrix(void) noexcept {
			auto OldPos = this->m_move.pos;
			//戦車座標反映
			this->m_move.mat *= MATRIX_ref::RotY(-this->m_b2mine.Rad() - Get_body_yrad());
			this->m_move.pos = VECTOR_ref::vget(this->m_b2mine.Pos().x, this->m_move.pos.y(), this->m_b2mine.Pos().y);
			float spdrec = this->m_spd_buf;
			Easing(&this->m_spd_buf, this->m_b2mine.Speed() * ((this->m_spd_buf > 0) ? 1.f : -1.f), 0.99f, EasingType::OutExpo);
			this->m_speed = this->m_spd_buf - spdrec;

			//転輪
			b2Vec2 Gravity2D = b2Vec2(
				(M_GR / FPS / 2.f) * (this->m_move.mat.zvec().dot(VECTOR_ref::up())),
				(M_GR / FPS / 2.f) * (this->m_move.mat.yvec().dot(VECTOR_ref::up())));
			for (auto& f : this->m_b2Foot) {
				f.LateExecute(
					&f == &this->m_b2Foot.front(), this->m_VecData, &GetObj(),
					Gravity2D, (&f == &this->m_b2Foot.front()) ? this->m_wheel_Left : this->m_wheel_Right,
					(this->m_move.pos - this->m_move.repos).Length() * 60.f / FPS);
			}
			UpdateMove();
			this->m_add_vec_real = this->m_move.pos - OldPos;
			this->m_Hit_active.Execute(GetObj());
			this->m_move.repos = this->m_move.pos;
		}
		//描画共通
		void			VehicleClass::DrawCommon(void) noexcept {
			if (!is_ADS()) {
				if (CheckCameraViewClip_Box(
					(this->GetMove().pos + VECTOR_ref::vget(-5, -5, -5)*Scale_Rate).get(),
					(this->GetMove().pos + VECTOR_ref::vget(5, 5, 5)*Scale_Rate).get()) == FALSE
					) {
					if (true) {
						MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Left * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
						GetObj().DrawMesh(0);
						MV1SetFrameTextureAddressTransform(GetObj().get(), 0, -this->m_wheel_Right * 0.1f, 0.f, 1.f, 1.f, 0.5f, 0.5f, 0.f);
						GetObj().DrawMesh(1);
						MV1ResetFrameTextureAddressTransform(GetObj().get(), 0);
						GetObj().DrawMesh(2);
						for (int i = 2; i < GetObj().mesh_num(); i++) {
							GetObj().DrawMesh(i);
						}
						//this->m_col.DrawModel();
						this->m_Hit_active.Draw();
					}
					else {
						GetObj().DrawModel();
					}
				}
			}
		}

		//
		const std::pair<bool, bool>		VehicleClass::CheckAmmoHit(AmmoClass* pAmmo, const VECTOR_ref& pShooterPos) noexcept {
			std::pair<bool, bool> isDamaged{ false,false };
			if (this->RefreshCol(pAmmo->GetMove().repos, pAmmo->GetMove().pos, 10.0f*Scale_Rate)) {
				if (this->CheckAmmoHited(*pAmmo)) {									//とりあえず当たったかどうか探す
					isDamaged.first = true;
					if (this->CalcAmmoHited(pAmmo, pShooterPos)) {
						isDamaged.second = true;
					}
				}
			}
			return isDamaged;
		}
		//
		void			VehicleClass::HitGround(const VECTOR_ref& pPos, const VECTOR_ref& pNorm, const VECTOR_ref& /*pVec*/) noexcept {
			EffectControl::SetOnce_Any(EffectResource::Effect::ef_gndsmoke, pPos, pNorm, 0.05f / 0.1f * Scale_Rate);
			//hit_obj_p.Set(a.GetCaliberSize() * Scale_Rate, pPos, pNorm, pVec);	//弾痕
		}
		//
		void			VehicleClass::DrawModuleView(int xp, int yp, int size) noexcept {
			auto base = GetLookVec().zvec()*-1.f;
			base.y(0.f);
			base = base.Norm();

			auto vec = VECTOR_ref::front();
			auto rad = std::atan2f(base.cross(vec).y(), base.dot(vec));


			for (auto&m : this->m_VecData->Get_module_view()[0]) {
				SetDrawBright(0, 255, 0);
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad(), true);
			}
			for (auto&m : this->m_VecData->Get_module_view()[1]) {
				SetDrawBright(0, 255, 0);
				m.first->DrawRotaGraph(xp, yp, (float)size / 200, rad + this->Get_body_yrad() + this->m_view_rad[0].y(), true);
			}
			SetDrawBright(255, 255, 255);
		}
	};
};
