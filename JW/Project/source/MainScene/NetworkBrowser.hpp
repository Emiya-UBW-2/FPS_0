#pragma once
#include	"../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class SequenceEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};
		class NetWorkBrowser {
			//サーバー専用
			ServerControl			m_ServerCtrl;																//
			//クライアント専用
			ClientControl			m_ClientCtrl;																//
			//共通
			bool					m_IsClient{ true };															//
			SequenceEnum			m_Sequence{ SequenceEnum::SelMode };										//
			bool					m_SeqFirst{ false };														//
			float					m_Tick{ 1.f };																//
			NewWorkSetting			m_NewWorkSetting;															//
			int						m_NewWorkSelect{ 0 };														//
			NewSetting				m_NewSetting;																//
			double					m_ClientFrame{ 0.0 };
			float					m_Ping{ 0.f };

			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };
		public:
			const auto& GetClient(void) const noexcept { return this->m_IsClient; }
			const auto& GetSequence(void) const noexcept { return this->m_Sequence; }
		public:
			const auto&		GetMyPlayerID(void) const noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetMyPlayer().ID : this->m_ServerCtrl.GetMyPlayer().ID; }
			const auto		GetNowServerPlayerData(int ID, bool isYradReset) noexcept { return (this->m_IsClient) ? this->m_ClientCtrl.GetNowServerPlayerData(ID, isYradReset) : this->m_ServerCtrl.GetNowServerPlayerData(ID, isYradReset); }
			void			GetRecvData(int ID, double ServerFrame) noexcept {
				if ((this->m_IsClient) ? this->m_ClientCtrl.GetRecvData(ID) : this->m_ServerCtrl.GetRecvData(ID)) {
					this->m_Ping = (float)(this->m_ClientFrame - ServerFrame)*1000.f;
				}
				printfDx("ping %lf \n", this->m_Ping);
			}
		public:
			void Init(void) noexcept {
				m_NewWorkSetting.Load();
				m_NewWorkSelect = 0;
			}
			void FirstExecute(const InputControl& MyInput, const SendInfo& SendMove) noexcept {
				this->m_LeftClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				if (!this->m_LeftClick.press()) {
					this->m_LeftPressTimer = 0.f;
				}

				if (this->m_IsClient) {
					m_ClientCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ClientCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, this->m_NewSetting.IP);
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ClientCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
				//サーバー
				else {
					m_ServerCtrl.SetMyPlayer(MyInput, SendMove.m_Pos, SendMove.m_Vec, SendMove.m_rad, this->m_ClientFrame, SendMove.m_Damage, SendMove.m_DamageSwitch);
					if ((this->m_Sequence == SequenceEnum::Matching) && m_SeqFirst) {
						m_ServerCtrl.Init(this->m_NewSetting.UsePort, this->m_Tick, IPDATA());
					}
					if ((this->m_Sequence >= SequenceEnum::Matching) && this->m_ServerCtrl.Execute()) {
						this->m_Sequence = SequenceEnum::MainGame;
					}
				}
			}
			void LateExecute(void) noexcept {
				if (this->m_Sequence == SequenceEnum::MainGame) {
					this->m_ClientFrame += 1.0 / (double)FPS;
				}
				else {
					this->m_ClientFrame = 0.0;
				}
			}
			void Draw(void) noexcept {
				//auto* ObjMngr = ObjectManager::Instance();
				//auto* PlayerMngr = PlayerManager::Instance();
				//auto* DrawParts = DXDraw::Instance();
				auto* Fonts = FontPool::Instance();
				auto Red = GetColor(255, 0, 0);
				//auto Blue = GetColor(50, 50, 255);
				//auto Green = GetColor(43, 163, 91);
				auto White = GetColor(255, 255, 255);
				auto Gray = GetColor(64, 64, 64);
				auto Black = GetColor(0, 0, 0);
				unsigned int color = Red;

				int xp, yp, xs, ys;
				xp = y_r(100);
				yp = y_r(250);
				xs = y_r(500);
				ys = y_r(300);

				int y_h = y_r(30);
				//bool Mid_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
				//bool Right_key = ((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
				int mx = DXDraw::Instance()->m_DispXSize / 2, my = DXDraw::Instance()->m_DispYSize / 2;
				GetMousePoint(&mx, &my);
				//ラムダ
				auto MsgBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					color = Black;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, String, args...);
				};
				auto ClickBox = [&](int xp1, int yp1, int xp2, int yp2, std::string String, auto&&... args) {
					bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
					color = (into) ? Black : Gray;
					DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, String, args...);
					return into && this->m_LeftClick.trigger();
				};
				auto AddSubBox = [&](int xp1, int yp1, std::function<void()> UpFunc, std::function<void()> DownFunc) {
					int xp2, yp2;
					{
						xp2 = xp1 + y_r(50);
						yp2 = yp1 + y_h;
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "+");
						if (into) {
							if (this->m_LeftClick.trigger()) {
								UpFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									UpFunc();
								}
							}
						}
					}
					{
						yp1 += y_r(50);
						yp2 += y_r(50);
						bool into = in2_(mx, my, xp1, yp1, xp2, yp2);
						color = (into) ? Black : Gray;
						DrawBox(xp1, yp1, xp2, yp2, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::MIDDLE, FontHandle::FontYCenter::TOP, xp1 + (xp2 - xp1) / 2, yp1, White, Black, "-");
						if (into) {
							if (this->m_LeftClick.trigger()) {
								DownFunc();
							}
							if (this->m_LeftClick.press()) {
								m_LeftPressTimer += 1.f / FPS;
								if (this->m_LeftPressTimer > 0.5f) {
									DownFunc();
								}
							}
						}
					}
				};
				//
				{
					DrawBox(xp - y_r(10), yp - y_r(10), xp + xs + y_r(10), yp + ys + y_r(10), GetColor(164, 164, 164), TRUE);//背景
					MsgBox(xp, yp, xp + xs, yp + y_h, " %d/%d", (int)this->m_Sequence, (int)SequenceEnum::MainGame);
					//ログ
					{
						int xp1, yp1;
						xp1 = xp;
						yp1 = yp + ys + y_r(10) + y_r(10);
						if (this->m_Sequence > SequenceEnum::SelMode) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "種別[%s]", this->m_IsClient ? "クライアント" : "サーバー"); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::Set_Port) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "使用ポート[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetTick) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "ティックレート[%4.1f]", Frame_Rate / this->m_Tick); yp1 += y_h;
						}
						if (this->m_Sequence > SequenceEnum::SetIP) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp1, yp1, White, Black, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4); yp1 += y_h;
						}
					}
				}
				auto Prev = this->m_Sequence;
				switch (this->m_Sequence) {
				case SequenceEnum::SelMode:
					if (ClickBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "クライアントになる")) {
						this->m_IsClient = true;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					if (ClickBox(xp, yp + y_r(100), xp + xs, yp + y_r(100) + y_h, "サーバーになる")) {
						this->m_IsClient = false;
						this->m_Tick = 1.f;
						this->m_Sequence = SequenceEnum::CheckPreset;
					}
					break;
				case SequenceEnum::CheckPreset:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "プリセット設定");
					for (int i = 0; i < this->m_NewWorkSetting.GetSize(); i++) {
						auto n = this->m_NewWorkSetting.Get(i);
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "[%d][%d,%d,%d,%d]", n.UsePort, n.IP.d1, n.IP.d2, n.IP.d3, n.IP.d4)) {
							this->m_NewSetting.UsePort = n.UsePort;
							this->m_NewSetting.IP = n.IP;
							this->m_Sequence = SequenceEnum::SetTick;
							m_NewWorkSelect = i;
							break;
						}
					}
					{
						int i = this->m_NewWorkSetting.GetSize();
						if (ClickBox(xp, yp + y_r(50)*(i + 2), xp + xs, yp + y_r(50)*(i + 2) + y_h, "設定を追加する")) {
							m_NewWorkSetting.Add();
							m_NewWorkSelect = i;
							this->m_Sequence = SequenceEnum::Set_Port;
						}
					}
					break;
				case SequenceEnum::Set_Port://ポート
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ポート=[%d-%d]", this->m_NewSetting.UsePort, this->m_NewSetting.UsePort + Player_num - 1);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_NewSetting.UsePort++; }, [&]() { this->m_NewSetting.UsePort--; });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetIP;//サーバ-は一応いらない
					}
					break;
				case SequenceEnum::SetIP://IP
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "IP=[%d,%d,%d,%d]", this->m_NewSetting.IP.d1, this->m_NewSetting.IP.d2, this->m_NewSetting.IP.d3, this->m_NewSetting.IP.d4);
					for (int i = 0; i < 4; i++) {
						auto* ip_tmp = &this->m_NewSetting.IP.d1;
						switch (i) {
						case 0:ip_tmp = &this->m_NewSetting.IP.d1; break;
						case 1:ip_tmp = &this->m_NewSetting.IP.d2; break;
						case 2:ip_tmp = &this->m_NewSetting.IP.d3; break;
						case 3:ip_tmp = &this->m_NewSetting.IP.d4; break;
						}
						AddSubBox(y_r(100 + 70 * i), yp + y_r(100),
							[&]() {
							if (*ip_tmp == 255) { *ip_tmp = 0; }
							else { (*ip_tmp)++; }
						}, [&]() {
							if (*ip_tmp == 0) { *ip_tmp = 255; }
							else { (*ip_tmp)--; }
						});
					}
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::SetTick;
						m_NewWorkSetting.Set(this->m_NewWorkSelect, this->m_NewSetting);
						m_NewWorkSetting.Save();
					}
					break;
				case SequenceEnum::SetTick:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "ティック=[%4.1f]", Frame_Rate / this->m_Tick);
					AddSubBox(xp, yp + y_r(100), [&]() { this->m_Tick = std::clamp(this->m_Tick - 1.f, 1.f, 20.f); }, [&]() { this->m_Tick = std::clamp(this->m_Tick + 1.f, 1.f, 20.f); });
					if (ClickBox(y_r(380), yp + y_r(100), y_r(380) + y_r(120), yp + y_r(100) + y_h, "Set")) {
						this->m_Sequence = SequenceEnum::Matching;
					}
					break;
				case SequenceEnum::Matching:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "他プレイヤー待機中…");
					for (int i = 0; i < Player_num; i++) {
						bool isActive = (((this->m_IsClient) ? this->m_ClientCtrl.GetServerDataCommon() : this->m_ServerCtrl.GetServerData()).PlayerData[i].IsActive == 1);
						int yp1 = yp + y_r(50) + y_r(35)*(i + 1);
						color = isActive ? Black : Gray;
						DrawBox(y_r(200), yp1, y_r(200) + y_r(300), yp1 + y_h, color, TRUE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200), yp1, White, Black, "Player");
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_h, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(200) + y_r(300), yp1, White, Black, (isActive ? "〇" : ""));
					}
					break;
				case SequenceEnum::MainGame:
					MsgBox(xp, yp + y_r(50), xp + xs, yp + y_r(50) + y_h, "通信中!");
					break;
				default:
					break;
				}
				m_SeqFirst = (Prev != this->m_Sequence);
			}
			void Dispose(void) noexcept {
				m_ServerCtrl.Dispose();
				m_ClientCtrl.Dispose();
			}
		};
	};
};
