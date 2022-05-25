#include"DXLib_ref/DXLib_ref.h"

enum class MOVES {
	
};

class VECTOR_2D {
private:
	int x = 0;
	int y = 0;
public:
	//
	const auto& GetX() { return x; }
	const auto& GetY() { return y; }
	const VECTOR_2D& Get() { return *this; }

	void SetX(int value) { x = value; }
	void SetY(int value) { y = value; }
	void Set(int valuex, int valuey) {
		this->x = valuex;
		this->y = valuey;
	}
	//
	static const VECTOR_2D& Get(int valuex, int valuey) {
		VECTOR_2D Buf;
		Buf.SetX(valuex);
		Buf.SetY(valuey);
		return Buf;
	}

	const VECTOR_2D& operator+(const VECTOR_2D& value) { return Get(x + value.x, y + value.y); }
	const VECTOR_2D& operator-(const VECTOR_2D& value) { return Get(x - value.x, y - value.y); }
	void operator+=(const VECTOR_2D& value) { *this = *this + value; }
};

class SearchClass {
public:
	VECTOR_2D Min;
	VECTOR_2D Max;
	int Check_Cnt = 0;
	int Check_Time = 0;
};

class MainSeq {
	std::vector<std::pair<WORD,int>> Keys;
public:
	MainSeq(int disp_x,int disp_y) {
		SetOutApplicationLogValidFlag(false ? TRUE : FALSE);				/*log*/
		SetMainWindowText("PPAP");											/*タイトル*/
		ChangeWindowMode(TRUE);												/*窓表示*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
		SetGraphMode(disp_x, disp_y, 32);									/*解像度*/
		SetUseDirectInputFlag(TRUE);										/**/
		SetDirectInputMouseMode(TRUE);										/**/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*ウインドウサイズを手動不可、ウインドウサイズに合わせて拡大もしないようにする*/
		SetWaitVSyncFlag(FALSE);											/*垂直同期*/
		DxLib_Init();														/**/
		SetSysCommandOffFlag(TRUE);											/**/
		SetAlwaysRunFlag(TRUE);												/*background*/
		Keys.clear();
	}

	~MainSeq() {
		Keys.clear();
	}

	void KeyPress(WORD code) {
		bool ok = false;
		for (auto& k : Keys) {
			if (k.first == code) {
				ok = true;
				k.second = 2;
				break;
			}
		}
		if (!ok) {
			Keys.resize(Keys.size() + 1);
			Keys.back().first = code;
			Keys.back().second = 2;
		}

		INPUT input[1];
		input[0].type = INPUT_KEYBOARD;
		input[0].ki.wScan = MapVirtualKey(code, 0);
		input[0].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_EXTENDEDKEY;
		input[0].ki.time = 0;
		input[0].ki.dwExtraInfo = ::GetMessageExtraInfo();
		SendInput(1, input, sizeof(INPUT));
	}
	void Execute() {
		//
		if (Keys.size() > 0) {
			for (auto& k : Keys) {
				if (k.second<=0) {
					INPUT input[1];
					input[0].type = INPUT_KEYBOARD;
					input[0].ki.wScan = MapVirtualKey(k.first, 0);
					input[0].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
					input[0].ki.time = 0;
					input[0].ki.dwExtraInfo = ::GetMessageExtraInfo();
					SendInput(1, input, sizeof(INPUT));
					k.first = 0;
				}
				k.second--;
			}
			for (int i = 0; i < Keys.size(); i++) {
				if (Keys[i].first == 0) {
					Keys[i] = Keys.back();
					Keys.pop_back();
					i--;
				}
			}
		}
		//
	}
};


