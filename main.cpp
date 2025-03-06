#include <iostream>
#include <memory>

#include <algorithm>
#include <vector>

#include <iterator>
#include <cstddef>

#include <cassert>
#include <map>


template<typename T>
struct Iterator
{
    using iterator_category = std::forward_iterator_tag;
    using difference_type   = std::ptrdiff_t;
    using value_type        = T;
    using pointer           = T*;
    using reference         = T&;

    Iterator(pointer ptr) : m_ptr(ptr) {}

    reference operator*() const {
        return *m_ptr;
    }
    pointer operator->() {
        return m_ptr;
    }
    Iterator& operator++() {
        m_ptr++;
        return *this;
    }
    Iterator operator++(int) {
        Iterator tmp = *this;
        ++(*this);
        return tmp;
    }
    friend bool operator== (const Iterator& a, const Iterator& b) {
        return a.m_ptr == b.m_ptr;
    };
    friend bool operator!= (const Iterator& a, const Iterator& b) {
        return a.m_ptr != b.m_ptr;
    };

private:
    pointer m_ptr;
};
template <typename T>
struct my_allocator {
    using value_type = T;
    using pointer = T*;
    using const_pointer = const T*;
    using reference = T&;
    using const_reference = const T&;

    my_allocator() noexcept {}
    T *allocate(size_t n) {
        return static_cast<T *>(::operator new(n * sizeof(T)));
    }
    void deallocate(T *ptr, size_t n) {
        ::operator delete(ptr);
    }

    template <class Up, class... Args>
    void construct(Up *ptr, Args &&...args) {
        ::new ((void *)ptr) Up(std::forward<Args>(args)...);
    }

    void destroy(pointer ptr) {
        ptr->~T();
    }
};
template <class T, class U>
constexpr bool operator==(const my_allocator<T> &, const my_allocator<U> &) noexcept {
    return true;
}

template <class T, class U>
constexpr bool operator!=(const my_allocator<T> &, const my_allocator<U> &) noexcept {
    return false;
}


template <class T, class Allocator = std::allocator<T>>
class Vector
{
private:
    using base_alloc = std::allocator_traits<Allocator>;
    using pointer = typename base_alloc::pointer;
public:
    using value_type = T;
    using iterator = Iterator<T>;

public:
    Vector(const Allocator& = Allocator()) : size(0), capacity(1), arr(new T[1]) {
    }
    void push_back(T&& element) {
        if (size == capacity) {
            reserve(2 * capacity);
        }
        arr[size] = std::move(element);
        size++;
    }
    virtual void reserve(size_t am) {
        if(am <= capacity) {
            return;
        } else {
            T *temp = base_alloc::allocate(alloc, am);
            for (size_t i = 0; i < capacity; ++i) {
                base_alloc::construct(alloc, &temp[i], arr[i]);
            }
            for (size_t i = 0; i < capacity; ++i) {
                base_alloc::destroy(alloc, &arr[i]);
            }
            base_alloc::deallocate(alloc, arr, capacity);
            capacity = am;
            arr = temp;
        }
    }
    T& operator[](size_t i) {
        return arr[i];
    }
    const T& operator[](size_t i) const {
       return arr[i];
    }
    iterator begin() const {
        return (iterator(&arr[0]));
    }
    iterator end() const {
        return (iterator(&arr[size]));
    }
    size_t size_() const {
        return size;
    }
    bool empty() const {
        return begin() == end();
    }

private:
    size_t size = 0;
    size_t capacity = 0;
    pointer arr;


    Allocator alloc;
};




int main()
{

    std::cout << "создание экземпляра std::map<int, int> с новым аллокатором: " << std::endl;
    std::map<int, int, std::less<int>, my_allocator<std::pair<int, int>>> map;
    int fact = 1;
    std::cout << "заполнение 10 элементами, где ключ - это число от 0 до 9, а значение - факториал ключа: " << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        fact *= i == 0 ? 1 : i;
        map.insert(std::make_pair(i, fact));
    }
    std::cout << "вывод на экран всех значений (ключ и значение разделены пробелом) хранящихся в контейнере: " << std::endl;
    for(const auto& [x, y] : map) {
        std::cout << "{" << x << ", " << y << "} ";
    }
    std::cout << std::endl;
    std::cout << "создание экземпляра своего контейнера для хранения значений типа int: " << std::endl;
    Vector<int, std::allocator<int>> vec;
    std::cout << std::boolalpha << "empty " << vec.empty() << std::endl;
    std::cout << "заполнение 10 элементами от 0 до 9: " << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        vec.push_back(i);
    }
    std::cout << std::boolalpha << "empty " << vec.empty() << std::endl;
    std::cout << "Выводим: " << std::endl;
    for(const auto& x : vec) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;
    std::cout << "создание экземпляра своего контейнера для хранения значений типа int с новым аллокатором: " << std::endl;
    Vector<int, my_allocator<int>> vec_m;
    std::cout << std::boolalpha << "empty " << vec_m.empty() << std::endl;
    std::cout << "заполнение 10 элементами от 0 до 9: " << std::endl;
    for (size_t i = 0; i < 10; ++i) {
        vec_m.push_back(i);
    }
    std::cout << std::boolalpha << "empty " << vec_m.empty() << std::endl;
    std::cout << "Выводим: " << std::endl;
    for(const auto& x : vec_m) {
        std::cout << x << ' ';
    }
    std::cout << std::endl;


    std::cout << "\nHello World! "<< std::endl;
    return 0;
}
