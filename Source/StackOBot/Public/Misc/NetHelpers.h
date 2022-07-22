#pragma once

#include "CoreMinimal.h"
#include "Functions/Functions.h"

template<typename TType, typename ... TArgs>
struct TLocalRoleHelper
{
	typedef void(TType::*TTypedSwitchFunction)(TArgs...);

	TLocalRoleHelper() = default;
	virtual ~TLocalRoleHelper() = default;
	
	TLocalRoleHelper& Authority(TType* owner, TTypedSwitchFunction function)
	{
		_authorityPtr = MakeShared<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& Authority(const TFunction<void(TArgs...)>& function)
	{
		_authorityPtr = MakeShared<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;
	}
	
	TLocalRoleHelper& AutonomousProxy(TType* owner, TTypedSwitchFunction function)
	{
		_autonomousProxyPtr = MakeShared<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& AutonomousProxy(const TFunction<void(TArgs...)>& function)
	{
		_autonomousProxyPtr = MakeShared<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;	
	}
	
	TLocalRoleHelper& SimulatedProxy(TType* owner, TTypedSwitchFunction function)
	{
		_simulatedProxyPtr = MakeShared<TMethodPtrStorage<TType, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelper& SimulatedProxy(const TFunction<void(TArgs...)>& function)
	{
		_simulatedProxyPtr = MakeShared<TFunctionStorage<void(TArgs...), TArgs...>>(function);
		return *this;
	}

	void Switch(const AActor* actor, TArgs...arguments) const
	{
		if (!IsValid(actor)) return;
	
		switch (actor->GetLocalRole()) {
		case ROLE_Authority: _authorityPtr->Execute(arguments...); break;
		case ROLE_AutonomousProxy: _autonomousProxyPtr->Execute(arguments...); break;
		case ROLE_SimulatedProxy: _simulatedProxyPtr->Execute(arguments...); break;
		default: break;
		}
	}

	TLocalRoleHelper(const TLocalRoleHelper& Other)
		: _authorityPtr(Other._authorityPtr),
		  _autonomousProxyPtr(Other._autonomousProxyPtr),
		  _simulatedProxyPtr(Other._simulatedProxyPtr)
	{
	}

	TLocalRoleHelper(TLocalRoleHelper&& Other) noexcept
		: _authorityPtr(std::move(Other._authorityPtr)),
		  _autonomousProxyPtr(std::move(Other._autonomousProxyPtr)),
		  _simulatedProxyPtr(std::move(Other._simulatedProxyPtr))
	{
	}

	TLocalRoleHelper& operator=(const TLocalRoleHelper& Other)
	{
		if (this == &Other)
			return *this;
		_authorityPtr = Other._authorityPtr;
		_autonomousProxyPtr = Other._autonomousProxyPtr;
		_simulatedProxyPtr = Other._simulatedProxyPtr;
		return *this;
	}

	TLocalRoleHelper& operator=(TLocalRoleHelper&& Other) noexcept
	{
		if (this == &Other)
			return *this;
		_authorityPtr = std::move(Other._authorityPtr);
		_autonomousProxyPtr = std::move(Other._autonomousProxyPtr);
		_simulatedProxyPtr = std::move(Other._simulatedProxyPtr);
		return *this;
	}

private:
	TSharedPtr<IFunctionStorage<TArgs...>> _authorityPtr;
	TSharedPtr<IFunctionStorage<TArgs...>> _autonomousProxyPtr;
	TSharedPtr<IFunctionStorage<TArgs...>> _simulatedProxyPtr;
};
