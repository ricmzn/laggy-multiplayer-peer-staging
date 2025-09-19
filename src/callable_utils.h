#ifndef LAGGYMULTIPLAYERPEER_CALLABLE_UTILS_H
#define LAGGYMULTIPLAYERPEER_CALLABLE_UTILS_H

#include <godot_cpp/variant/variant.hpp>

#define SNAME(name) []() { static StringName _name = StringName(name, true); return _name; }()

using namespace godot;

typedef CowData<void*>::Size Size;

class CallableUtils {
	static void log_call_error(const Callable &p_callable, const Variant **p_argptrs, int p_argcount, const GDExtensionCallError &p_call_error);

public:
	template <typename... Args>
	static Variant call(const Callable &p_callable, Error &out_error, Args... args) {
		Variant wrapped_callable = Variant(p_callable);
		std::array<Variant, sizeof...(args)> vargs = { args... };
		std::array<const Variant *, sizeof...(args)> argptrs;
		for (size_t i = 0; i < vargs.size(); i++) {
			argptrs[i] = &vargs[i];
		}

		Variant result;
		GDExtensionCallError call_error;
		wrapped_callable.callp(SNAME("call"), argptrs.data(), argptrs.size(), result, call_error);
		if (call_error.error == GDEXTENSION_CALL_OK) {
			out_error = OK;
		} else {
			log_call_error(p_callable, argptrs.data(), argptrs.size(), call_error);
			out_error = FAILED;
		}
		return result;
	}
};

#endif //LAGGYMULTIPLAYERPEER_CALLABLE_UTILS_H
