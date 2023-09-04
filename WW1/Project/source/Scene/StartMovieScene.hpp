#pragma once
#include	"../Header.hpp"
#include "../MainScene/Object/ObjectBase.hpp"

namespace FPS_n2 {
	namespace Sceneclass {

		class PlaneMove {
		public:
			VECTOR_ref Aim;
			float Speed{ 0.f };
		};

		class LoadScriptClass {
		private:
			struct VARIABLE {
				std::string Base;
				std::string After;
			};
		private:
			std::vector<VARIABLE> m_Variable;
			std::string m_Func;
			std::vector<std::string> m_Args;
		private:
			static void Sub_Func(std::string& func_t, const char& in_str) noexcept {
				size_t str_switch = 0;
				size_t str_in = std::string::npos;
				bool in = false;
				while (true) {
					if (str_switch != std::string::npos) { str_switch = func_t.find('\"', str_switch + 1); in ^= 1; }
					str_in = func_t.find(in_str, str_in + 1);
					if (str_in != std::string::npos) {
						if (str_switch != std::string::npos && str_switch < str_in && in) {
							continue;
						}
						func_t = func_t.erase(str_in, 1);
						continue;
					}
					break;
				}
			}
		public:
			//Getter
			const auto& Getfunc(void) const noexcept { return m_Func; }
			const auto& Getargs(void) const noexcept { return m_Args; }
			//スクリプト読み込み処理
			void LoadScript(std::string_view func_t) noexcept {
				m_Args.clear();
				m_Func = func_t;
				{
					// //を削除
					size_t sls = m_Func.find("//");
					if (sls != std::string::npos) { m_Func = m_Func.substr(0, sls); }
					//いらない要素を排除
					Sub_Func(m_Func, '{');
					Sub_Func(m_Func, '}');
					Sub_Func(m_Func, ' ');
					Sub_Func(m_Func, '\t');
					Sub_Func(m_Func, ';');
					Sub_Func(m_Func, '\"');
				}
				//()と,で囲われた部分から引数を取得
				if (m_Func != "") {
					std::string tmp_func = m_Func;
					size_t left = tmp_func.find("(");
					size_t right = tmp_func.rfind(")");
					if (left != std::string::npos && right != std::string::npos) {
						tmp_func = tmp_func.substr(left + 1, right - 1 - left);
					}
					while (true) {
						size_t in_str = tmp_func.find(",");
						if (in_str == std::string::npos) {
							m_Args.emplace_back(tmp_func);
							break;
						}
						else {
							std::string arg = tmp_func.substr(0, in_str);
							tmp_func = tmp_func.substr(in_str + 1);
							m_Args.emplace_back(arg);
						}
					}
				}
			}
		};

