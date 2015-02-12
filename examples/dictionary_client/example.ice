// -*- mode: c++; coding: utf-8 -*-

module Example {
    dictionary<string, string> StringDict;

    interface PropertyService {
	void update(StringDict props);
    };
};
