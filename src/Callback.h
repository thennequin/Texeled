#ifndef __CALLBACK_H__
#define __CALLBACK_H__

namespace Callback
{
	//Caller
	//No arg
	template<typename R>
	class Caller
	{
	public:
		virtual R					Call() = 0;
	};

	template<typename R>
	class FunctionCaller : public Caller<R>
	{
	public:
		FunctionCaller(R(*pFunctionPtr)())
		{
			m_pFunctionPtr = pFunctionPtr;
		}
		virtual R					Call()
		{
			return m_pFunctionPtr();
		}
		R(*m_pFunctionPtr)();
	};

	template<typename C, typename R>
	class InstanceCaller : public Caller<R>
	{
	public:
		InstanceCaller(C* pThis, R(C::*pMemberPtr)())
		{
			m_pThis = pThis;
			m_pMemberPtr = pMemberPtr;
		}
		virtual R					Call()
		{
			return (m_pThis->*m_pMemberPtr)();
		}
		R(C::*m_pMemberPtr)();
		C* m_pThis;
	};

	//One args
	template<typename T1, typename R>
	class CallerOneArg
	{
	public:
		virtual R					Call(const T1& oT1) = 0;
	};

	template<typename T1, typename R>
	class FunctionCallerOneArg : public CallerOneArg<T1, R>
	{
	public:
		FunctionCallerOneArg(R(*pFunctionPtr)(T1))
		{
			m_pFunctionPtr = pFunctionPtr;
		}
		virtual R					Call(const T1& oT1)
		{
			return m_pFunctionPtr(oT1);
		}
		R(*m_pFunctionPtr)(T1);
	};

	template<typename C, typename T1, typename R>
	class InstanceCallerOneArg : public CallerOneArg<T1, R>
	{
	public:
		InstanceCallerOneArg(C* pThis, R(C::*pMemberPtr)(T1))
		{
			m_pThis = pThis;
			m_pMemberPtr = pMemberPtr;
			m_pMemberPtrConstRef = NULL;
		}
		InstanceCallerOneArg(C* pThis, R(C::*pMemberPtrConstRef)(const T1&))
		{
			m_pThis = pThis;
			m_pMemberPtr = NULL;
			m_pMemberPtrConstRef = pMemberPtrConstRef;
		}
		virtual R					Call(const T1& oT1)
		{
			if (m_pMemberPtr != NULL)
				return (m_pThis->*m_pMemberPtr)(oT1);
			return (m_pThis->*m_pMemberPtrConstRef)(oT1);
		}
		R(C::*m_pMemberPtr)(T1);
		R(C::*m_pMemberPtrConstRef)(const T1&);
		C* m_pThis;
	};

	//Two args
	template<typename T1, typename T2, typename R>
	class CallerTwoArg
	{
	public:
		virtual R					Call(const T1& oT1, const T2& oT2) = 0;
	};

	template<typename T1, typename T2, typename R>
	class FunctionCallerTwoArg : public CallerTwoArg<T1, T2, R>
	{
	public:
		FunctionCallerTwoArg(R(*pFunctionPtr)(T1, T2))
		{
			m_pFunctionPtr = pFunctionPtr;
		}
		virtual R					Call(const T1& oT1, const T2& oT2)
		{
			return m_pFunctionPtr(oT1, oT2);
		}
		R(*m_pFunctionPtr)(T1, T2);
	};

	template<typename C, typename T1, typename T2, typename R>
	class InstanceCallerTwoArg : public CallerTwoArg<T1, T2, R>
	{
	public:
		InstanceCallerTwoArg(C* pThis, R(C::*pMemberPtr)(T1, T2))
		{
			m_pThis = pThis;
			m_pMemberPtr = pMemberPtr;
		}
		virtual R					Call(const T1& oT1, const T2& oT2)
		{
			return (m_pThis->*m_pMemberPtr)(oT1, oT2);
		}
		R(C::*m_pMemberPtr)(T1, T2);
		C* m_pThis;
	};

	//Callbacks
	template<typename R>
	class Callback
	{
	public:
		Callback()
		{
			m_pCaller = 0;
		}
		~Callback()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)())
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCaller<R>(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)())
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCaller<C, R>(pThis, pMemberPtr);
		}

		R								operator()()
		{
			if (0 != m_pCaller) return m_pCaller->Call();
			return R();
		}
	protected:
		Caller<R>*						m_pCaller;
	};

	template<typename T1, typename R>
	class CallbackOneArg
	{
	public:
		CallbackOneArg()
		{
			m_pCaller = 0;
		}
		~CallbackOneArg()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)(T1))
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCallerOneArg<T1, R>(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)(T1))
		{
			if (NULL != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCallerOneArg<C, T1, R>(pThis, pMemberPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtrConstRef)(const T1&))
		{
			if (NULL != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCallerOneArg<C, T1, R>(pThis, pMemberPtrConstRef);
		}

		R								operator()(const T1& oT1)
		{
			if (0 != m_pCaller) return m_pCaller->Call(oT1);
			return R();
		}
	protected:
		CallerOneArg<T1, R>*			m_pCaller;
	};

	template<typename T1, typename T2, typename R>
	class CallbackTwoArg
	{
	public:
		CallbackTwoArg()
		{
			m_pCaller = 0;
		}
		~CallbackTwoArg()
		{
			if (0 != m_pCaller) delete m_pCaller;
		}
		void							Set(R(*pFuncPtr)(T1, T2))
		{
			if (0 != m_pCaller) delete m_pCaller;
			m_pCaller = new FunctionCallerTwoArg<T1, T2, R>(pFuncPtr);
		}

		template<typename C>
		void							Set(C* pThis, R(C::*pMemberPtr)(T1, T2))
		{
			if (NULL != m_pCaller) delete m_pCaller;
			m_pCaller = new InstanceCallerTwoArg<C, T1, T2, R>(pThis, pMemberPtr);
		}

		R								operator()(const T1& oT1, const T2& oT2)
		{
			if (0 != m_pCaller) return m_pCaller->Call(oT1, oT2);
			return R();
		}
	protected:
		CallerTwoArg<T1, T2, R>*		m_pCaller;
	};
}
//namespace Callback

#endif //__CALBACK_H__