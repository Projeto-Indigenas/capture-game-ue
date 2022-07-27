#pragma once

#include "CoreMinimal.h"
#include "Functions/Functions.h"

template<typename TType, typename TRet, typename ... TArgs>
struct TLocalRoleHelperBase
{
	typedef TRet(TType::*TTypedSwitchFunction)(TArgs...);

	virtual ~TLocalRoleHelperBase() = default;

	TLocalRoleHelperBase& Authority(TType* owner, TTypedSwitchFunction function)
	{
		_authorityPtr = MakeShared<TMethodPtrStorage<TType, TRet, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelperBase& Authority(const TFunction<TRet(TArgs...)>& function)
	{
		_authorityPtr = MakeShared<TFunctionStorage<TRet, TArgs...>>(function);
		return *this;
	}
	
	TLocalRoleHelperBase& AutonomousProxy(TType* owner, TTypedSwitchFunction function)
	{
		_autonomousProxyPtr = MakeShared<TMethodPtrStorage<TType, TRet, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelperBase& AutonomousProxy(const TFunction<TRet(TArgs...)>& function)
	{
		_autonomousProxyPtr = MakeShared<TFunctionStorage<TRet, TArgs...>>(function);
		return *this;	
	}
	
	TLocalRoleHelperBase& SimulatedProxy(TType* owner, TTypedSwitchFunction function)
	{
		_simulatedProxyPtr = MakeShared<TMethodPtrStorage<TType, TRet, TArgs...>>(owner, function);
		return *this;
	}

	TLocalRoleHelperBase& SimulatedProxy(const TFunction<TRet(TArgs...)>& function)
	{
		_simulatedProxyPtr = MakeShared<TFunctionStorage<TRet, TArgs...>>(function);
		return *this;
	}
	
protected:
	TSharedPtr<IFunctionStorage<TRet, TArgs...>> _authorityPtr;
	TSharedPtr<IFunctionStorage<TRet, TArgs...>> _autonomousProxyPtr;
	TSharedPtr<IFunctionStorage<TRet, TArgs...>> _simulatedProxyPtr;

	TLocalRoleHelperBase() = default;
	
	TLocalRoleHelperBase(const TLocalRoleHelperBase& other)
		: _authorityPtr(other._authorityPtr),
		  _autonomousProxyPtr(other._autonomousProxyPtr),
		  _simulatedProxyPtr(other._simulatedProxyPtr)
	{
	}

	TLocalRoleHelperBase(TLocalRoleHelperBase&& other) noexcept
	{
		Move(_authorityPtr, other._authorityPtr);
		Move(_autonomousProxyPtr, other._autonomousProxyPtr);
		Move(_simulatedProxyPtr, other._simulatedProxyPtr);
	}

	virtual TLocalRoleHelperBase& operator=(const TLocalRoleHelperBase& other)
	{
		if (this == &other) return *this;
		_authorityPtr = other._authorityPtr;
		_autonomousProxyPtr = other._autonomousProxyPtr;
		_simulatedProxyPtr = other._simulatedProxyPtr;
		return *this;
	}

	virtual TLocalRoleHelperBase& operator=(TLocalRoleHelperBase&& other) noexcept
	{
		if (this == &other) return *this;
		Move(_authorityPtr, other._authorityPtr);
		Move(_autonomousProxyPtr, other._autonomousProxyPtr);
		Move(_simulatedProxyPtr, other._simulatedProxyPtr);
		return *this;
	}

};

template<typename TType, typename TRet, typename ... TArgs>
struct TLocalRoleHelper : TLocalRoleHelperBase<TType, TRet, TArgs...>
{
	TLocalRoleHelper() = default;
	virtual ~TLocalRoleHelper() override = default;

	TLocalRoleHelper(const TLocalRoleHelper& other) : TLocalRoleHelperBase(other)
	{
		//
	}

	TLocalRoleHelper(TLocalRoleHelper&& other) noexcept : TLocalRoleHelperBase(other)
	{
		//
	}

	virtual TLocalRoleHelper& operator=(const TLocalRoleHelperBase<TType, TRet, TArgs...>& other) override
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, TRet, TArgs...>::operator =(other);
		return *this;
	}

	TLocalRoleHelper& operator=(const TLocalRoleHelper& other)
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator =(other);
		return *this;
	}

	virtual TLocalRoleHelper& operator=(TLocalRoleHelperBase<TType, TRet, TArgs...>&& other) noexcept override
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, TRet, TArgs...>::operator=(other);
		return *this;
	}

	TLocalRoleHelper& operator=(TLocalRoleHelper&& other) noexcept
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator =(other);
		return *this;
	}
	
	TRet Switch(const AActor* actor, TArgs...arguments) const
	{
		if (!IsValid(actor)) return TRet();

		switch (actor->GetLocalRole())
		{
		case ROLE_Authority:
			if (this->_authorityPtr.IsValid())
			{
				return (*this->_authorityPtr)(arguments...);
			}
			break;
			
		case ROLE_AutonomousProxy:
			if (this->_autonomousProxyPtr.IsValid())
			{
				return (*this->_autonomousProxyPtr)(arguments...);
			}
			break;

		case ROLE_SimulatedProxy:
			if (this->_simulatedProxyPtr.IsValid())
			{
				return (*this->_simulatedProxyPtr)(arguments...);
			}
			break;
			
		default: break;
		}

		return TRet();
	}
};

template<typename TType, typename ... TArgs>
struct TLocalRoleHelper<TType, void, TArgs...> : TLocalRoleHelperBase<TType, void, TArgs...>
{
	TLocalRoleHelper() = default;
	virtual ~TLocalRoleHelper() override = default;

	TLocalRoleHelper(const TLocalRoleHelper& other) : TLocalRoleHelperBase(other)
	{
		//
	}

	TLocalRoleHelper(TLocalRoleHelper&& other) noexcept : TLocalRoleHelperBase(other)
	{
		//
	}

	virtual TLocalRoleHelper& operator=(const TLocalRoleHelperBase<TType, void, TArgs...>& other) override
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator =(other);
		return *this;
	}

	TLocalRoleHelper& operator=(const TLocalRoleHelper& other)
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator =(other);
		return *this;
	}

	virtual TLocalRoleHelper& operator=(TLocalRoleHelperBase<TType, void, TArgs...>&& other) noexcept override
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator=(other);
		return *this;
	}

	TLocalRoleHelper& operator=(TLocalRoleHelper&& other) noexcept
	{
		if (this == &other) return *this;
		TLocalRoleHelperBase<TType, void, TArgs...>::operator =(other);
		return *this;
	}

	void Switch(const AActor* actor, TArgs...arguments) const
	{
		if (!IsValid(actor)) return;

		switch (actor->GetLocalRole())
		{
		case ROLE_Authority:
			if (this->_authorityPtr.IsValid())
			{
				(*this->_authorityPtr)(arguments...);
			}
			break;
			
		case ROLE_AutonomousProxy:
			if (this->_autonomousProxyPtr.IsValid())
			{
				(*this->_autonomousProxyPtr)(arguments...);
			}
			break;

		case ROLE_SimulatedProxy:
			if (this->_simulatedProxyPtr.IsValid())
			{
				(*this->_simulatedProxyPtr)(arguments...);
			}
			break;
			
		default: break;
		}
	}
};
