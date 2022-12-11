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
			bool		m_IsChangeScrollY{ false };
			int			m_BaseScrollY{ 0 };
			float		m_NowScrollYPer{ 0.f };
		public:
			const auto&		GetNowScrollYPer(void) const noexcept { return this->m_NowScrollYPer; }
			void			ScrollBox(int xp1, int yp1, int xp2, int yp2, float TotalPer, bool IsActive) {
				auto* Input = InputControl::Instance();
				unsigned int color = Gray25;

				int length = (int)((float)(yp2 - yp1) / TotalPer);
				float Total = (float)(yp2 - yp1 - length);
				int Yp_t = (int)(Total * m_NowScrollYPer);
				int Yp_s = std::max(yp1, yp1 + Yp_t);
				int Yp_e = std::min(yp2, Yp_s + length);

				if (IsActive) {
					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
						if (Input->GetLeftClick().trigger()) {
							m_IsChangeScrollY = true;
						}
					}
					if (m_IsChangeScrollY) {
						if (Input->GetLeftClick().press()) {
							color = White;
							m_NowScrollYPer = std::clamp((float)(Input->GetMouseY() - m_BaseScrollY) / Total, 0.f, 1.f);
						}
						else {
							m_IsChangeScrollY = false;
						}
					}
					else {
						m_BaseScrollY = Input->GetMouseY() - Yp_t;
						if (Input->GetMouseY() < Yp_s) {
							m_BaseScrollY += Yp_s - Input->GetMouseY();
						}

						if (Input->GetMouseY() > Yp_e) {
							m_BaseScrollY += Yp_e - Input->GetMouseY();
						}
					}
				}
				SetBox(xp1, yp1, xp2, yp2, Gray50);
				SetBox(xp1 + y_r(1), Yp_s, xp2 - y_r(1), Yp_e, color);
			};
		};
		class WindowControl {
		public:
			bool				m_isDelete{ false };
		private:
			bool				m_ActiveSwitch{ false };
			bool				m_IsActive{ false };
			int					m_PosX{ 0 }, m_PosY{ 0 };
			int					m_SizeX{ 100 }, m_SizeY{ 100 };
			int					m_TotalSizeY{ 200 };
			std::string			m_TabName;
			std::function<void(WindowControl*)> m_DoingOnWindow;

			ScrollBoxClass		m_Scroll;
		private:
			bool				m_isMaxSize{ false };
			int					m_PosXSave{ 0 }, m_PosYSave{ 0 };
			int					m_SizeXSave{ 100 }, m_SizeYSave{ 100 };

			bool				m_CanChageSize{ false };

			bool				m_IsMoving{ false };
			int					m_PosAddX{ 0 }, m_PosAddY{ 0 };

			bool				m_IsChangeScaleXY{ false };
			bool				m_IsChangeScale1X{ false };
			bool				m_IsChangeScale1Y{ false };
			bool				m_IsChangeScale2X{ false };
			bool				m_IsChangeScale2Y{ false };
			int					m_BaseScaleX{ 0 }, m_BaseScaleY{ 0 };
			int					m_BasePos1X{ 0 }, m_BasePos1Y{ 0 };
			int					m_BaseScale1X{ 0 }, m_BaseScale1Y{ 0 };
			int					m_BaseScale2X{ 0 }, m_BaseScale2Y{ 0 };
		public:
			const auto		GetIsEditing(void) const noexcept {
				auto* Input = InputControl::Instance();
				int xp1 = m_PosX;
				int yp1 = m_PosY;
				int xp2 = m_PosX + m_SizeX;
				int yp2 = m_PosY + m_SizeY;
				return
					in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2) ||
					this->m_CanChageSize ||
					this->m_IsMoving ||
					this->m_IsChangeScaleXY ||
					this->m_IsChangeScale1X ||
					this->m_IsChangeScale1Y ||
					this->m_IsChangeScale2X ||
					this->m_IsChangeScale2Y;
			}

			const auto&		GetPosX(void) const noexcept { return this->m_PosX; }
			const auto&		GetPosY(void) const noexcept { return this->m_PosY; }
			const auto&		GetSizeX(void) const noexcept { return this->m_SizeX; }
			const auto&		GetSizeY(void) const noexcept { return this->m_SizeY; }

			const auto&		GetActiveSwitch(void) const noexcept { return this->m_ActiveSwitch; }
			void			SetIsActive(bool value) noexcept { m_IsActive = value; }
			const auto&		GetIsActive(void) const noexcept { return this->m_IsActive; }

			void			SetTotalSizeY(bool value) noexcept { this->m_TotalSizeY = value; }
			const auto&		GetNowScrollPer(void) const noexcept { return this->m_Scroll.GetNowScrollYPer(); }
		public:
			void Set(int posx, int posy, int sizex, int sizey, int Totalsizey, const char* tabName, bool canChageSize, const std::function<void(WindowControl*)>& DoingOnWindow) noexcept {
				this->m_PosX = posx;
				this->m_PosY = posy;


				this->m_SizeX = sizex;
				this->m_SizeY = LineHeight + sizey;
				this->m_TotalSizeY = Totalsizey;
				this->m_TabName = tabName;

				int widthLimit = SetMsg(0, 0, 0, 0 + LineHeight, LineHeight - y_r(6), FontHandle::FontXCenter::LEFT, m_TabName) + EdgeSize * 2 + LineHeight * 2;
				this->m_SizeX = std::max(this->m_SizeX, widthLimit);

				this->m_CanChageSize = canChageSize;
				this->m_DoingOnWindow = DoingOnWindow;
			}
			void Draw(void) noexcept {
				auto* DrawParts = DXDraw::Instance();
				auto* Input = InputControl::Instance();
				int xp1 = m_PosX;
				int yp1 = m_PosY;
				int xp2 = m_PosX + m_SizeX;
				int yp2 = m_PosY + m_SizeY;
				int widthLimit = 0;
				//背景
				{
					int add = y_r(2);
					DrawBox(xp1 + add, yp1 + add, xp2 + add, yp2 + add, Black, TRUE);
					SetBox(xp1, yp1, xp2, yp2, Gray10);
				}

				m_ActiveSwitch = false;
				if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
					if (Input->GetLeftClick().trigger()) {
						m_ActiveSwitch = true;
					}
				}
				//内容
				m_DoingOnWindow(this);
				//スクロールバー
				{
					float Total = (float)this->m_TotalSizeY / (m_SizeY - LineHeight);
					if (Total > 1.f) {
						m_Scroll.ScrollBox(xp2 - y_r(24), yp1 + LineHeight, xp2, yp2, Total, m_IsActive);
					}
				}
				//タブ
				{
					int xp3 = m_PosX + m_SizeX - LineHeight - LineHeight;
					unsigned int color = Gray25;
					SetBox(xp1, yp1, xp2, yp1 + LineHeight, color);
					widthLimit = SetMsg(xp1, yp1, xp3, yp1 + LineHeight, LineHeight - y_r(6), FontHandle::FontXCenter::LEFT, m_TabName) + EdgeSize * 2 + LineHeight * 2;
				}
				//最大化ボタン
				if (m_CanChageSize) {
					int xp3 = m_PosX + m_SizeX - LineHeight - LineHeight;
					int yp3 = m_PosY + EdgeSize;
					int xp4 = m_PosX + m_SizeX - LineHeight - EdgeSize;
					int yp4 = m_PosY + LineHeight - EdgeSize;
					unsigned int color = Gray25;

					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize)) {
						color = White;
						if (Input->GetLeftClick().trigger()) {
							this->m_isMaxSize ^= 1;
							if (this->m_isMaxSize) {
								this->m_PosXSave = this->m_PosX;
								this->m_PosYSave = this->m_PosY;
								this->m_SizeXSave = this->m_SizeX;
								this->m_SizeYSave = this->m_SizeY;

								this->m_PosX = y_r(0);
								this->m_PosY = y_r(0);
								this->m_SizeX = DrawParts->m_DispXSize;
								this->m_SizeY = DrawParts->m_DispYSize;
							}
							else {
								this->m_PosX = this->m_PosXSave;
								this->m_PosY = this->m_PosYSave;
								this->m_SizeX = this->m_SizeXSave;
								this->m_SizeY = this->m_SizeYSave;
							}
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}
					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, !this->m_isMaxSize ? "□" : "ﾛ");
				}
				//×ボタン
				{
					int xp3 = this->m_PosX + this->m_SizeX - LineHeight;
					int yp3 = this->m_PosY + EdgeSize;
					int xp4 = this->m_PosX + this->m_SizeX - EdgeSize;
					int yp4 = this->m_PosY + LineHeight - EdgeSize;
					unsigned int color = Red25;

					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize)) {
						color = Red;
						if (Input->GetLeftClick().trigger()) {
							//color = Red50;
							this->m_isDelete = true;
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
						SetCursor(hCursor);
					}

					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, "X");
				}
				if (this->m_CanChageSize && !this->m_isMaxSize && this->m_IsActive) {
					//xyサイズ
					{
						unsigned int color = Gray25;
						auto radius = y_r(3);
						if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp2 - EdgeSize - radius, yp2 - EdgeSize - radius, xp2 - EdgeSize + radius, yp2 - EdgeSize + radius)) {
							color = White;
							if (Input->GetLeftClick().trigger()) {
								this->m_IsChangeScaleXY = true;
							}
						}
						if (this->m_IsChangeScaleXY) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								this->m_SizeX = std::max((Input->GetMouseX() - this->m_BaseScaleX) - this->m_PosX, widthLimit);
								this->m_SizeY = std::max((Input->GetMouseY() - this->m_BaseScaleY) - this->m_PosY, LineHeight + y_r(10));
							}
							else {
								this->m_IsChangeScaleXY = false;
							}
						}
						else {
							this->m_BaseScaleX = Input->GetMouseX() - (this->m_PosX + this->m_SizeX);
							this->m_BaseScaleY = Input->GetMouseY() - (this->m_PosY + this->m_SizeY);
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
								this->m_IsChangeScale2Y = true;
							}
						}
						if (this->m_IsChangeScale2Y) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								this->m_SizeY = std::max((Input->GetMouseY() - this->m_BaseScale2Y) - this->m_PosY, LineHeight + y_r(10));
							}
							else {
								this->m_IsChangeScale2Y = false;
							}
						}
						else {
							this->m_BaseScale2Y = Input->GetMouseY() - (this->m_PosY + this->m_SizeY);
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
								this->m_IsChangeScale2X = true;
							}
						}
						if (this->m_IsChangeScale2X) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								this->m_SizeX = std::max((Input->GetMouseX() - this->m_BaseScale2X) - this->m_PosX, widthLimit);
							}
							else {
								this->m_IsChangeScale2X = false;
							}
						}
						else {
							this->m_BaseScale2X = Input->GetMouseX() - (this->m_PosX + this->m_SizeX);
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
								this->m_IsChangeScale1Y = true;
							}
						}
						if (this->m_IsChangeScale1Y) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								this->m_PosY = std::min((Input->GetMouseY() - this->m_BasePos1Y), this->m_BaseScale1Y - (LineHeight + y_r(10)));
								this->m_SizeY = this->m_BaseScale1Y - this->m_PosY;
							}
							else {
								this->m_IsChangeScale1Y = false;
							}
						}
						else {
							this->m_BasePos1Y = Input->GetMouseY() - this->m_PosY;
							this->m_BaseScale1Y = this->m_PosY + this->m_SizeY;
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
								this->m_IsChangeScale1X = true;
							}
						}
						if (this->m_IsChangeScale1X) {
							if (Input->GetLeftClick().press()) {
								color = Gray50;
								this->m_PosX = std::min((Input->GetMouseX() - this->m_BasePos1X), this->m_BaseScale1X - widthLimit);
								this->m_SizeX = this->m_BaseScale1X - this->m_PosX;
							}
							else {
								this->m_IsChangeScale1X = false;
							}
						}
						else {
							this->m_BasePos1X = Input->GetMouseX() - this->m_PosX;
							this->m_BaseScale1X = this->m_PosX + this->m_SizeX;
						}
						if (color != Gray25) {
							DrawBox(xp1 - EdgeSize, yp1, xp1 + EdgeSize, yp2 - EdgeSize + radius, color, TRUE);
						}
					}
				}
				//非アクティブ
				if (!this->m_IsActive) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 24);
					DrawBox(xp1, yp1, xp2, yp2, Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
				}

				//タブ演算
				{
					int xp3 = this->m_PosX + this->m_SizeX - LineHeight - LineHeight;
					unsigned int color = Gray25;

					if (this->m_IsActive && in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize, yp1 + EdgeSize, xp3 - EdgeSize, yp1 + LineHeight - EdgeSize)) {
						if (Input->GetLeftClick().trigger()) {
							this->m_IsMoving = true;
						}
						HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
						SetCursor(hCursor);
					}
					if (this->m_IsMoving) {
						if (Input->GetLeftClick().press()) {
							color = Gray50;
							this->m_PosX = Input->GetMouseX() - this->m_PosAddX;
							this->m_PosY = Input->GetMouseY() - this->m_PosAddY;


							HCURSOR hCursor = LoadCursor(NULL, IDC_SIZEALL);
							SetCursor(hCursor);
						}
						else {
							if (this->m_CanChageSize) {
								if (y_r(10) < Input->GetMouseY() && Input->GetMouseY() < DrawParts->m_DispYSize - y_r(10)) {
									if (Input->GetMouseX() < y_r(10)) {
										this->m_PosX = y_r(0);
										this->m_PosY = y_r(0);
										this->m_SizeX = DrawParts->m_DispXSize / 2;
										this->m_SizeY = DrawParts->m_DispYSize;
									}
									if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
										this->m_PosX = DrawParts->m_DispXSize / 2;
										this->m_PosY = y_r(0);
										this->m_SizeX = DrawParts->m_DispXSize / 2;
										this->m_SizeY = DrawParts->m_DispYSize;
									}
								}
								else {
									if (Input->GetMouseX() < y_r(10)) {
										this->m_PosX = y_r(0);
										this->m_PosY = (Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2;
										this->m_SizeX = DrawParts->m_DispXSize / 2;
										this->m_SizeY = DrawParts->m_DispYSize / 2;
									}
									if (Input->GetMouseX() > DrawParts->m_DispXSize - y_r(10)) {
										this->m_PosX = DrawParts->m_DispXSize / 2;
										this->m_PosY = (Input->GetMouseY() < DrawParts->m_DispYSize / 2) ? y_r(0) : DrawParts->m_DispYSize / 2;
										this->m_SizeX = DrawParts->m_DispXSize / 2;
										this->m_SizeY = DrawParts->m_DispYSize / 2;
									}
								}
							}
							this->m_IsMoving = false;
						}
					}
					else {
						this->m_PosAddX = Input->GetMouseX() - this->m_PosX;
						this->m_PosAddY = Input->GetMouseY() - this->m_PosY;
					}
				}

				if (this->m_CanChageSize && this->m_IsMoving && Input->GetLeftClick().press()) {
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
					if (m_WindowControl[i]->m_isDelete) {
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
			std::string			m_Name;
			EnumFilterParamType	m_Type{ 0 };
			long long int		m_Min{ 0 };
			long long int		m_Max{ 0 };
			long long int		m_Param{ 0 };
			std::array<int, 3>	m_Color{ 0 };
		public:
			void Set_Num(const char* name, int min, int max) {
				this->m_Name = name;
				this->m_Type = EnumFilterParamType::Num;
				this->m_Min = min;
				this->m_Max = max;
				m_Param = std::clamp(m_Param, this->m_Min, this->m_Max);
			}
			void Set_Color(const char* name) {
				this->m_Name = name;
				this->m_Type = EnumFilterParamType::Color;
				this->m_Color[0] = 255;
				this->m_Color[1] = 255;
				this->m_Color[2] = 255;
				m_Param = GetColor(this->m_Color[0], this->m_Color[1], this->m_Color[2]);
			}
			void Set_GaussWidth(const char* name) {
				this->m_Name = name;
				this->m_Type = EnumFilterParamType::GaussWidth;
				this->m_Param = 8;
			}
			void Set_CmpType(const char* name) {
				this->m_Name = name;
				this->m_Type = EnumFilterParamType::CmpType;
				this->m_Param = DX_CMP_LESS;
			}
			void Set_TrueFalse(const char* name) {
				this->m_Name = name;
				this->m_Type = EnumFilterParamType::TrueFalse;
				this->m_Param = TRUE;
			}

			void Add() {
				switch (this->m_Type) {
				case EnumFilterParamType::None:
					break;
				case EnumFilterParamType::Num:
					this->m_Param = std::clamp(this->m_Param + 1, this->m_Min, this->m_Max);
					break;
				case EnumFilterParamType::Color:
					break;
				case EnumFilterParamType::CmpType:
					switch (this->m_Param) {
					case DX_CMP_LESS:
						this->m_Param = DX_CMP_GREATER;
						break;
					default:
						this->m_Param = DX_CMP_LESS;
						break;
					}
					break;
				case EnumFilterParamType::GaussWidth:
					switch (this->m_Param) {
					case 8:
						this->m_Param = 16;
						break;
					default:
						this->m_Param = 32;
						break;
					}
					break;
				case EnumFilterParamType::TrueFalse:
					this->m_Param ^= 1;
					break;
				default:
					break;
				}
			}
			void Sub() {
				switch (this->m_Type) {
				case EnumFilterParamType::None:
					break;
				case EnumFilterParamType::Num:
					this->m_Param = std::clamp(this->m_Param - 1, this->m_Min, this->m_Max);
					break;
				case EnumFilterParamType::Color:
					break;
				case EnumFilterParamType::CmpType:
					switch (this->m_Param) {
					case DX_CMP_LESS:
						this->m_Param = DX_CMP_GREATER;
						break;
					default:
						this->m_Param = DX_CMP_LESS;
						break;
					}
					break;
				case EnumFilterParamType::GaussWidth:
					switch (this->m_Param) {
					case 32:
						this->m_Param = 16;
						break;
					default:
						this->m_Param = 8;
						break;
					}
					break;
				case EnumFilterParamType::TrueFalse:
					this->m_Param ^= 1;
					break;
				default:
					break;
				}
			}
		};

		class Canvas {
			class SaveParam {
				GraphHandle		m_GraphBuf;
			public:
				auto& GetGraphHandle(void) noexcept { return this->m_GraphBuf; }
			public:
				void		Set(const GraphHandle& BaseGraph) {
					int x, y;
					BaseGraph.GetSize(&x, &y);
					this->m_GraphBuf = GraphHandle::Make(x, y, false);
					this->m_GraphBuf.SetDraw_Screen();
					{
						DrawGraph(0, 0, BaseGraph.get(), FALSE);
					}
					SetDrawScreen(DX_SCREEN_BACK);
				}
				void		UndoWrite(GraphHandle* OutGraph) {
					OutGraph->SetDraw_Screen(false);
					{
						this->m_GraphBuf.DrawGraph(0, 0, false);
					}
					SetDrawScreen(DX_SCREEN_BACK);
				}
				void		Dispose() {
					this->m_GraphBuf.Dispose();
				}
			};
		private:
			int					m_xsize{ 1024 };
			int					m_ysize{ 768 };
		private:
			int					m_xpos{ 0 };
			int					m_ypos{ 0 };

			int					m_xposBase{ 0 };
			int					m_yposBase{ 0 };

			int					m_xposPrev;
			int					m_yposPrev;

			int					m_PenSize{ 1 };
			float				m_PenSizeChangeTimer = 1.f;

			unsigned int		m_LeftColor{ GetColor(0, 0, 0) };
			std::vector<unsigned int> m_ColorList;

			int					m_imgHandle{ -1 };
			GraphHandle			m_OutScreen;
			GraphHandle			m_OutScreen_Before;
			GraphHandle			m_OutScreen_After;

			float				m_scale{ 1.f };

			EnumDrawType		m_DrawType{ EnumDrawType::Write };

			bool				m_isDraw{ true };

			float				m_CtrlChangeTimer = 1.f;

			DialogManager		m_DialogManager;
			bool				m_DialogClick{ false };

			bool				m_IsNewFile{ false };
			std::string			m_SavePath;

			std::string			m_FilterName;
			std::array<FilterParamValue, 5>	m_FilterParam;

			int					m_FilterType{ 0 };
			bool				m_FilterChangeSwitch{ false };
			bool				m_FilterSetSwitch{ false };

			bool				m_IsWriting{ true };
			int					m_WriteMinX{ 0 };
			int					m_WriteMinY{ 0 };
			int					m_WriteMaxX{ 0 };
			int					m_WriteMaxY{ 0 };


			std::list<std::unique_ptr<SaveParam>> m_SaveParams;
			std::list<std::unique_ptr<SaveParam>>::iterator m_NowSaveSelect;
			float				m_SaveChangeTime{ 0.f };
		public:
			const auto&		GetLeftColor(void) const noexcept { return this->m_LeftColor; }
			void			SetLeftColor(unsigned int value) noexcept { this->m_LeftColor = value; }

			const auto&		GetFilterSelect(void) const noexcept { return this->m_FilterName; }
			auto&			GetFilterParam(void) noexcept { return this->m_FilterParam; }

			auto&			GetColorList(void) noexcept { return this->m_ColorList; }
			void			SetIsDraw(bool value) noexcept { this->m_isDraw = value; }

			void			FilterChange() noexcept { this->m_FilterChangeSwitch = true; }
			void			FilterSet() noexcept { this->m_FilterSetSwitch = true; }
		private:
			void SetDo() {
				if (!m_SaveParams.empty()) {
					if (m_NowSaveSelect != m_SaveParams.end()) {
						m_SaveParams.erase(m_NowSaveSelect, m_SaveParams.end());
					}
				}
				m_SaveParams.emplace_back(std::make_unique<SaveParam>());
				m_SaveParams.back()->Set(m_OutScreen_Before);
				m_NowSaveSelect = m_SaveParams.end();
				m_OutScreen.SetDraw_Screen(false);
				{
					m_OutScreen_After.DrawGraph(0, 0, false);
				}
				SetDrawScreen(DX_SCREEN_BACK);
			}
		private:
			void CreatePic() noexcept {
				this->m_imgHandle = MakeSoftImage(this->m_xsize, this->m_ysize);
				this->m_OutScreen = GraphHandle::Make(this->m_xsize, this->m_ysize, false);
				this->m_OutScreen_Before = GraphHandle::Make(this->m_xsize, this->m_ysize, false);
				this->m_OutScreen_After = GraphHandle::Make(this->m_xsize, this->m_ysize, false);
			}
			void DisposePic() noexcept {
				if (this->m_imgHandle != -1) {
					DeleteSoftImage(this->m_imgHandle);
				}
				this->m_OutScreen.Dispose();
				this->m_OutScreen_Before.Dispose();
				this->m_OutScreen_After.Dispose();
			}
			void InitPic() noexcept {
				this->m_SavePath = "新しいファイル";

				DisposePic();
				CreatePic();
				this->m_OutScreen.SetDraw_Screen();
				{
					DrawBox(0, 0, this->m_xsize, this->m_ysize, GetColor(255, 255, 255), TRUE);
				}
				SetDrawScreen(DX_SCREEN_BACK);
				this->m_IsNewFile = true;
			}
			void Load() noexcept {
				if (m_DialogManager.Open()) {
					this->m_SavePath = m_DialogManager.GetPath();

					GraphHandle	Buffer = GraphHandle::Load(this->m_SavePath);
					Buffer.GetSize(&this->m_xsize, &this->m_ysize);

					DisposePic();
					CreatePic();
					this->m_OutScreen.SetDraw_Screen();
					{
						DrawBox(0, 0, this->m_xsize, this->m_ysize, GetColor(255, 255, 255), TRUE);
						Buffer.DrawGraph(0, 0, false);
					}
					SetDrawScreen(DX_SCREEN_BACK);
					Buffer.Dispose();

					this->m_xpos = 0;
					this->m_ypos = 0;
					this->m_scale = 1.f;
				}
			}
			void Save(void) noexcept {
				if (this->m_IsNewFile) {
					if (m_DialogManager.Save()) {
						this->m_SavePath = m_DialogManager.GetPath();
					}
				}
				{
					if (this->m_SavePath.find(".bmp") != std::string::npos) {
						SaveDrawValidGraphToBMP(this->m_OutScreen.get(), 0, 0, this->m_xsize, this->m_ysize, this->m_SavePath.c_str());					// ＢＭＰ形式で保存する
					}
					else if (this->m_SavePath.find(".dds") != std::string::npos) {
						SaveDrawValidGraphToDDS(this->m_OutScreen.get(), 0, 0, this->m_xsize, this->m_ysize, this->m_SavePath.c_str());					// ＤＤＳ形式で保存する
					}
					else if (this->m_SavePath.find(".jpg") != std::string::npos) {
						SaveDrawValidGraphToJPEG(this->m_OutScreen.get(), 0, 0, this->m_xsize, this->m_ysize, this->m_SavePath.c_str(), 100, TRUE);		// ＪＰＥＧ形式で保存する Quality = 画質、値が大きいほど低圧縮高画質,0～100 
					}
					else if (this->m_SavePath.find(".png") != std::string::npos) {
						SaveDrawValidGraphToPNG(this->m_OutScreen.get(), 0, 0, this->m_xsize, this->m_ysize, this->m_SavePath.c_str(), 0);				// ＰＮＧ形式で保存する CompressionLevel = 圧縮率、値が大きいほど高圧縮率高負荷、０は無圧縮,0～9
					}
					else {
						SaveDrawValidGraphToBMP(this->m_OutScreen.get(), 0, 0, this->m_xsize, this->m_ysize, this->m_SavePath.c_str());
					}
				}
			}

			void SetDraw(EnumDrawType DrawType) {
				auto* Input = InputControl::Instance();
				int m_x = std::clamp((int)((float)(Input->GetMouseX() - this->m_xpos) / this->m_scale), -1, this->m_xsize);
				int m_y = std::clamp((int)((float)(Input->GetMouseY() - this->m_ypos) / this->m_scale), -1, this->m_ysize);
				switch (DrawType) {
				case EnumDrawType::Write:
					if (Input->GetLeftClick().press()) {
						if (!this->m_IsWriting) {
							this->m_OutScreen_Before.SetDraw_Screen(false);
							{
								this->m_OutScreen.DrawGraph(0, 0, false);
							}
							this->m_OutScreen_After.SetDraw_Screen(false);
							{
								this->m_OutScreen_Before.DrawGraph(0, 0, false);
							}
							this->m_IsWriting = true;
						}
						if ((in2_(this->m_xposPrev, this->m_yposPrev, 0, 0, this->m_xsize - 1, this->m_ysize - 1)) || (in2_(m_x, m_y, 0, 0, this->m_xsize - 1, this->m_ysize - 1))) {
							this->m_OutScreen_After.SetDraw_Screen(false);
							{
								DrawLine(this->m_xposPrev, this->m_yposPrev, m_x, m_y, this->m_LeftColor, this->m_PenSize);
								if (this->m_PenSize == 1) {
									DrawPixel(this->m_xposPrev, this->m_yposPrev, this->m_LeftColor);
									DrawPixel(m_x, m_y, this->m_LeftColor);
								}
								else if (this->m_PenSize == 2) {
									DrawBox(this->m_xposPrev, this->m_yposPrev, this->m_xposPrev, this->m_yposPrev, this->m_LeftColor, TRUE);
									DrawBox(m_x, m_y, m_x, m_y, this->m_LeftColor, TRUE);
								}
								else {
									DrawCircle(this->m_xposPrev, this->m_yposPrev, this->m_PenSize / 2, this->m_LeftColor);
									DrawCircle(m_x, m_y, (this->m_PenSize - 1) / 2, this->m_LeftColor);
								}
							}
							this->m_WriteMinX = std::min(m_x - this->m_PenSize / 2, this->m_WriteMinX);
							this->m_WriteMinY = std::min(m_y - this->m_PenSize / 2, this->m_WriteMinY);
							this->m_WriteMaxX = std::max(m_x + this->m_PenSize / 2 + 1, this->m_WriteMaxX);
							this->m_WriteMaxY = std::max(m_y + this->m_PenSize / 2 + 1, this->m_WriteMaxY);
						}
						if (Input->GetRightClick().trigger()) {//リセット
							this->m_OutScreen_After.SetDraw_Screen(false);
							{
								this->m_OutScreen_Before.DrawGraph(0, 0, false);
							}
							this->m_WriteMinX = m_x;
							this->m_WriteMinY = m_y;
							this->m_WriteMaxX = m_x;
							this->m_WriteMaxY = m_y;
						}
					}
					else {
						if (this->m_IsWriting) {
							this->m_IsWriting = false;
							SetDo();
						}
						this->m_WriteMinX = m_x;
						this->m_WriteMinY = m_y;
						this->m_WriteMaxX = m_x;
						this->m_WriteMaxY = m_y;
					}
					this->m_xposPrev = m_x;
					this->m_yposPrev = m_y;
					break;
				case EnumDrawType::Paint:
					if ((in2_(m_x, m_y, 0, 0, this->m_xsize - 1, this->m_ysize - 1)) && Input->GetLeftClick().trigger()) {
						this->m_OutScreen_Before.SetDraw_Screen(false);
						{
							this->m_OutScreen.DrawGraph(0, 0, false);
						}
						this->m_OutScreen_After.SetDraw_Screen(false);
						{
							this->m_OutScreen_Before.DrawGraph(0, 0, false);
							Paint(m_x, m_y, this->m_LeftColor);
						}
						SetDo();
					}
					break;
				default:
					break;
				}

				if ((in2_(m_x, m_y, 0, 0, this->m_xsize - 1, this->m_ysize - 1)) && Input->GetMiddleClick().trigger()) {
					this->m_OutScreen.SetDraw_Screen(false);
					{
						GetDrawScreenSoftImage(0, 0, this->m_xsize, this->m_ysize, this->m_imgHandle);
					}
					int r, g, b, a;
					GetPixelSoftImage(this->m_imgHandle, m_x, m_y, &r, &g, &b, &a);
					this->m_LeftColor = GetColor(r, g, b);
				}
			}

			void SetFilterParamData(int FilterSet) {
				for (auto& p : this->m_FilterParam) {
					p.m_Type = EnumFilterParamType::None;
					p.m_Param = 0;
					p.m_Min = 0;
					p.m_Max = 0;
				}
				switch (FilterSet) {
				case DX_GRAPH_FILTER_MONO:
					this->m_FilterName = "モノトーンフィルタ";
					this->m_FilterParam[0].Set_Num("青色差", -255, 255);
					this->m_FilterParam[1].Set_Num("赤色差", -255, 255);
					break;
				case DX_GRAPH_FILTER_GAUSS:
					this->m_FilterName = "ガウスフィルタ";
					this->m_FilterParam[0].Set_GaussWidth("使用幅");
					this->m_FilterParam[1].Set_Num("ぼかし", 0, 2000);
					break;
				case DX_GRAPH_FILTER_BRIGHT_CLIP:
					this->m_FilterName = "明るさクリップフィルタ";
					this->m_FilterParam[0].Set_CmpType("クリップタイプ");
					this->m_FilterParam[1].Set_Num("クリップしきい値", 0, 255);
					this->m_FilterParam[2].Set_TrueFalse("クリップしたピクセルを塗りつぶすかどうか");
					this->m_FilterParam[3].Set_Color("塗る色");
					this->m_FilterParam[4].Set_Num("塗るアルファ", 0, 255);
					break;
				case DX_GRAPH_FILTER_HSB:
					this->m_FilterName = "色相,彩度,明度フィルタ";
					this->m_FilterParam[0].Set_TrueFalse("色相が絶対値(0～360)か否(-180～180)か");
					this->m_FilterParam[1].Set_Num("色相の絶対値か元の色相に対する相対値", -180, 360);
					this->m_FilterParam[2].Set_Num("彩度", -255, 1024);
					this->m_FilterParam[3].Set_Num("輝度", -255, 255);
					break;
				case DX_GRAPH_FILTER_INVERT:
					this->m_FilterName = "反転フィルタ";
					break;
				case DX_GRAPH_FILTER_LEVEL:
					this->m_FilterName = "レベル補正フィルタ";
					this->m_FilterParam[0].Set_Num("変換元の下限値", 0, 255);
					this->m_FilterParam[1].Set_Num("変換元の上限値", 0, 255);
					this->m_FilterParam[2].Set_Num("ガンマ値x100", 100, 1000);
					this->m_FilterParam[3].Set_Num("変換後の最低値", 0, 255);
					this->m_FilterParam[4].Set_Num("変換後の最大値", 0, 255);
					break;
				case DX_GRAPH_FILTER_TWO_COLOR:
					this->m_FilterName = "２階調化フィルタ";
					this->m_FilterParam[0].Set_Num("閾値", 0, 255);
					this->m_FilterParam[1].Set_Color("閾値より値が低かった場合の色");
					this->m_FilterParam[2].Set_Num("閾値より値が低かった場合のアルファ", 0, 255);
					this->m_FilterParam[3].Set_Color("閾値より値が高かった場合の色");
					this->m_FilterParam[4].Set_Num("閾値より値が高かった場合のアルファ", 0, 255);
					break;
				default:
					this->m_FilterName = "フィルタ無し";
					break;
				}
			}
		public:
			void ChangeSize(int x_size, int y_size) {
				this->m_xsize = x_size;
				this->m_ysize = y_size;

				GraphHandle	Buffer = GraphHandle::Make(this->m_xsize, this->m_ysize, false);
				Buffer.SetDraw_Screen();
				{
					this->m_OutScreen.DrawGraph(0, 0, false);
				}
				DisposePic();
				CreatePic();
				this->m_OutScreen.SetDraw_Screen();
				{
					Buffer.DrawGraph(0, 0, false);
				}
				SetDrawScreen(DX_SCREEN_BACK);
				Buffer.Dispose();

			}
			const auto&		GetXsize(void) const noexcept { return this->m_xsize; }
			const auto&		GetYsize(void) const noexcept { return this->m_ysize; }
		public:
			void Set(void) noexcept {
				m_DialogManager.Init();

				InitPic();

				this->m_LeftColor = GetColor(0, 0, 0);
				this->m_IsWriting = false;

				SetFilterParamData(-1);
			}
			void Execute(void) noexcept {
				auto* Input = InputControl::Instance();

				if (Input->GetRightClick().press()) {
					this->m_xpos = Input->GetMouseX() - this->m_xposBase;
					this->m_ypos = Input->GetMouseY() - this->m_yposBase;
				}
				else {
					this->m_xposBase = Input->GetMouseX() - this->m_xpos;
					this->m_yposBase = Input->GetMouseY() - this->m_ypos;
				}

				if (Input->GetWheelAdd() != 0) {
					if (Input->GetKey('C').press()) {
						if (!this->m_IsWriting) {
							this->m_PenSize = std::clamp(this->m_PenSize + Input->GetWheelAdd(), 1, 100);
							this->m_PenSizeChangeTimer = 1.f;
						}
					}
					else {
						auto PrevScale = this->m_scale;
						this->m_scale = std::clamp(this->m_scale + (float)Input->GetWheelAdd() / 10.f, 0.1f, 100.f);
						this->m_xpos -= (int)((float)(Input->GetMouseX() - this->m_xpos) * (this->m_scale - PrevScale) / this->m_scale);
						this->m_ypos -= (int)((float)(Input->GetMouseY() - this->m_ypos) * (this->m_scale - PrevScale) / this->m_scale);
					}
				}

				if (!this->m_DialogClick) {
					if (Input->GetCtrlKey().press()) {
						if (Input->GetKey('S').trigger()) {
							Save();
							this->m_DialogClick = true;
						}
						if (Input->GetKey('O').trigger()) {
							Load();
							this->m_DialogClick = true;
						}
						if (Input->GetKey('Z').trigger()) {
							if (!m_SaveParams.empty()) {
								if (this->m_NowSaveSelect != m_SaveParams.begin()) {
									this->m_NowSaveSelect--;
									(*this->m_NowSaveSelect)->UndoWrite(&this->m_OutScreen);
									this->m_SaveChangeTime = 2.f;
								}
							}
						}
						if (Input->GetKey('Y').trigger()) {
							if (!m_SaveParams.empty()) {
								if (this->m_NowSaveSelect != m_SaveParams.end()) {
									this->m_NowSaveSelect++;
									if (this->m_NowSaveSelect != m_SaveParams.end()) {
										(*this->m_NowSaveSelect)->UndoWrite(&this->m_OutScreen);
									}
									else {
										this->m_OutScreen.SetDraw_Screen(false);
										{
											this->m_OutScreen_After.DrawGraph(0, 0, false);
										}
										SetDrawScreen(DX_SCREEN_BACK);
									}
									this->m_SaveChangeTime = 2.f;
								}
							}
						}
						if (Input->GetKey('V').trigger()) {
							GraphHandle ClipBoadBmp;
							bool IsGetClipBoadBmp = FPS_n2::GetClipBoardGraphHandle(&ClipBoadBmp);
							if (IsGetClipBoadBmp) {
								this->m_OutScreen_Before.SetDraw_Screen(false);
								{
									this->m_OutScreen.DrawGraph(0, 0, false);
								}
								this->m_OutScreen_After.SetDraw_Screen(false);
								{
									this->m_OutScreen_Before.DrawGraph(0, 0, false);
									ClipBoadBmp.DrawGraph(0, 0, false);
								}
								SetDo();
								ClipBoadBmp.Dispose();
							}
						}
					}
				}
				else {
					if (CheckHitKeyAll()) {
						this->m_isDraw = false;
					}
					else {
						this->m_DialogClick = false;
					}
				}
				this->m_SaveChangeTime = std::clamp(this->m_SaveChangeTime - 1.f / FPS, 0.f, 2.f);


				if (!Input->GetLeftClick().press()) {
					m_DrawType = EnumDrawType::Write;
					if (Input->GetShiftKey().press()) {
						m_DrawType = EnumDrawType::Paint;
					}
				}
				if (this->m_isDraw) {
					SetDraw(m_DrawType);
				}

				if (this->m_FilterSetSwitch) {
					this->m_OutScreen_Before.SetDraw_Screen(false);
					{
						this->m_OutScreen.DrawGraph(0, 0, false);
					}
					SetDo();
					this->m_FilterType = 0;
				}
				this->m_FilterSetSwitch = false;

				this->m_PenSizeChangeTimer = std::max(this->m_PenSizeChangeTimer - 1.f / FPS, 0.f);
				Easing(&this->m_CtrlChangeTimer, (Input->GetCtrlKey().press() ? 1.f : 0.f), 0.8f, EasingType::OutExpo);

			}
			void Draw(void) noexcept {
				auto* Input = InputControl::Instance();
				auto* Fonts = FontPool::Instance();
				auto* DrawParts = DXDraw::Instance();

				auto prevmode = GetDrawMode();
				if (this->m_scale < 1.f) {
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
					if (this->m_FilterChangeSwitch) {
						++this->m_FilterType %= 8;
						SetFilterParamData(FILTERS[this->m_FilterType]);
					}
					this->m_FilterChangeSwitch = false;
					if (FILTERS[this->m_FilterType] != -1) {
						GraphFilterBlt(this->m_OutScreen.get(), this->m_OutScreen_After.get(), FILTERS[this->m_FilterType], this->m_FilterParam[0].m_Param, this->m_FilterParam[1].m_Param, this->m_FilterParam[2].m_Param, this->m_FilterParam[3].m_Param, this->m_FilterParam[4].m_Param);
						this->m_OutScreen_After.DrawExtendGraph(y_r(this->m_xpos), y_r(this->m_ypos), y_r(this->m_xpos + (int)((float)this->m_xsize * this->m_scale)), y_r(this->m_ypos + (int)((float)this->m_ysize * this->m_scale)), false);
					}
					else {
						this->m_OutScreen.DrawExtendGraph(y_r(this->m_xpos), y_r(this->m_ypos), y_r(this->m_xpos + (int)((float)this->m_xsize * this->m_scale)), y_r(this->m_ypos + (int)((float)this->m_ysize * this->m_scale)), false);
						if (this->m_IsWriting) {
							this->m_OutScreen_After.DrawExtendGraph(y_r(this->m_xpos), y_r(this->m_ypos), y_r(this->m_xpos + (int)((float)this->m_xsize * this->m_scale)), y_r(this->m_ypos + (int)((float)this->m_ysize * this->m_scale)), false);
						}
					}
				}
				SetDrawMode(prevmode);

				int r, g, b;
				GetColor2(this->m_LeftColor, &r, &g, &b);
				unsigned int backcolor = GetColor(255 - r, 255 - g, 255 - b);

				//普通のガイド
				if (this->m_CtrlChangeTimer < 0.95f) {
					int Posx = Input->GetMouseX();
					int Posy = Input->GetMouseY();
					int Size = (int)((float)LineHeight*(1.f - this->m_CtrlChangeTimer));

					switch (m_DrawType) {
					case EnumDrawType::Write:
						DrawCircle(Posx, Posy, (int)((float)this->m_PenSize / 2.f*this->m_scale), backcolor, FALSE, 3);
						DrawCircle(Posx, Posy, (int)((float)this->m_PenSize / 2.f*this->m_scale), this->m_LeftColor, FALSE);
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy, this->m_LeftColor, backcolor, " ペン");
						if (Input->GetKey('C').press() || (this->m_PenSizeChangeTimer > 0)) {
							Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy + Size, this->m_LeftColor, backcolor, "太さ:%d", this->m_PenSize);
						}
						break;
					case EnumDrawType::Paint:
						Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, Posx, Posy, this->m_LeftColor, backcolor, " バケツ");
						break;
					default:
						break;
					}

				}
				//ctrlガイド
				if (this->m_CtrlChangeTimer > 0.05f) {
					int Posx = Input->GetMouseX();
					int Posy = Input->GetMouseY();
					int Size = (int)((float)LineHeight*this->m_CtrlChangeTimer);
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 4, this->m_LeftColor, backcolor, "ペースト:V ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 3, this->m_LeftColor, backcolor, "戻る:Z ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 2, this->m_LeftColor, backcolor, "やり直す:Y ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 1, this->m_LeftColor, backcolor, "開く:O ");
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(Size, FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::BOTTOM, Posx, Posy - Size * 0, this->m_LeftColor, backcolor, "保存:S ");
				}
				//場所ガイド
				{
					int xp = y_r(1440);
					int yp = y_r(820);

					int xs = y_r(320);
					int ys = y_r(180);

					int x_p1 = std::max(xp + y_r(this->m_xpos) * xs / DrawParts->m_DispXSize, xp - xs / 2);
					int y_p1 = std::max(yp + y_r(this->m_ypos) * ys / DrawParts->m_DispYSize, yp - ys / 2);
					int x_p2 = std::min(xp + y_r(this->m_xpos + (int)((float)this->m_xsize * this->m_scale)) * xs / DrawParts->m_DispXSize, xp + xs + xs / 2);
					int y_p2 = std::min(yp + y_r(this->m_ypos + (int)((float)this->m_ysize * this->m_scale)) * ys / DrawParts->m_DispYSize, yp + ys + ys / 2);

					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 64);
					DrawBox(x_p1, y_p1, x_p2, y_p2, GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
					DrawBox(x_p1, y_p1, x_p2, y_p2, GetColor(0, 200, 0), FALSE);
					DrawBox(xp, yp, xp + xs, yp + ys, Red, FALSE);
				}
				//ファイル名
				{
					Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, y_r(12), y_r(12), this->m_LeftColor, backcolor, "[%s]", this->m_SavePath.c_str());
				}
				//
				if (this->m_SaveChangeTime > 0.f) {
					auto prevmodet = GetDrawMode();
					SetDrawMode(DX_DRAWMODE_BILINEAR);

					int xp, yp, xs, ys;

					xs = y_r(300);
					ys = y_r(300)*this->m_ysize / this->m_xsize;


					xp = y_r(10);
					yp = DrawParts->m_DispYSize / 2 - ys / 2;
					{
						auto Itr = this->m_NowSaveSelect;
						if (Itr != m_SaveParams.begin()) {
							for (int i = 1; i < 5; i++) {
								Itr--;
								(*Itr)->GetGraphHandle().DrawExtendGraph(xp, yp - (ys + y_r(5)) * i, xp + xs, yp + ys - (ys + y_r(5)) * i, false);
								DrawBox(xp, yp - (ys + y_r(5)) * i, xp + xs, yp + ys - (ys + y_r(5)) * i, Green, FALSE);
								if (Itr == m_SaveParams.begin()) { break; }
							}
						}
					}
					xp = y_r(110);
					{
						auto Itr = this->m_NowSaveSelect;
						for (int i = 0; i < 5; i++) {
							if (Itr == m_SaveParams.end()) {
								DrawBox(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, Green, FALSE);
								this->m_OutScreen_After.DrawExtendGraph(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, false);
								Fonts->Get(FontPool::FontType::Nomal_Edge).DrawString(y_r(24), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP, xp, yp + (ys + y_r(5)) * i, this->m_LeftColor, backcolor, "[NEW]");
								break;
							}
							(*Itr)->GetGraphHandle().DrawExtendGraph(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, false);
							DrawBox(xp, yp + (ys + y_r(5)) * i, xp + xs, yp + ys + (ys + y_r(5)) * i, Green, FALSE);
							Itr++;
							xp = y_r(10);
						}
					}
					SetDrawMode(prevmodet);
				}

				printfDx("\n");
				printfDx("\n");
				printfDx("\n");
				printfDx("左クリック\n");
				printfDx("　何も押さない               ペン\n");
				printfDx("　　左クリック中に右クリック リセット\n");
				printfDx("　LShift押しながら           バケツ\n");
				printfDx("ホイールクリック             スポイト\n");
				printfDx("右クリック                   視点移動\n");
				printfDx("何も押さずマウスホイール     ズーム\n");
				printfDx("Cを押しながらマウスホイール  ペンの太さチェンジ\n");
				printfDx("LCtrl                        特殊メニュー\n");
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
						switch (p.m_Type) {
						case DrawSystem::EnumFilterParamType::Num:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Value:%4d", p.m_Param);
							break;
						case DrawSystem::EnumFilterParamType::Color:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height * 3 / 4, FontHandle::FontXCenter::LEFT, "Color:%3d,%3d,%3d", p.m_Color[0], p.m_Color[1], p.m_Color[2]);
							WindowSystem::SetBox(xp + y_r(190), yp, xp + y_r(230), yp + height, GetColor(p.m_Color[0], p.m_Color[1], p.m_Color[2]));
							break;
						case DrawSystem::EnumFilterParamType::CmpType:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Cmp  :%s", (p.m_Param == DX_CMP_LESS) ? "LESS" : "GREATER");
							break;
						case DrawSystem::EnumFilterParamType::GaussWidth:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Gauss:%4d", p.m_Param);
							break;
						case DrawSystem::EnumFilterParamType::TrueFalse:
							WindowSystem::SetMsg(xp, yp, xp + xs, yp + height, height, FontHandle::FontXCenter::LEFT, "Bool :%s", (p.m_Param == TRUE) ? "TRUE" : "FALSE");
							break;
						default:
							continue;
						}
						WindowSystem::SetMsg(xp, yp + height, xp + xs, yp + height + height, height * 3 / 4, FontHandle::FontXCenter::LEFT, "%s", p.m_Name.c_str());
						//ボタン
						switch (p.m_Type) {
						case DrawSystem::EnumFilterParamType::Color:
							for (int i = 0; i < 3; i++) {
								if (WindowSystem::ClickCheckBox(xp + y_r((384 - 20 - 4 - 20) - (2 - i) * 50), yp, xp + y_r((384 - 20 - 4) - (2 - i) * 50), yp + height, true, win->GetIsActive(), Gray25, "+")) {
									++p.m_Color[i] %= 255;
								}
								if (WindowSystem::ClickCheckBox(xp + y_r((384 - 20) - (2 - i) * 50), yp, xp + y_r(384 - (2 - i) * 50), yp + height, true, win->GetIsActive(), Gray25, "-")) {
									--p.m_Color[i]; if (p.m_Color[0] < 0) { p.m_Color[0] = 255; }
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
