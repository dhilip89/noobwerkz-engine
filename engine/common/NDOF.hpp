// Used to represent input from SpaceMouse NDOF, which was dead-simple to use along with libspnav (linked-in separately
#pragma once

#include <noob/math/math_funcs.hpp>

#include "Logger.hpp"

namespace noob
{
	class ndof
	{
		public:
			struct data
			{
				bool movement;
				noob::vec3f translation;
				noob::vec3f rotation;
			};

			enum button_state
			{
				PRESS, RELEASE, HELD
			};

			struct button_press
			{
				size_t button;
				noob::ndof::button_state state;
			};

			// bool init();
			void run();
			noob::ndof::data get_data();
			//std::vector<size_t> get_button_presses();
			//std::vector<size_t> get_held_buttons();
		protected:
			double x, y, z, rx, ry, rz;
			//std::vector<size_t> held_buttons;
			size_t ticks;
	};
}
