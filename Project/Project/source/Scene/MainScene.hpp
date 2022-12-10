#pragma once
#include"Header.hpp"

#define LineHeight	y_r(32)
#define EdgeSize	y_r(2)

namespace FPS_n2 {

	static const unsigned int Red{ GetColor(255, 0, 0) };
	static const unsigned int Red25{ GetColor(192, 0, 0) };
	static const unsigned int Red50{ GetColor(128, 0, 0) };

	static const unsigned int Blue{ GetColor(50, 50, 255) };
	static const unsigned int Green{ GetColor(43, 255, 91) };
	static const unsigned int White{ GetColor(255, 255, 255) };
	static const unsigned int Gray10{ GetColor(230, 230, 230) };
	static const unsigned int Gray25{ GetColor(192, 192, 192) };
	static const unsigned int Gray50{ GetColor(128, 128, 128) };
	static const unsigned int Gray75{ GetColor(64, 64, 64) };
	static const unsigned int Black{ GetColor(0, 0, 0) };

	class InputControl : public SingletonBase<InputControl> {
	private:
		friend class SingletonBase<InputControl>;
	private:
		int						m_mouse_x, m_mouse_y;

		switchs					m_LeftClick;
		float					m_LeftPressTimer{ 0.f };

		switchs					m_MiddleClick;

		switchs					m_RightClick;

		switchs					m_ShiftKey;
		switchs					m_SpaceKey;
		switchs					m_CtrlKey;
		std::array<switchs, 26>	m_AtoZKey;

		int						m_WheelAdd{ 0 };
	private:
		InputControl(void) noexcept {
			m_LeftClick.Set(false);
			m_MiddleClick.Set(false);
			m_RightClick.Set(false);
			m_ShiftKey.Set(false);
			m_SpaceKey.Set(false);
			m_CtrlKey.Set(false);
			for (auto& k : m_AtoZKey) {
				k.Set(false);
			}
		}
		~InputControl(void) noexcept { }
	public:
		const auto&		GetMouseX(void) const noexcept { return this->m_mouse_x; }
		const auto&		GetMouseY(void) const noexcept { return this->m_mouse_y; }
		const auto&		GetLeftClick(void) const noexcept { return this->m_LeftClick; }
		const auto&		GetLeftPressTimer(void) const noexcept { return this->m_LeftPressTimer; }
		const auto&		GetMiddleClick(void) const noexcept { return this->m_MiddleClick; }
		const auto&		GetRightClick(void) const noexcept { return this->m_RightClick; }
		const auto&		GetShiftKey(void) const noexcept { return this->m_ShiftKey; }
		const auto&		GetSpaceKey(void) const noexcept { return this->m_SpaceKey; }
		const auto&		GetCtrlKey(void) const noexcept { return this->m_CtrlKey; }
		const auto&		GetKey(char AtoZ) const noexcept {
			if ('A' <= AtoZ && AtoZ <= 'Z') {
				return this->m_AtoZKey.at(AtoZ - 'A');
			}
			if ('a' <= AtoZ && AtoZ <= 'z') {
				return this->m_AtoZKey.at(AtoZ - 'a');
			}
			return this->m_AtoZKey.at(0);
		}

