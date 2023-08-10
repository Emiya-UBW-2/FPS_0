#pragma once
#include	"Header.hpp"

class NewWorkControl {
	//通信関連
	int NetUDPHandle{ -1 };			// ネットワークハンドル
	IPDATA IP{ 127,0,0,1 };			// 送信用ＩＰアドレスデータ
	int UsePort{ -1 };				// 通信用ポート
	//サーバー専用
	IPDATA RecvIp{ 127,0,0,1 };		// 受信用ＩＰアドレスデータ
	int RecvPort{ 0 };				// 受信用ポート
public:
	void			InitClient() {
		// 送信用ソケットを作って初送信
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(-1);
		}
	}
	void			InitServer() {
		// ＵＤＰ通信用のソケットハンドルを作成
		if (NetUDPHandle == -1) {
			NetUDPHandle = MakeUDPSocket(UsePort);
		}
	}
	void			Dispose() {
		if (NetUDPHandle != -1) {
			DeleteUDPSocket(NetUDPHandle);	// ＵＤＰソケットハンドルの削除
			NetUDPHandle = -1;
			UsePort = -1;
		}
	}
public:
	void			SetIP(const IPDATA& pIP) { IP = pIP; }
	void			Set_Port(int PORT) { UsePort = PORT; }

	template<class T>
	void			SendtoServer(const T& Data) { Send(IP, UsePort, Data); }
	template<class T>
	void			SendtoClient(const T& Data) { Send(RecvIp, RecvPort, Data); }
	template<class T>
	void			Send(IPDATA& Ip, int SendPort, const T& Data) {
		if (NetUDPHandle != -1) {
			NetWorkSendUDP(NetUDPHandle, Ip, SendPort, &Data, sizeof(T));
			//printfDx("送信:[%d,%d,%d,%d][%d]\n", Ip.d1, Ip.d2, Ip.d3, Ip.d4, SendPort);
		}
	}
	template<class T>
	bool Recv(T* Data) {
		switch (CheckNetWorkRecvUDP(NetUDPHandle)) {
		case TRUE:
			NetWorkRecvUDP(NetUDPHandle, &RecvIp, &RecvPort, Data, sizeof(T), FALSE);		// 受信
			return true;
			break;
		case FALSE://待機
			break;
		default://error
			break;
		}
		return false;
	}
};

namespace FPS_n2 {
	namespace Sceneclass {
		//通信
		static const int		Player_num = 42;
		struct NewSetting {
			IPDATA					IP{ 127,0,0,1 };
			int						UsePort{ 10850 };
		};
		class NewWorkSetting {
			std::vector<NewSetting>	m_NewWorkSetting;
		public:
			void Load(void) noexcept {
				SetOutApplicationLogValidFlag(FALSE);
				int mdata = FileRead_open("data/NetWorkSetting.txt", FALSE);
				while (true) {
					m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
					m_NewWorkSetting.back().UsePort = std::clamp<int>(getparams::_int(mdata), 0, 50000);
					m_NewWorkSetting.back().IP.d1 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d2 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d3 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					m_NewWorkSetting.back().IP.d4 = (unsigned char)std::clamp((int)getparams::_int(mdata), 0, 255);
					if (FileRead_eof(mdata) != 0) {
						break;
					}
				}
				FileRead_close(mdata);
				SetOutApplicationLogValidFlag(TRUE);
			}
			void Save(void) noexcept {
				std::ofstream outputfile("data/NetWorkSetting.txt");
				for (auto& n : this->m_NewWorkSetting) {
					int ID = (int)(&n - &this->m_NewWorkSetting.front());
					outputfile << "Setting" + std::to_string(ID) + "_Port=" + std::to_string(n.UsePort) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP1=" + std::to_string(n.IP.d1) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP2=" + std::to_string(n.IP.d2) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP3=" + std::to_string(n.IP.d3) + "\n";
					outputfile << "Setting" + std::to_string(ID) + "_IP4=" + std::to_string(n.IP.d4) + "\n";
				}
				outputfile.close();
			}

			const auto		GetSize(void) const noexcept { return (int)m_NewWorkSetting.size(); }
			const auto&		Get(int ID) const noexcept { return this->m_NewWorkSetting[ID]; }
			const auto		Add(void) noexcept {
				m_NewWorkSetting.resize(this->m_NewWorkSetting.size() + 1);
				m_NewWorkSetting.back().UsePort = 10850;
				m_NewWorkSetting.back().IP.d1 = 127;
				m_NewWorkSetting.back().IP.d2 = 0;
				m_NewWorkSetting.back().IP.d3 = 0;
				m_NewWorkSetting.back().IP.d4 = 1;
				return (int)m_NewWorkSetting.size() - 1;;
			}
			void Set(int ID, const NewSetting& per)noexcept { this->m_NewWorkSetting[ID] = per; }
		};