		class TelopClass {
		private:
			class Cut_tex {
				int xpos = 0;
				int ypos = 0;
				int size = 0;
				int LMR = 1;
				std::string str;
				LONGLONG START_TIME = 0;
				LONGLONG END_TIME = 0;
			public:
				Cut_tex(void) noexcept {
					xpos = 0;
					ypos = 0;
					size = 12;
					str = "test";
					START_TIME = 1000000 * 1 / 100;
					END_TIME = 1000000 * 101 / 100;
				}
				void Set(int xp, int yp, int Fontsize, std::string_view mag, LONGLONG StartF, LONGLONG ContiF, int m_LMR) noexcept {
					this->xpos = xp;
					this->ypos = yp;
					this->size = Fontsize;
					this->str = mag;
					this->START_TIME = StartF;
					this->END_TIME = StartF + ContiF;;
					this->LMR = m_LMR;
				}
				void Draw(LONGLONG nowTimeWait) const noexcept {
					if (this->START_TIME < nowTimeWait && nowTimeWait < this->END_TIME) {
						auto* Fonts = FontPool::Instance();
						switch (this->LMR) {
						case 0:
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						case 1:
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						case 2:
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(this->size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::TOP, this->xpos, this->ypos, GetColor(255, 255, 255), GetColor(0, 0, 0), this->str);
							break;
						default:
							break;
						}
					}
				}
			};
		private:
			std::vector<Cut_tex> Texts;
			LONGLONG StartF = 0;
			LONGLONG ContiF = 0;
		public:
			void Init(void) noexcept {
				StartF = 0;
				ContiF = 0;
			}
			void LoadTelop(const std::string &func, const std::vector<std::string>& args) noexcept {
				if (func.find("SetTelopTime") != std::string::npos) {
					StartF = (LONGLONG)(1000000.f * std::stof(args[0]));
					ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
				}
				else if (func.find("AddTelopTime") != std::string::npos) {
					StartF += (LONGLONG)(1000000.f * std::stof(args[0]));
					ContiF = (LONGLONG)(1000000.f * std::stof(args[1]));
				}
				else if (func.find("SetTelop") != std::string::npos) {
					int t = 0;
					if (args[4].find("LEFT") != std::string::npos) { t = 0; }
					else if (args[4].find("MIDDLE") != std::string::npos) { t = 1; }
					else if (args[4].find("RIGHT") != std::string::npos) { t = 2; }
					Texts.resize(Texts.size() + 1);
					Texts.back().Set(y_r(std::stoi(args[0])), y_r(std::stoi(args[1])), y_r(std::stoi(args[2])), args[3], StartF, ContiF, t);
				}
			}
			void Draw(LONGLONG nowTimeWait) const noexcept {
				for (auto& t : Texts) {
					t.Draw(nowTimeWait);
				}
			}
		};

		class StartMovieScene : public TEMPSCENE, public EffectControl {
		private:
			//リソース関連
			std::shared_ptr<BackGroundClassMovie>			m_BackGround;				//BG

			std::vector<std::shared_ptr<ObjectBaseClass>> character_Pool;				//ポインター別持ち
			std::vector<std::shared_ptr<ObjectBaseClass>> plane_Pool;				//ポインター別持ち
			std::vector<PlaneMove>		AimPos;
			VECTOR_ref m_CamPosRandBuf, m_CamPosRand, m_CamPosRand2;

			VECTOR_ref	m_CamPos;
			VECTOR_ref	m_CamVec;
			VECTOR_ref	m_CamUp;
			float		m_near{ 0.f }, m_far{ 0.f }, m_fov{ 0.f };

			int			m_Seq{ 0 };
			float		m_SeqCount{ 0.f };

			float		m_SkipCount{ 0.f };

			float		m_Seq0Count{ 0.f };
			float		m_Seq1Yadd{ 0.f };
			float		m_Propeller{ 0.f };


			LoadScriptClass LSClass;		//スクリプト読み込み
			TelopClass TLClass;				//テロップ

			LONGLONG BaseTime{ 0 }, WaitTime{ 0 }, NowTimeWait{ 0 };//
		public:
			StartMovieScene(void) noexcept { }
			void			Set_Sub(void) noexcept override;
			//
			bool			Update_Sub(void) noexcept override;
			void			Dispose_Sub(void) noexcept override;
			//
			void			Depth_Draw_Sub(void) noexcept override {
				this->m_BackGround->Draw();
			}
			void			BG_Draw_Sub(void) noexcept override {
				this->m_BackGround->BG_Draw();
			}
			void			ShadowDraw_Far_Sub(void) noexcept override {
				this->m_BackGround->Shadow_Draw_Far();
			}
			void			ShadowDraw_NearFar_Sub(void) noexcept override;
			void			ShadowDraw_Sub(void) noexcept override;
			void			MainDraw_Sub(void) noexcept override;
			void			MainDrawbyDepth_Sub(void) noexcept override;
			//UI表示
			void			DrawUI_Base_Sub(void) noexcept  override {}
			void			DrawUI_In_Sub(void) noexcept override;
		};
	};
};
