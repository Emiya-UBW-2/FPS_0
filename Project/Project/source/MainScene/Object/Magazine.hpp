#pragma once
#include	"../../Header.hpp"

namespace FPS_n2 {
	namespace Sceneclass {
		class MagazineClass : public ObjectBaseClass {
		private:
			std::array<std::shared_ptr<CartClass>, 2>	m_Cart;
			int											m_Capacity{ 0 };
			int											m_CapacityMax{ 0 };
			MATRIX_ref									HandMatrix;
			float										HandPer{ 0.f };
			std::vector<AmmoData>						m_AmmoSpec;
			RELOADTYPE									m_ReloadTypeBuf{ RELOADTYPE::MAG };
		public://ƒQƒbƒ^[
			void			SetHandMatrix(const MATRIX_ref& value, float pPer, RELOADTYPE ReloadType) {
				this->HandMatrix = value;
				this->HandPer = pPer;
				this->m_ReloadTypeBuf = ReloadType;
			}

			void			SetAmmo(int value) { this->m_Capacity = std::clamp(value, 0, this->m_CapacityMax); }
			void			SubAmmo(void) { SetAmmo(this->m_Capacity - 1); }
			void			AddAmmo(void) { SetAmmo(this->m_Capacity + 1); }
			const auto		IsEmpty(void) const noexcept { return this->m_Capacity == 0; }
			const auto		IsFull(void) const noexcept { return this->m_Capacity == this->m_CapacityMax; }
			const auto*		GetAmmoSpec(void) const noexcept { return &(this->m_AmmoSpec[0]); }
			const auto&		GetAmmoNum(void) const noexcept { return this->m_Capacity; }
			const auto&		GetAmmoAll(void) const noexcept { return  this->m_CapacityMax; }

			void			SetCartPtr(void);
		public:
			MagazineClass(void) { this->m_objType = ObjType::Magazine; }
			~MagazineClass(void) { }
		public:
			void			Init(void) override {
				ObjectBaseClass::Init();
				{
					int mdata = FileRead_open((this->m_FilePath + "data.txt").c_str(), FALSE);
					this->m_CapacityMax = (HitPoint)getparams::_int(mdata);		//‘’e”
					while (true) {
						auto stp = getparams::Getstr(mdata);
						if (stp.find("useammo" + std::to_string(this->m_AmmoSpec.size())) == std::string::npos) {
							break;
						}
						this->m_AmmoSpec.resize(this->m_AmmoSpec.size() + 1);
						this->m_AmmoSpec.back().Set("data/ammo/", getparams::getright(stp));
						//"data/ammo/"+ getparams::getright(stp)
					}
					FileRead_close(mdata);
				}
				this->m_Capacity = this->m_CapacityMax;
				SetCartPtr();
			}
			void			FirstExecute(void) override {
				{
					switch (m_ReloadTypeBuf) {
					case RELOADTYPE::MAG:
					{
						{
							auto tmp = GetMove();
							SetMove(
								tmp.mat.GetRot(),
								Lerp(tmp.pos, this->HandMatrix.pos(), this->HandPer));
						}
						{
							this->m_Cart[0]->SetActive(0 < this->m_Capacity);
							auto mat = GetObj().GetFrameLocalWorldMatrix(1);
							this->m_Cart[0]->SetMove(MATRIX_ref::RotVec2(VECTOR_ref::front(), mat.GetRot().zvec()), mat.pos());
						}
						{
							this->m_Cart[1]->SetActive(1 < this->m_Capacity);
							auto mat = GetObj().GetFrameLocalWorldMatrix(2);
							auto mat2 = GetObj().GetFrameLocalWorldMatrix(1);
							this->m_Cart[1]->SetMove(mat.GetRot(), mat.pos());
						}
					}
					break;
					case RELOADTYPE::AMMO:
					{
						//‚±‚¢‚Â‚ðe‘¤‚É•t‚¯‚é
						{
							this->m_Cart[0]->SetActive(0 < this->m_Capacity);
							auto mat = GetObj().GetFrameLocalWorldMatrix(1);
							this->m_Cart[0]->SetMove(
								MATRIX_ref::RotVec2(VECTOR_ref::front(), Lerp(mat.GetRot().zvec(), this->HandMatrix.zvec(), this->HandPer)),
								Lerp(mat.pos(), this->HandMatrix.pos(), this->HandPer));
						}
						//‚±‚¢‚Â‚Í‚»‚Ì‚Ü‚Ü
						{
							this->m_Cart[1]->SetActive(1 < this->m_Capacity);
							auto mat = GetObj().GetFrameLocalWorldMatrix(2);
							auto mat2 = GetObj().GetFrameLocalWorldMatrix(1);
							this->m_Cart[1]->SetMove(
								mat.GetRot(),
								Lerp(mat.pos(), mat2.pos(), this->HandPer)
							);
						}
					}
						break;
					default:
						break;
					}
				}
				//‹¤’Ê
				ObjectBaseClass::FirstExecute();
			}
			void			Dispose(void) override {
				this->m_AmmoSpec.clear();
			}
		public:
		};
	};
};
