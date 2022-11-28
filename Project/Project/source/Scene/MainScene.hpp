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

	class InputControl {
		SET_SINGLETON(InputControl)
	private:
		int						m_mouse_x, m_mouse_y;
		switchs					m_LeftClick;
		float					m_LeftPressTimer{ 0.f };
	private:
		InputControl(void) noexcept {
			m_LeftClick.Set(false);
		}
		~InputControl(void) noexcept { }
	public:
		const auto&		GetMouseX(void) const noexcept { return this->m_mouse_x; }
		const auto&		GetMouseY(void) const noexcept { return this->m_mouse_y; }
		const auto&		GetLeftClick(void) const noexcept { return this->m_LeftClick; }
		const auto&		GetLeftPressTimer(void) const noexcept { return this->m_LeftPressTimer; }
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
		}
	};

	//��
	static void SetBox(int xp1, int yp1, int xp2, int yp2, unsigned int colorSet) {
		DrawBox(xp1, yp1, xp2, yp2, Gray75, TRUE);
		DrawBox(xp1 + EdgeSize, yp1 + EdgeSize, xp2 - EdgeSize, yp2 - EdgeSize, colorSet, TRUE);
	};
	//����
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
		return Fonts->Get(FontPool::FontType::Nomal_Edge).GetStringWidth(size, ((std::string)String).c_str(), args...) + y_r(6) + 2;//�G�b�W��
	};

	class MAINLOOP : public TEMPSCENE {
	private:
		struct WindowControl {
		public:
			bool isDelete{ false };
		private:
			bool ActiveSwitch{ false };
			bool IsActive{ false };
			int PosX{ 0 }, PosY{ 0 };
			int SizeX{ 100 }, SizeY{ 100 };
			std::string TabName;
			std::function<void(WindowControl*)> m_DoingOnWindow;
		private:
			bool isMaxSize{ false };
			int PosXSave{ 0 }, PosYSave{ 0 };
			int SizeXSave{ 100 }, SizeYSave{ 100 };

			bool CanChageSize{ false };

			bool IsMoving{ false };
			int PosAddX{ 0 }, PosAddY{ 0 };

			bool IsChangeScaleXY{ false };
			bool IsChangeScale1X{ false };
			bool IsChangeScale1Y{ false };
			bool IsChangeScale2X{ false };
			bool IsChangeScale2Y{ false };
			int BaseScaleX{ 0 }, BaseScaleY{ 0 };
			int BasePos1X{ 0 }, BasePos1Y{ 0 };
			int BaseScale1X{ 0 }, BaseScale1Y{ 0 };
			int BaseScale2X{ 0 }, BaseScale2Y{ 0 };
		public:
			const auto&		GetPosX(void) const noexcept { return this->PosX; }
			const auto&		GetPosY(void) const noexcept { return this->PosY; }
			const auto&		GetSizeX(void) const noexcept { return this->SizeX; }
			const auto&		GetSizeY(void) const noexcept { return this->SizeY; }

			const auto&		GetActiveSwitch(void) const noexcept { return this->ActiveSwitch; }
			void			SetIsActive(bool value) noexcept { IsActive = value; }
		public:
			void Set(int posx, int posy, int sizex, int sizey, const char* tabName, bool canChageSize, const std::function<void(WindowControl*)>& DoingOnWindow) noexcept {
				this->PosX = posx;
				this->PosY = posy;


				this->SizeX = sizex;
				this->SizeY = LineHeight + sizey;
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
				//�w�i
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
				//���e
				m_DoingOnWindow(this);
				//�^�u
				{
					int xp3 = PosX + SizeX - LineHeight - LineHeight;
					unsigned int color = Gray25;

					if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1 + EdgeSize, yp1 + EdgeSize, xp3 - EdgeSize, yp1 + LineHeight - EdgeSize)) {
						if (Input->GetLeftClick().trigger()) {
							IsMoving = true;
						}
					}
					if (IsMoving) {
						if (Input->GetLeftClick().press()) {
							color = Gray50;
							PosX = Input->GetMouseX() - PosAddX;
							PosY = Input->GetMouseY() - PosAddY;
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

					SetBox(xp1, yp1, xp2, yp1 + LineHeight, color);
					widthLimit = SetMsg(xp1, yp1, xp3, yp1 + LineHeight, LineHeight - y_r(6), FontHandle::FontXCenter::LEFT, TabName) + EdgeSize * 2 + LineHeight * 2;
				}
				//�ő剻�{�^��
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
					}
					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, !isMaxSize ? "��" : "�");
				}
				//�~�{�^��
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
					}

					DrawBox(xp3 + EdgeSize, yp3 + EdgeSize, xp4 - EdgeSize, yp4 - EdgeSize, color, TRUE);
					SetMsg(xp3, yp3, xp4, yp4, LineHeight - EdgeSize * 2 - y_r(6), FontHandle::FontXCenter::MIDDLE, "X");
				}
				if (CanChageSize && !isMaxSize && IsActive) {
					//xy�T�C�Y
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
					//y�T�C�Y��
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
					//x�T�C�Y�E
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
					//y�T�C�Y��
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
					//x�T�C�Y��
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
				//��A�N�e�B�u
				if (!IsActive) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 24);
					DrawBox(xp1, yp1, xp2, yp2, Black, TRUE);
					SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);
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
		std::vector<std::shared_ptr<WindowControl>> m_WindowControl;
	private:
		template <typename... Args>
		bool ClickCheckBox(int xp1, int yp1, int xp2, int yp2, bool isrepeat, std::string_view String, Args&&... args) {
			auto* Input = InputControl::Instance();
			unsigned int color = Gray25;
			bool isHit = false;
			if (in2_(Input->GetMouseX(), Input->GetMouseY(), xp1, yp1, xp2, yp2)) {
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
			}
			SetBox(xp1, yp1, xp2, yp2, color);
			SetMsg(xp1, yp1, xp2, yp2, LineHeight - y_r(6), FontHandle::FontXCenter::MIDDLE, String, args...);
			return isHit;
		};

		int CheckDialogAns{ -1 };
		void AddCheckDialog(const char* tabName) {
			auto* DrawParts = DXDraw::Instance();
			int size_x = y_r(350);
			int size_y = y_r(120);

			CheckDialogAns = -1;
			m_WindowControl.emplace_back(std::make_shared<WindowControl>());
			m_WindowControl.back()->Set(DrawParts->m_DispXSize / 2 - size_x / 2, DrawParts->m_DispYSize / 2 - size_y / 2, size_x, size_y, tabName, false, [&](WindowControl* win) {
				int size_x = y_r(350);
				int size_y = y_r(120);
				{
					int xs = y_r(100);
					int ys = LineHeight;
					int xp = win->GetPosX() + size_x / 2 - xs / 2, yp = win->GetPosY() + LineHeight + size_y / 2 - ys / 2 - ys * 6 / 10;
					int xp2 = xp + xs, yp2 = yp + ys;
					if (xp2 < win->GetPosX() + win->GetSizeX() && yp2 < win->GetPosY() + win->GetSizeY()) {
						if (ClickCheckBox(xp, yp, xp2, yp2, false, "YES")) {
							win->isDelete = true;
							CheckDialogAns = 0;
						}
					}
				}
				{
					int xs = y_r(100);
					int ys = LineHeight;
					int xp = win->GetPosX() + size_x / 2 - xs / 2, yp = win->GetPosY() + LineHeight + size_y / 2 - ys / 2 + ys * 6 / 10;
					int xp2 = xp + xs, yp2 = yp + ys;
					if (xp2 < win->GetPosX() + win->GetSizeX() && yp2 < win->GetPosY() + win->GetSizeY()) {
						if (ClickCheckBox(xp, yp, xp2, yp2, false, "NO")) {
							win->isDelete = true;
							CheckDialogAns = 1;
						}
					}
				}
			});
		}

		void AddBlankWindow(const char* tabName) {
			int size_x = y_r(500);
			int size_y = y_r(500);
			m_WindowControl.emplace_back(std::make_shared<WindowControl>());
			m_WindowControl.back()->Set(y_r(350), y_r(350), size_x, size_y, tabName, true, [&](WindowControl*) {});
		}
	public:
		void Load_Sub(void) noexcept override {
			//�T�E���h
			auto* SE = SoundPool::Instance();
			SE->Add((int)SoundEnum::UI_OK, 1, "data/Sound/UI/hit.wav");
			SE->Add((int)SoundEnum::UI_CANCEL, 1, "data/Sound/UI/cancel.wav");
			SE->Add((int)SoundEnum::UI_SELECT, 3, "data/Sound/SE/UI/cursor.wav");
		}

		void Set_Sub(void) noexcept override {
			//�T�E���h
			auto* SE = SoundPool::Instance();
			SE->Get((int)SoundEnum::UI_OK).SetVol_Local(128);
			SE->Get((int)SoundEnum::UI_CANCEL).SetVol_Local(128);
			SE->Get((int)SoundEnum::UI_SELECT).SetVol_Local(128);
			//
			InputControl::Create();
		}
		bool Update_Sub(void) noexcept override {
			bool isend = false;
			//FirstDoing
			if (GetIsFirstLoop()) {
				SetMousePoint(DXDraw::Instance()->m_DispXSize / 2, DXDraw::Instance()->m_DispYSize / 2);
			}
			auto* Input = InputControl::Instance();
			Input->Execute();
			{
				if (GetIsFirstLoop()) {
					AddCheckDialog("���V�[���֍s����");
					AddBlankWindow("Tab");
				}
				switch (CheckDialogAns) {
				case 0:
					isend = true;//���V�[���֍s��
					break;
				case 1:
					break;
				default:
					break;
				}
			}
			//�E�B���h�E�A�N�e�B�u
			if ((m_WindowControl.size() > 1) && !m_WindowControl.back()->GetActiveSwitch()) {
				for (int i = (int)(m_WindowControl.size()) - 2; i >= 0; i--) {
					if (m_WindowControl[i]->GetActiveSwitch()) {
						m_WindowControl.emplace_back(m_WindowControl[i]);
						m_WindowControl.erase(m_WindowControl.begin() + i);
						break;
					}
				}
			}

			for (int i = 0; i < m_WindowControl.size(); i++) {
				m_WindowControl[i]->SetIsActive(false);
				if (m_WindowControl[i]->isDelete) {
					std::swap(m_WindowControl[i], m_WindowControl.back());
					m_WindowControl.pop_back();
					i--;
				}
			}
			if (m_WindowControl.size() > 0) {
				m_WindowControl.back()->SetIsActive(true);
			}
			return !isend;
		}
		void Dispose_Sub(void) noexcept override {
		}
	public:
		void BG_Draw_Sub(void) noexcept override {}
		//UI�\��
		void DrawUI_Base_Sub(void) noexcept  override {
			auto* DrawParts = DXDraw::Instance();
			DrawBox(0, 0, DrawParts->m_DispXSize, DrawParts->m_DispXSize, GetColor(218, 218, 218), TRUE);

			for (auto& w : m_WindowControl) {
				w->Draw();
			}

		}
	};
};