		struct SendInfo {
			VECTOR_ref			m_Pos;
			VECTOR_ref			m_Vec;
			VECTOR_ref			m_rad;
			const DamageEvent*	m_Damage{ nullptr };
			unsigned long long	m_DamageSwitch{ 0 };
		};

		struct PlayerNetData {
		public:
			size_t			CheckSum{ 0 };		//8 * 1	=  8byte
			InputControl	Input;				//4 * 5	= 20byte
			VECTOR_ref		PosBuf;				//4 * 3	= 12byte
			VECTOR_ref		VecBuf;				//4 * 3	= 12byte
			VECTOR_ref		radBuf;				//4 * 3	= 12byte
			PlayerID		ID{ 0 };			//1	* 1	=  1byte
			char			IsActive{ 0 };		//1	* 1	=  1byte
			double			Frame{ 0.0 };		//8 * 1 =  8byte
			unsigned long long	DamageSwitch{ 0 };	//1 * 1 =  1byte
			DamageEvent		Damage;				//9 * 1 =  9byte
												//		  84byte
		public:
			const auto	CalcCheckSum(void) noexcept {
				return (
					((int)(PosBuf.x()*100.f) + (int)(PosBuf.y()*100.f) + (int)(PosBuf.z()*100.f)) +
					((int)(VecBuf.x()*100.f) + (int)(VecBuf.y()*100.f) + (int)(VecBuf.z()*100.f)) +
					(int)(rad2deg(radBuf.y())) +
					(int)(ID)
					);
			}

			const PlayerNetData operator+(const PlayerNetData& o) const noexcept {
				PlayerNetData tmp;

				tmp.ID = o.ID;
				tmp.Input = this->Input + o.Input;
				tmp.PosBuf = this->PosBuf + o.PosBuf;
				tmp.VecBuf = this->VecBuf + o.VecBuf;
				tmp.radBuf = this->radBuf + o.radBuf;

				return tmp;
			}
			const PlayerNetData operator-(const PlayerNetData& o) const noexcept {
				PlayerNetData tmp;

				tmp.ID = o.ID;
				tmp.Input = this->Input - o.Input;
				tmp.PosBuf = this->PosBuf - o.PosBuf;
				tmp.VecBuf = this->VecBuf - o.VecBuf;
				tmp.radBuf = this->radBuf - o.radBuf;

				return tmp;
			}
			const PlayerNetData operator*(float per) const noexcept {
				PlayerNetData tmp;

				tmp.ID = this->ID;
				tmp.Input = this->Input*per;
				tmp.PosBuf = this->PosBuf*per;
				tmp.VecBuf = this->VecBuf*per;
				tmp.radBuf = this->radBuf*per;
				return tmp;
			}
		};
		struct ServerNetData {
			int					Tmp1{ 0 };				//4
			int					StartFlag{ 0 };			//4
			size_t				ServerFrame{ 0 };		//8
			PlayerNetData		PlayerData[Player_num];	//37 * 3
		};
		class NetWorkControl {
		protected:
			size_t											m_ServerFrame{ 0 };
			std::array<int, Player_num>						m_LeapFrame{ 0 };
			ServerNetData									m_ServerDataCommon, m_PrevServerData;
			PlayerNetData									m_PlayerData;
			float											m_TickCnt{ 0.f };
			float											m_TickRate{ 10.f };
		public:
			const auto		GetRecvData(int pPlayerID) const noexcept { return this->m_LeapFrame[pPlayerID] <= 1; }
			const auto&		GetServerDataCommon(void) const noexcept { return this->m_ServerDataCommon; }
			const auto&		GetMyPlayer(void) const noexcept { return this->m_PlayerData; }
			void			SetMyPlayer(const InputControl& pInput, const VECTOR_ref& pPos, const VECTOR_ref& pVec, const VECTOR_ref& prad, double pFrame, const DamageEvent* pDamage, unsigned long long pDamageSwitch) noexcept {
				this->m_PlayerData.Input = pInput;
				this->m_PlayerData.PosBuf = pPos;
				this->m_PlayerData.VecBuf = pVec;
				this->m_PlayerData.radBuf = prad;
				this->m_PlayerData.Frame = pFrame;
				if (pDamage != nullptr) {
					this->m_PlayerData.Damage = *pDamage;
				}
				this->m_PlayerData.DamageSwitch = pDamageSwitch;
				this->m_PlayerData.CheckSum = (size_t)this->m_PlayerData.CalcCheckSum();
			}

