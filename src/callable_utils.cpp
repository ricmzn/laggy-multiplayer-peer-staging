#include "callable_utils.h"

#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/variant/utility_functions.hpp>

void CallableUtils::log_call_error(const Callable &p_callable, const Variant **p_argptrs, int p_argcount, const GDExtensionCallError &p_call_error) {
	String err_text;

	if (p_call_error.error == GDEXTENSION_CALL_ERROR_INVALID_ARGUMENT) {
		int errorarg = p_call_error.argument;
		if (p_argptrs) {
			err_text = "Cannot convert argument " + itos(errorarg + 1) + " from " + Variant::get_type_name(p_argptrs[errorarg]->get_type()) + " to " + Variant::get_type_name(Variant::Type(p_call_error.expected));
		} else {
			err_text = "Cannot convert argument " + itos(errorarg + 1) + " from [missing argptr, type unknown] to " + Variant::get_type_name(Variant::Type(p_call_error.expected));
		}
	} else if (p_call_error.error == GDEXTENSION_CALL_ERROR_TOO_MANY_ARGUMENTS) {
		err_text = "Method expected " + itos(p_call_error.expected) + " arguments, but called with " + itos(p_argcount);
	} else if (p_call_error.error == GDEXTENSION_CALL_ERROR_TOO_FEW_ARGUMENTS) {
		err_text = "Method expected " + itos(p_call_error.expected) + " arguments, but called with " + itos(p_argcount);
	} else if (p_call_error.error == GDEXTENSION_CALL_ERROR_INVALID_METHOD) {
		err_text = "Method not found";
	} else if (p_call_error.error == GDEXTENSION_CALL_ERROR_INSTANCE_IS_NULL) {
		err_text = "Instance is null";
	} else if (p_call_error.error == GDEXTENSION_CALL_ERROR_METHOD_NOT_CONST) {
		err_text = "Method not const in const instance";
	} else {
		err_text = "Unexpected call error";
	}

	String base_text;
	if (Object *base = p_callable.get_object()) {
		base_text = base->get_class();
		if (Ref<Resource> script = base->get_script(); script.is_valid()) {
			base_text += "(" + script->get_path().get_file() + ")";
		}
		base_text += "::";
	}

	UtilityFunctions::push_error("'" + base_text + String(p_callable.get_method()) + "': " + err_text);
}