int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd) {
	int disp_x = 640;
	int disp_y = 480;

	int search_x = -1920;
	int search_y = 3;

	int scr_x = 1024;
	int scr_y = 764;

	std::array<float, 60> FPS_AVG{ 0 };
	int avg_cnt = 0;

	unsigned char KeyPush = 0;

	int cnt = 0;
	int cnt2 = 0;
	int cnt3 = 0;

	SearchClass PlayerCheck;
	
	std::vector<VECTOR_2D> PlayersVec;
	float PlayerRad = 0.f;
	float PlayerRad_Buf = 0.f;

	auto mainSeqence = std::make_unique< MainSeq>(disp_x, disp_y);

	GraphHandle Screen = GraphHandle::Make(scr_x, scr_y, false);

	// 変換したいグラフィックハンドルと同じサイズの「描画対象にできるグラフィックハンドル」を作成
	int map_xs = 245;
	int map_ys = 245;
	GraphHandle MapScreen = GraphHandle::Make(map_xs, map_ys, false);
	// 変換したいグラフィックハンドルと同じサイズの「CPUで扱うイメージ」を作成
	int SIHandle = MakeARGB8ColorSoftImage(map_xs, map_ys);


	while (ProcessMessage()==0){
		auto WaitTimer = GetNowHiPerformanceCount();
		GetDesktopScreenGraph(search_x, search_y, search_x + scr_x, search_y + scr_y, Screen.get());
		{
			if (cnt % 60 == 0) {
				++KeyPush %= 6;

			}
			{
				KeyPush = 0;
				if (CheckHitKey(KEY_INPUT_U) != 0) {
					KeyPush = 1;
				}
				if (CheckHitKey(KEY_INPUT_J) != 0) {
					KeyPush = 5;
				}
				if (CheckHitKey(KEY_INPUT_K) != 0) {
					KeyPush = 2;
				}
				if (CheckHitKey(KEY_INPUT_H) != 0) {
					KeyPush = 8;
				}
			}

			//方向操作
			if (cnt % 2 == 0) {
				if (CheckHitKey(KEY_INPUT_V) == 0) {
					KeyPush = std::clamp<unsigned char>(KeyPush, 0, 8);
					switch (KeyPush) {
					case 1://Front
						mainSeqence->KeyPress('W');
						break;
					case 2:
						if (cnt2 % 2 == 0) {
							mainSeqence->KeyPress('W');
						}
						else {
							mainSeqence->KeyPress('D');
						}
break;
					case 3:
						mainSeqence->KeyPress('D');
						break;
					case 4:
						if (cnt2 % 2 == 0) {
							mainSeqence->KeyPress('S');
						}
						else {
							mainSeqence->KeyPress('A');
						}
						break;
					case 5:
						mainSeqence->KeyPress('S');
						break;
					case 6:
						if (cnt2 % 2 == 0) {
							mainSeqence->KeyPress('S');
						}
						else {
							mainSeqence->KeyPress('D');
						}
						break;
					case 7:
						mainSeqence->KeyPress('A');
						break;
					case 8:
						if (cnt2 % 2 == 0) {
							mainSeqence->KeyPress('W');
						}
						else {
							mainSeqence->KeyPress('A');
						}
						break;
					default:
						break;
					}
					cnt2++;
				}
				mainSeqence->Execute();
			}
			//マウス操作
			else {
			int x = 0, y = 0;

			switch (KeyPush) {
			case 2:
			case 4:
				if (cnt3 % 3 == 0) {
					x += 3;
				}
				else {
					x += 5;
				}
				break;
			case 3:
				x += 4;
				break;
			case 6:
			case 8:
				if (cnt3 % 3 == 0) {
					x += -3;
				}
				else {
					x += -5;
				}
				break;
			case 7:
				x += -4;
				break;
			default:
				break;
			}
			if (CheckHitKey(KEY_INPUT_UP) != 0) {
				y += -10;
			}
			if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
				y += 10;
			}
			if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
				x += -10;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
				x += 10;
			}
			if (CheckHitKey(KEY_INPUT_B) == 0) {
				if (x != 0 || y != 0) {
					INPUT input[1];
					input[0].type = INPUT_MOUSE;
					input[0].mi.dx = x;
					input[0].mi.dy = y;
					input[0].mi.dwFlags = MOUSEEVENTF_MOVE;
					input[0].mi.time = 0;
					input[0].mi.dwExtraInfo = ::GetMessageExtraInfo();
					SendInput(1, input, sizeof(INPUT));
				}
			}
			cnt3++;
			}
		}
		{
			if (CheckHitKey(KEY_INPUT_UP) != 0) {
				search_y -= 1;
			}
			if (CheckHitKey(KEY_INPUT_DOWN) != 0) {
				search_y += 1;
			}
			if (CheckHitKey(KEY_INPUT_LEFT) != 0) {
				search_x -= 1;
			}
			if (CheckHitKey(KEY_INPUT_RIGHT) != 0) {
				search_x += 1;
			}
		}
		if (cnt % 30 == 0) {
			// 「描画対象にできるグラフィックハンドル」を描画対象に変更
			MapScreen.SetDraw_Screen();
			{
				// 変換したいグラフィックハンドルを描画
				// 描画元の色値をそのまま書き込むためのブレンドモード DX_BLENDMODE_SRCCOLOR を使用する
				SetDrawBlendMode(DX_BLENDMODE_SRCCOLOR, 255);
				int map_xp = 0;
				int map_yp = scr_y - map_ys;
				DrawRectGraph(
					0, 0,
					map_xp, map_yp,
					map_xs, map_ys, Screen.get(), FALSE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
			}
			// 「CPUで扱うイメージ」に「描画対象にできるグラフィックハンドル」の内容を取り込み
			GetDrawScreenSoftImage(0, 0, map_xs, map_ys, SIHandle);
		}
		if (cnt % 6 == 1 || (PlayerCheck.Check_Cnt == 0)) {
			int r, g, b;

#define COLORCHECK(R,G,B,buf) ((R - buf <= r && r <= R + buf) && (G - buf <= g && g <= G + buf) && (B - buf <= b && b <= B + buf))
			int col_buf = 5;

			switch (PlayerCheck.Check_Cnt) {
			case 0:
				PlayerCheck.Min.Set(0, 0);
				PlayerCheck.Max.Set(map_xs, map_ys);
				break;
			case 1:
				PlayerCheck.Min = PlayersVec.back() - VECTOR_2D::Get(10, 10);
				PlayerCheck.Max = PlayersVec.back() + VECTOR_2D::Get(10, 10);
				break;
			case 2:
				PlayerCheck.Min = PlayersVec.back() - VECTOR_2D::Get(8, 8);
				PlayerCheck.Max = PlayersVec.back() + VECTOR_2D::Get(8, 8);
				break;
			case 3:
				PlayerCheck.Min = PlayersVec.back() - VECTOR_2D::Get(5, 5);
				PlayerCheck.Max = PlayersVec.back() + VECTOR_2D::Get(5, 5);
				break;
			default:
				break;
			}

			if (cnt % 30 == 1) {
				PlayerCheck.Check_Cnt = std::max(PlayerCheck.Check_Cnt - 1, 0);
			}

			for (int X = PlayerCheck.Min.GetX(); X < PlayerCheck.Max.GetX(); X++) {
				for (int Y = PlayerCheck.Min.GetY(); Y < PlayerCheck.Max.GetY(); Y++) {
					GetPixelSoftImage(SIHandle, X, Y, &r, &g, &b, nullptr);
					if ((PlayerCheck.Check_Cnt != 3) && COLORCHECK(211, 248, 244, col_buf)) {
						PlayerCheck.Check_Cnt = 3;
						PlayersVec.resize(PlayersVec.size() + 1);
						PlayersVec.back().SetX(X);
						PlayersVec.back().SetY(Y);
					}
				}
			}

		}
		if (PlayerCheck.Check_Cnt == 0) {
			PlayerCheck.Check_Time++;
			if (PlayerCheck.Check_Time >= 6 * 30) {
				PlayerCheck.Check_Time = 0;
				PlayersVec.clear();
			}
		}
		else {
			PlayerCheck.Check_Time = 0;
		}
		//
		SetDrawScreen(DX_SCREEN_BACK);
		ClearDrawScreen();
		{
			Screen.DrawExtendGraph(disp_x - scr_x* disp_y / scr_y, 0, disp_x, scr_y * disp_y / scr_y, false);
			//ミニマップ
			{
				int DrawMapx = 0;
				int DrawMapy = disp_y - map_ys;
				MapScreen.DrawGraph(DrawMapx, DrawMapy, false);
				if (PlayersVec.size() > 1) {
					for (int i = 0; i < PlayersVec.size() - 1; i++) {
						DrawLine(
							DrawMapx + PlayersVec[i].GetX(), DrawMapy + PlayersVec[i].GetY(),
							DrawMapx + PlayersVec[i + 1].GetX(), DrawMapy + PlayersVec[i + 1].GetY(),
							GetColor(255, 255, 0), 2);
					}
				}
				if (PlayersVec.size() > 4) {
					{
						int i = PlayersVec.size() - 1;

						auto P = PlayersVec[i] - PlayersVec[i - 4];

						float Spd_Buf = std::hypotf((float)P.GetX(), (float)P.GetY());
						float Rad_Buf = std::atan2f((float)P.GetX(), (float)P.GetY());

						if (Spd_Buf > 3) {
							PlayerRad_Buf = Rad_Buf;
						}
						easing_set(&PlayerRad, PlayerRad_Buf, 0.9f);
						DrawLine(
							DrawMapx + PlayersVec[i].GetX(), DrawMapy + PlayersVec[i].GetY(),
							DrawMapx + PlayersVec[i].GetX() + (int)(100.f*sin(PlayerRad)), DrawMapy + PlayersVec[i].GetY() + (int)(100.f*cos(PlayerRad)),
							GetColor(0, 255, 0), 2);
					}
				}
				{
					DrawBox(
						DrawMapx + PlayerCheck.Min.GetX(), DrawMapy + PlayerCheck.Min.GetY(),
						DrawMapx + PlayerCheck.Max.GetX(), DrawMapy + PlayerCheck.Max.GetY(),
						GetColor(0, 255, 0), FALSE
					);
					//
				}
			}
			//エイム箇所
			{
				int map_xs2 = 100;
				int map_ys2 = scr_y / 3;
				int map_xp = scr_x / 2 - map_xs2 / 2;
				int map_yp = scr_y / 2 - map_ys2 / 2;
				DrawRectGraph(
					(disp_x - map_xs2) / 2, (disp_y - map_ys2) / 2,
					map_xp, map_yp,
					map_xs2, map_ys2, Screen.get(), FALSE);
				DrawBox(
					(disp_x - map_xs2) / 2, (disp_y - map_ys2) / 2,
					(disp_x - map_xs2) / 2 + map_xs2, (disp_y - map_ys2) / 2 + map_ys2,
					GetColor(255, 0, 0),
					FALSE);

				//211, 248, 244;
				//253,  13,  22;
			}
			clsDx();
			{
				FPS_AVG[avg_cnt] = GetFPS();
				float total = 0;
				for (const auto& f : FPS_AVG) {
					total += f;
				}
				printfDx("%5.2f FPS(AVG : %5.2f)\n", FPS_AVG[avg_cnt], total/60.f);
				++avg_cnt %= 60;
			}
			{
				printfDx("(%d, %d)\n", search_x, search_y);
			}
			printfDx("aaa\n");
		}
		ScreenFlip();
		while ((GetNowHiPerformanceCount() - WaitTimer) <= (1000 * 1000 / 60)) {}
		cnt++;
		if (CheckHitKey(KEY_INPUT_ESCAPE) != 0) { break; }
	}
	DxLib_End();        // ＤＸライブラリ使用の終了処理
	return 0;        // ソフトの終了
}
