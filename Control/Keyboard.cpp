#include "Keyboard.h"

Keyboard_State::Keyboard_State() {
	active = true;
	keyboard_fd = 0;
	keyboard_ev = new input_event();
	keyboard_st = new keyboard_state();  
    keyboard_fd = open(KEYBOARD_DEV, O_RDONLY | O_NONBLOCK);
	if (keyboard_fd > 0) {
		ioctl(keyboard_fd, EVIOCGNAME(256), name);
		std::cout << "   Name: " << name << std::endl;
		active = true;
        pthread_create(&thread, 0, &Keyboard_State::loop, this);
    } else {
        std::cout<<"failed to open keyboard event file"<<std::endl;
    }
}

Keyboard_State::~Keyboard_State() {
	if (keyboard_fd > 0) {
		active = false;
		pthread_join(thread, 0);
		close(keyboard_fd);
	}
	delete keyboard_st;
	delete keyboard_ev;
	keyboard_fd = 0;
}

void* Keyboard_State::loop(void *obj) {
    while (reinterpret_cast<Keyboard_State *>(obj)->active) reinterpret_cast<Keyboard_State *>(obj)->readEv();
}

void Keyboard_State::readEv() {
	int bytes = read(keyboard_fd, keyboard_ev, sizeof(*keyboard_ev));
	if (bytes > 0) {
		if (keyboard_ev->type & EV_KEY) {
			keyboard_st->keys[keyboard_ev->code] = keyboard_ev->value;
		}
	}
}

short Keyboard_State::getKeyState(short key) {
	return keyboard_st->keys[key];
}
