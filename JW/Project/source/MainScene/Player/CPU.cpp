#include	"CPU.hpp"
#include "../../NetWork.hpp"
#include "../../MainScene/Object/Character.hpp"
#include "../../Scene/MainScene.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//AI操作
		void AIControl::SetNextWaypoint(const VECTOR_ref& vec_z) {
			auto* ObjMngr = ObjectManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_MyCharaID));
			int now = Get_next_waypoint(this->cpu_do.wayp_pre, MyChara->GetMove().pos, vec_z);
			if (now != -1) {
				this->cpu_do.Set_wayp_pre(now);
			}
		}
		void AIControl::AI_move(InputControl* MyInput) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_MyCharaID));

			bool W_key{ false };
			bool S_key{ false };
			bool A_key{ false };
			bool D_key{ false };
			bool Run_key{ false };
			bool Q_key{ false };
			bool E_key{ false };
			bool R_key{ false };
			bool F_key{ false };
			bool shotMain_Key{ false };

			int32_t x_m{ 0 }, y_m{ 0 };
			//AI
			if (MyChara->GetHP() != 0) {
				//操作
				VECTOR_ref vec_to = VECTOR_ref::zero();
				//
				auto Mypos = MyChara->GetMove().pos;
				auto vec_mat = MyChara->GetMove().mat;
				auto vec_x = vec_mat.xvec();
				auto vec_y = vec_mat.yvec();
				auto vec_z = vec_mat.zvec() * -1.f;
				{
					std::vector<std::shared_ptr<CharacterClass>*>	chara_Pool;
					int count = 0;
					while (true) {
						auto* ptr = ObjMngr->GetObj(ObjType::Plane, count);
						if (ptr) {
							chara_Pool.emplace_back(&(std::shared_ptr<CharacterClass>&)(*ptr));
							count++;
						}
						else {
							break;
						}
					}
					//
					SearchCount += 1.f / FPS;
					if (SearchCount > SearchTotal) {
						SearchCount -= SearchTotal;
						//
						if (AfterKillCount == 0.f && !tgtLock) {
							size_t tmp_id = chara_Pool.size();
							VECTOR_ref tgt_pos = Mypos + vec_z * 100.f*Scale_Rate;
							float dot_t = -1.f;
							for (auto& cptr : chara_Pool) {
								auto& c = *cptr;
								auto vec = (c->GetMove().pos - Mypos);
								auto len = vec.size();
								auto dot = vec_z.dot(vec.Norm());
								if (&MyChara == &c ||
									c->GetHP() == 0 ||
									MyChara->GetCharaType() == c->GetCharaType() ||
									c->aim_cnt >= 1 ||
									len >= 350.f*Scale_Rate
									) {
									continue;
								}
								if (dot_t < dot) {
									dot_t = dot;
									tmp_id = &cptr - &chara_Pool[0];
									tgt_pos = c->GetMove().pos;
								}
							}
							tgtID = (tmp_id != chara_Pool.size()) ? (int)tmp_id : -1;
						}
					}
					//
					if (AfterKillCount == 0.f) {
						if (tgtID != -1) {
							auto& c = *chara_Pool.at(tgtID);
							tgtpos = c->GetMove().pos + c->GetMove().mat.zvec()*-1.f * 5.f*Scale_Rate;
							c->aim_cnt++;

							//ターゲットに向くベクトルを取得
							VECTOR_ref tgt_zvec = (tgtpos - Mypos);
							auto dot = tgt_zvec.Norm().dot(vec_z);
							if (dot > cos(deg2rad(5))) {
								//ロックオン
								tgtLock = true;
							}
							if (
								dot < -cos(deg2rad(45)) ||
								tgt_zvec.Length() > 300.f*Scale_Rate
								) {
								tgtLock = false;
							}

							//賢者
							if (c->GetHP() == 0) {
								tgtID = -1;
								tgtLock = false;
								AfterKillCount = 3.f;
							}
						}
						else {
							tgtpos = Mypos + vec_z * 200.f*Scale_Rate;//まっすぐ
							tgtpos.y(Mypos.y());
						}
					}
					else {
						//賢者タイム
						{
							tgtpos = Mypos + vec_z * 50.f*Scale_Rate;//まっすぐ
							tgtpos.y(Mypos.y() + 25.f * Scale_Rate);
						}
						AfterKillCount = std::max(AfterKillCount - 1.f / FPS, 0.f);
					}
					bool ret = false;
					bool up = false;
					{
						auto tmpv = Mypos;
						tmpv.y(0);
						if (tmpv.size() >= 500.f * Scale_Rate) {
							tgtpos = VECTOR_ref::vget(0, 0, 0);//中央戻り
							ret = true;
						}
						if (Mypos.y() <= 0.f * Scale_Rate) {
							auto End = Mypos + VECTOR_ref::up()*-(20.f*Scale_Rate + std::clamp(vec_z.y()*-1.f, 0.f, 1.f)*100.f*Scale_Rate);
							if (this->m_BackGround->CheckLinetoMap(Mypos, &End, false)) {
								tmpv.y(500.f * Scale_Rate);
								tgtpos = tmpv;
								up = true;
							}
						}
					}
					//回避行動
					bool IsAvoid = false;
					{
						if (MyChara->m_NearAmmo) {
							AvoidCount = 3.f;
							IsAvoidLR = (GetRand(100) <= 50);
						}
						if (AvoidCount > 0.f) {
							if (!ret && !up) {
								IsAvoid = true;
								//キー操作
								S_key = true;

								if (IsAvoidLR) {
									D_key = true;
									Q_key = true;
								}
								else {
									A_key = true;
									E_key = true;
								}
							}
						}
						AvoidCount = std::max(AvoidCount - 1.f / FPS, 0.f);
					}
					if (!IsAvoid) {
						//ターゲットに向くベクトルを取得
						VECTOR_ref tgt_zvec = (tgtpos - Mypos).Norm();
						auto tgt_xvec = tgt_zvec.cross(vec_z).Norm();//ターゲットと自分とのXVec
						if (tgt_xvec == VECTOR_ref::vget(-1.f, -1.f, -1.f)) {
							tgt_xvec = vec_x;
						}
						auto tgt_yvec = tgt_zvec.cross(vec_x).Norm();//ターゲットと自分とのYVec
						if (tgt_yvec == VECTOR_ref::vget(-1.f, -1.f, -1.f)) {
							tgt_yvec = vec_y;
						}
						auto dotpitch = tgt_xvec.dot(vec_x);
						auto dotyaw = tgt_zvec.dot(vec_x);
						//ピッチ
						{
							if (dotpitch >= 0.f) {
								W_key = (std::abs(dotpitch) < sin(deg2rad(20))) ? (GetRand(100) <= 50) : true;
							}
							else {
								S_key = true;
							}
						}
						//ヨー
						{
							if (dotyaw >= 0.f) {
								Q_key = (std::abs(dotyaw) < sin(deg2rad(5))) ? (GetRand(100) <= 50) : true;
							}
							else {
								E_key = (std::abs(dotyaw) < sin(deg2rad(5))) ? (GetRand(100) <= 50) : true;
							}
						}
						//ロール
						{
							if (tgtID != -1) {
								if (((tgtpos - Mypos).size() >= 200.f * Scale_Rate)) {
									auto XBest = vec_z.cross(VECTOR_ref::up()).Norm();
									auto YBest = XBest.cross(vec_z).Norm();
									auto cross = vec_z.dot(vec_y.cross(YBest));
									if (cross > 0) {
										A_key = true;
									}
									else if (cross < 0) {
										D_key = true;
									}
								}
								else
									if (
										!((sin(deg2rad(15)) > std::abs(dotyaw) && std::abs(dotyaw) > sin(deg2rad(10))) && (std::abs(dotpitch) < sin(deg2rad(10))))
										)
									{
										auto dot = tgt_xvec.dot(vec_y);
										if (tgt_zvec.dot(vec_z) > sin(deg2rad(10))) {
											if (std::abs(dot) > sin(deg2rad(10))) {
												if (dot >= 0.f) {
													A_key = true;
												}
												else {
													D_key = true;
												}
											}
										}
									}
							}
							else {
								auto XBest = vec_z.cross(VECTOR_ref::up()).Norm();
								auto YBest = XBest.cross(vec_z).Norm();
								auto cross = vec_z.dot(vec_y.cross(YBest));
								if (cross > 0) {
									A_key = true;
								}
								else if (cross < 0) {
									D_key = true;
								}
							}
						}
						//射撃
						if (tgtID != -1 && !ret && !up) {
							auto dot = tgt_zvec.dot(vec_z);
							if (dot > cos(deg2rad(5))) {
								if ((tgtpos - Mypos).size() <= 100.f * Scale_Rate) {
									if (MyChara->GetCharaType() == CharaTypeID::Enemy) {
										if (tgtID == 0) {//プレイヤー相手にはそんなに撃たない
											shotMain_Key = GetRand(100) <= 1;   //射撃
										}
										else {
											shotMain_Key = GetRand(100) <= 5;   //射撃
										}
									}
									else {
										shotMain_Key = GetRand(100) <= 20;   //射撃
									}
								}
							}
						}
					}
				}
				R_key = GetRand(100) <= 10;
				if ((tgtpos - Mypos).size() >= 50.f * Scale_Rate) {
					R_key = true;
				}
				//敵だけガバる
				if (MyChara->GetCharaType() == CharaTypeID::Enemy) {
					if (GabaCount > 0.4f * 0.9f) {
						W_key = false;
						S_key = false;
						A_key = false;
						D_key = false;
						Q_key = false;
						E_key = false;

						R_key = false;

						shotMain_Key = false;
					}
					GabaCount += 1.f / FPS;
					if (GabaCount > 0.4f) {
						GabaCount -= 0.4f;
					}
				}
			}
			/*
			W_key = false;
			S_key = false;
			//A_key = false;
			//D_key = false;
			Q_key = false;
			E_key = false;
			//*/
			MyInput->SetInput(
				(float)x_m / 100.f,
				(float)y_m / 100.f,
				W_key, S_key, A_key, D_key,
				Run_key,
				Q_key, E_key,//QE
				false, false, false, false,
				false,//1
				R_key,
				F_key,
				false,
				shotMain_Key,
				false
			);
		}
		void AIControl::Init(const std::shared_ptr<BackGroundClassBase>& BackBround_t, PlayerID MyCharaID) noexcept {
			m_MyCharaID = MyCharaID;
			auto* ObjMngr = ObjectManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_MyCharaID));

			this->m_BackGround = BackBround_t;

			//AIの選択をリセット
			int now = Get_next_waypoint(this->cpu_do.wayp_pre, MyChara->GetMove().pos, VECTOR_ref::zero());
			this->cpu_do.Spawn((now != -1) ? now : 0);
			SearchCount = SearchTotal * ((float)MyChara->GetMyPlayerID() / (float)Player_num);
		}
		void AIControl::Draw() noexcept {
#ifdef DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto& MyChara = (std::shared_ptr<CharacterClass>&)(*ObjMngr->GetObj(ObjType::Plane, m_MyCharaID));

			std::vector<std::shared_ptr<CharacterClass>*>	chara_Pool;
			int count = 0;
			while (true) {
				auto* ptr = ObjMngr->GetObj(ObjType::Plane, count);
				if (ptr) {
					chara_Pool.emplace_back(&(std::shared_ptr<CharacterClass>&)(*ptr));
					count++;
				}
				else {
					break;
				}
			}

			//if (!MyChara->Get_alive()) { return; }
			if (MyChara == *chara_Pool.at(0)) { return; }
			auto MyPos = MyChara->GetFrameWorldMat(CharaFrame::Upper).pos();
			cpu_do.Draw_Debug(MyPos);
			if (this->cpu_do.ai_phase == 1) {
				auto& tgt = *chara_Pool.at(this->cpu_do.ai_AimTarget);
				VECTOR_ref StartPos = MyChara->GetEyePosition() + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);
				VECTOR_ref EndPos = tgt->GetMove().pos + VECTOR_ref::vget(0.f, 5.f*Scale_Rate, 0.f);

				DrawCapsule_3D(StartPos, EndPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 0, 0));
			}
			//*
			if (MyChara->GetHP() != 0) {
				auto Mypos = MyChara->GetMove().pos;

				VECTOR_ref StartPos = Mypos + MyChara->GetMove().mat.yvec() * 2.f*Scale_Rate;
				VECTOR_ref EndPos = StartPos + (tgtpos - Mypos).Norm() * Scale_Rate*5.f;

				DrawCapsule_3D(StartPos, EndPos, 0.1f*Scale_Rate, GetColor(255, 0, 0), GetColor(255, 255, 255));
			}
			//*/
#endif
		}
	};
};
