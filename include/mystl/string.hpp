#pragma once

#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <ostream>

#include "mystl/utility.hpp"

namespace mystl {

class string {
public:
    using size_type = std::size_t;
    using iterator = char*;
    using const_iterator = const char*;

    static constexpr size_type npos = static_cast<size_type>(-1);

private:
    static constexpr size_type INLINE_CAPACITY = 15;

    size_type size_ = 0;
    size_type cap_ = INLINE_CAPACITY;
    union {
        char inline_buf_[INLINE_CAPACITY + 1];
        char* heap_;
    };

    bool is_small() const {
        return cap_ <= INLINE_CAPACITY;
    }

    char* raw() {
        return is_small() ? inline_buf_ : heap_;
    }

    const char* raw() const {
        return is_small() ? inline_buf_ : heap_;
    }

    void init_raw(size_type required) {
        if (required <= INLINE_CAPACITY) {
            cap_ = INLINE_CAPACITY;
        } else {
            heap_ = new char[required + 1];
            cap_ = required;
        }
    }

    void ensure_capacity(size_type need) {
        if (need <= cap_) {
            return;
        }
        size_type new_cap = cap_ * 2;
        if (new_cap < need) {
            new_cap = need;
        }
        reserve(new_cap);
    }

public:
    string() {
        inline_buf_[0] = '\0';
    }

    string(const char* s) : string(s, std::strlen(s)) {}

    string(const char* s, size_type n) {
        init_raw(n);
        std::memcpy(raw(), s, n);
        raw()[n] = '\0';
        size_ = n;
    }

    string(size_type count, char c) {
        init_raw(count);
        char* d = raw();
        for (size_type i = 0; i < count; ++i) {
            d[i] = c;
        }
        d[count] = '\0';
        size_ = count;
    }

    string(const string& o) {
        init_raw(o.size_);
        std::memcpy(raw(), o.raw(), o.size_);
        raw()[o.size_] = '\0';
        size_ = o.size_;
    }

    string(string&& o) noexcept {
        if (o.is_small()) {
            std::memcpy(inline_buf_, o.inline_buf_, o.size_ + 1);
            cap_ = INLINE_CAPACITY;
        } else {
            heap_ = o.heap_;
            cap_ = o.cap_;
        }
        size_ = o.size_;

        o.cap_ = INLINE_CAPACITY;
        o.size_ = 0;
        o.inline_buf_[0] = '\0';
    }

    ~string() {
        if (!is_small()) {
            delete[] heap_;
        }
    }

    string& operator=(const string& o) {
        if (this != &o) {
            if (o.size_ <= cap_) {
                std::memcpy(raw(), o.raw(), o.size_);
                raw()[o.size_] = '\0';
                size_ = o.size_;
            } else {
                char* nb = new char[o.size_ + 1];
                std::memcpy(nb, o.raw(), o.size_);
                nb[o.size_] = '\0';
                if (!is_small()) {
                    delete[] heap_;
                }
                heap_ = nb;
                cap_ = o.size_;
                size_ = o.size_;
            }
        }
        return *this;
    }

    string& operator=(string&& o) noexcept {
        if (this != &o) {
            if (!is_small()) {
                delete[] heap_;
            }
            if (o.is_small()) {
                std::memcpy(inline_buf_, o.inline_buf_, o.size_ + 1);
                cap_ = INLINE_CAPACITY;
            } else {
                heap_ = o.heap_;
                cap_ = o.cap_;
            }
            size_ = o.size_;

            o.cap_ = INLINE_CAPACITY;
            o.size_ = 0;
            o.inline_buf_[0] = '\0';
        }
        return *this;
    }

    string& operator=(const char* s) {
        return *this = string(s);
    }

    size_type size() const {
        return size_;
    }

    size_type length() const {
        return size_;
    }

    size_type capacity() const {
        return cap_;
    }

    bool empty() const {
        return size_ == 0;
    }

    const char* c_str() const {
        return raw();
    }

    const char* data() const {
        return raw();
    }

    char* data() {
        return raw();
    }

    char& operator[](size_type pos) {
        return raw()[pos];
    }

    const char& operator[](size_type pos) const {
        return raw()[pos];
    }

    char& at(size_type pos) {
        if (pos >= size_) {
            throw std::out_of_range("mystl::string::at");
        }
        return raw()[pos];
    }

    const char& at(size_type pos) const {
        if (pos >= size_) {
            throw std::out_of_range("mystl::string::at");
        }
        return raw()[pos];
    }

    char& front() {
        return raw()[0];
    }

    const char& front() const {
        return raw()[0];
    }

    char& back() {
        return raw()[size_ - 1];
    }

    const char& back() const {
        return raw()[size_ - 1];
    }

    void reserve(size_type new_cap) {
        if (new_cap <= cap_) {
            return;
        }
        char* nb = new char[new_cap + 1];
        std::memcpy(nb, raw(), size_ + 1);
        if (!is_small()) {
            delete[] heap_;
        }
        heap_ = nb;
        cap_ = new_cap;
    }

    void shrink_to_fit() {
        if (is_small()) {
            return;
        }
        if (size_ <= INLINE_CAPACITY) {
            char* old = heap_;
            std::memcpy(inline_buf_, old, size_ + 1);
            cap_ = INLINE_CAPACITY;
            delete[] old;
        } else if (cap_ > size_) {
            char* nb = new char[size_ + 1];
            std::memcpy(nb, heap_, size_ + 1);
            delete[] heap_;
            heap_ = nb;
            cap_ = size_;
        }
    }

