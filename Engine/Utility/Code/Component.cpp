#include "Component.h"

USING(Engine)

Engine::CComponent::CComponent(void)
{

}

Engine::CComponent::~CComponent(void)
{

}

_int Engine::CComponent::Update_Component(const _float& fTimeDelta)
{
	return 0;
}
_int Engine::CComponent::LateUpdate_Component(const _float& fTimeDelta)
{
	return 0;
}