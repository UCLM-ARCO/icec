// -*- mode: c++; coding: utf-8 -*-

module Base {
    module Example {
	interface Iface1 {
	    void sayHello();
	};
    };
};

module Extended {
    interface Iface2 extends Base::Example::Iface1 {
	void sayBye();
    };
};
