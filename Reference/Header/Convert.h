#ifndef Convert_h__
#define Convert_h__

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CConvert
{
private:
	explicit CConvert(void);
	virtual ~CConvert(void);

public:
	static void D3DToClientCoordinate2D(const _float& fWinCX, const _float& fWinCY, _float & fX, _float & fY);
	static void ClientToD3DCoordinate2D(const _float& fWinCX, const _float& fWinCY, _float & fX, _float & fY);

};

END
#endif // Convert_h__
