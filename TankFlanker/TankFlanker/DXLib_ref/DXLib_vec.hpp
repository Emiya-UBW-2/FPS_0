#pragma once
#include <DxLib.h>
class VECTOR_ref {
	VECTOR value;
public:
	VECTOR_ref(void) noexcept : value(DxLib::VGet(0, 0, 0)) {}
	VECTOR_ref(VECTOR value) { this->value = value; }
	//加算
	VECTOR_ref operator+(VECTOR_ref obj) { return VECTOR_ref(DxLib::VAdd(this->value, obj.value)); }
	VECTOR_ref operator+=(VECTOR_ref obj) {
		this->value = DxLib::VAdd(this->value, obj.value);
		return this->value;
	}
	//減算
	VECTOR_ref operator-(const VECTOR_ref&  obj) { return VECTOR_ref(DxLib::VSub(this->value, obj.value)); }
	VECTOR_ref operator-=(const VECTOR_ref&  obj) {
		this->value = DxLib::VSub(this->value, obj.value);
		return VECTOR_ref(DxLib::VSub(this->value, obj.value));
	}
	//外積
	VECTOR_ref operator*(const VECTOR_ref&  obj) { return VECTOR_ref(DxLib::VCross(this->value, obj.value)); }
	//内積
	float operator%(const VECTOR_ref&  obj) { return DxLib::VDot(this->value, obj.value); }
	//サイズ変更
	VECTOR_ref Scale(const float& p1) const noexcept { return VECTOR_ref(DxLib::VScale(this->value, p1)); }
	//正規化
	VECTOR_ref Norm(void) const noexcept { return VECTOR_ref(DxLib::VNorm(this->value)); }
	//サイズ
	float size(void) const noexcept { return DxLib::VSize(this->value); }
	//出力
	VECTOR get(void) const noexcept { return this->value; }
	float x(void) const noexcept { return this->value.x; }
	float y(void) const noexcept { return this->value.y; }
	float z(void) const noexcept { return this->value.z; }

	float x(const float& p) noexcept { this->value.x = p; return this->value.x; }
	float y(const float& p) noexcept { this->value.y = p; return this->value.y; }
	float z(const float& p) noexcept { this->value.z = p; return this->value.z; }

	float xadd(const float& p) noexcept { this->value.x += p; return this->value.x; }
	float yadd(const float& p) noexcept { this->value.y += p; return this->value.y; }
	float zadd(const float& p) noexcept { this->value.z += p; return this->value.z; }
};
