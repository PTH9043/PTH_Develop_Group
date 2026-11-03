#pragma once 

namespace NOT_OPEN_METHOD
{
	static void Debug_Message(const _string& _strFile, const _uint& _iLine, const _string& _Func,
		const _string& _str)
	{
		_string Out = _strFile;
		Out += "\n";
		_char LINE[MAX_PATH]{ "" };
		_itoa_s(_iLine, LINE, sizeof(_char) * MAX_PATH);
		Out.append(LINE);
		Out += "\n";
		Out.append(_Func);
		Out += "\n";
		Out.append(_str);
		MessageBoxA(nullptr, Out.c_str(), nullptr, MB_OK);
	}

	static void Check_Error(const HRESULT& _hReslt, const _string& _strFile, 
		const _uint& _iLine, const _string& _Func) {
		_string Out = _strFile;
		Out += "\n";
		_char LINE[MAX_PATH]{ "" };
		_itoa_s(_iLine, LINE, sizeof(_char) * MAX_PATH);
		Out.append(LINE);
		Out += "\n";
		Out.append(_Func);
		Out += "\n";
		_char RESULT[MAX_PATH]{ "" };
		_itoa_s(_hReslt, RESULT, MAX_PATH);
		Out.append(RESULT);
		Out.append("\n");
		MessageBoxA(nullptr, Out.c_str(), nullptr, MB_OK);
	}
}

template<class T>
static void Safe_Free(T& _pData)
{
	if (nullptr == _pData)
		return;

	_uint iValue = _pData.use_count();
	if (1 ==  iValue){
		_pData->Free();
		_pData.reset();
	}
}

template<class ConstT, class NormalT>
static void ChageConstValue(ConstT& _value, const NormalT& _fChangeValue) {
	NormalT* value = const_cast<NormalT*>(&_value);
	*value = _fChangeValue;
}

template<typename T>
static void Swap(T& _t1, T& _t2) {
	T tmp = _t2;
	_t2 = _t1;
	_t1 = tmp;
}