    void clear() {
        size_ = 0;
        raw()[0] = '\0';
    }

    void resize(size_type n, char c = '\0') {
        if (n > size_) {
            ensure_capacity(n);
            char* d = raw();
            for (size_type i = size_; i < n; ++i) {
                d[i] = c;
            }
            size_ = n;
            d[size_] = '\0';
        } else {
            size_ = n;
            raw()[size_] = '\0';
        }
    }

    void push_back(char c) {
        ensure_capacity(size_ + 1);
        char* d = raw();
        d[size_] = c;
        d[size_ + 1] = '\0';
        ++size_;
    }

    void pop_back() {
        --size_;
        raw()[size_] = '\0';
    }

    string& append(const char* s, size_type n) {
        ensure_capacity(size_ + n);
        char* d = raw();
        std::memcpy(d + size_, s, n);
        size_ += n;
        d[size_] = '\0';
        return *this;
    }

    string& append(const char* s) {
        return append(s, std::strlen(s));
    }

    string& append(const string& o) {
        return append(o.raw(), o.size_);
    }

    string& operator+=(const string& o) {
        return append(o);
    }

    string& operator+=(const char* s) {
        return append(s);
    }

    string& operator+=(char c) {
        push_back(c);
        return *this;
    }

    string substr(size_type pos = 0, size_type count = npos) const {
        if (pos > size_) {
            throw std::out_of_range("mystl::string::substr");
        }
        size_type n = (count < size_ - pos) ? count : size_ - pos;
        return string(raw() + pos, n);
    }

    size_type find(char c, size_type pos = 0) const {
        for (size_type i = pos; i < size_; ++i) {
            if (raw()[i] == c) {
                return i;
            }
        }
        return npos;
    }

    size_type find(const char* s, size_type pos = 0) const {
        size_type n = std::strlen(s);
        if (n == 0) {
            return pos <= size_ ? pos : npos;
        }
        if (n > size_) {
            return npos;
        }
        for (size_type i = pos; i + n <= size_; ++i) {
            if (std::memcmp(raw() + i, s, n) == 0) {
                return i;
            }
        }
        return npos;
    }

    size_type rfind(char c, size_type pos = npos) const {
        if (size_ == 0) {
            return npos;
        }
        size_type i = (pos < size_ - 1) ? pos : size_ - 1;
        for (;; --i) {
            if (raw()[i] == c) {
                return i;
            }
            if (i == 0) {
                break;
            }
        }
        return npos;
    }

    void swap(string& o) {
        string t = mystl::move(*this);
        *this = mystl::move(o);
        o = mystl::move(t);
    }

    int compare(const string& o) const {
        size_type n = size_ < o.size_ ? size_ : o.size_;
        int c = std::memcmp(raw(), o.raw(), n);
        if (c != 0) {
            return c < 0 ? -1 : 1;
        }
        if (size_ < o.size_) {
            return -1;
        }
        if (size_ > o.size_) {
            return 1;
        }
        return 0;
    }

    iterator begin() {
        return raw();
    }

    const_iterator begin() const {
        return raw();
    }

    iterator end() {
        return raw() + size_;
    }

    const_iterator end() const {
        return raw() + size_;
    }
};

inline bool operator==(const string& a, const string& b) {
    return a.size() == b.size() && a.compare(b) == 0;
}

inline bool operator!=(const string& a, const string& b) {
    return !(a == b);
}

inline bool operator<(const string& a, const string& b) {
    return a.compare(b) < 0;
}

inline bool operator>(const string& a, const string& b) {
    return b < a;
}

inline bool operator<=(const string& a, const string& b) {
    return !(b < a);
}

inline bool operator>=(const string& a, const string& b) {
    return !(a < b);
}

inline bool operator==(const string& a, const char* b) {
    return a.compare(string(b)) == 0;
}

inline bool operator==(const char* a, const string& b) {
    return string(a).compare(b) == 0;
}

inline bool operator!=(const string& a, const char* b) {
    return !(a == b);
}

inline bool operator!=(const char* a, const string& b) {
    return !(a == b);
}

inline bool operator<(const string& a, const char* b) {
    return a.compare(string(b)) < 0;
}

inline bool operator<(const char* a, const string& b) {
    return string(a).compare(b) < 0;
}

inline bool operator>(const string& a, const char* b) {
    return b < a;
}

inline bool operator>(const char* a, const string& b) {
    return b < a;
}

inline bool operator<=(const string& a, const char* b) {
    return !(b < a);
}

inline bool operator<=(const char* a, const string& b) {
    return !(b < a);
}

inline bool operator>=(const string& a, const char* b) {
    return !(a < b);
}

inline bool operator>=(const char* a, const string& b) {
    return !(a < b);
}

inline string operator+(const string& a, const string& b) {
    string r;
    r.reserve(a.size() + b.size());
    r += a;
    r += b;
    return r;
}

inline string operator+(const string& a, const char* b) {
    string r(a);
    r += b;
    return r;
}

inline string operator+(const char* a, const string& b) {
    string r(a);
    r += b;
    return r;
}

inline std::ostream& operator<<(std::ostream& os, const string& s) {
    return os.write(s.data(), static_cast<std::streamsize>(s.size()));
}

}
