#pragma once
#include	"StartMovieScene.hpp"
#include "../ObjectManager.hpp"
#include "../sub.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		void			StartMovieScene::Set_Sub(void) noexcept {
			Get_Next()->Load();

			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
			//
			SetAmbientShadow(
				VECTOR_ref::vget(Scale_Rate*-60.f, Scale_Rate*-10.f, Scale_Rate*-60.f),
				VECTOR_ref::vget(Scale_Rate*60.f, Scale_Rate*50.f, Scale_Rate*60.f),
				VECTOR_ref::vget(-0.f, -0.25f, -0.8f),
				GetColorF(0.92f, 0.91f, 0.90f, 0.0f));
			//Load
			//BG
			this->m_BackGround = std::make_shared<BackGroundClassMovie>();
			this->m_BackGround->Init("data/model/map_airport/", "data/model/sky/");
			//
			ObjMngr->Init(this->m_BackGround);
			//
			{
				auto kill = FPS_n2::SaveDataClass::Instance()->GetParam("KillCount");
				auto killTotal = 80;
				if (g_Mode == 1 && (kill >= killTotal)) {
					plane_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Plane/Albatros_2/"));
					for (int i = 1; i < 3; i++) {
						plane_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Plane/Albatros/"));
					}
				}
				else {
					for (int i = 0; i < 3; i++) {
						plane_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Plane/Albatros/"));
					}
				}
			}
			character_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Chara/mechanic1/"));
			character_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Chara/mechanic2/"));
			character_Pool.emplace_back(*ObjMngr->AddObject(ObjType::Movie, "data/Chara/mechanic1/"));
			//ロード
			TLClass.Init();
			//テロップ
			{
				int mdata = FileRead_open("data/Cut.txt", FALSE);
				while (FileRead_eof(mdata) == 0) {
					LSClass.LoadScript(getparams::Getstr(mdata));
					const auto& args = LSClass.Getargs();
					const auto& func = LSClass.Getfunc();
					if (func == "") { continue; }
					TLClass.LoadTelop(func, args);
				}
				FileRead_close(mdata);
			}
			//Set
			m_CamPosRandBuf = VECTOR_ref::zero();
			m_CamPosRand = VECTOR_ref::zero();
			m_CamPosRand2 = VECTOR_ref::zero();
			m_CamPos = VECTOR_ref::vget(0.f*Scale_Rate, 1.6f*Scale_Rate, 15.f*Scale_Rate);
			m_CamVec = VECTOR_ref::front()*-1.f;
			m_CamUp = VECTOR_ref::up();
			m_near = 0.1f*Scale_Rate;
			m_far = 20.f*Scale_Rate;
			m_fov = deg2rad(45);


			m_Seq = 0;
			m_SeqCount = 0.f;
			m_SkipCount = 0.f;
			m_Seq0Count = 0.f;
			m_Seq1Yadd = 0.f;
			m_Propeller = 0.f;
			//player
			for (auto& c : plane_Pool) {
				int index = (int)(&c - &plane_Pool.front());

				c->GetObj().get_anime(0).per = 1.f;
				c->GetObj().get_anime(4).per = 1.f - c->GetObj().get_anime(0).per;

				//c->GetObj().get_anime(1).per = 1.f;
				c->GetObj().get_anime(2).per = 1.f;
				c->GetObj().get_anime(3).per = 1.f;

				if (index == 0) {
					c->SetMove(MATRIX_ref::RotY(deg2rad(-30.f)), VECTOR_ref::vget((float)index * 13.f*Scale_Rate, 0.f, 0.f));
				}
				else {
					c->SetMove(MATRIX_ref::RotY(deg2rad(GetRandf(40.f))), VECTOR_ref::vget((float)index * 13.f*Scale_Rate, 0.f, 0.f));
				}
				c->UpdateMove();

				AimPos.resize(AimPos.size() + 1);
				AimPos.back().Speed = 0.f;
				AimPos.back().Aim = VECTOR_ref::vget((15.f + GetRandf(30.f))*Scale_Rate, 0.f, -80.f*Scale_Rate - (float)index * (float)(40)*Scale_Rate);
			}
			for (auto& c : character_Pool) {
				auto p = plane_Pool.at(0)->GetMove();
				c->SetMove(p.mat, p.pos);
				c->UpdateMove();
			}
			{
				auto& c = character_Pool.at(0);
				c->GetObj().get_anime(0).per = 1.f;
			}
			{
				auto& c = character_Pool.at(1);
				c->GetObj().get_anime(1).per = 1.f;
			}
			{
				auto& c = character_Pool.at(2);
				c->GetObj().get_anime(2).per = 1.f;
			}
			//Cam
			SetMainCamera().SetCamInfo(deg2rad(65), 1.f, 100.f);
			SetMainCamera().SetCamPos(VECTOR_ref::vget(0, 15, -20), VECTOR_ref::vget(0, 15, 0), VECTOR_ref::vget(0, 1, 0));
			//サウンド
			SetCreate3DSoundFlag(FALSE);
			//this->m_BGM = SoundHandle::Load("data/Sound/BGM/Beethoven8_2.wav");
			SE->Add((int)SoundEnum::EngineStart, 1, "data/Sound/SE/enginestart.wav", false);
			SE->Add((int)SoundEnum::Engine2, 1, "data/Sound/SE/engine.wav", false);
			SE->Add((int)SoundEnum::Propeller2, 1, "data/Sound/SE/Propeller.wav", false);

			SE->Add((int)SoundEnum::RunFoot, 6, "data/Sound/SE/move/runfoot.wav", false);
			SE->Add((int)SoundEnum::StandUp, 1, "data/Sound/SE/move/sliding.wav", false);
			SE->Add((int)SoundEnum::Siren, 1, "data/Sound/SE/siren.wav", false);

			SetCreate3DSoundFlag(FALSE);
			//this->m_BGM.vol(128);
			SE->Get((int)SoundEnum::EngineStart).SetVol_Local(192);
			SE->Get((int)SoundEnum::Engine2).SetVol_Local(64);
			SE->Get((int)SoundEnum::Propeller2).SetVol_Local(128);
			SE->Get((int)SoundEnum::RunFoot).SetVol_Local(128);
			SE->Get((int)SoundEnum::Siren).SetVol_Local(128);

			auto* KeyGuide = FPS_n2::KeyGuideClass::Instance();

			KeyGuide->AddGuide("", "何れかのキーを押してスキップ");
		}
		//
		bool			StartMovieScene::Update_Sub(bool*  isPause) noexcept {
			//auto* Pad = FPS_n2::PadControl::Instance();
			if (*isPause) {
				return true;
			}
			{
				auto kill = FPS_n2::SaveDataClass::Instance()->GetParam("KillCount");
				auto killTotal = 80;
				if (g_Mode == 1 && (kill < killTotal)) {
					return false;
				}
			}
#ifdef DEBUG
			auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
#ifdef DEBUG
			DebugParts->SetPoint("update start");
#endif // DEBUG
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();
#ifdef DEBUG
			//auto* DebugParts = DebugClass::Instance();					//デバッグ
#endif // DEBUG
				//FirstDoingv
			if (GetIsFirstLoop()) {
				BaseTime = GetNowHiPerformanceCount();
			}

			auto time = GetNowHiPerformanceCount() - BaseTime;
			NowTimeWait += (time);
			BaseTime = GetNowHiPerformanceCount();

			//Input,AI
			for (auto& c : plane_Pool) {
				int index = (int)(&c - &plane_Pool.front());

				c->SetAnimLoop(3, m_Propeller);
				c->GetObj().work_anime();

				auto Move = c->GetMove();
				if (AimPos.at(index).Speed > 0.06f) {
					auto Mine = Move.mat.zvec()*-1.f; Mine.y(0.f);
					auto Vec = AimPos.at(index).Aim - Move.pos; Vec.y(0.f);

					Move.mat *= MATRIX_ref::RotY(deg2rad(-1.f * (Mine.Norm().cross(Vec.Norm()).y()) * 60.f / FPS));
				}
				Move.pos += Move.mat.zvec()*-1.f *(AimPos.at(index).Speed)*0.3f*60.f / FPS;
				c->SetMove(Move.mat, Move.pos);
				c->UpdateMove();
			}
			for (auto& c : character_Pool) {
				c->SetAnimOnce(0, 1.0f);
				if (m_Seq >= 2) {
					c->SetAnimOnce(3, 1.0f);
				}
				c->GetObj().work_anime();
			}
			//Execute
			ObjMngr->ExecuteObject();
			//いらないオブジェクトの除去
			ObjMngr->DeleteCheck();
			//背景更新
			this->m_BackGround->FirstExecute();
			ObjMngr->LateExecuteObject();
			//視点
			{
				auto Prev = m_Seq;
				switch (m_Seq) {
				case 0:
				{
					if (m_SeqCount == 0.f) {
						SE->Get((int)SoundEnum::Siren).Play(0, DX_PLAYTYPE_LOOP, TRUE);
					}
					m_CamPos.zadd(-60.f / FPS);
					m_CamPosRandBuf = VECTOR_ref::vget(1.f, 0.5f, 1.f)*Scale_Rate;
					if (m_CamPos.z() < 1.8f*Scale_Rate) {
						m_Seq++;
						//
						m_Seq1Yadd = 5.f*Scale_Rate;
						m_CamUp = VECTOR_ref::vget(-0.5f, 0.8f, -0.5f).Norm();
						SE->Get((int)SoundEnum::StandUp).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}

					if (m_Seq0Count > 0.3f) {
						m_Seq0Count -= 0.3f;
						SE->Get((int)SoundEnum::RunFoot).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					else {
						m_Seq0Count += 1.f / FPS;
					}
				}
				break;
				case 1:
				{
					m_CamPos.yadd(m_Seq1Yadd / FPS);

					m_Seq1Yadd = std::max(m_Seq1Yadd - 3.f * 60.f / FPS, 0.f);

					m_CamPosRandBuf = VECTOR_ref::vget(0.1f, 0.1f, 0.1f)*Scale_Rate;

					Easing(&m_CamUp, VECTOR_ref::up(), 0.95f, EasingType::OutExpo);
					Easing(&m_CamVec, plane_Pool.at(0)->GetMatrix().zvec()*-1.f, 0.95f, EasingType::OutExpo);
					Easing(&m_CamPos, plane_Pool.at(0)->GetObj().GetFrameLocalWorldMatrix(20).pos(), 0.95f, EasingType::OutExpo);//頭　仮

					if (m_SeqCount == 0.f) {
						SE->Get((int)SoundEnum::EngineStart).Play(0, DX_PLAYTYPE_BACK, TRUE);
					}
					m_Propeller += 1.f / FPS;
					if (m_Propeller > 5.f) {
						m_Seq++;
						//
						for (auto& c : plane_Pool) {
							c->GetObj().get_anime(1).per = 1.f;
							c->GetObj().get_anime(2).per = 0.f;
						}
						character_Pool.at(0)->SetActive(false);
						{
							auto& c = character_Pool.at(1);
							c->GetObj().get_anime(1).per = 0.f;
							c->GetObj().get_anime(3).per = 1.f;

							c->SetMove(c->GetMove().mat*MATRIX_ref::RotY(deg2rad(90.f)), c->GetMove().pos + c->GetMove().mat.zvec()*-1.f*3.f * Scale_Rate);
							c->UpdateMove();
						}
						{
							auto& c = character_Pool.at(2);
							c->GetObj().get_anime(2).per = 0.f;
							c->GetObj().get_anime(3).per = 1.f;

							c->SetMove(c->GetMove().mat*MATRIX_ref::RotY(deg2rad(-90.f)), c->GetMove().pos + c->GetMove().mat.zvec()*-1.f*3.f * Scale_Rate);
							c->UpdateMove();
						}

						SE->Get((int)SoundEnum::EngineStart).StopAll(0);
						SE->Get((int)SoundEnum::Engine2).Play(0, DX_PLAYTYPE_LOOP, TRUE);
						SE->Get((int)SoundEnum::Propeller2).Play(0, DX_PLAYTYPE_LOOP, TRUE);

						m_Propeller = 1.f;
					}
				}
				break;
				case 2:
				{
					m_CamPosRandBuf = VECTOR_ref::vget(0.3f, 0.1f, 0.3f)*Scale_Rate;

					m_CamPos = VECTOR_ref::vget(-1.f*Scale_Rate, 0.9f*Scale_Rate, -25.f*Scale_Rate);
					m_CamVec = (plane_Pool.at(0)->GetObj().GetFrameLocalWorldMatrix(20).pos() - m_CamPos).Norm();
					m_CamUp = VECTOR_ref::up();

					m_near = 1.f*Scale_Rate;
					m_far = 40.f*Scale_Rate;
					m_fov = deg2rad(25);

					for (auto& c : plane_Pool) {
						int index = (int)(&c - &plane_Pool.front());

						AimPos.at(index).Speed = std::clamp(AimPos.at(index).Speed + 1.f / FPS, 0.f, 1.f);
					}
					if (m_SeqCount > 3.f) {
						for (auto& c : character_Pool) {
							c->SetActive(false);
						}
					}
					if (m_SeqCount > 4.f) {
						m_Seq++;
						SE->Get((int)SoundEnum::Engine2).SetVol_Local(192);
						SE->Get((int)SoundEnum::Propeller2).SetVol_Local(255);
					}
				}
				break;
				case 3:
				{
					m_CamPosRandBuf = VECTOR_ref::vget(0.3f, 0.6f, 0.3f)*Scale_Rate;

					m_CamPos = VECTOR_ref::vget(-7.f*Scale_Rate, -0.6f*Scale_Rate, 3.f*Scale_Rate)*2.f;
					m_CamPos += plane_Pool.at(0)->GetObj().GetFrameLocalWorldMatrix(20).pos();
					m_CamVec = (plane_Pool.at(0)->GetObj().GetFrameLocalWorldMatrix(20).pos() - m_CamPos).Norm();

					m_near = 0.3f*Scale_Rate;
					m_far = 20.f*Scale_Rate;
					m_fov = deg2rad(25);

					for (auto& c : plane_Pool) {
						int index = (int)(&c - &plane_Pool.front());

						AimPos.at(index).Aim.xadd(40.f*1.f / FPS);

						AimPos.at(index).Speed = std::clamp(AimPos.at(index).Speed + 2.f / FPS, 0.f, 30.f);
						if (AimPos.at(index).Speed > 12.f) {
							Easing(&c->GetObj().get_anime(0).per, 0.f, 0.995f, EasingType::OutExpo);
							c->GetObj().get_anime(4).per = 1.f - c->GetObj().get_anime(0).per;
						}

						if (AimPos.at(index).Speed >= 18.f) {
							auto Move = c->GetMove();

							Move.pos += VECTOR_ref::up()* 0.03f *(1.f + GetRandf(0.5f)) * Scale_Rate * 60.f / FPS;

							c->SetMove(Move.mat, Move.pos);
						}
					}
					if (m_SeqCount > 10.f) {
						m_Seq++;
						SE->Get((int)SoundEnum::Engine2).SetVol_Local(64);
						SE->Get((int)SoundEnum::Propeller2).SetVol_Local(96);
					}
				}
				break;
				case 4:
				{
					m_CamPosRandBuf = VECTOR_ref::vget(0.3f, 0.6f, 0.3f)*Scale_Rate;

					m_CamPos = VECTOR_ref::vget(-7.f*Scale_Rate, 1.6f*Scale_Rate, 3.f*Scale_Rate)*2.f;
					m_CamVec = (plane_Pool.at(0)->GetObj().GetFrameLocalWorldMatrix(20).pos() - m_CamPos).Norm();

					m_near = 1.f*Scale_Rate;
					m_far = 50.f*Scale_Rate;
					m_fov = deg2rad(15);
					if (m_SeqCount > 3.f) {
						m_fov = deg2rad(5);
					}

					for (auto& c : plane_Pool) {
						int index = (int)(&c - &plane_Pool.front());

						AimPos.at(index).Aim.xadd(40.f*1.f / FPS);

						{
							auto Move = c->GetMove();

							Move.pos += VECTOR_ref::up()* 0.045f *(1.f + GetRandf(0.5f)) * Scale_Rate * 60.f / FPS;

							c->SetMove(Move.mat, Move.pos);
						}
					}

				}
				break;
				default:
					break;
				}
				if (Prev != m_Seq) {
					m_SeqCount = 0.f;
				}
				else {
					m_SeqCount += 1.f / FPS;
				}

				Easing(&m_CamPosRand, VECTOR_ref::vget(GetRandf(m_CamPosRandBuf.x()), GetRandf(m_CamPosRandBuf.y()), GetRandf(m_CamPosRandBuf.z())), 0.9f, EasingType::OutExpo);
				Easing(&m_CamPosRand2, m_CamPosRand, 0.9f, EasingType::OutExpo);

				{
					VECTOR_ref CamUp = m_CamUp;
					VECTOR_ref CamVec = m_CamVec;
					VECTOR_ref CamPos = m_CamPos + m_CamPosRand2;

					SetMainCamera().SetCamPos(
						CamPos,
						CamPos + CamVec,
						CamUp);
				}
				auto fov_t = GetMainCamera().GetCamFov();
				auto near_t = GetMainCamera().GetCamNear();
				auto far_t = GetMainCamera().GetCamFar();
				Easing(&near_t, m_near, 0.9f, EasingType::OutExpo);
				Easing(&far_t, m_far, 0.9f, EasingType::OutExpo);
				Easing(&fov_t, m_fov, 0.9f, EasingType::OutExpo);
				SetMainCamera().SetCamInfo(fov_t, near_t, far_t);
			}
			//
#ifdef DEBUG
			DebugParts->SetPoint("update end");
#endif // DEBUG
			//
			if (!(m_SkipCount == 0.f && !CheckHitKeyAll())) {
				m_SkipCount += 1.f / FPS / 0.5f;
			}

			if (m_SkipCount > 1.f) {
				return false;
			}
			if (m_Seq == 4) {
				if (m_SeqCount > 10.f) {
					return false;
				}
			}
			return true;
		}
		void			StartMovieScene::Dispose_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			auto* SE = SoundPool::Instance();

			SE->Get((int)SoundEnum::EngineStart).StopAll(0);
			SE->Get((int)SoundEnum::Engine2).StopAll(0);
			SE->Get((int)SoundEnum::Propeller2).StopAll(0);
			SE->Get((int)SoundEnum::Siren).StopAll(0);
			EffectControl::Dispose();
			for (auto& c : plane_Pool) {
				c.reset();
			}
			plane_Pool.clear();
			for (auto& c : character_Pool) {
				c.reset();
			}
			AimPos.clear();
			character_Pool.clear();
			ObjMngr->DisposeObject();
			this->m_BackGround->Dispose();
			this->m_BackGround.reset();
		}

		void			StartMovieScene::ShadowDraw_NearFar_Sub(void) noexcept {
			this->m_BackGround->Shadow_Draw_NearFar();
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawObject_Shadow();
		}
		void			StartMovieScene::ShadowDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			this->m_BackGround->Shadow_Draw();
			//ObjMngr->DrawObject_Shadow();
			ObjMngr->DrawObject();
		}
		void			StartMovieScene::MainDraw_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();

			SetFogStartEnd(Scale_Rate * 1200.f, Scale_Rate * 1600.f);
			this->m_BackGround->Draw();

			ObjMngr->DrawObject();
		}
		void			StartMovieScene::MainDrawbyDepth_Sub(void) noexcept {
			auto* ObjMngr = ObjectManager::Instance();
			ObjMngr->DrawDepthObject();
		}
		void			StartMovieScene::DrawUI_In_Sub(void) noexcept {
			auto* DrawParts = DXDraw::Instance();
			if (m_Seq == 4) {
				auto per = std::clamp((m_SeqCount - 6.f)*2.f, 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}
			{
				auto per = std::clamp((m_SkipCount), 0.f, 1.f);
				SetDrawBlendMode(DX_BLENDMODE_ALPHA, std::clamp((int)(255.f*per), 0, 255));

				DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, GetColor(0, 0, 0), TRUE);

				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
			}

			if (NowTimeWait > 0) {
				TLClass.Draw(NowTimeWait);
			}
		}
	};
};
