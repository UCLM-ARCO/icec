// -*- mode: c++; coding: utf-8 -*-

module Ice {
    sequence<string> StrSeq;

    interface ObjectIface {
        idempotent void   ice_ping();
        idempotent bool   ice_isA(string typeID);
        idempotent string ice_id();
        idempotent StrSeq ice_ids();
    };
};