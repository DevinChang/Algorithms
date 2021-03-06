#pragma once
#include <iostream>
#include <string>
#include <memory>
#include <initializer_list>
#include <algorithm>
#include <utility>




template <typename> class Vector;
template <typename T>
std::ostream &operator<< (std::ostream &os, const Vector<T> &t);

template <typename T> class Vector {
	friend std::ostream& operator<< <T>(std::ostream &os, const Vector<T> &t);
public:
	//constructor
	Vector() = default;
	Vector(std::initializer_list<T>);
	//copy constructor
	Vector(const Vector<T> &);
	//move constructor
	Vector(Vector<T> &&) noexcept;
	//assignment operator
	Vector &operator= (const Vector<T> &);
	Vector &operator= (std::initializer_list<T>);
	Vector &operator= (Vector<T> &&) noexcept;
	//subscript operator
	T &operator[] (size_t n);
	const T &operator[] (std::size_t n) const;
	//destructor
	~Vector();
public:
	//interface
	void push_back(const T&);
	T &pop() { return *--first_free; }
	size_t size() const { return first_free - elements; }
	size_t capacity() const { return cap - elements; }
	T *begin() { return elements; }
	T *end() { return first_free; }
	template <typename...Args>
	void emplace_back(Args&&...args);
	void resize(size_t n);
	void resize(size_t n, T &);
	void reserve(size_t);
	bool empty();
private:
	static std::allocator<T> alloc;
	void check_n_copy() { if (size() == capacity()) reallocate(); }
	std::pair<T *, T *> alloc_n_copy(const T *, const T *);
	void reallocate();
	void reallocate(size_t);
	void free();
	T *elements;
	T *first_free;
	T *cap;
};

template <typename T>
std::allocator<T> Vector<T>::alloc;


template<typename T>
Vector<T>::Vector(std::initializer_list<T> il) {
	auto newdata = alloc_n_copy(il.begin(), il.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}

template<typename T>
Vector<T>::Vector(const Vector<T> &Vector) {
	auto newdata = alloc_n_copy(Vector.begin(), Vector.end());
	elements = newdata.first;
	first_free = cap = newdata.second;
}

template<typename T>
Vector<T>::Vector(Vector<T> &&Vector) noexcept : elements(Vector.elements), first_free(Vector.firstr_free), cap(Vector.cap) {
	Vector.elements = Vector.first_free = Vector.cap = nullptr;
}

template<typename T>
Vector<T> & Vector<T>::operator=(const Vector<T> &v) {
	auto newdata = alloc_n_copy(v.begin(), v.end());
	free();
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this;
}

template<typename T>
Vector<T> & Vector<T>::operator=(std::initializer_list<T> il) {
	auto newdata = alloc_n_copy(il.begin(), il.end());
	free();
	elements = newdata.first;
	first_free = cap = newdata.second;
	return *this;
}

template<typename T>
Vector<T> & Vector<T>::operator=(Vector<T> &&v) noexcept {
	if (this != &v) {
		free();
		elements = v.elements;
		first_free = v.first_free;
		cap = v.cap;
		v.elements = v.first_free = v.cap = nullptr;
	}
	return *this;
}

template<typename T>
T & Vector<T>::operator[](size_t n) {
	return elements[n];
}

template<typename T>
const T & Vector<T>::operator[](std::size_t n) const {
	return elements[n];
}

template<typename T>
Vector<T>::~Vector() {
	free();
}

template<typename T>
void Vector<T>::push_back(const T &t) {
	check_n_copy();
	alloc.construct(first_free++, t);
}

template<typename T>
void Vector<T>::resize(size_t n) {
	if (n > size()) {
		while (n < size())
			push_back(T());
	}
	else if (size() > n) {
		while (size() > n)
			alloc.destroy(--first_free);
	}
}

template<typename T>
void Vector<T>::resize(size_t n, T &t) {
	if (n > size()) {
		while (n > size())
			push_back(t);
	}
}

template<typename T>
void Vector<T>::reserve(size_t n) {
	if (n > capacity())
		reallocate(n);
}

template<typename T>
inline bool Vector<T>::empty(){
	if (first_free - elements == 0)
		return true;
	return false;
}

template<typename T>
std::pair<T*, T*> Vector<T>::alloc_n_copy(const T *b, const T *e) {
	auto newdata = alloc.allocate(e - b);
	return{ newdata, std::uninitialized_copy(b, e, newdata) };
}

template<typename T>
void Vector<T>::reallocate() {
	auto newcap = size() ? 2 * size() : 1;
	auto newdata = alloc.allocate(newcap);
	auto dest = newdata;
	auto elem = elements;
	for (size_t i = 0; i != size(); ++i)
		alloc.construct(dest++, std::move(*elem++));
	free();
	elements = newdata;
	first_free = dest;
	cap = elements + newcap;
}

template<typename T>
void Vector<T>::reallocate(size_t n) {
	auto newdata = alloc.allocate(n);
	auto dest = newdata;
	auto elem = elements;
	for (size_t i = 0; i != size(); ++i)
		alloc.construct(dest++, std::move(*elem++));
	free();
	elements = newdata;
	first_free = dest;
	cap = elements + n;
}


template<typename T>
void Vector<T>::free() {
	if (elements) {
		std::for_each(elements, first_free, [](const T &t) {alloc.destroy(&t); });
		alloc.deallocate(elements, cap - elements);
	}
}

template<typename T>
template<typename ...Args>
void Vector<T>::emplace_back(Args && ...args) {
	check_n_copy();
	alloc.construct(first_free++, std::forward<Args>(args)...);
}

template<typename T>
std::ostream & operator<<(std::ostream & os, const Vector<T>& t) {
	return os << *t.elements;
}