#pragma once
//
// Defined deprecated D3Dx9.h struct because the old DX9 code uses these structs nowadays deprecated d3dx9 SDK data types. 
// Better option would be to modify the DX9 code ot use the newer DX SDK API, but let's do it this way for now because it does the job good enough.
//

typedef struct D3DXVECTOR4 {
#pragma pack(push,1)
	float x;
	float y;
	float z;
	float w;
#pragma pack(pop)

	D3DXVECTOR4() { x = y = z = w = 0; }

	D3DXVECTOR4(float initX, float initY, float initZ, float initW)
	{
		x = initX; y = initY; z = initZ; w = initW;
	}
} D3DXVECTOR4, * LPD3DXVECTOR4;

// Defined deprecated D3Dx9.h struct
typedef struct D3DXVECTOR3 {
#pragma pack(push,1)
	float x;
	float y;
	float z;
#pragma pack(pop)

	//D3DXVECTOR3() { x = y = z = 0; }

} D3DXVECTOR3, * LPD3DXVECTOR3;

// Defined deprecated D3Dx9math.h struct
typedef struct D3DXQUATERNION {
#pragma pack(push,1)
	float  x;
	float  y;
	float  z;
	float  w;
#pragma pack(pop)
} D3DXQUATERNION, * LPD3DXQUATERNION;

typedef D3DMATRIX D3DXMATRIX;
/*// Defined deprecated D3Dx9math.h struct
typedef struct _D3DXMATRIX {
#pragma pack(push,1)
	union {
		struct {
			float        _11, _12, _13, _14;
			float        _21, _22, _23, _24;
			float        _31, _32, _33, _34;
			float        _41, _42, _43, _44;

		};
		float m[4][4];
	};
#pragma pack(pop)
} D3DXMATRIX;
*/