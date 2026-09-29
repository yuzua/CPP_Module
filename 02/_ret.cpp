struct Trivial {
    int v;
    Trivial add(Trivial const &rhs) const {
        Trivial r;
        r.v = v + rhs.v;
        return r;
    }
};

struct User {
    int v;
    User(void) : v(0) {}
    User(User const &o) : v(o.v) {}
    ~User(void) {}
    User add(User const &rhs) const {
        User r;
        r.v = v + rhs.v;
        return r;
    }
};

Trivial tadd(Trivial const &a, Trivial const &b) { return a.add(b); }
User uadd(User const &a, User const &b) { return a.add(b); }