			const auto		GetNowServerPlayerData(int pPlayerID, bool isyradReset) noexcept {
				auto Total = (int)this->m_ServerDataCommon.ServerFrame - (int)this->m_PrevServerData.ServerFrame;
				if (Total <= 0) { Total = 20; }
				auto Per = (float)this->m_LeapFrame[pPlayerID] / (float)Total;
				auto tmp = Lerp(this->m_PrevServerData.PlayerData[pPlayerID], m_ServerDataCommon.PlayerData[pPlayerID], Per);

				if (isyradReset) {
					auto radvec = Lerp(MATRIX_ref::RotY(this->m_PrevServerData.PlayerData[pPlayerID].radBuf.y()).zvec(), MATRIX_ref::RotY(this->m_ServerDataCommon.PlayerData[pPlayerID].radBuf.y()).zvec(), Per).Norm();
					tmp.radBuf.y(-atan2f(radvec.x(), radvec.z()));
				}
				tmp.Frame = this->m_ServerDataCommon.PlayerData[pPlayerID].Frame;
				tmp.Damage = this->m_ServerDataCommon.PlayerData[pPlayerID].Damage;
				tmp.DamageSwitch = this->m_ServerDataCommon.PlayerData[pPlayerID].DamageSwitch;
				this->m_LeapFrame[pPlayerID] = std::clamp<int>(this->m_LeapFrame[pPlayerID] + 1, 0, Total);
				return tmp;
			}
			virtual void	SetParam(int pPlayerID, const VECTOR_ref& pPos) noexcept {
				this->m_ServerDataCommon.PlayerData[pPlayerID].PosBuf = pPos;
				this->m_ServerDataCommon.ServerFrame = 0;
				this->m_PrevServerData.PlayerData[pPlayerID].PosBuf = this->m_ServerDataCommon.PlayerData[pPlayerID].PosBuf;	// サーバーデータ
				this->m_PrevServerData.ServerFrame = 0;
			}
		protected:
			void			CommonInit(void) noexcept {
				this->m_ServerFrame = 0;
			}
			void			NetCommonExecute(const ServerNetData& pData) {
				auto& tmpData = pData;
				if (this->m_ServerFrame <= tmpData.ServerFrame && ((tmpData.ServerFrame - this->m_ServerFrame) < 60)) {
					for (int i = 0; i < Player_num; i++) {
						this->m_LeapFrame[i] = 0;
					}
					this->m_PrevServerData = this->m_ServerDataCommon;
					this->m_ServerFrame = tmpData.ServerFrame;
					this->m_ServerDataCommon = tmpData;
				}
			}
		public:
			virtual void	Init(int, float, const IPDATA&) noexcept {}
			virtual bool	Execute(void) noexcept { return false; }
			virtual void	Dispose(void) noexcept {}
		};
		class ServerControl : public NetWorkControl {
			ServerNetData			m_ServerData;
			std::array<std::pair<NewWorkControl, int>, Player_num - 1>		m_NetWork;
		public:
			const auto&		GetServerData(void) const noexcept { return this->m_ServerData; }
			void			SetParam(int pPlayerID, const VECTOR_ref& pPos) noexcept override {
				NetWorkControl::SetParam(pPlayerID, pPos);
				this->m_ServerData.PlayerData[pPlayerID].PosBuf = this->m_ServerDataCommon.PlayerData[pPlayerID].PosBuf;
				this->m_ServerData.PlayerData[pPlayerID].IsActive = 0;
			}
		public:
			void			Init(int pPort, float pTick, const IPDATA&) noexcept override {
				CommonInit();
				int i = 0;
				for (auto & n : this->m_NetWork) {
					n.first.Set_Port(pPort + i); i++;
					n.first.InitServer();
					n.second = 0;
				}
				this->m_ServerDataCommon.ServerFrame = 0;

				this->m_ServerData.Tmp1 = 0;
				this->m_ServerData.StartFlag = 0;
				this->m_ServerData.PlayerData[0].IsActive = 1;
				this->m_ServerData.ServerFrame = 0;

				this->m_PlayerData.ID = 0;
				this->m_TickRate = pTick;
			}
			bool			Execute(void) noexcept override {
				bool canMatch = true;
				bool canSend = false;
				for (auto & n : this->m_NetWork) {
					if (!(n.second >= 2)) {
						canMatch = false;
						break;
					}
				}
				if (canMatch) {
					// ティックレート用演算
					this->m_TickCnt += 60.f / FPS;
					if (this->m_TickCnt > this->m_TickRate) {
						this->m_TickCnt -= this->m_TickRate;
						canSend = true;
					}
					//サーバーデータの更新
					this->m_ServerData.StartFlag = 1;
					this->m_ServerData.PlayerData[GetMyPlayer().ID] = this->m_PlayerData;		// サーバープレイヤーののプレイヤーデータ
					this->m_ServerData.ServerFrame++;											// サーバーフレーム更新
				}
				for (auto & n : this->m_NetWork) {
					size_t index = (&n - &this->m_NetWork.front()) + 1;
					int tmpData = -1;
					switch (n.second) {
					case 0:										//無差別受付
						if (n.first.Recv(&tmpData)) {			// 該当ソケットにクライアントからなにか受信したら
							n.second++;
						}
						break;
					case 1:
						this->m_ServerData.Tmp1 = (int)index;
						this->m_ServerData.StartFlag = 0;
						this->m_ServerData.PlayerData[index].IsActive = 1;

						n.first.SendtoClient(this->m_ServerData);					//クライアント全員に送る

						if (n.first.Recv(&tmpData)) {
							if (tmpData == 1) {					// ID取れたと識別出来たら
								n.second++;
							}
						}
						break;
					case 2://揃い待ち
						if (canMatch) { n.second++; }
						break;
					case 3:
					{
						PlayerNetData tmpPData;
						if (n.first.Recv(&tmpPData)) {							// クライアントから受信したら
							if (tmpPData.CheckSum == tmpPData.CalcCheckSum()) {
								this->m_ServerData.PlayerData[tmpPData.ID] = tmpPData;	// 更新
							}
						}
					}
					if (canSend) {
						n.first.SendtoClient(this->m_ServerData);						//クライアント全員に送る
					}
					break;
					default:
						break;
					}
				}
				if (canSend) {
					NetCommonExecute(this->m_ServerData);			// 更新
				}
				return canMatch;
			}
			void			Dispose(void) noexcept override {
				for (auto & n : this->m_NetWork) {
					n.first.Dispose();
				}
			}
		};
		class ClientControl : public NetWorkControl {
			int						m_NetWorkSel{ 0 };
			float					m_CannotConnectTimer{ 0.f };
			int						m_Port{ 0 };
			IPDATA					m_IP{ 127,0,0,1 };
			std::pair<NewWorkControl, int>	m_NetWork;
		public:
			void			Init(int pPort, float pTick, const IPDATA& pIP) noexcept override {
				CommonInit();

				this->m_CannotConnectTimer = 0.f;
				this->m_Port = pPort;
				this->m_TickRate = pTick;
				this->m_IP = pIP;

				this->m_NetWorkSel = 0;
				this->m_NetWork.first.Set_Port(this->m_Port + this->m_NetWorkSel);
				this->m_NetWork.first.SetIP(pIP);
				this->m_NetWork.first.InitClient();
				this->m_NetWork.second = 0;

				this->m_PlayerData.ID = 1;
			}
			bool			Execute(void) noexcept override {
				ServerNetData tmpData;
				bool canMatch = true;
				bool canSend = false;
				canMatch = (this->m_NetWork.second >= 2);
				if (canMatch) {
					// ティックレート用演算
					this->m_TickCnt += 60.f / FPS;
					if (this->m_TickCnt > this->m_TickRate) {
						this->m_TickCnt -= this->m_TickRate;
						canSend = true;
					}
				}
				switch (this->m_NetWork.second) {
				case 0:
					this->m_NetWork.first.SendtoServer(0);			// 通信リクエスト
					//サーバーからの自分のIDを受信
					if (this->m_NetWork.first.Recv(&tmpData)) {
						NetCommonExecute(tmpData);								//更新
						m_CannotConnectTimer = 0.f;
						if (tmpData.Tmp1 > 0) {
							this->m_PlayerData.ID = (PlayerID)tmpData.Tmp1;
							this->m_NetWork.second++;
						}
					}
					else {
						m_CannotConnectTimer += 1.f / FPS;
						if (this->m_CannotConnectTimer > 1.f) {
							m_CannotConnectTimer = 0.f;
							this->m_NetWork.first.Dispose();
							this->m_NetWorkSel++;
							this->m_NetWork.first.Set_Port(this->m_Port + this->m_NetWorkSel);
							this->m_NetWork.first.SetIP(this->m_IP);
							this->m_NetWork.first.InitClient();
							this->m_NetWork.second = 0;
							if (this->m_NetWorkSel >= Player_num) {
								//満タン
							}
						}
					}
					break;
				case 1:
					this->m_NetWork.first.SendtoServer(1);			// ID取れたよ
					//サーバーからのデータを受信したら次へ
					if (this->m_NetWork.first.Recv(&tmpData)) {
						NetCommonExecute(tmpData);								//更新
						if (tmpData.StartFlag == 1) {
							this->m_NetWork.second++;
						}
					}
					break;
				case 2:
					if (canSend) {
						this->m_NetWork.first.SendtoServer(this->m_PlayerData);				//自身のデータを送信
					}
					if (this->m_NetWork.first.Recv(&tmpData)) {					//サーバーからのデータを受信したら
						NetCommonExecute(tmpData);								//更新
					}
					break;
				default:
					break;
				}
				return canMatch;
			}
			void			Dispose(void) noexcept override {
				this->m_NetWork.first.Dispose();
			}
		};
	};
};