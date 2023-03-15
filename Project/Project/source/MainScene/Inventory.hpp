namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		enum class InventoryEnum {
			SelMode,
			CheckPreset,
			Set_Port,
			SetIP,
			SetTick,
			Matching,
			MainGame,
		};
		class InventoryClass {
			int						m_mousex{ 0 };
			int						m_mousey{ 0 };
			switchs					m_LeftClick;
			float					m_LeftPressTimer{ 0.f };

			float					m_Alpha{ 0.f };
		public:
			//const auto& GetIsClient(void) const noexcept { return this->m_IsClient; }
		public:
		public:
			void Init(void) noexcept {
				m_Alpha = 0.f;
			}
			void FirstExecute(void) {
				this->m_LeftClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
			}
			void LateExecute(void) noexcept {
			}
			void Draw(bool isDraw,const VECTOR_ref& MyPos) noexcept {
				Easing(&m_Alpha, isDraw ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				if (m_Alpha >= 0.1f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192.f*m_Alpha));
					DrawBox(0, 0, y_r(1920), y_r(1080), GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
				if (isDraw) {
					auto* ObjMngr = ObjectManager::Instance();
					//auto* PlayerMngr = PlayerManager::Instance();
					//auto* DrawParts = DXDraw::Instance();
					auto* Fonts = FontPool::Instance();
					//auto Red = GetColor(255, 0, 0);
					//auto Blue = GetColor(50, 50, 255);
					//auto Green = GetColor(43, 163, 91);
					//auto White = GetColor(255, 255, 255);
					//auto Gray = GetColor(64, 64, 64);
					//auto Black = GetColor(0, 0, 0);
					//unsigned int color = Red;

					int xp, yp, xs, ys;
					xp = y_r(1920 - 540 - 50);
					yp = y_r(50);
					xs = y_r(540);
					ys = y_r(30);

					//アイテム
					{
						if (GetJoypadNum() > 0) {
							DINPUT_JOYSTATE input;
							int padID = DX_INPUT_PAD1;
							GetJoypadInputState(padID);
							switch (GetJoypadType(padID)) {
							case DX_PADTYPE_OTHER:
							case DX_PADTYPE_DUAL_SHOCK_4:
							case DX_PADTYPE_DUAL_SENSE:
							case DX_PADTYPE_SWITCH_JOY_CON_L:
							case DX_PADTYPE_SWITCH_JOY_CON_R:
							case DX_PADTYPE_SWITCH_PRO_CTRL:
							case DX_PADTYPE_SWITCH_HORI_PAD:
								GetJoypadDirectInputState(DX_INPUT_PAD1, &input);
								{
									//m_mousex += std::clamp(-(float)(-input.Rz) / 100.f*1.f, -9.f, 9.f);
									//m_mousey += std::clamp((float)(input.Z) / 100.f*1.f, -9.f, 9.f);
								}
								break;
							case DX_PADTYPE_XBOX_360:
							case DX_PADTYPE_XBOX_ONE:
								break;
							default:
								break;
							}
						}
						else {//キーボード
							GetMousePoint(&m_mousex, &m_mousey);
						}

						bool M_In = false;
						std::string InfoStr;
						int loop = 0;
						while (true) {
							auto item = ObjMngr->GetObj(ObjType::Item, loop);
							if (item) {
								auto& ip = (std::shared_ptr<ItemClass>&)(*item);
								if (ip->IsActive()) {
									auto Len = (ip->GetMove().pos - MyPos).Length();
									if (Len <= 10.f*Scale_Rate) {
										ip->DrawItemData(xp, yp, xp + xs, yp + ys);
										if (in2_(m_mousex, m_mousey, xp, yp, xp + xs, yp + ys)) {
											M_In = true;
											InfoStr = ip->GetItemData()->GetInfo();
										}
										yp += ys + y_r(5);
									}
								}
							}
							else {
								break;
							}
							loop++;
						}
						if (M_In) {
							Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
								m_mousex, m_mousey, GetColor(255, 255, 255), GetColor(0, 0, 0), "%s", InfoStr.c_str());
						}
					}
				}
			}
			void Dispose(void) noexcept {
			}
		};
	};
};
