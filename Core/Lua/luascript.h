/********************************************************************
filename: 	lua.h
author:		(c)IROV
*********************************************************************/
#ifndef _I_LUA_H
#define _I_LUA_H

#pragma warning (disable : 4204)  // phci
 
#
#	if !BOOST_PP_IS_ITERATING
#
#
#	ifndef _IROV_luascript_h_2004_24_12
#	define _IROV_luascript_h_2004_24_12
#
#include "miniLua.h"
#
//#define LUABIND_NO_ERROR_CHECKING
#define LUABIND_NO_EXCEPTIONS
#	include "luabind/luabind.hpp"
/*#	include "luabind/class.hpp"
#	include "luabind/function.hpp"
#	include "luabind/detail/call_function.hpp"
/*#	include "luabind/dependency_policy.hpp"
#	include "luabind/out_value_policy.hpp"
#	include "luabind/adopt_policy.hpp"
#	include "luabind/copy_policy.hpp"
#	include "luabind/detail/class_registry.hpp"
#	include "luabind/container_policy.hpp"
#	include "luabind/discard_result_policy.hpp"
*/


namespace lua
{


	typedef luabind::object	object;
    typedef luabind::object& objectref;
	typedef luabind::detail::proxy_object proxy_object;
	
	typedef object * pobject;
	extern DWORD LUA_SYSTEM;
	//extern lua_State *GetState();
	//extern object variables;

	
	pobject    DLLTYPE GetVariables();

	//LPCSTR defclass(LPCSTR Class);
	LPCSTR  DLLTYPE getclassName(DWORD Type);
	

	template<class T>
	class type_
	{
		GString name;
	public:
		inline type_(const char *var):	name(var){}
		inline type_ & operator = (const T& var)
		{
			set(name.c_str(),var);
			return *this;
		}

		inline operator T (){return get<T>(name.c_str());}
	};

	typedef type_<int> integer;
	typedef type_<float> lfloat;
	typedef type_<const char *> string;

    enum NT_TYPES
    {
     NT_NOT   =0,
     NT_TABLE =1,
     NT_FUNC  =2
    };
    
    inline int NOTTABLE(LPCSTR S)
	{
        BYTE RET=NT_NOT;
		while(*(S++))
		{
			const char c=*S;
			if(c=='.' || c=='[' || c==':' ) RET|=NT_TABLE;
            if(c=='(' || c=='"'           ) RET|=NT_FUNC;
		}
        if(RET&NT_FUNC)RET=NT_FUNC;
		return RET;
	}

    object DLLTYPE GetFinalObject(LPCSTR Name);

	template<class T>inline T    gets(LPCSTR name,const T def)
	{
		switch(NOTTABLE(name))
        {
          case NT_NOT  : return luabind::object_cast<T>((*GetVariables())[name]);
          case NT_TABLE: return luabind::object_cast<T>(GetFinalObject(name));
          case NT_FUNC : {doVstring("tmpg=%s",name);return gets("tmpg",def);};
		}
        return def;
	}
	template<class T>inline T    get(LPCSTR name            ){return gets(name);}//luabind::object_cast<T>((*GetVariables())[name]);}
	template<class T>inline T    get(const object &obj      ){return luabind::object_cast<T>(obj);}
	template<class T>inline T    get(const proxy_object &obj){return luabind::object_cast<T>(obj);}
	template<class T>inline void get(LPCSTR name, T &value  ){value = gets(Name);}
//	                 inline void get(LPCSTR name,LPSTR value){	strcpy(value,luabind::object_cast<const char *>((*GetVariables())[name]));}
	template<class T>inline void set(LPCSTR name,const T &value)
	{
		switch(NOTTABLE(name))
        {
            case NT_NOT  :(*GetVariables())[name] = value;break;
            case NT_TABLE://GetFinalObject(name)=value;break;
            case NT_FUNC :set("tmps",value);
			              doVstring("%s=tmps",name);
                          break;
		}
	}
//	                 inline void set(LPCSTR name,LPCSTR value  ){(*GetVariables())[name] = value;}
	template<class T>inline void set(const object &obj,const T &value){obj = value;}
//	                 inline void set(const object &obj,const char *value){obj = value;}

	template<class F>
	inline void function_def(LPCSTR fun_name,F fun_ptr)
	{
		luabind::module(GetState())
		[luabind::def(fun_name,fun_ptr)];
	}

	template<class F>
	class functor	:	public luabind::functor<F>
	{
	public:
		functor(LPCSTR functor_name)
			:	luabind::functor<F>(GetState(),functor_name)
		{}
	};

	inline luabind::module_proxy class_def(){return luabind::module(GetState());}

	luabind::module_proxy namemodule(LPCSTR Name){return luabind::module(GetState(),Name);}
	luabind::module_proxy namemodule(           ){return luabind::module(GetState());}

	template<class F,class P>
	inline void pfunction_def(LPCSTR fun_name,F fun_ptr,const P &fun_policies)
	{
		luabind::module(GetState())
		[luabind::def(fun_name,fun_ptr,fun_policies)];	
	}

#define BOOST_PP_ITERATION_PARAMS_1 (4, (0, LUABIND_MAX_ARITY, "luascript.h", 1))
#include BOOST_PP_ITERATE()

};

#endif

#elif BOOST_PP_ITERATION_FLAGS() == 1

#define LUABIND_TUPLE_PARAMS(z, n, data) const A##n *
#define LUABIND_OPERATOR_PARAMS(z, n, data) const A##n & a##n

template<class Ret BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), class A)>
inline typename boost::mpl::if_<boost::is_void<Ret>
, luabind::detail::proxy_function_void_caller<boost::tuples::tuple<BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_TUPLE_PARAMS, _)> >
, luabind::detail::proxy_function_caller<Ret, boost::tuples::tuple<BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_TUPLE_PARAMS, _)> > >::type
function_call(const char *fun_name BOOST_PP_COMMA_IF(BOOST_PP_ITERATION()) BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_OPERATOR_PARAMS, _) )
{
	typedef boost::tuples::tuple<BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_TUPLE_PARAMS, _)> tuple_t;
#if BOOST_PP_ITERATION() == 0
	tuple_t args;
#else
	tuple_t args(BOOST_PP_ENUM_PARAMS(BOOST_PP_ITERATION(), &a));
#endif
	typedef typename boost::mpl::if_<boost::is_void<Ret>
		, luabind::detail::proxy_function_void_caller<boost::tuples::tuple<BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_TUPLE_PARAMS, _)> >
		, luabind::detail::proxy_function_caller<Ret, boost::tuples::tuple<BOOST_PP_ENUM(BOOST_PP_ITERATION(), LUABIND_TUPLE_PARAMS, _)> > >::type proxy_type;

	return proxy_type(GetState(), fun_name, args);
}

#undef LUABIND_OPERATOR_PARAMS
#undef LUABIND_TUPLE_PARAMS

#endif

#endif