		const auto&		GetWheelAdd(void) const noexcept { return this->m_WheelAdd; }
	public:
		void	Execute(void) noexcept {
			GetMousePoint(&this->m_mouse_x, &this->m_mouse_y);
			this->m_LeftClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
			if (this->m_LeftClick.press()) {
				this->m_LeftPressTimer += 1.f / FPS;
			}
			else {
				this->m_LeftPressTimer = 0.f;
			}
			this->m_MiddleClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_MIDDLE) != 0);
			this->m_RightClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_RIGHT) != 0);
			this->m_ShiftKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LSHIFT) != 0);
			this->m_SpaceKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_SPACE) != 0);
			this->m_CtrlKey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LCONTROL) != 0);

			int KEYS[26] = {
				KEY_INPUT_A,
				KEY_INPUT_B,
				KEY_INPUT_C,
				KEY_INPUT_D,
				KEY_INPUT_E,
				KEY_INPUT_F,
				KEY_INPUT_G,
				KEY_INPUT_H,
				KEY_INPUT_I,
				KEY_INPUT_J,
				KEY_INPUT_K,
				KEY_INPUT_L,
				KEY_INPUT_M,
				KEY_INPUT_N,
				KEY_INPUT_O,
				KEY_INPUT_P,
				KEY_INPUT_Q,
				KEY_INPUT_R,
				KEY_INPUT_S,
				KEY_INPUT_T,
				KEY_INPUT_U,
				KEY_INPUT_V,
				KEY_INPUT_W,
				KEY_INPUT_X,
				KEY_INPUT_Y,
				KEY_INPUT_Z,
			};
			for (int loop = 0; loop < m_AtoZKey.size(); loop++) {
				m_AtoZKey[loop].Execute(CheckHitKeyWithCheck(KEYS[loop]) != 0);
			}
			m_WheelAdd = GetMouseWheelRotVolWithCheck();
		}
	};

	namespace WindowSystem {
		//箱
		static void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
			DrawBox(xp1, yp1, xp2, yp2, Gray75, TRUE);
			DrawBox(xp1 + EdgeSize, yp1 + EdgeSize, xp2 - EdgeSize, yp2 - EdgeSize, colorSet, TRUE);
		};
		//文字
		template <typename... Args>
		static const auto SetMsg(int xp1, int yp1, int xp2, int yp2, int size, FontHandle::FontXCenter FontX, std::string_view String, Args&&... args) {
			auto* Fonts = FontPool::Instance();
			switch (FontX) {
			case FontHandle::FontXCenter::LEFT:
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE,
					xp1 + y_r(6), yp1 + (yp2 - yp1) / 2, White, Black, ((std::string)String).c_str(), args...);
				break;
			case FontHandle::FontXCenter::MIDDLE:
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE,
					xp1 + (xp2 - xp1) / 2, yp1 + (yp2 - yp1) / 2, White, Black, ((std::string)String).c_str(), args...);
				break;
			case FontHandle::FontXCenter::RIGHT:
				Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(size, FontX, FontHandle::FontYCenter::MIDDLE,
					xp2 - y_r(6), yp1 + (yp2 - yp1) / 2, White, Black, ((std::string)String).c_str(), args...);
				break;
			default:
				break;
			}
			return Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(size, ((std::string)String).c_str(), args...) + y_r(6) + 2;//エッジ分
		};
		//
		template <typename... Args>
		bool ClickCheckBox(int xp1, int yp1, int xp2, int yp2, bool isrepeat, bool IsActive, unsigned int defaultcolor, std::string_view String, Args&&... args) {
			auto* Input = InputControl::Instance();
			unsigned int color = defaultcolor;
			bool isHit = false;
			if (IsActive && in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
				color = White;
				if (Input->GetLeftClick().press()) {
					if (Input->GetLeftClick().trigger()) {
						isHit = true;
					}
				}
				if (0 < Input->GetLeftPressTimer() && Input->GetLeftPressTimer() < 0.1f) {
					color = Black;
				}
				if (isrepeat && Input->GetLeftPressTimer() > 0.5f) {
					isHit = true;
					color = Gray50;
				}

				HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
				SetCursor(hCursor);
			}
			SetBox(xp1, yp1, xp2, yp2, color);
			SetMsg(xp1, yp1, xp2, yp2, LineHeight - y_r(6), FontHandle::FontXCenter::MIDDLE, String, args...);
			return isHit;
		};
		//
		class ScrollBoxClass {
			bool		IsChangeScrollY{ false };
			int			BaseScrollY{ 0 };
			float		NowScrollYPer{ 0.f };
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) {
				auto* Input = InputControl::Instance();
				unsigned int color = Gray25;

				int length = (int)((float)(yp2 - yp1) / TotalPer);
				float Total = (float)(yp2 - yp1 - length);
				int Yp_t = (int)(Total * NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive) {
					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
						if (Input->GetLeftClick().trigger()) {
							IsChangeScrollY = true;
						}
					}
					if (IsChangeScrollY) {
						if (Input->GetLeftClick().press()) {
							color = White;
							NowScrollYPer = std::clamp((float)(Input->GetMouseY() - BaseScrollY) / Total, 0.f, 1.f);
						}
						else {
							IsChangeScrollY = false;
						}
					}
					else {
						BaseScrollY = Input->GetMouseY() - Yp_t;
						if (Input->GetMouseY() < Yp_s) {
							BaseScrollY += Yp_s - Input->GetMouseY();
						}

						if (Input->GetMouseY() > Yp_e) {
							BaseScrollY += Yp_e - Input->GetMouseY();
						}
					}
				}
				SetBox(xp1, yp1, xp2, yp2, Gray50);
				SetBox(xp1 + y_r(1), Yp_s, xp2 - y_r(1), Yp_e, color);
			};
		};
		class WindowControl {
		public:
			bool				isDelete{ false };
		private:
			bool				ActiveSwitch{ false };
			bool				IsActive{ false };
			int					PosX{ 0 }, PosY{ 0 };
			int					SizeX{ 100 }, SizeY{ 100 };
			int					TotalSizeY{ 200 };
			std::string			TabName;
			std::function<void(WindowControl*)> m_DoingOnWindow;

			ScrollBoxClass		Scroll;
		private:
			bool				isMaxSize{ false };
			int					PosXSave{ 0 }, PosYSave{ 0 };
			int					SizeXSave{ 100 }, SizeYSave{ 100 };

			bool				CanChageSize{ false };

			bool				IsMoving{ false };
			int					PosAddX{ 0 }, PosAddY{ 0 };

			bool				IsChangeScaleXY{ false };
			bool				IsChangeScale1X{ false };
			bool				IsChangeScale1Y{ false };
			bool				IsChangeScale2X{ false };
			bool				IsChangeScale2Y{ false };
			int					BaseScaleX{ 0 }, BaseScaleY{ 0 };
			int					BasePos1X{ 0 }, BasePos1Y{ 0 };
			int					BaseScale1X{ 0 }, BaseScale1Y{ 0 };
			int					BaseScale2X{ 0 }, BaseScale2Y{ 0 };
		public:
			const auto		GetIsEditing(void) const noexcept {
				auto* Input = InputControl::Instance();
				int xp1 = PosX;
				int yp1 = PosY;
				int xp2 = PosX + SizeX;
				int yp2 = PosY + SizeY;
				return
					in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2) ||
					this->CanChageSize ||
					this->IsMoving ||
					this->IsChangeScaleXY ||
					this->IsChangeScale1X ||
					this->IsChangeScale1Y ||
					this->IsChangeScale2X ||
					this->IsChangeScale2Y;
			}

			const auto&		GetPosX(void) const noexcept { return this->PosX; }
			const auto&		GetPosY(void) const noexcept { return this->PosY; }
			const auto&		GetSizeX(void) const noexcept { return this->SizeX; }
			const auto&		GetSizeY(void) const noexcept { return this->SizeY; }

			const auto&		GetActiveSwitch(void) const noexcept { return this->ActiveSwitch; }
			void			SetIsActive(bool value) noexcept { IsActive = value; }
			const auto&		GetIsActive(void) const noexcept { return this->IsActive; }

			void			SetTotalSizeY(bool value) noexcept { this->TotalSizeY = value; }
			const auto&		GetNowScrollPer(void) const noexcept { return this->Scroll.GetNowScrollYPer(); }
		public:
			void Set(int posx, int posy, int sizex, int sizey, int Totalsizey, const char* tabName, bool canChageSize, const std::function<void(WindowControl*)>& DoingOnWindow) noexcept {
				this->PosX = posx;
				this->PosY = posy;


				this->SizeX = sizex;
				this->SizeY = LineHeight + sizey;
				this->TotalSizeY = Totalsizey;
				this->TabName = tabName;

				int widthLimit = SetMsg(0, 0, 0, 0 + LineHeight, LineHeight - y_r(6), FontHandle::FontXCenter::LEFT, TabName) + EdgeSize * 2 + LineHeight * 2;
				this->SizeX = std::max(this->SizeX, widthLimit);

				this->CanChageSize = canChageSize;
				this->m_DoingOnWindow = DoingOnWindow;
			}
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* Input = InputControl::Instance();
				int xp1 = PosX;
				int yp1 = PosY;
				int xp2 = PosX + SizeX;
				int yp2 = PosY + SizeY;
				int widthLimit = 0;
				//背景
				{
					int add = y_r(2);
					DrawBox(xp1 + add, yp1 + add, xp2 + add, yp2 + add, Black, TRUE);
					SetBox(xp1, yp1, xp2, yp2, Gray10);
				}

				ActiveSwitch = false;
				if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
					if (Input->GetLeftClick().trigger()) {
						ActiveSwitch = true;
					}
				}
				//内容
				m_DoingOnWindow(this);
				//スクロールバー
				{
					float Total = (float)this->TotalSizeY / (SizeY - LineHeight);
					if (Total > 1.f) {
						Scroll.ScrollBox(xp2 - y_r(24), yp1 + LineHeight, xp2, yp2, Total, IsActive);
					}
				}
				//タブ
				{
					int xp3 = PosX + SizeX - LineHeight - LineHeight;
					unsigned int color = Gray25;
					SetBox(xp1, yp1, xp2, yp1 + LineHeight, color);
					widthLimit = SetMsg(xp1, yp1, xp3, yp1 + LineHeight, LineHeight - y_r(6), FontHandle::FontXCenter::LEFT, TabName) + EdgeSize * 2 + LineHeight * 2;
				}
				//最大化ボタン
				if (CanChageSize) {
					int xp3 = PosX + SizeX - LineHeight - LineHeight;
					int yp3 = PosY + EdgeSize;
					int xp4 = PosX + SizeX - LineHeight - EdgeSize;
					int yp4 = PosY + LineHeight - EdgeSize;
					unsigned int color = Gray25;

					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize)) {
						color = White;
						if (Input->GetLeftClick().trigger()) {
							isMaxSize ^= 1;
							if (isMaxSize) {
								PosXSave = PosX;
								PosYSave = PosY;
								SizeXSave = SizeX;
								SizeYSave = SizeY;

								PosX = y_r(0);
								PosY = y_r(0);
								SizeX = DrawParts->m_DispXSize;
								SizeY = DrawParts->m_DispYSize;
							}
							else {
								PosX = PosXSave;
								PosY = PosYSave;
								SizeX = SizeXSave;
								SizeY = SizeYSave;
							}
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}
					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, !isMaxSize ? "□" : "ﾛ");
				}
				//×ボタン
				{
					int xp3 = PosX + SizeX - LineHeight;
					int yp3 = PosY + EdgeSize;
					int xp4 = PosX + SizeX - EdgeSize;
					int yp4 = PosY + LineHeight - EdgeSize;
					unsigned int color = Red25;

					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize)) {
						color = Red;
						if (Input->GetLeftClick().trigger()) {
							//color = Red50;
							isDelete = true;
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}

					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, "X");
				}
				if (CanChageSize && !isMaxSize && IsActive) {
					//xyサイズ
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp2 - EdgeSize - radius, yp2 - EdgeSize - radius, xp2 - EdgeSize + radius, yp2 - EdgeSize + radius)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								IsChangeScaleXY = true;
							}
						}
						if (IsChangeScaleXY) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								SizeX = std::max((Input->GetMouseX() - BaseScaleX) - PosX, widthLimit);
								SizeY = std::max((Input->GetMouseY() - BaseScaleY) - PosY, LineHeight + y_r(10));
							}
							else {
								IsChangeScaleXY = false;
							}
						}
						else {
							BaseScaleX = Input->GetMouseX() - (PosX + SizeX);
							BaseScaleY = Input->GetMouseY() - (PosY + SizeY);
						}
						if (color != Gray25) {
							DrawCircle(xp2 - EdgeSize, yp2 - EdgeSize, radius, color);
						}
					}
					//yサイズ下
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize - radius, yp2 - EdgeSize - radius, xp2 - EdgeSize - radius, yp2 - EdgeSize + radius)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								IsChangeScale2Y = true;
							}
						}
						if (IsChangeScale2Y) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								SizeY = std::max((Input->GetMouseY() - BaseScale2Y) - PosY, LineHeight + y_r(10));
							}
							else {
								IsChangeScale2Y = false;
							}
						}
						else {
							BaseScale2Y = Input->GetMouseY() - (PosY + SizeY);
						}
						if (color != Gray25) {
							DrawBox(xp1 + EdgeSize, yp2 - EdgeSize - radius, xp2 - EdgeSize, yp2 - EdgeSize + radius, color, TRUE);
						}
					}
					//xサイズ右
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp2 + EdgeSize - radius, yp1 - EdgeSize - radius, xp2 - EdgeSize + radius, yp2 - EdgeSize - radius)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								IsChangeScale2X = true;
							}
						}
						if (IsChangeScale2X) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								SizeX = std::max((Input->GetMouseX() - BaseScale2X) - PosX, widthLimit);
							}
							else {
								IsChangeScale2X = false;
							}
						}
						else {
							BaseScale2X = Input->GetMouseX() - (PosX + SizeX);
						}
						if (color != Gray25) {
							DrawBox(xp2 - EdgeSize, yp1, xp2 + EdgeSize, yp2 - EdgeSize + radius, color, TRUE);
						}
					}
					//yサイズ上
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize - radius, yp1 - radius * 2, xp2 - EdgeSize - radius, yp1)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								IsChangeScale1Y = true;
							}
						}
						if (IsChangeScale1Y) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								PosY = std::min((Input->GetMouseY() - BasePos1Y), BaseScale1Y - (LineHeight + y_r(10)));
								SizeY = BaseScale1Y - PosY;
							}
							else {
								IsChangeScale1Y = false;
							}
						}
						else {
							BasePos1Y = Input->GetMouseY() - PosY;
							BaseScale1Y = PosY + SizeY;
						}
						if (color != Gray25) {
							DrawBox(xp1 + EdgeSize, yp1 - radius * 2, xp2 - EdgeSize, yp1, color, TRUE);
						}
					}
					//xサイズ左
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize - radius, yp1 - EdgeSize - radius, xp1 + EdgeSize + radius, yp2 - EdgeSize - radius)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								IsChangeScale1X = true;
							}
						}
						if (IsChangeScale1X) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								PosX = std::min((Input->GetMouseX() - BasePos1X), BaseScale1X - widthLimit);
								SizeX = BaseScale1X - PosX;
							}
							else {
								IsChangeScale1X = false;
							}
						}
						else {
							BasePos1X = Input->GetMouseX() - PosX;
							BaseScale1X = PosX + SizeX;
						}
						if (color != Gray25) {
							DrawBox(xp1 - EdgeSize, yp1, xp1 + EdgeSize, yp2 - EdgeSize + radius, color, TRUE);
						}
					}
				}
				//非アクティブ
				if (!IsActive) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 24);
					DrawBox(xp1, yp1, xp2, yp2, Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}

				//タブ演算
				{
					int xp3 = PosX + SizeX - LineHeight - LineHeight;
					unsigned int color = Gray25;

					if (IsActive && in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize, yp1 + EdgeSize, xp3 - EdgeSize, yp1 + LineHeight - EdgeSize)) {
						if (Input->GetLeftClick().trigger()) {
							IsMoving = true;
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
						SetCursor(hCursor);
					}
					if (IsMoving) {
						if (Input->GetLeftClick().press()) {
							color = Gray50;
							PosX = Input->GetMouseX() - PosAddX;
							PosY = Input->GetMouseY() - PosAddY;


							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
							SetCursor(hCursor);
						}
						else {
							if (CanChageSize) {
								if (y_r(10) < Input->GetMouseY() && Input->GetMouseY() < DrawParts->m_DispYSize - y_r(10)) {
									if (Input->GetMouseX() < y_r(10)) {
										PosX = y_r(0);
										PosY = y_r(0);
										SizeX = DrawParts->m_DispXSize / 2;
										SizeY = DrawParts->m_DispYSize;
									}
									if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
										PosX = DrawParts->m_DispXSize / 2;
										PosY = y_r(0);
										SizeX = DrawParts->m_DispXSize / 2;
										SizeY = DrawParts->m_DispYSize;
									}
								}
								else {
									if (Input->GetMouseX() < y_r(10)) {
										PosX = y_r(0);
										PosY = (Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2;
										SizeX = DrawParts->m_DispXSize / 2;
										SizeY = DrawParts->m_DispYSize / 2;
									}
									if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
										PosX = DrawParts->m_DispXSize / 2;
										PosY = (Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2;
										SizeX = DrawParts->m_DispXSize / 2;
										SizeY = DrawParts->m_DispYSize / 2;
									}
								}
							}
							IsMoving = false;
						}
					}
					else {
						PosAddX = Input->GetMouseX() - PosX;
						PosAddY = Input->GetMouseY() - PosY;
					}
				}

				if (CanChageSize && IsMoving && Input->GetLeftClick().press()) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 24);
					if (y_r(10) < Input->GetMouseY() && Input->GetMouseY() < DrawParts->m_DispYSize - y_r(10)) {
						if (Input->GetMouseX() < y_r(10)) {
							DrawBox(0, 0, DrawParts->m_DispXSize / 2, DrawParts->m_DispYSize, Black, TRUE);
						}
						if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
							DrawBox(DrawParts->m_DispXSize / 2, 0, DrawParts->m_DispXSize, DrawParts->m_DispYSize, Black, TRUE);
						}
					}
					else {
						if (Input->GetMouseX() < y_r(10)) {
							DrawBox(
								y_r(0),
								(Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2,
								DrawParts->m_DispXSize / 2,
								(Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? DrawParts->m_DispYSize / 2 : DrawParts->m_DispYSize,
								Black, TRUE);
						}
						if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
							DrawBox(
								DrawParts->m_DispXSize / 2,
								(Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2,
								DrawParts->m_DispXSize,
								(Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? DrawParts->m_DispYSize / 2 : DrawParts->m_DispYSize,
								Black, TRUE);
						}
					}
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}
			};
		};
		class WindowManager {
		private:
			std::vector<std::shared_ptr<WindowControl>> m_WindowControl;
		public:
			const auto	PosHitCheck() const noexcept {
				for (auto& w : m_WindowControl) {
					if (w->GetIsEditing()) {
						return true;
					}
				}
				return false;
			}
		public:
			const auto&	Add() noexcept {
				m_WindowControl.emplace_back(std::make_shared<WindowControl>());
				return m_WindowControl.back();
			}
			void		Execute(void) noexcept {
				//ウィンドウアクティブチェック
				if ((m_WindowControl.size() > 1) && !m_WindowControl.back()->GetActiveSwitch()) {
					for (int i = (int)(m_WindowControl.size()) - 2; i >= 0; i--) {
						if (m_WindowControl[i]->GetActiveSwitch()) {
							m_WindowControl.emplace_back(m_WindowControl[i]);
							m_WindowControl.erase(m_WindowControl.begin() + i);
							break;
						}
					}
				}
				if (m_WindowControl.size() > 0) {
					for (int i = 0; i < m_WindowControl.size(); i++) {
						m_WindowControl[i]->SetIsActive(false);
					}
					m_WindowControl.back()->SetIsActive(true);
				}
				//削除チェック
				for (int i = 0; i < m_WindowControl.size(); i++) {
					if (m_WindowControl[i]->isDelete) {
						std::swap(m_WindowControl[i], m_WindowControl.back());
						m_WindowControl.pop_back();
						i--;
					}
				}
			}
			void		Draw(void) noexcept {
				for (auto& w : m_WindowControl) {
					w->Draw();
				}
			}
			void		Dispose(void) noexcept {
				m_WindowControl.clear();
			}
		};
	};

	namespace DrawSystem {
		enum class EnumDrawType : int {
			Write,
			Paint,
			Dropper,
		};
		enum class EnumFilterParamType : int {
			None,
			Num,
			Color,
			CmpType,
			GaussWidth,
			TrueFalse,
		};
		class FilterParamValue {
		public:
			std::string			Name;
			EnumFilterParamType	Type{ 0 };
			long long int		Min{ 0 };
			long long int		Max{ 0 };
			long long int		Param{ 0 };
			std::array<int, 3>	Color{ 0 };
		public:
			void Set_Num(const char* name, int min, int max) {
				this->Name = name;
				this->Type = EnumFilterParamType::Num;
				this->Min = min;
				this->Max = max;
				Param = std::clamp(Param, this->Min, this->Max);
			}
			void Set_Color(const char* name) {
				this->Name = name;
				this->Type = EnumFilterParamType::Color;
				this->Color[0] = 255;
				this->Color[1] = 255;
				this->Color[2] = 255;
				Param = GetColor(this->Color[0], this->Color[1], this->Color[2]);
			}
			void Set_GaussWidth(const char* name) {
				this->Name = name;
				Type = EnumFilterParamType::GaussWidth;
				Param = 8;
			}
			void Set_CmpType(const char* name) {
				this->Name = name;
				Type = EnumFilterParamType::CmpType;
				Param = DX_CMP_LESS;
			}
			void Set_TrueFalse(const char* name) {
				this->Name = name;
				Type = EnumFilterParamType::TrueFalse;
				Param = TRUE;
			}

			void Add() {
				switch (Type) {
				case EnumFilterParamType::None:
					break;
				case EnumFilterParamType::Num:
					Param = std::clamp(Param + 1, this->Min, this->Max);
					break;
				case EnumFilterParamType::Color:
					break;
				case EnumFilterParamType::CmpType:
					switch (Param) {
					case DX_CMP_LESS:
						Param = DX_CMP_GREATER;
						break;
					case DX_CMP_GREATER:
						Param = DX_CMP_LESS;
						break;
					default:
						Param = DX_CMP_LESS;
						break;
					}
					break;
				case EnumFilterParamType::GaussWidth:
					switch (Param) {
					case 8:
						Param = 16;
						break;
					case 16:
						Param = 32;
						break;
					case 32:
						Param = 32;
						break;
					default:
						Param = 8;
						break;
					}
					break;
				case EnumFilterParamType::TrueFalse:
					Param ^= 1;
					break;
				default:
					break;
				}
			}
			void Sub() {
				switch (Type) {
				case EnumFilterParamType::None:
					break;
				case EnumFilterParamType::Num:
					Param = std::clamp(Param - 1, this->Min, this->Max);
					break;
				case EnumFilterParamType::Color:
					break;
				case EnumFilterParamType::CmpType:
					switch (Param) {
					case DX_CMP_LESS:
						Param = DX_CMP_GREATER;
						break;
					case DX_CMP_GREATER:
						Param = DX_CMP_LESS;
						break;
					default:
						Param = DX_CMP_LESS;
						break;
					}
					break;
				case EnumFilterParamType::GaussWidth:
					switch (Param) {
					case 8:
						Param = 8;
						break;
					case 16:
						Param = 8;
						break;
					case 32:
						Param = 16;
						break;
					default:
						Param = 8;
						break;
					}
					break;
				case EnumFilterParamType::TrueFalse:
					Param ^= 1;
					break;
				default:
					break;
				}
			}
		};

		class Canvas {
			class SaveParam {
				GraphHandle		GraphBuf;
			public:
				auto& GetGraphHandle(void) noexcept { return GraphBuf; }
			public:
				void		Set(const GraphHandle& BaseGraph) {
					int x, y;
					BaseGraph.GetSize(&x, &y);
					GraphBuf = GraphHandle::Make(x, y, true);
					GraphBuf.SetDraw_Screen();
					{
						DrawGraph(0, 0, BaseGraph.get(), FALSE);
					}
					SetDrawScreen(DX_SCREEN_BACK);
				}
				void		UndoWrite(GraphHandle* OutGraph) {
					OutGraph->SetDraw_Screen(false);
					{
						GraphBuf.DrawGraph(0, 0, true);
					}
					SetDrawScreen(DX_SCREEN_BACK);
				}
				void		Dispose() {
					GraphBuf.Dispose();
				}
			};
		private:
			int					xsize{ 1024 };
			int					ysize{ 768 };
		private:
			int					xpos{ 0 };
			int					ypos{ 0 };

			int					xposBase{ 0 };
			int					yposBase{ 0 };

			int					xposPrev;
			int					yposPrev;

			int					PenSize{ 1 };
			float				PenSizeChangeTimer = 1.f;

			unsigned int		LeftColor{ GetColor(0, 0, 0) };
			std::vector<unsigned int> ColorList;

			int					imgHandle{ -1 };
			GraphHandle			OutScreen;
			GraphHandle			OutScreen_Before;
			GraphHandle			OutScreen_After;

			float				scale{ 1.f };

			EnumDrawType		m_DrawType{ EnumDrawType::Write };

			bool				isDraw{ true };

			float				CtrlChangeTimer = 1.f;

			DialogManager		m_DialogManager;
			bool				DialogClick{ false };

			bool				IsNewFile{ false };
			std::string			SavePath;

			std::string			FilterName;
			std::array<FilterParamValue, 5>	FilterParam;

			int					FilterType{ 0 };
			bool				FilterChangeSwitch{ false };
			bool				FilterSetSwitch{ false };

			bool				IsWriting{ true };
			int					WriteMinX{ 0 };
			int					WriteMinY{ 0 };
			int					WriteMaxX{ 0 };
			int					WriteMaxY{ 0 };


			std::list<std::unique_ptr<SaveParam>> m_SaveParams;
			std::list<std::unique_ptr<SaveParam>>::iterator NowSaveSelect;
			float				SaveChangeTime{ 0.f };
		public:
			const auto&		GetLeftColor(void) const noexcept { return this->LeftColor; }
			void			SetLeftColor(unsigned int value) noexcept { this->LeftColor = value; }

			const auto&		GetFilterSelect(void) const noexcept { return this->FilterName; }
			auto&			GetFilterParam(void) noexcept { return this->FilterParam; }

			auto&			GetColorList(void) noexcept { return this->ColorList; }
			void			SetIsDraw(bool value) noexcept { this->isDraw = value; }

			void			FilterChange() noexcept { this->FilterChangeSwitch = true; }
			void			FilterSet() noexcept { this->FilterSetSwitch = true; }
		private:
			void SetDo() {
				if (!m_SaveParams.empty()) {
					if (NowSaveSelect != m_SaveParams.end()) {
						m_SaveParams.erase(NowSaveSelect, m_SaveParams.end());
					}
				}
				m_SaveParams.emplace_back(std::make_unique<SaveParam>());
				m_SaveParams.back()->Set(OutScreen_Before);
				NowSaveSelect = m_SaveParams.end();
				OutScreen.SetDraw_Screen(false);
				{
					OutScreen_After.DrawGraph(0, 0, true);
				}
				SetDrawScreen(DX_SCREEN_BACK);
			}
		private:
			void CreatePic() noexcept {
				imgHandle = MakeSoftImage(xsize, ysize);
				OutScreen = GraphHandle::Make(xsize, ysize, true);
				OutScreen_Before = GraphHandle::Make(xsize, ysize, true);
				OutScreen_After = GraphHandle::Make(xsize, ysize, true);
			}
			void DisposePic() noexcept {
				if (imgHandle != -1) {
					DeleteSoftImage(imgHandle);
				}
				OutScreen.Dispose();
				OutScreen_Before.Dispose();
				OutScreen_After.Dispose();
			}
			void InitPic() noexcept {
				SavePath = "新しいファイル";

				DisposePic();
				CreatePic();
				OutScreen.SetDraw_Screen();
				{
					DrawBox(0, 0, xsize, ysize, GetColor(255, 255, 255), TRUE);
				}
				SetDrawScreen(DX_SCREEN_BACK);
				IsNewFile = true;
			}
			void Load() noexcept {
				if (m_DialogManager.Open()) {
					SavePath = m_DialogManager.GetPath();

					GraphHandle	Buffer = GraphHandle::Load(SavePath);
					Buffer.GetSize(&xsize, &ysize);

					DisposePic();
					CreatePic();
					OutScreen.SetDraw_Screen();
					{
						DrawBox(0, 0, xsize, ysize, GetColor(255, 255, 255), TRUE);
						Buffer.DrawGraph(0, 0, false);
					}
					SetDrawScreen(DX_SCREEN_BACK);
					Buffer.Dispose();

					xpos = 0;
					ypos = 0;
					scale = 1.f;
				}
			}
			void Save(void) noexcept {
				if (IsNewFile) {
					if (m_DialogManager.Save()) {
						SavePath = m_DialogManager.GetPath();
					}
				}
				{
					if (SavePath.find(".bmp") != std::string::npos) {
						SaveDrawValidGraphToBMP(OutScreen.get(), 0, 0, xsize, ysize, SavePath.c_str());					// ＢＭＰ形式で保存する
					}
					else if (SavePath.find(".dds") != std::string::npos) {
						SaveDrawValidGraphToDDS(OutScreen.get(), 0, 0, xsize, ysize, SavePath.c_str());					// ＤＤＳ形式で保存する
					}
					else if (SavePath.find(".jpg") != std::string::npos) {
						SaveDrawValidGraphToJPEG(OutScreen.get(), 0, 0, xsize, ysize, SavePath.c_str(), 100, TRUE);		// ＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0～100 
					}
					else if (SavePath.find(".png") != std::string::npos) {
						SaveDrawValidGraphToPNG(OutScreen.get(), 0, 0, xsize, ysize, SavePath.c_str(), 0);				// ＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0～9
					}
					else {
						SaveDrawValidGraphToBMP(OutScreen.get(), 0, 0, xsize, ysize, SavePath.c_str());
					}
				}
			}

			void SetDraw(EnumDrawType DrawType) {
				auto* Input = InputControl::Instance();
				int m_x = std::clamp((int)((float)(Input->GetMouseX() - xpos) / scale), -1, xsize);
				int m_y = std::clamp((int)((float)(Input->GetMouseY() - ypos) / scale), -1, ysize);
				switch (DrawType) {
				case EnumDrawType::Write:
					if (Input->GetLeftClick().press()) {
						if (!IsWriting) {
							OutScreen_Before.SetDraw_Screen(false);
							{
								OutScreen.DrawGraph(0, 0, true);
							}
							OutScreen_After.SetDraw_Screen(false);
							{
								OutScreen_Before.DrawGraph(0, 0, true);
							}
							IsWriting = true;
						}
						if ((in2_(xposPrev, yposPrev, 0, 0, xsize - 1, ysize - 1)) || (in2_(m_x, m_y, 0, 0, xsize - 1, ysize - 1))) {
							OutScreen_After.SetDraw_Screen(false);
							{
								DrawLine(xposPrev, yposPrev, m_x, m_y, LeftColor, PenSize);
								if (PenSize == 1) {
									DrawPixel(xposPrev, yposPrev, LeftColor);
									DrawPixel(m_x, m_y, LeftColor);
								}
								else if (PenSize == 2) {
									DrawBox(xposPrev, yposPrev, xposPrev, yposPrev, LeftColor, TRUE);
									DrawBox(m_x, m_y, m_x, m_y, LeftColor, TRUE);
								}
								else {
									DrawCircle(xposPrev, yposPrev, PenSize / 2, LeftColor);
									DrawCircle(m_x, m_y, (PenSize - 1) / 2, LeftColor);
								}
							}
							WriteMinX = std::min(m_x - PenSize / 2, WriteMinX);
							WriteMinY = std::min(m_y - PenSize / 2, WriteMinY);
							WriteMaxX = std::max(m_x + PenSize / 2 + 1, WriteMaxX);
							WriteMaxY = std::max(m_y + PenSize / 2 + 1, WriteMaxY);
						}
						if (Input->GetRightClick().trigger()) {//リセット
							OutScreen_After.SetDraw_Screen(false);
							{
								OutScreen_Before.DrawGraph(0, 0, true);
							}
							WriteMinX = m_x;
							WriteMinY = m_y;
							WriteMaxX = m_x;
							WriteMaxY = m_y;
						}
					}
					else {
						if (IsWriting) {
							IsWriting = false;
							SetDo();
						}
						WriteMinX = m_x;
						WriteMinY = m_y;
						WriteMaxX = m_x;
						WriteMaxY = m_y;
					}
					xposPrev = m_x;
					yposPrev = m_y;
					break;
				case EnumDrawType::Paint:
					if ((in2_(m_x, m_y, 0, 0, xsize - 1, ysize - 1)) && Input->GetLeftClick().trigger()) {
						OutScreen_Before.SetDraw_Screen(false);
						{
							OutScreen.DrawGraph(0, 0, true);
						}
						OutScreen_After.SetDraw_Screen(false);
						{
							OutScreen_Before.DrawGraph(0, 0, true);
							Paint(m_x, m_y, LeftColor);
						}
						SetDo();
					}
					break;
				case EnumDrawType::Dropper:
					if ((in2_(m_x, m_y, 0, 0, xsize - 1, ysize - 1)) && Input->GetLeftClick().trigger()) {
						OutScreen.SetDraw_Screen(false);
						{
							GetDrawScreenSoftImage(0, 0, xsize, ysize, imgHandle);
						}
						int r, g, b, a;
						GetPixelSoftImage(imgHandle, m_x, m_y, &r, &g, &b, &a);
						LeftColor = GetColor(r, g, b);
					}
					break;
				default:
					break;
				}
			}

			void SetFilterParamData(int FilterSet) {
				for (auto& p : FilterParam) {
					p.Type = EnumFilterParamType::None;
					p.Param = 0;
					p.Min = 0;
					p.Max = 0;
				}
				switch (FilterSet) {
				case DX_GRAPH_FILTER_MONO:
					FilterName = "モノトーンフィルタ";
					FilterParam[0].Set_Num("青色差", -255, 255);
					FilterParam[1].Set_Num("赤色差", -255, 255);
					break;
				case DX_GRAPH_FILTER_GAUSS:
					FilterName = "ガウスフィルタ";
					FilterParam[0].Set_GaussWidth("使用幅");
					FilterParam[1].Set_Num("ぼかし", 0, 2000);
					break;
				case DX_GRAPH_FILTER_BRIGHT_CLIP:
					FilterName = "明るさクリップフィルタ";
					FilterParam[0].Set_CmpType("クリップタイプ");
					FilterParam[1].Set_Num("クリップしきい値", 0, 255);
					FilterParam[2].Set_TrueFalse("クリップしたピクセルを塗りつぶすかどうか");
					FilterParam[3].Set_Color("塗る色");
					FilterParam[4].Set_Num("塗るアルファ", 0, 255);
					break;
				case DX_GRAPH_FILTER_HSB:
					FilterName = "色相,彩度,明度フィルタ";
					FilterParam[0].Set_TrueFalse("色相が絶対値(0～360)か否(-180～180)か");
					FilterParam[1].Set_Num("色相の絶対値か元の色相に対する相対値", -180, 360);
					FilterParam[2].Set_Num("彩度", -255, 1024);
					FilterParam[3].Set_Num("輝度", -255, 255);
					break;
				case DX_GRAPH_FILTER_INVERT:
					FilterName = "反転フィルタ";
					break;
				case DX_GRAPH_FILTER_LEVEL:
					FilterName = "レベル補正フィルタ";
					FilterParam[0].Set_Num("変換元の下限値", 0, 255);
					FilterParam[1].Set_Num("変換元の上限値", 0, 255);
					FilterParam[2].Set_Num("ガンマ値x100", 100, 1000);
					FilterParam[3].Set_Num("変換後の最低値", 0, 255);
					FilterParam[4].Set_Num("変換後の最大値", 0, 255);
					break;
				case DX_GRAPH_FILTER_TWO_COLOR:
					FilterName = "２階調化フィルタ";
					FilterParam[0].Set_Num("閾値", 0, 255);
					FilterParam[1].Set_Color("閾値より値が低かった場合の色");
					FilterParam[2].Set_Num("閾値より値が低かった場合のアルファ", 0, 255);
					FilterParam[3].Set_Color("閾値より値が高かった場合の色");
					FilterParam[4].Set_Num("閾値より値が高かった場合のアルファ", 0, 255);
					break;
				default:
					FilterName = "フィルタ無し";
					break;
				}
			}
		public:
			void ChangeSize(int x_size, int y_size) {
				xsize = x_size;
				ysize = y_size;

				GraphHandle	Buffer = GraphHandle::Make(xsize, ysize, true);
				Buffer.SetDraw_Screen();
				{
					OutScreen.DrawGraph(0, 0, true);
				}
				DisposePic();
				CreatePic();
				OutScreen.SetDraw_Screen();
				{
					Buffer.DrawGraph(0, 0, false);
				}
				SetDrawScreen(DX_SCREEN_BACK);
				Buffer.Dispose();

			}
			const auto&		GetXsize(void) const noexcept { return this->xsize; }
			const auto&		GetYsize(void) const noexcept { return this->ysize; }
		public:
			void Set(void) noexcept {
				m_DialogManager.Init();

				InitPic();

				LeftColor = GetColor(0, 0, 0);
				IsWriting = false;

				SetFilterParamData(-1);
			}
			void Execute(void) noexcept {
				auto* Input = InputControl::Instance();

				if (Input->GetRightClick().press()) {
					xpos = Input->GetMouseX() - xposBase;
					ypos = Input->GetMouseY() - yposBase;
				}
				else {
					xposBase = Input->GetMouseX() - xpos;
					yposBase = Input->GetMouseY() - ypos;
				}

				if (Input->GetWheelAdd() != 0) {
					if (Input->GetMiddleClick().press()) {
						if (!IsWriting) {
							PenSize = std::clamp(PenSize + Input->GetWheelAdd(), 1, 100);
							PenSizeChangeTimer = 1.f;
						}
					}
					else {
						auto PrevScale = scale;
						scale = std::clamp(scale + (float)Input->GetWheelAdd() / 10.f, 0.1f, 100.f);
						xpos -= (int)((float)(Input->GetMouseX() - xpos) * (scale - PrevScale) / scale);
						ypos -= (int)((float)(Input->GetMouseY() - ypos) * (scale - PrevScale) / scale);
					}
				}

				if (!DialogClick) {
					if (Input->GetCtrlKey().press()) {
						if (Input->GetKey('S').trigger()) {
							Save();
							DialogClick = true;
						}
						if (Input->GetKey('O').trigger()) {
							Load();
							DialogClick = true;
						}
						if (Input->GetKey('Z').trigger()) {
							if (!m_SaveParams.empty()) {
								if (NowSaveSelect != m_SaveParams.begin()) {
									NowSaveSelect--;
									(*NowSaveSelect)->UndoWrite(&OutScreen);
									SaveChangeTime = 2.f;
								}
							}
						}
						if (Input->GetKey('Y').trigger()) {
							if (!m_SaveParams.empty()) {
								if (NowSaveSelect != m_SaveParams.end()) {
									NowSaveSelect++;
									if (NowSaveSelect != m_SaveParams.end()) {
										(*NowSaveSelect)->UndoWrite(&OutScreen);
									}
									else {
										OutScreen.SetDraw_Screen(false);
										{
											OutScreen_After.DrawGraph(0, 0, true);
										}
										SetDrawScreen(DX_SCREEN_BACK);
									}
									SaveChangeTime = 2.f;
								}
							}
						}
					}
				}
				else {
					if (CheckHitKeyAll()) {
						this->isDraw = false;
					}
					else {
						DialogClick = false;
					}
				}
				SaveChangeTime = std::clamp(SaveChangeTime - 1.f / FPS, 0.f, 2.f);


				if (!Input->GetLeftClick().press()) {
					m_DrawType = EnumDrawType::Write;
					if (Input->GetShiftKey().press()) {
						m_DrawType = EnumDrawType::Paint;
					}
					if (Input->GetSpaceKey().press()) {
						m_DrawType = EnumDrawType::Dropper;
					}
				}
				if (this->isDraw) {
					SetDraw(m_DrawType);
				}

				if (FilterSetSwitch) {
					OutScreen_Before.SetDraw_Screen(false);
					{
						OutScreen.DrawGraph(0, 0, true);
					}
					SetDo();
					FilterType = 0;
				}
				FilterSetSwitch = false;

				PenSizeChangeTimer = std::max(PenSizeChangeTimer - 1.f / FPS, 0.f);
				Easing(&CtrlChangeTimer, (Input->GetCtrlKey().press() ? 1.f : 0.f), 0.8f, EasingType::OutExpo);

			}
			void Draw(void) noexcept {
				auto* Input = InputControl::Instance();
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();

				auto prevmode = GetDrawMode();
				if (scale < 1.f) {
					SetDrawMode(DX_DRAWMODE_BILINEAR);
				}
				else {
					SetDrawMode(DX_DRAWMODE_NEAREST);
				}
				{
					int FILTERS[8] = {
						-1,
						DX_GRAPH_FILTER_MONO,
						DX_GRAPH_FILTER_GAUSS,
						DX_GRAPH_FILTER_BRIGHT_CLIP,
						DX_GRAPH_FILTER_HSB,
						DX_GRAPH_FILTER_INVERT,
						DX_GRAPH_FILTER_LEVEL,
						DX_GRAPH_FILTER_TWO_COLOR,
					};
					if (FilterChangeSwitch) {
						++FilterType %= 8;
						SetFilterParamData(FILTERS[FilterType]);
					}
					FilterChangeSwitch = false;
					if (FILTERS[FilterType] != -1) {
						GraphFilterBlt(OutScreen.get(), OutScreen_After.get(), FILTERS[FilterType], FilterParam[0].Param, FilterParam[1].Param, FilterParam[2].Param, FilterParam[3].Param, FilterParam[4].Param);
						OutScreen_After.DrawExtendGraph(y_r(xpos), y_r(ypos), y_r(xpos + (int)((float)xsize * scale)), y_r(ypos + (int)((float)ysize * scale)), true);
					}
					else {
						OutScreen.DrawExtendGraph(y_r(xpos), y_r(ypos), y_r(xpos + (int)((float)xsize * scale)), y_r(ypos + (int)((float)ysize * scale)), true);
						if (IsWriting) {
							OutScreen_After.DrawExtendGraph(y_r(xpos), y_r(ypos), y_r(xpos + (int)((float)xsize * scale)), y_r(ypos + (int)((float)ysize * scale)), true);
						}
					}
				}
				SetDrawMode(prevmode);

				int r, g, b;
				GetColor2(LeftColor, &r, &g, &b);
				unsigned int backcolor = GetColor(255 - r, 255 - g, 255 - b);

				//普通のガイド
				if (CtrlChangeTimer < 0.95f) {
					int Posx = Input->GetMouseX();
					int Posy = Input->GetMouseY();
					int Size = (int)((float)LineHeight*(1.f - CtrlChangeTimer));

					switch (m_DrawType) {
					case EnumDrawType::Write:
						DrawCircle(Posx, Posy, (int)((float)PenSize / 2.f*scale), backcolor, FALSE, 3);
						DrawCircle(Posx, Posy, (int)((float)PenSize / 2.f*scale), LeftColor, FALSE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy, LeftColor, backcolor, " ペン");
						if (Input->GetMiddleClick().press() || (PenSizeChangeTimer > 0)) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy + Size, LeftColor, backcolor, "太さ:%d", PenSize);
						}
						break;
					case EnumDrawType::Paint:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy, LeftColor, backcolor, " バケツ");
						break;
					case EnumDrawType::Dropper:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy, LeftColor, backcolor, " スポイト");
						break;
					default:
						break;
					}

				}
				//ctrlガイド
				if (CtrlChangeTimer > 0.05f) {
					int Posx = Input->GetMouseX();
					int Posy = Input->GetMouseY();
					int Size = (int)((float)LineHeight*CtrlChangeTimer);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 3, LeftColor, backcolor, "戻る:Z ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 2, LeftColor, backcolor, "やり直す:Y ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 1, LeftColor, backcolor, "開く:O ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 0, LeftColor, backcolor, "保存:S ");
				}
				//場所ガイド
				{
					int xp = y_r(1440);
					int yp = y_r(820);

					int xs = y_r(320);
					int ys = y_r(180);

					int x_p1 = std::max(xp + y_r(xpos) * xs / DrawParts->m_DispXSize, xp - xs / 2);
					int y_p1 = std::max(yp + y_r(ypos) * ys / DrawParts->m_DispYSize, yp - ys / 2);
					int x_p2 = std::min(xp + y_r(xpos + (int)((float)xsize * scale)) * xs / DrawParts->m_DispXSize, xp + xs + xs / 2);
					int y_p2 = std::min(yp + y_r(ypos + (int)((float)ysize * scale)) * ys / DrawParts->m_DispYSize, yp + ys + ys / 2);

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
					DrawBox(x_p1, y_p1, x_p2, y_p2, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					DrawBox(x_p1, y_p1, x_p2, y_p2, GetColor(0, 200, 0), FALSE);
					DrawBox(xp, yp, xp + xs, yp + ys, Red, FALSE);
				}
				//ファイル名
				{
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(12), y_r(12), LeftColor, backcolor, "[%s]", SavePath.c_str());
				}
				//
				if (SaveChangeTime > 0.f) {
					int xp, yp, xs, ys;

					xs = y_r(300);
					ys = y_r(300)*ysize / xsize;


					xp = y_r(10);
					yp = DrawParts->m_DispYSize / 2 - ys / 2;
					{
						auto Itr = NowSaveSelect;
						if (Itr != m_SaveParams.begin()) {
							for (int i = 1; i < 5; i++) {
								Itr--;
								(*Itr)->GetGraphHandle().DrawExtendGraph(xp, yp - (ys + y_r(5)) * i, xp + xs, yp + ys - (ys + y_r(5)) * i, true);
								DrawBox(xp, yp - (ys + y_r(5)) * i, xp + xs, yp + ys - (ys + y_r(5)) * i, Green, FALSE);
								if (Itr == m_SaveParams.begin()) { break; }
							}
						}
					}
					xp = y_r(110);
					{
						auto Itr = NowSaveSelect;
						for (int i = 0; i < 5; i++) {
							if (Itr == m_SaveParams.end()) { break; }
							(*Itr)->GetGraphHandle().DrawExtendGraph(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, true);
							DrawBox(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, Green, FALSE);
							Itr++;
							xp = y_r(10);
						}
					}
				}
				
				printfDx("左クリック　塗る\n");
				printfDx("　何も押さない      ペン\n");
				printfDx("　　左クリック中に右クリックでリセット\n");
				printfDx("　LShift押しながら  バケツ\n");
				printfDx("　Space押しながら   スポイト\n");
				printfDx("左クリック　視点移動\n");
				printfDx("何も押さずマウスホイール　ズーム\n");
				printfDx("ホイールを押しながらマウスホイール　ペンの太さチェンジ\n");

				printfDx("Lctrl ＋ O  ファイルを開く\n");
				printfDx("Lctrl ＋ S  ファイルを保存\n");
				printfDx("Lctrl ＋ Z  元に戻る\n");
				printfDx("Lctrl ＋ Y  やり直す\n");
			}
		};
	};

	class MAINLOOP : public TEMPSCENE {
	private:
		std::unique_ptr<WindowSystem::WindowManager> m_Window;
		std::unique_ptr<DrawSystem::Canvas> m_Canvas;
		int size_buf[2];
	public:
		void Load_Sub(void) noexcept override {
			//サウンド
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::UI_OK, 1, "data/Sound/UI/hit.wav");
			SE->Add((int)SoundEnum::UI_CANCEL, 1, "data/Sound/UI/cancel.wav");
			SE->Add((int)SoundEnum::UI_SELECT, 3, "data/Sound/SE/UI/cursor.wav");
		}

		void Set_Sub(void) noexcept override {
			//サウンド
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::UI_OK).SetVol_Local(128);
			SE->Get((int)SoundEnum::UI_CANCEL).SetVol_Local(128);
			SE->Get((int)SoundEnum::UI_SELECT).SetVol_Local(128);
			//
			InputControl::Create();
			m_Window = std::make_unique<WindowSystem::WindowManager>();
			m_Canvas = std::make_unique<DrawSystem::Canvas>();

			m_Canvas->Set();
		}

		bool Update_Sub(void) noexcept override {
			bool isend = false;
			//FirstDoing
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
			}
			auto* Input = InputControl::Instance();
			Input->Execute();

			if (GetIsFirstLoop()) {
				m_Window->Add()->Set(y_r(1460), y_r(450), y_r(10 + 255 + 10), y_r(10 + 255 + 10), 0, "カラーパレット", false, [&](WindowSystem::WindowControl*win) {
					int xs = y_r(255);
					int xp = win->GetPosX() + y_r(10);
					int yp = win->GetPosY() + LineHeight + y_r(10);
					int height = y_r(24);

					unsigned int Color = m_Canvas->GetLeftColor();
					int R, G, B;
					GetColor2(Color, &R, &G, &B);
					//ここでセット

					WindowSystem::SetBox(xp, yp, xp + xs, yp + LineHeight, Color);
					yp += height * 2;
					WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "R : %4d", R);
					if (WindowSystem::ClickCheckBox(xp + y_r(150), yp, xp + y_r(200), yp + height, true, win->GetIsActive(), Gray25, "+")) {
						R = std::clamp(R + 1, 0, 255);
					}
					if (WindowSystem::ClickCheckBox(xp + y_r(205), yp, xp + xs, yp + height, true, win->GetIsActive(), Gray25, "-")) {
						R = std::clamp(R - 1, 0, 255);
					}
					yp += height + y_r(4);
					WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "G : %4d", G);
					if (WindowSystem::ClickCheckBox(xp + y_r(150), yp, xp + y_r(200), yp + height, true, win->GetIsActive(), Gray25, "+")) {
						G = std::clamp(G + 1, 0, 255);
					}
					if (WindowSystem::ClickCheckBox(xp + y_r(205), yp, xp + xs, yp + height, true, win->GetIsActive(), Gray25, "-")) {
						G = std::clamp(G - 1, 0, 255);
					}
					yp += height + y_r(4);
					WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "B : %4d", B);
					if (WindowSystem::ClickCheckBox(xp + y_r(150), yp, xp + y_r(200), yp + height, true, win->GetIsActive(), Gray25, "+")) {
						B = std::clamp(B + 1, 0, 255);
					}
					if (WindowSystem::ClickCheckBox(xp + y_r(205), yp, xp + xs, yp + height, true, win->GetIsActive(), Gray25, "-")) {
						B = std::clamp(B - 1, 0, 255);
					}
					m_Canvas->SetLeftColor(GetColor(R, G, B));

					yp += height * 3 / 2;
					if (WindowSystem::ClickCheckBox(xp + y_r(150) + 1, yp, xp + y_r(250) - 1, yp + height, false, win->GetIsActive(), Gray25, "ADD")) {
						m_Canvas->GetColorList().emplace_back(Color);
					}
					yp += height * 3 / 2;
					int cnt = 0;
					for (auto& CL : m_Canvas->GetColorList()) {
						int x_a = cnt % 10;
						int y_a = cnt / 10;
						if (WindowSystem::ClickCheckBox(xp + xs * x_a / 10 + 1, yp + height * y_a + 1, xp + xs * (x_a + 1) / 10 - 1, yp + height * (y_a + 1) - 1, false, win->GetIsActive(), CL, "")) {
							m_Canvas->SetLeftColor(CL);
						}
						cnt++;
					}
				});

				m_Window->Add()->Set(y_r(1400), y_r(50), y_r(10 + 384 + 10), y_r(10 + 320 + 10), 0, "フィルターセッティング", false, [&](WindowSystem::WindowControl*win) {
					int xs = y_r(384);
					int xp = win->GetPosX() + y_r(10);
					int yp = win->GetPosY() + LineHeight + y_r(10);
					int height = y_r(24);

					WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, m_Canvas->GetFilterSelect().c_str());

					if (WindowSystem::ClickCheckBox(xp + y_r(304), yp, xp + xs, yp + height, false, win->GetIsActive(), Gray25, "変更")) {
						m_Canvas->FilterChange();
					}

					yp += height + y_r(4);

					for (auto& p : m_Canvas->GetFilterParam()) {
						switch (p.Type) {
						case DrawSystem::EnumFilterParamType::Num:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Value:%4d", p.Param);
							break;
						case DrawSystem::EnumFilterParamType::Color:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height * 3 / 4, FontHandle::FontXCenter::LEFT, "Color:%3d,%3d,%3d", p.Color[0], p.Color[1], p.Color[2]);
							WindowSystem::SetBox(xp + y_r(190), yp, xp + y_r(230), yp + height, GetColor(p.Color[0], p.Color[1], p.Color[2]));
							break;
						case DrawSystem::EnumFilterParamType::CmpType:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Cmp  :%s", (p.Param == DX_CMP_LESS) ? "LESS" : "GREATER");
							break;
						case DrawSystem::EnumFilterParamType::GaussWidth:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Gauss:%4d", p.Param);
							break;
						case DrawSystem::EnumFilterParamType::TrueFalse:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Bool :%s", (p.Param == TRUE) ? "TRUE" : "FALSE");
							break;
						default:
							continue;
						}
						WindowSystem::SetMsg(xp, yp + height, xp + xs, yp + height + height, height * 3 / 4, FontHandle::FontXCenter::LEFT, "%s", p.Name.c_str());
						//ボタン
						switch (p.Type) {
						case DrawSystem::EnumFilterParamType::Color:
							for (int i = 0; i < 3; i++) {
								if (WindowSystem::ClickCheckBox(xp + y_r((384 - 20 - 4 - 20) - (2 - i) * 50), yp, xp + y_r((384 - 20 - 4) - (2 - i) * 50), yp + height, true, win->GetIsActive(), Gray25, "+")) {
									++p.Color[i] %= 255;
								}
								if (WindowSystem::ClickCheckBox(xp + y_r((384 - 20) - (2 - i) * 50), yp, xp + y_r(384 - (2 - i) * 50), yp + height, true, win->GetIsActive(), Gray25, "-")) {
									--p.Color[i]; if (p.Color[0] < 0) { p.Color[0] = 255; }
								}
							}
							break;
						case DrawSystem::EnumFilterParamType::Num:
						case DrawSystem::EnumFilterParamType::CmpType:
						case DrawSystem::EnumFilterParamType::GaussWidth:
						case DrawSystem::EnumFilterParamType::TrueFalse:
							if (WindowSystem::ClickCheckBox(xp + y_r(384 - 60 - 4 - 60), yp, xp + y_r(384 - 60 - 4), yp + height, true, win->GetIsActive(), Gray25, "+")) {
								p.Add();
							}
							if (WindowSystem::ClickCheckBox(xp + y_r(384 - 60), yp, xp + xs, yp + height, true, win->GetIsActive(), Gray25, "-")) {
								p.Sub();
							}
							break;
						default:
							continue;
						}
						yp += height * 2 + y_r(4);
					}

					if (WindowSystem::ClickCheckBox(xp, yp, xp + xs, yp + height, false, win->GetIsActive(), Gray25, "このフィルターを適応")) {
						m_Canvas->FilterSet();
					}
				});

				size_buf[0] = m_Canvas->GetXsize();
				size_buf[1] = m_Canvas->GetYsize();
				m_Window->Add()->Set(y_r(1100), y_r(50), y_r(10 + 255 + 10), y_r(10 + 120 + 10), 0, "サイズ変更", false, [&](WindowSystem::WindowControl*win) {
					int xs = y_r(255);
					int xp = win->GetPosX() + y_r(10);
					int yp = win->GetPosY() + LineHeight + y_r(10);
					int height = y_r(24);

					for (int i = 0; i < 2; i++) {
						WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "%s:%4d", (i == 0) ? "X" : "Y", size_buf[i]);
						//ボタン
						if (WindowSystem::ClickCheckBox(xp + y_r(255 - 60 - 4 - 60), yp, xp + y_r(255 - 60 - 4), yp + height, true, win->GetIsActive(), Gray25, "+")) {
							size_buf[i] += 1;
						}
						if (WindowSystem::ClickCheckBox(xp + y_r(255 - 60), yp, xp + xs, yp + height, true, win->GetIsActive(), Gray25, "-")) {
							size_buf[i] = std::max(size_buf[i] - 1, 1);
						}
						yp += height + y_r(4);
					}

					if (WindowSystem::ClickCheckBox(xp, yp, xp + xs, yp + height, false, win->GetIsActive(), Gray25, "サイズ変更")) {
						m_Canvas->ChangeSize(size_buf[0], size_buf[1]);
					}
				});
			}
			m_Canvas->SetIsDraw(!m_Window->PosHitCheck());

			m_Canvas->Execute();
			m_Window->Execute();
			return !isend;
		}
		void Dispose_Sub(void) noexcept override {
			m_Window.reset();
			m_Canvas.reset();
		}
	public:
		void BG_Draw_Sub(void) noexcept override {}
		//UI表示
		void DrawUI_Base_Sub(void) noexcept  override {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispXSize, GetColor(218, 218, 218), TRUE);

			m_Canvas->Draw();

			m_Window->Draw();
		}
	};
};
