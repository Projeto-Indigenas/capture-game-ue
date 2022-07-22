#pragma once

#include "CoreMinimal.h"

template<typename TType, typename ...TArgs>
class TLocalRoleHelper
{
	typedef void(TType::*TSwitchFunction)(TArgs...);
	
	TTuple<TType*, TSwitchFunction> _authority;
	TTuple<TType*, TSwitchFunction> _autonomousProxy;
	TTuple<TType*, TSwitchFunction> _simulatedProxy;

	static void Execute(TType* owner, TSwitchFunction function, TArgs&...arguments)
	{
		if (owner == nullptr) return;
		if (function == nullptr) return;

		(owner->*function)(arguments...);
	}
	
public:
	TLocalRoleHelper& Authority(TType* owner, TSwitchFunction function)
	{
		_authority = TTuple<TType*, TSwitchFunction>(owner, function);
		return *this;
	}
	
	TLocalRoleHelper& AutonomousProxy(TType* owner, TSwitchFunction function)
	{
		_autonomousProxy = TTuple<TType*, TSwitchFunction>(owner, function);
		return *this;
	}
	
	TLocalRoleHelper& SimulatedProxy(TType* owner, TSwitchFunction function)
	{
		_simulatedProxy = TTuple<TType*, TSwitchFunction>(owner, function);
		return *this;
	}

	void Switch(const AActor* actor, TArgs&...arguments)
	{
		if (!IsValid(actor)) return;
	
		switch (actor->GetLocalRole()) {
		case ROLE_Authority: Execute(_authority.Key, _authority.Value, arguments...); break;
		case ROLE_AutonomousProxy: Execute(_autonomousProxy.Key, _autonomousProxy.Value, arguments...); break;
		case ROLE_SimulatedProxy: Execute(_simulatedProxy.Key, _simulatedProxy.Value, arguments...); break;
		default: break;
		}
	}
};
