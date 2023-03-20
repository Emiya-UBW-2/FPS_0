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
			switchs					m_Rkey;
			switchs					m_Deletekey;

			float					m_Alpha{ 0.f };

			const std::shared_ptr<CellItem>* m_DragIn{ nullptr };
			const std::shared_ptr<ItemClass>* m_DragOut{ nullptr };
			const std::shared_ptr<ItemData>* m_Drag{ nullptr };
			bool Is90{ false };
		public:
			//const auto& GetIsClient(void) const noexcept { return this->m_IsClient; }
		public:
		public:
			void Init(void) noexcept {
				m_Alpha = 0.f;
			}
			void FirstExecute(void) {
				this->m_LeftClick.Execute((GetMouseInputWithCheck() & MOUSE_INPUT_LEFT) != 0);
				this->m_Rkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_R) != 0);
				this->m_Deletekey.Execute(CheckHitKeyWithCheck(KEY_INPUT_DELETE) != 0);
			}
			void LateExecute(void) noexcept {
			}
			void Draw(bool isDraw, const VECTOR_ref& MyPos) noexcept {
				Easing(&m_Alpha, isDraw ? 1.f : 0.f, 0.9f, EasingType::OutExpo);
				if (m_Alpha >= 0.1f) {
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, (int)(192.f*m_Alpha));
					DrawBox(0, 0, y_r(1920), y_r(1080), GetColor(0, 0, 0), TRUE);
					SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
				}
				if (isDraw) {
					auto* ObjMngr = ObjectManager::Instance();
					auto* PlayerMngr = PlayerManager::Instance();//todo:GetMyPlayerID()
					//auto* DrawParts = DXDraw::Instance();
					auto* Fonts = FontPool::Instance();
					auto Red = GetColor(255, 0, 0);
					//auto Blue = GetColor(50, 50, 255);
					auto Green = GetColor(0, 255, 0);
					auto White = GetColor(255, 255, 255);
					auto Gray = GetColor(128, 128, 128);
					auto Yellow = GetColor(255, 255, 0);
					auto Black = GetColor(0, 0, 0);

					int xp{ 0 }, yp{ 0 }, xs{ 0 }, ys{ 0 };

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
					//インベントリ
					int size = y_r(64);
					{
						bool isHit = false;
						for (auto& I : PlayerMngr->GetPlayer(0).GetInventorys()) {
							int loop = (int)(&I - &PlayerMngr->GetPlayer(0).GetInventorys().front());
							switch (loop) {
							case 0:
								xp = y_r(24);
								yp = y_r(24);
								break;
							case 1:
								xp = y_r(24 + 150 + 10);
								yp = y_r(442);
								break;
							case 2:
								xp = y_r(24 + 24);
								yp = y_r(424);
								break;
							case 3:
								xp = y_r(24 + 640 - 24 - 128);
								yp = y_r(424);
								break;
							case 4:
								xp = y_r(24 + 640 + 24);
								yp = y_r(24);
								break;
							default:
								break;
							}
							xs = (int)I.size() * size;
							ys = (int)I.back().size() * size;
							for (auto& xo : I) {
								int x = (int)(&xo - &I.front());
								for (auto& yo : xo) {
									int y = (int)(&yo - &xo.front());
									if (yo.get()) {
										if (m_DragIn && (*m_DragIn).get()) {
											if (yo == (*m_DragIn)) {
												auto xsize = this->Is90 ? (*m_Drag)->GetYsize() : (*m_Drag)->GetXsize();
												auto ysize = this->Is90 ? (*m_Drag)->GetXsize() : (*m_Drag)->GetYsize();
												DrawBox(xp + x * size, yp + y * size, xp + (x + xsize) * size, yp + (y + ysize) * size, Yellow, TRUE);
											}
										}
									}
								}
							}
							for (auto& xo : I) {
								int x = (int)(&xo - &I.front());
								for (auto& yo : xo) {
									int y = (int)(&yo - &xo.front());
									DrawBox(xp + x * size, yp + y * size, xp + (x + 1) * size, yp + (y + 1) * size, Gray, FALSE);
								}
							}
							for (auto& xo : I) {
								int x = (int)(&xo - &I.front());
								for (auto& yo : xo) {
									int y = (int)(&yo - &xo.front());
									if (yo.get()) {
										if (m_DragIn && (*m_DragIn).get()) {
											if (yo == (*m_DragIn)) {
												continue;
											}
										}
										yo->Draw(xp + x * size, yp + y * size);
									}
								}
							}
							DrawBox(xp, yp, xp + xs, yp + ys, White, FALSE);
							{
								for (auto& xo : I) {
									int x = (int)(&xo - &I.front());
									for (auto& yo : xo) {
										int y = (int)(&yo - &xo.front());
										if (in2_(m_mousex, m_mousey, xp + x * size, yp + y * size, xp + (x + 1) * size, yp + (y + 1) * size)) {
											{
												const auto* Ptr = PlayerMngr->GetPlayer(0).GetInventory(loop, x, y);
												if (Ptr) {
													if (!m_Drag) {
														if (this->m_Deletekey.trigger()) {
															//アイテム放出
															auto& item = *ObjMngr->AddObject(ObjType::Item, "data/model/item/");
															auto Vec = (MATRIX_ref::RotX(GetRandf(deg2rad(90)))*MATRIX_ref::RotY(GetRandf(deg2rad(360)))).yvec()*Scale_Rate;
															item->SetMove(
																MATRIX_ref::RotVec2(VECTOR_ref::up(), Vec.Norm()),
																PlayerMngr->GetPlayer(0).GetPos() + Vec * 6.f,
																Vec*25.f / FPS * (1.0f*GetRandf(0.5f)));

															auto& ip = (std::shared_ptr<ItemClass>&)(item);
															ip->SetData((*Ptr)->GetItemData());
															PlayerMngr->GetPlayer(0).DeleteInventory(*Ptr);
														}
														else if (this->m_LeftClick.trigger()) {
															m_Drag = &(*Ptr)->GetItemData();
															m_DragIn = Ptr;
															Is90 = (*m_DragIn)->GetIs90();
														}
														HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
														SetCursor(hCursor);
													}
												}
											}
											if (m_Drag) {
												auto xsize = this->Is90 ? (*m_Drag)->GetYsize() : (*m_Drag)->GetXsize();
												auto ysize = this->Is90 ? (*m_Drag)->GetXsize() : (*m_Drag)->GetYsize();
												if (PlayerMngr->GetPlayer(0).CanPutInventory(loop, x, y, xsize, ysize, m_DragIn)) {
													isHit = true;
													DrawBox(xp + x * size, yp + y * size, xp + (x + xsize) * size, yp + (y + ysize) * size, Green, TRUE);
													if (!this->m_LeftClick.press()) {
														PlayerMngr->GetPlayer(0).PutInventory(loop, x, y, *m_Drag, Is90);
														if (m_DragIn && (*m_DragIn).get() && (yo != (*m_DragIn))) {
															PlayerMngr->GetPlayer(0).DeleteInventory(*m_DragIn);
														}
														if (m_DragOut && (*m_DragOut).get()) { (*m_DragOut)->SetIsDelete(true); }
														m_DragIn = nullptr;
														m_DragOut = nullptr;
														m_Drag = nullptr;
														Is90 = false;
													}
												}
											}
										}
									}
								}
							}
						}
						if (m_Drag && !isHit && !this->m_LeftClick.press()) {
							m_DragIn = nullptr;
							m_DragOut = nullptr;
							m_Drag = nullptr;
							Is90 = false;
						}
					}
					//アイテム
					{
						xp = y_r(1920 - 540 - 24);
						yp = y_r(24);
						xs = y_r(540);
						ys = y_r(30);

						DrawBox(xp, y_r(24), xp + xs, y_r(1080 - 24), White, FALSE);

						{
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
											ip->DrawItemData(xp + 2, yp + 2, xp + xs - 2, yp + ys - 2);
											if (!m_Drag && in2_(m_mousex, m_mousey, xp + 2, yp + 2, xp + xs - 2, yp + ys - 2)) {
												DrawBox(xp + 2, yp + 2, xp + xs - 2, yp + ys - 2, Red, FALSE);
												M_In = true;
												InfoStr = ip->GetItemData()->GetInfo();
												if (this->m_LeftClick.trigger()) {
													m_Drag = &ip->GetItemData();
													m_DragOut = &ip;
												}
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
									m_mousex, m_mousey, White, Black, "%s", InfoStr.c_str());
							}
						}

						if (m_Drag) {
							auto xsize = this->Is90 ? (*m_Drag)->GetYsize() : (*m_Drag)->GetXsize();
							auto ysize = this->Is90 ? (*m_Drag)->GetXsize() : (*m_Drag)->GetYsize();

							(*m_Drag)->GetSlotPic().DrawRotaGraph(m_mousex + (xsize - 1) * size / 2, m_mousey + (ysize - 1) * size / 2, (float)size / 64.f, deg2rad(Is90 ? 90.f : 0.f), false);
							HCURSOR hCursor = LoadCursor(NULL, IDC_HAND);
							SetCursor(hCursor);
							if (this->m_Rkey.trigger()) {
								Is90 ^= 1;
							}
						}
					}
				}
			}
			void Dispose(void) noexcept {
			}
		};
	};
};
