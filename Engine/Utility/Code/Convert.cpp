#include "Convert.h"

USING(Engine)

Engine::CConvert::CConvert(void)
{

}

Engine::CConvert::~CConvert(void)
{

}

void CConvert::D3DToClientCoordinate2D(const _float& fWinCX, const _float& fWinCY, _float & fX, _float & fY)
{
	// 데카르트 좌표계(D3D) -> 윈도우 좌표계(클라이언트 영역 기준) 변환
	// x좌표 = x +  윈도우 가로 크기 * 0.5
	// y좌표 = (-y) + 윈도우 세로 크기 * 0.5;

	fX = fX + fWinCX * 0.5f;
	fY = (-fY) + fWinCY * 0.5f;
}

void CConvert::ClientToD3DCoordinate2D(const _float& fWinCX, const _float& fWinCY, _float & fX, _float & fY)
{	
	// 데카르트 좌표계(D3D) <- 윈도우 좌표계(클라이언트 영역 기준) 변환
	// x좌표 = x - 윈도우 가로 크기 * 0.5
	// y좌표 = (-y) + 윈도우 세로 크기 * 0.5;

	fX = fX - fWinCX * 0.5f;
	fY = (-fY) + fWinCY * 0.5f;
}
