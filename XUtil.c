#include "XHelper.h"

namespace XBoost {
	namespace Helper {
		boost::filesystem::path get_initial_path() {
			return boost::filesystem::initial_path<boost::filesystem::path>();
		}

		boost::filesystem::path get_current_path() {
			return boost::filesystem::current_path();
		}

		boost::filesystem::path get_program_path() {
			return boost::dll::program_location();
		}

		boost::filesystem::path get_module_path() {
			return boost::dll::this_line_location();
		}
	}
}

