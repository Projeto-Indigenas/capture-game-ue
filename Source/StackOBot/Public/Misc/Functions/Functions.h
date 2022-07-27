#pragma once

template<typename TRet, typename ... TArgs>
struct IFunctionStorage
{
	virtual ~IFunctionStorage() = default;
	virtual TRet Execute(TArgs&...arguments) { return TRet(); }

	FORCEINLINE_DEBUGGABLE virtual TRet operator()(TArgs&...arguments)
	{
		return Execute(arguments...);
	}
};

template<typename ... TArgs>
struct IFunctionStorage<void, TArgs...>
{
	virtual ~IFunctionStorage() = default;
	virtual void Execute(TArgs&...arguments) { }
	
	FORCEINLINE_DEBUGGABLE virtual void operator()(TArgs&...arguments)
	{
		Execute(arguments...);
	}
};

template<typename TRet, typename ... TArgs>
struct TFunctionStorage : IFunctionStorage<TRet, TArgs...>
{
	TFunctionStorage(const TFunction<TRet(TArgs...)>& func) : IFunctionStorage<TRet, TArgs...>()
	{
		_func = func;
	}
	
	virtual TRet Execute(TArgs&...arguments) override
	{
		if (!_func) return TRet();
		
		return _func(arguments...);
	}

private:
	TFunction<TRet(TArgs...)> _func;
};

template<typename ... TArgs>
struct TFunctionStorage<void, TArgs...> : IFunctionStorage<TArgs...>
{
	TFunctionStorage(const TFunction<void(TArgs&...)>& func) : IFunctionStorage<TArgs...>()
	{
		_func = func;
	}

	virtual void Execute(TArgs&...arguments) override
	{
		if (!_func) return;

		return _func(arguments...);
	}

private:
	TFunction<void(TArgs&...)> _func;
};

template<typename TType, typename TRet, typename ... TArgs>
struct TMethodPtrStorage : IFunctionStorage<TRet, TArgs...>
{
	typedef TRet(TType::*TFunctionPtr)(TArgs&...);
	
	TMethodPtrStorage(TType* owner, TFunctionPtr func) : IFunctionStorage<TRet, TArgs...>()
	{
		_owner = owner;
		_func = func;
	}

	virtual TRet Execute(TArgs&...arguments) override
	{
		if (_owner == nullptr) return TRet();
		if (_func == nullptr) return TRet();

		return (_owner->*_func)(arguments...);
	}

protected:
	TType* _owner;
	TFunctionPtr _func;
};

template<typename TType, typename ... TArgs>
struct TMethodPtrStorage<TType, void, TArgs...> : IFunctionStorage<void, TArgs...>
{
	typedef void(TType::*TFunctionPtr)(TArgs...);

	TMethodPtrStorage(TType* owner, TFunctionPtr func) : IFunctionStorage<void, TArgs...>()
	{
		_owner = owner;
		_func = func;
	}

	virtual void Execute(TArgs&...arguments) override
	{
		if (_owner == nullptr) return;
		if (_func == nullptr) return;

		return (_owner->*_func)(arguments...);
	}

protected:
	TType* _owner;
	TFunctionPtr _func;
};
