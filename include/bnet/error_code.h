#pragma once
#include <system_error>
#include <string>

namespace bnet {
	enum class ERROR_CODE {
		kSuccess = 0,
		kFailed,
		kDecodeVarint,
	};
	const char* error_msg[] = {
		"kSuccess",
		"kFailed",
	};
	const int kErrorMsgSize = sizeof(error_msg) / sizeof(char*);
	const char* kUndefinedErrorMsg = "undefined error msg.";

	class ErrorCategory : public std::error_category
	{
	public:
		ErrorCategory() {}

		std::string message(int c) const override
		{
			if (c >= 0 && c < kErrorMsgSize) {
				return error_msg[c];
			}
			return kUndefinedErrorMsg;
		}
		const char* name() const noexcept override { return "ErrorCategory"; }

		const static error_category& get()
		{
			const static ErrorCategory category_const;
			return category_const;
		}
	};
}

namespace std {
	template<>
	struct is_error_code_enum<bnet::ERROR_CODE> : true_type { };

	inline error_code make_error_code(bnet::ERROR_CODE e)
	{
		return error_code(static_cast<int>(e), bnet::ErrorCategory::get());
	}
}

namespace bnet {
	class ErrorCode {
	public:
		ErrorCode(ERROR_CODE code) : err_code_(code) { }
		ErrorCode(ERROR_CODE code, const char* str) : err_code_(code), extra_msg_(str) { }
		ErrorCode(ERROR_CODE code, std::string str) : err_code_(code), extra_msg_(std::move(str)) { }
		
		ERROR_CODE value() const {
			return (ERROR_CODE)err_code_.value();
		}
		std::string message() const {
			if (extra_msg_.empty()) {
				return err_code_.message();
			}
			return err_code_.message() + ",extra=" + extra_msg_;
		}
		explicit operator bool() const noexcept {
			return err_code_.value() != 0;
		}
		bool operator==(const ErrorCode& right) noexcept {
			return err_code_ == right.err_code_;
		}
		bool operator!=(const ErrorCode& other) noexcept {
			return !(*this == other);
		}
	private:
		std::error_code err_code_;
		std::string extra_msg_;
	};
}