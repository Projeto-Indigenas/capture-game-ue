#pragma once

template<typename ... TArgs>
struct IFunctionStorage
{
	virtual ~IFunctionStorage() = default;
	virtual void Execute(TArgs&...arguments) { };
};

template<typename TFuncType, typename ... TArgs>
struct TFunctionStorage : IFunctionStorage<TArgs...>
{
	TFunctionStorage(const TFunction<TFuncType>& func) : IFunctionStorage<TArgs...>()
	{
		_func = func;
	}
	
	virtual void Execute(TArgs&...arguments) override
	{
		if (!_func) return;
		
		_func(arguments...);
	}

private:
	TFunction<TFuncType> _func;
};

template<typename TType, typename ... TArgs>
struct TMethodPtrStorage : IFunctionStorage<TArgs...>
{
	typedef void(TType::*TFunctionPtr)(TArgs...);
	
	TMethodPtrStorage(TType* owner, TFunctionPtr func) : IFunctionStorage<TArgs...>()
	{
		_owner = owner;
		_func = func;
	}

	virtual void Execute(TArgs&...arguments) override
	{
		if (_owner == nullptr) return;
		if (_func == nullptr) return;

		(_owner->*_func)(arguments...);
	}

protected:
	TType* _owner;
	TFunctionPtr _func;
};
