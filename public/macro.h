#ifndef Macro_h__
#define Macro_h__

#define RETURN_VOID(p) \
	if((p)) \
		return;

#define RETURN_VAL(p, ret) \
	if((p)) \
		return ret;

#endif // Macro_h__
