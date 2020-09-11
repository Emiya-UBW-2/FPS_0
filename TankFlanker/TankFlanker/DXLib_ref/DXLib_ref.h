#pragma once
#include "DxLib.h"
#include"EffekseerForDXLib.h"

#include"DXLib_mat.hpp"

#include "SoundHandle.hpp"
#include "GraphHandle.hpp"
#include "FontHandle.hpp"
#include "MV1ModelHandle.hpp"
#include "EffekseerEffectHandle.hpp"

#include "useful.hpp"

#include <array>
#include <list>
#include <vector>
enum Effect {
	ef_fire, //���C��
	ef_reco, //�����a���e
	ef_smoke, //�e�̋O��
	effects, //�ǂݍ���
};
struct EffectS {
	bool flug{ false };		 /**/
	size_t id = 0;
	Effekseer3DPlayingHandle handle; /**/
	VECTOR_ref pos;			 /**/
	VECTOR_ref nor;			 /**/
	float scale = 1.f;		 /**/
};

class DXDraw {
private:
	bool use_shadow = true;			/*�e�`��*/
	int shadow_near = 0;			/*�߉e*/
	int shadow_far = 0;				/*���e*/
	size_t shadow_size = 10;		/*�e�T�C�Y*/
	bool use_pixellighting = true;			     /**/
	bool use_vsync = false;				     /*��������*/
	float frate = 60.f;				     /*�t���[�����[�g*/
	std::array<EffekseerEffectHandle, effects> effHndle; /*�G�t�F�N�g���\�[�X*/
	EffekseerEffectHandle gndsmkHndle;		     /*�G�t�F�N�g���\�[�X*/
	int disp_x = 1920;
	int disp_y = 1080;

