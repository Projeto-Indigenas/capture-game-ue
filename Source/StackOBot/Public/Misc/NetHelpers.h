#pragma once

#include "CoreMinimal.h"
#include "Functions/Functions.h"

template<typename TType, typename ... TArgs>
struct TLocalRoleHelper
{
	typedef void(TType::*TTypedSwitchFunction)(TArgs...);
	
	TLocalRoleHelper& Authority(TType* owner, TTypedSwitchFunction function)
	{
		_authorityPtr = MakeUnique<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& Authority(const TFunction<void(TArgs...)>& function)
	{
		_authorityPtr = MakeUnique<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;
	}
	
	TLocalRoleHelper& AutonomousProxy(TType* owner, TTypedSwitchFunction function)
	{
		_autonomousProxyPtr = MakeUnique<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& AutonomousProxy(const TFunction<void(TArgs...)>& function)
	{
		_autonomousProxyPtr = MakeUnique<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;	
	}
	
	TLocalRoleHelper& SimulatedProxy(TType* owner, TTypedSwitchFunction function)
	{
		_simulatedProxyPtr = MakeUnique<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& SimulatedProxy(const TFunction<void(TArgs...)>& function)
	{
		_simulatedProxyPtr = MakeUnique<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;
	}

	void Switch(const AActor* actor, TArgs...arguments)
	{
		if (!IsValid(actor)) return;
	
		switch (actor->GetLocalRole()) {
		case ROLE_Authority: _authorityPtr->Execute(arguments...); break;
		case ROLE_AutonomousProxy: _autonomousProxyPtr->Execute(arguments...); break;
		case ROLE_SimulatedProxy: _simulatedProxyPtr->Execute(arguments...); break;
		default: break;
		}
	}

private:
	TUniquePtr<IFunctionStorage<TArgs...>> _authorityPtr;
	TUniquePtr<IFunctionStorage<TArgs...>> _autonomousProxyPtr;
	TUniquePtr<IFunctionStorage<TArgs...>> _simulatedProxyPtr;
};
