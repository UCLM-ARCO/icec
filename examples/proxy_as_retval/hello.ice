// -*- mode: c++; coding: utf-8 -*-

module Example {

    interface Speaker {
	void say(string msg);
    };

    interface Manager {
	Speaker* getSpeaker();
    };

};
