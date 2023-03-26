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
			switchs					m_LCtrlkey;
			switchs					m_LAltkey;

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
				this->m_LCtrlkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LCONTROL) != 0);
				this->m_LAltkey.Execute(CheckHitKeyWithCheck(KEY_INPUT_LALT) != 0);
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
					auto& Player = PlayerMngr->GetPlayer(0);
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
						bool M_In = false;
						std::string InfoStr;
						bool isHit = false;
						for (auto& I : Player.GetInventorys()) {
							int loop = (int)(&I - &Player.GetInventorys().front());
							switch (loop) {
							case 0:
								xp = y_r(24) + (10 - Player.GetInventoryXSize(loop))*size / 2;
								yp = y_r(24) + (6 - Player.GetInventoryYSize(loop))*size / 2;
								break;
							case 1:
								xp = y_r(24 + 150 + 10) + (5 - Player.GetInventoryXSize(loop))*size / 2;
								yp = y_r(442);
								break;
							case 2:
								xp = y_r(24 + 24) + (2 - Player.GetInventoryXSize(loop))*size / 2;
								yp = y_r(424);
								break;
							case 3:
								xp = y_r(24 + 640 - 24 - 128) + (2 - Player.GetInventoryXSize(loop))*size / 2;
								yp = y_r(424);
								break;
							case 4:
								xp = y_r(24 + 640 + 24);
								yp = y_r(24);
								break;
							default:
								break;
							}
							for (auto& xo : I) {
								int x = (int)(&xo - &I.front());
								for (auto& yo : xo) {
									int y = (int)(&yo - &xo.front());
									if (yo.get()) {
										if (m_Drag) {
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
							DrawBox(xp, yp, xp + Player.GetInventoryXSize(loop) * size, yp + Player.GetInventoryYSize(loop) * size, White, FALSE);
							{
								for (auto& xo : I) {
									int x = (int)(&xo - &I.front());
									for (auto& yo : xo) {
										int y = (int)(&yo - &xo.front());
										if (in2_(m_mousex, m_mousey, xp + x * size, yp + y * size, xp + (x + 1) * size, yp + (y + 1) * size)) {


											{
												const auto* Ptr = Player.GetInventory(loop, x, y);
												if (Ptr) {
													if (!m_Drag) {
														M_In = true;
														InfoStr = (*Ptr)->GetItemData()->GetInfo();

														if (this->m_Deletekey.trigger()) {
															//アイテム放出
															auto& item = *ObjMngr->AddObject(ObjType::Item, "data/model/item/");
															auto Vec = (MATRIX_ref::RotX(GetRandf(deg2rad(90)))*MATRIX_ref::RotY(GetRandf(deg2rad(360)))).yvec()*Scale_Rate;
															item->SetMove(
																MATRIX_ref::RotVec2(VECTOR_ref::up(), Vec.Norm()),
																Player.GetPos() + Vec * 6.f,
																Vec*25.f / FPS * (1.0f*GetRandf(0.5f)));

															auto& ip = (std::shared_ptr<ItemClass>&)(item);
															ip->SetData((*Ptr)->GetItemData(), (*Ptr)->GetCount());
															Player.DeleteInventory(*Ptr);
															if (loop == 2) {
																const auto* Ptrt = Player.GetInventory(loop, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
																if (!Ptrt) {
																	auto& Vehicle = Player.GetVehicle();
																	Vehicle->ClashParts(Vehicle->Get_module_mesh()[0]);
																}
															}
															else if (loop == 3) {
																const auto* Ptrt = Player.GetInventory(loop, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
																if (!Ptrt) {
																	auto& Vehicle = Player.GetVehicle();
																	Vehicle->ClashParts(Vehicle->Get_module_mesh()[1]);
																}
															}
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
												if (Player.CanPutInventory(loop, x, y, xsize, ysize, m_Drag, m_DragIn)) {
													isHit = true;
													DrawBox(xp + x * size, yp + y * size, xp + (x + xsize) * size, yp + (y + ysize) * size, Green, TRUE);
													if (!this->m_LeftClick.press()) {
														if (m_DragIn && (*m_DragIn).get() && (yo != (*m_DragIn))) {
															Player.PutInventory(loop, x, y, *m_Drag, (*m_DragIn)->GetCount(), Is90);
															if (loop == 2) {
																auto& Vehicle = Player.GetVehicle();
																Vehicle->RepairParts(Vehicle->Get_module_mesh()[0]);
															}
															else if (loop == 3) {
																auto& Vehicle = Player.GetVehicle();
																Vehicle->RepairParts(Vehicle->Get_module_mesh()[1]);
															}

															auto IDDel = (*m_DragIn)->GetSlotID();

															Player.DeleteInventory(*m_DragIn);

															if (IDDel == 2) {
																const auto* Ptrt = Player.GetInventory(IDDel, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
																if (!Ptrt) {
																	auto& Vehicle = Player.GetVehicle();
																	Vehicle->ClashParts(Vehicle->Get_module_mesh()[0]);
																}
															}
															else if (IDDel == 3) {
																const auto* Ptrt = Player.GetInventory(IDDel, [&](const std::shared_ptr<CellItem>& tgt) { return tgt.get(); });
																if (!Ptrt) {
																	auto& Vehicle = Player.GetVehicle();
																	Vehicle->ClashParts(Vehicle->Get_module_mesh()[1]);
																}
															}


															m_DragIn = nullptr;
														}
														else if (m_DragOut && (*m_DragOut).get()) {
															Player.PutInventory(loop, x, y, *m_Drag, (*m_DragOut)->GetCount(), Is90);
															if (loop == 2) {
																auto& Vehicle = Player.GetVehicle();
																Vehicle->RepairParts(Vehicle->Get_module_mesh()[0]);
															}
															else if (loop == 3) {
																auto& Vehicle = Player.GetVehicle();
																Vehicle->RepairParts(Vehicle->Get_module_mesh()[1]);
															}
															(*m_DragOut)->SetIsDelete(true);
															m_DragOut = nullptr;
														}
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
						if (!isHit && !this->m_LeftClick.press()) {
							m_DragIn = nullptr;
							m_DragOut = nullptr;
							m_Drag = nullptr;
							Is90 = false;
						}
						if (M_In) {
							Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
								m_mousex, m_mousey, White, Black, "%s", InfoStr.c_str());
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
							struct Items {
								std::vector<const std::shared_ptr<ItemClass>*> Ptr;
								int ypos;
								int count;
							};
							std::vector<Items> Ntuple;

							bool M_In = false;
							std::string InfoStr;
							{
								int loop = 0;
								while (true) {
									auto item = ObjMngr->GetObj(ObjType::Item, loop);
									if (item) {
										auto& ip = (std::shared_ptr<ItemClass>&)(*item);
										if (ip->IsActive()) {
											auto Len = (ip->GetMove().pos - MyPos).Length();
											if (Len <= 10.f*Scale_Rate) {

												auto Check = std::find_if(Ntuple.begin(), Ntuple.end(), [&](const Items& tgt) {
													return (*tgt.Ptr[0])->GetItemData() == ip->GetItemData();
												});

												if (Check == Ntuple.end()) {
													ip->DrawItemData(xp + 2, yp + 2, xp + xs - 2, yp + ys - 2);
													Ntuple.resize(Ntuple.size() + 1);
													Ntuple.back().Ptr.emplace_back(&ip);
													Ntuple.back().ypos = yp;
													Ntuple.back().count = 1;
													yp += ys + y_r(5);
												}
												else {
													Check->Ptr.emplace_back(&ip);
													Check->count++;
												}
											}
										}
									}
									else {
										break;
									}
									loop++;
								}
							}
							for (auto& n : Ntuple) {
								yp = n.ypos;
								if (n.count > 1) {
									Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(24), FontHandle::FontXCenter::RIGHT, FontHandle::FontYCenter::MIDDLE,
										xp + xs - 2, yp + ys / 2, GetColor(255, 255, 255), GetColor(0, 0, 0), "x%d", n.count);
								}
								if (!m_Drag && in2_(m_mousex, m_mousey, xp + 2, yp + 2, xp + xs - 2, yp + ys - 2)) {
									DrawBox(xp + 2, yp + 2, xp + xs - 2, yp + ys - 2, Red, FALSE);
									M_In = true;
									InfoStr = (*n.Ptr[0])->GetItemData()->GetInfo();
									if (this->m_LeftClick.trigger()) {
										const std::shared_ptr<ItemClass>* Buff = n.Ptr[0];//一番キャパが多いものを選ぶ
										for (auto& p : n.Ptr) {
											if ((*Buff)->GetCount() < (*p)->GetCount()) {
												Buff = p;
											}
										}
										if (this->m_LCtrlkey.press()) {
											bool isHit = false;
											for (int y = 0; y < Player.GetInventoryYSize(4); y++) {
												for (int x = 0; x < Player.GetInventoryXSize(4); x++) {
													if (Player.CanPutInventory(4, x, y, (*n.Ptr[0])->GetItemData()->GetXsize(), (*n.Ptr[0])->GetItemData()->GetYsize(), &(*n.Ptr[0])->GetItemData())) {
														Player.PutInventory(4, x, y, (*n.Ptr[0])->GetItemData(), (*Buff)->GetCount(), false);
														(*Buff)->SetIsDelete(true);
														isHit = true;
														break;
													}
												}
												if (isHit) { break; }
											}
										}
										else if (this->m_LAltkey.press()) {
											for (auto& p : n.Ptr) {
												bool isHit = false;
												for (int y = 0; y < Player.GetInventoryYSize(4); y++) {
													for (int x = 0; x < Player.GetInventoryXSize(4); x++) {
														if (Player.CanPutInventory(4, x, y, (*p)->GetItemData()->GetXsize(), (*p)->GetItemData()->GetYsize(), &(*p)->GetItemData())) {
															Player.PutInventory(4, x, y, (*p)->GetItemData(), (*p)->GetCount(), false);
															(*p)->SetIsDelete(true);
															isHit = true;
															break;
														}
													}
													if (isHit) { break; }
												}
											}
										}
										else {
											m_Drag = &(*n.Ptr[0])->GetItemData();
											m_DragOut = Buff;
										}
										//
									}
								}
							}

							if (M_In) {
								Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
									m_mousex + y_r(32), m_mousey, White, Black, "%s", InfoStr.c_str());
								if (this->m_LCtrlkey.press()) {
									Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
										m_mousex + y_r(32), m_mousey + y_r(20), White, Black, "車外に搭載する");
								}
								else if (this->m_LAltkey.press()) {
									Fonts->Get(FontPool::FontType::Nomal_EdgeL).DrawString(y_r(20), FontHandle::FontXCenter::LEFT, FontHandle::FontYCenter::TOP,
										m_mousex + y_r(32), m_mousey + y_r(20), White, Black, "まとめて車外に搭載する");
								}
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