	//��
	struct Mirror_mod {
		VECTOR_ref WorldPos[4];	// ���̃��[���h���W
		COLOR_F AmbientColor;	// ���� Ambient Color
		COLOR_U8 DiffuseColor;	// ���� Diffuse Color
		int BlendParam[2];		// ���̃u�����h���[�h�ƃp�����[�^
		GraphHandle Handle;		// ���ɉf��f���̎擾�Ɏg�p����X�N���[��
		FLOAT4 ScreenPosW[4];	// ���ɉf��f���̎擾�Ɏg�p����N���[���̒��̋��̎l���̍��W( �������W )
		bool canlook = false;	//���������Ă��邩�ۂ�
	};
	std::vector<Mirror_mod> Mirror_obj;
	int MIRROR_POINTNUM = 64;	// ���̕`��Ɏg�p����ʂ̒��_������
	int MIRROR_NUM = 2;			// ���̐�
	std::vector <VERTEX3D> Vert;
	std::vector <unsigned short> Index;
	MATERIALPARAM Material;
	VECTOR_ref HUnitPos;
	VECTOR_ref VUnitPos[2];
	VECTOR_ref HPos;
	VECTOR_ref VPos[2];
	FLOAT4 HUnitUV;
	FLOAT4 VUnitUV[2];
	FLOAT4 HUV;
	FLOAT4 VUV[2];
	VECTOR_ref MirrorNormal;
	COLOR_U8 DiffuseColor;
	COLOR_U8 SpecularColor;
	int TextureW, TextureH;
public:
	EffekseerEffectHandle& get_effHandle(int p1) noexcept { return effHndle[p1]; }
	const EffekseerEffectHandle& get_effHandle(int p1) const noexcept { return effHndle[p1]; }
	EffekseerEffectHandle& get_gndhitHandle() noexcept { return gndsmkHndle; }
	const EffekseerEffectHandle& get_gndhitHandle() const noexcept { return gndsmkHndle; }
	template<class Y, class D>
	DXDraw(const char* title, std::unique_ptr<Y, D>& settings, const float& fps = 60.f) {
		this->use_shadow = settings->shadow_e;
		this->shadow_size = settings->shadow_level_e;
		this->disp_x = settings->dispx;
		this->disp_y = settings->dispy;

		frate = fps;
		SetOutApplicationLogValidFlag(settings->getlog_e ? TRUE : FALSE);	/*log*/
		SetMainWindowText(title);											/*�^�C�g��*/
		ChangeWindowMode(TRUE);												/*���\��*/
		SetUseDirect3DVersion(DX_DIRECT3D_11);								/*directX ver*/
		SetGraphMode(this->disp_x, this->disp_y, 32);						/*�𑜓x*/
		SetUseDirectInputFlag(TRUE);										/**/
		SetDirectInputMouseMode(TRUE);										/**/
		SetWindowSizeChangeEnableFlag(FALSE, FALSE);						/*�E�C���h�E�T�C�Y���蓮�s�A�E�C���h�E�T�C�Y�ɍ��킹�Ċg������Ȃ��悤�ɂ���*/
		SetUsePixelLighting(use_pixellighting ? TRUE : FALSE);				/*�s�N�Z���V�F�[�_�̎g�p*/
		SetFullSceneAntiAliasingMode(4, 2);									/*�A���`�G�C���A�X*/
		SetEnableXAudioFlag(TRUE);
		Set3DSoundOneMetre(1.0f);
		SetWaitVSyncFlag(use_vsync ? TRUE : FALSE);							/*��������*/
		DxLib_Init();														/**/
		Effekseer_Init(8000);												/*Effekseer*/
		SetChangeScreenModeGraphicsSystemResetFlag(FALSE);					/*Effekseer*/
		Effekseer_SetGraphicsDeviceLostCallbackFunctions();					/*Effekseer*/
		SetAlwaysRunFlag(TRUE);												/*background*/
		SetUseZBuffer3D(TRUE);												/*zbufuse*/
		SetWriteZBuffer3D(TRUE);											/*zbufwrite*/
		MV1SetLoadModelPhysicsWorldGravity(-9.8f);							/*�d��*/
		//�G�t�F�N�g
		{
			size_t j = 0;
			for (auto& e : effHndle) {
				e = EffekseerEffectHandle::load("data/effect/" + std::to_string(j++) + ".efk");
			}
			gndsmkHndle = EffekseerEffectHandle::load("data/effect/gndsmk.efk");
		}
		SetWindowSize(settings->out_dispx, settings->out_dispy);
		SetWindowPosition(
			//*
			deskx+
			//*/
			(deskx - settings->out_dispx) / 2 - 8, (desky - settings->out_dispy) / 2 - 32);
	}
	~DXDraw(void) {
		Effkseer_End();
		DxLib_End();
	}
	template <typename T>
	bool Set_Light_Shadow(const VECTOR_ref& farsize, const VECTOR_ref& nearsize, const VECTOR_ref& Light_dir, T doing) {
		SetGlobalAmbientLight(GetColorF(0.12f, 0.11f, 0.10f, 0.0f));
		SetLightDirection(Light_dir.get());
		if (this->use_shadow) {
			shadow_near = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			shadow_far = MakeShadowMap(int(pow(2, this->shadow_size)), int(pow(2, this->shadow_size)));
			SetShadowMapAdjustDepth(shadow_near, 0.0005f);
			SetShadowMapLightDirection(shadow_near, Light_dir.get());
			SetShadowMapLightDirection(shadow_far, Light_dir.get());
			SetShadowMapDrawArea(shadow_far, nearsize.get(), farsize.get());
			ShadowMap_DrawSetup(shadow_far);
			doing();
			ShadowMap_DrawEnd();
		}
		return true;
	}
	bool Delete_Shadow() {
		if (this->use_shadow) {
			DeleteShadowMap(shadow_near);
			DeleteShadowMap(shadow_far);
		}
		return true;
	}
	template <typename T>
	bool Ready_Shadow(const VECTOR_ref& pos, T doing, const VECTOR_ref& nearsize) {
		if (this->use_shadow) {
			SetShadowMapDrawArea(shadow_near, (nearsize*(-1.f) + pos).get(), (VECTOR_ref(nearsize) + pos).get());
			ShadowMap_DrawSetup(shadow_near);
			doing();
			ShadowMap_DrawEnd();
			return true;
		}
		return false;
	}
	template <typename T>
	bool Draw_by_Shadow(T doing) {
		if (this->use_shadow) {
			SetUseShadowMap(0, shadow_near);
			SetUseShadowMap(1, shadow_far);
		}
		doing();
		if (this->use_shadow) {
			SetUseShadowMap(0, -1);
			SetUseShadowMap(1, -1);
		}
		return true;
	}
	bool Screen_Flip(const LONGLONG& waits) {
		ScreenFlip();
		if (!use_vsync) {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / frate) {}
		}
		return true;
	}
	static bool Screen_Flip(const LONGLONG& waits,const float& f_rate) {
		ScreenFlip();
		if (GetWaitVSyncFlag()==FALSE) {
			while (GetNowHiPerformanceCount() - waits < 1000000.0f / f_rate) {}
		}
		return true;
	}
	static bool Screen_Flip(void) {
		ScreenFlip();
		return true;
	}
	static bool Capsule3D(const VECTOR_ref& p1, const VECTOR_ref& p2, const float& range, const unsigned int& color, const unsigned int& speccolor) {
		return DxLib::DrawCapsule3D(p1.get(), p2.get(), range, 8, color, speccolor, TRUE) == TRUE;
	}
	static bool Line2D(const int& p1x, const int& p1y, const int& p2x, const int& p2y, const unsigned int& color, const int& thickness=1) {
		return DxLib::DrawLine(p1x,p1y,p2x,p2y,color,thickness) == TRUE;
	}
	//��
	VECTOR_ref Mirrorcampos, Mirrorcamtgt;
	auto& get_Mirror_obj() {
		return Mirror_obj;
	}
	auto& get_Mirror_obj(int i) {
		return Mirror_obj[std::min(i, int(Mirror_obj.size() - 1))];
	}
	// ���̏�����
	void Mirror_init(int p) {
		MIRROR_NUM = p;
		Mirror_obj.clear();
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj.resize(Mirror_obj.size() + 1);
			Mirror_obj.back().Handle = GraphHandle::Make(this->disp_x, this->disp_y, FALSE);	// ���ɉf��f���̎擾�Ɏg�p����X�N���[���̍쐬
		}
		Vert.resize(MIRROR_POINTNUM * MIRROR_POINTNUM);
		Index.resize((MIRROR_POINTNUM - 1) * (MIRROR_POINTNUM - 1) * 6);
	}
	void set_Mirror_obj(int i,
		VECTOR_ref pos1, VECTOR_ref pos2, VECTOR_ref pos3, VECTOR_ref pos4,
		COLOR_F ambcol,
		COLOR_U8 difcol,
		int param0,int param1
		) {
		auto& m = Mirror_obj[std::min(i, int(Mirror_obj.size() - 1))];
		m.WorldPos[0] = pos1;
		m.WorldPos[1] = pos2;
		m.WorldPos[2] = pos3;
		m.WorldPos[3] = pos4;
		m.AmbientColor = ambcol;
		m.DiffuseColor = difcol;
		m.BlendParam[0] = param0;
		m.BlendParam[1] = param1;
	}
	void Mirror_delete() {
		for (int i = 0; i < MIRROR_NUM; i++) {
			Mirror_obj[i].Handle.Dispose();
		}
		Mirror_obj.clear();
		Vert.clear();
		Index.clear();
	}
	// ���ɉf��f����`�悷�邽�߂̃J�����̐ݒ���s��.Mirrorcampos,Mirrorcamtgt�ɔ��f
	void Mirror_SetupCamera(Mirror_mod& MirrorNo, const VECTOR_ref& campos, const VECTOR_ref& camtgt, const VECTOR_ref& camup, const float& fov, const float& far_distance = 1000.f, const float& near_distance = 100.f) {
		auto& id = MirrorNo;
		// ���̖ʂ̖@�����Z�o
		MirrorNormal = ((id.WorldPos[1] - id.WorldPos[0]).cross(id.WorldPos[2] - id.WorldPos[0])).Norm();
		// ���̖ʂ���J�����̍��W�܂ł̍ŒZ�����A���̖ʂ���J�����̒����_�܂ł̍ŒZ�������Z�o
		float EyeLength = Plane_Point_MinLength(id.WorldPos[0].get(), MirrorNormal.get(), campos.get());
		float TargetLength = Plane_Point_MinLength(id.WorldPos[0].get(), MirrorNormal.get(), camtgt.get());
		// ���ɉf��f����`�悷��ۂɎg�p����J�����̍��W�ƃJ�����̒����_���Z�o
		Mirrorcampos = VECTOR_ref(campos) + MirrorNormal * (-EyeLength * 2.0f);
		Mirrorcamtgt = VECTOR_ref(camtgt) + MirrorNormal * (-TargetLength * 2.0f);
		// ���ɉf��f���̒��ł̋��̎l���̍��W���Z�o( �������W )
		id.Handle.SetDraw_Screen(far_distance, near_distance, fov, Mirrorcampos, Mirrorcamtgt, VGet(0, 1.f, 0.f));
		for (int i = 0; i < 4; i++) {
			id.ScreenPosW[i] = ConvWorldPosToScreenPosPlusW(id.WorldPos[i].get());
		}
		// ���ɉf��f���̒��ł̋��̎l���̍��W���Z�o( �������W )
		id.Handle.SetDraw_Screen(far_distance, near_distance, fov, campos, camtgt, camup);
		id.canlook = true;
		for (int z = 0; z < 4; z++) {
			if (id.canlook) {
				float p = ConvWorldPosToScreenPos(id.WorldPos[z].get()).z;
				if (p < 0.0f || p > 1.1f) {
					id.canlook = false;
				}
			}
		}
		//�~���[�I�t
		id.canlook = false;

	}
	// ���̕`��
	void Mirror_Render(void) {
		for (auto& obj : Mirror_obj) {
			if (obj.canlook) {
				// ���̕`��Ɏg�p����}�e���A���̃Z�b�g�A�b�v
				Material.Ambient = obj.AmbientColor;
				Material.Diffuse = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Emissive = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Specular = GetColorF(0.0f, 0.0f, 0.0f, 0.0f);
				Material.Power = 1.0f;
				SetMaterialParam(Material);
				// ���̖ʂ̖@�����Z�o
				MirrorNormal = ((obj.WorldPos[1] - obj.WorldPos[0]).cross(obj.WorldPos[2] - obj.WorldPos[0])).Norm();
				// ���ɉf��f�����������񂾉摜�̃e�N�X�`���̃T�C�Y���擾
				GetGraphTextureSize(obj.Handle.get(), &TextureW, &TextureH);
				// ���̕`��Ɏg�p���钸�_�̃Z�b�g�A�b�v
				{
					VUnitPos[0] = (obj.WorldPos[2] - obj.WorldPos[0])*(1.0f / (MIRROR_POINTNUM - 1));
					VUnitPos[1] = (obj.WorldPos[3] - obj.WorldPos[1])*(1.0f / (MIRROR_POINTNUM - 1));
					VUnitUV[0] = F4Scale(F4Sub(obj.ScreenPosW[2], obj.ScreenPosW[0]), 1.0f / (MIRROR_POINTNUM - 1));
					VUnitUV[1] = F4Scale(F4Sub(obj.ScreenPosW[3], obj.ScreenPosW[1]), 1.0f / (MIRROR_POINTNUM - 1));
					DiffuseColor = obj.DiffuseColor;
					SpecularColor = GetColorU8(0, 0, 0, 0);
					VPos[0] = obj.WorldPos[0];
					VPos[1] = obj.WorldPos[1];
					VUV[0] = obj.ScreenPosW[0];
					VUV[1] = obj.ScreenPosW[1];
					int k = 0;
					for (auto& v : Vert) {
						if (k%MIRROR_POINTNUM == 0) {
							HUnitPos = (VPos[1] - VPos[0])*(1.0f / (MIRROR_POINTNUM - 1));
							HPos = VPos[0];
							HUnitUV = F4Scale(F4Sub(VUV[1], VUV[0]), 1.0f / (MIRROR_POINTNUM - 1));
							HUV = VUV[0];
						}
						{
							v.pos = HPos.get();
							v.norm = MirrorNormal.get();
							v.dif = DiffuseColor;
							v.spc = SpecularColor;
							v.u = HUV.x / (HUV.w * TextureW);
							v.v = HUV.y / (HUV.w * TextureH);
							v.su = 0.0f;
							v.sv = 0.0f;
							HUV = F4Add(HUV, HUnitUV);
							HPos += HUnitPos;
						}
						if (k%MIRROR_POINTNUM == 0) {
							VUV[0] = F4Add(VUV[0], VUnitUV[0]);
							VUV[1] = F4Add(VUV[1], VUnitUV[1]);
							VPos[0] += VUnitPos[0];
							VPos[1] += VUnitPos[1];
						}
						k++;
					}
				}
				// ���̕`��Ɏg�p���钸�_�C���f�b�N�X���Z�b�g�A�b�v
				{
					int k = 0;
					for (int i = 0; i < MIRROR_POINTNUM - 1; i++) {
						for (int j = 0; j < MIRROR_POINTNUM - 1; j++) {
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 1);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 1);
							Index[k++] = unsigned short((i + 1) * MIRROR_POINTNUM + j + 0);
							Index[k++] = unsigned short((i + 0) * MIRROR_POINTNUM + j + 1);
						}
					}
				}
				// ����`��
				SetDrawMode(DX_DRAWMODE_BILINEAR);
				SetDrawBlendMode(obj.BlendParam[0], obj.BlendParam[1]);
				DrawPolygonIndexed3D(&Vert[0], int(Vert.size()), &Index[0], int(Index.size() / 3), obj.Handle.get(), FALSE);
				SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 255);
				SetDrawMode(DX_DRAWMODE_NEAREST);
			}
		}
	}
};

void set_effect(EffectS* efh, VECTOR_ref pos, VECTOR_ref nor, float scale = 1.f) {
	efh->flug = true;
	efh->pos = pos;
	efh->nor = nor;
	efh->scale = scale;
}
void set_pos_effect(EffectS* efh, const EffekseerEffectHandle& handle) {
	if (efh->flug) {
		if (efh->handle.IsPlaying()) {
			efh->handle.Stop();
		}
		efh->handle = handle.Play3D();
		efh->handle.SetPos(efh->pos);
		efh->handle.SetRotation(atan2(efh->nor.y(), std::hypot(efh->nor.x(), efh->nor.z())), atan2(-efh->nor.x(), -efh->nor.z()), 0);
		efh->handle.SetScale(efh->scale);
		efh->flug = false;
	}
}
typedef std::pair<int, VECTOR_ref> frames;
class switchs {
public:
	bool first;
	uint8_t second;

	switchs() {
		first = false;
		second = 0;
	};

	void ready(bool on) {
		first = on;
		second = 0;
	}

	void get_in(bool key) {
		second = std::clamp<uint8_t>(second + 1, 0, (key ? 2 : 0));
		if (second == 1) {
			first ^= 1;
		}
	}
};
