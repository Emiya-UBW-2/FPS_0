#pragma once
class Mapclass:Mainclass {
private:
	MV1 map, map_col;					    //’n–Ê
	//MV1 tree_model, tree_far;				    //–Ø
	MV1 sky;	  //‹ó
	//MV1 sea;	  //ŠC
	GraphHandle SkyScreen;
	int disp_x = 1920;
	int disp_y = 1080;

	SoundHandle envi;
public:
	Mapclass(const int& xd, const int& yd) {
		disp_x = xd;
		disp_y = yd;

		SkyScreen = GraphHandle::Make(disp_x, disp_y);    //‹ó•`‰æ
	}

	~Mapclass() {

	}
	void set_map_pre() {
		MV1::Load("data/map/model.mv1", &map, true);		   //map
		MV1::Load("data/map/col.mv1", &map_col, true);		   //mapƒRƒŠƒWƒ‡ƒ“
		MV1::Load("data/model/sky/model.mv1", &sky, true);	 //‹ó
		SetUseASyncLoadFlag(TRUE);
		envi = SoundHandle::Load("data/audio/envi.wav");
		SetUseASyncLoadFlag(FALSE);
	}
	void set_map(const char* item_txt,
			std::vector<Items>& gunitem,
			std::vector<Items>& magitem,
			std::vector<Gun>& gun_data
		) {
		map.material_AlphaTestAll(true, DX_CMP_GREATER, 128);
		VECTOR_ref size;
		for (int i = 0; i < map_col.mesh_num(); i++) {
			VECTOR_ref sizetmp = map_col.mesh_maxpos(i) - map_col.mesh_minpos(i);
			if (size.x() < sizetmp.x()) {
				size.x(sizetmp.x());
			}
			if (size.y() < sizetmp.y()) {
				size.y(sizetmp.y());
			}
			if (size.z() < sizetmp.z()) {
				size.z(sizetmp.z());
			}
		}
		for (int i = 0; i < map_col.mesh_num(); i++) {
			map_col.SetupCollInfo(int(size.x() / 5.f), int(size.y() / 5.f), int(size.z() / 5.f), 0, i);
		}
		SetFogStartEnd(0.0f, 300.f);
		SetFogColor(128, 128, 128);

		gunitem.clear();
		magitem.clear();
		{
			int mdata = FileRead_open(item_txt, FALSE);
			//gunitem
			while (true) {
				auto p = getparams::_str(mdata);
				if (getright(p.c_str()).find("end") == std::string::npos) {
					int p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : gun_data) {
						if (p.find(g.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					gunitem.resize(gunitem.size() + 1);
					gunitem.back().set(&gun_data[p1], VGet(p2, p3, p4), MGetIdent(),0);
				}
				else {
					break;
				}
			}
			//magitem
			while (true) {
				auto p = getparams::_str(mdata);
				if (getright(p.c_str()).find("end") == std::string::npos) {
					int p1 = 0;
					float p2 = 0.f, p3 = 0.f, p4 = 0.f;
					for (auto& g : gun_data) {
						if (p.find(g.mag.name) != std::string::npos) {
							p1 = g.id;
							break;
						}
					}
					p2 = getparams::_float(mdata);
					p3 = getparams::_float(mdata);
					p4 = getparams::_float(mdata);

					magitem.resize(magitem.size() + 1);
					magitem.back().set(&gun_data[p1], VGet(p2, p3, p4), MGetIdent(),1);
					if (magitem.back().ptr != nullptr) {
						magitem.back().cap = magitem.back().ptr->ammo_max;
					}
				}
				else {
					break;
				}
			}
			FileRead_close(mdata);
		}
	}

	void start_map() {
		envi.play(DX_PLAYTYPE_LOOP, TRUE);
	}
	void delete_map() {
		map.Dispose();		   //map
		map_col.Dispose();		   //mapƒRƒŠƒWƒ‡ƒ“
		sky.Dispose();	 //‹ó
		envi.Dispose();
	}

	auto& map_get() { return map; }

	auto& map_col_get() { return map_col; }

	auto map_col_line(const VECTOR_ref& startpos, const VECTOR_ref& endpos, const int&  i) {
		return map_col.CollCheck_Line(startpos, endpos, 0, i);
	}

	//‹ó•`‰æ
	GraphHandle& sky_draw(const VECTOR_ref& campos, const VECTOR_ref&camvec, const VECTOR_ref& camup, float fov) {
		SkyScreen.SetDraw_Screen(1000.0f, 5000.0f, fov, VECTOR_ref(campos) - camvec, VGet(0, 0, 0), camup);
		{
			SetFogEnable(FALSE);
			SetUseLighting(FALSE);
			sky.DrawModel();
			SetUseLighting(TRUE);
			SetFogEnable(TRUE);
		}
		return SkyScreen;
	}

};
