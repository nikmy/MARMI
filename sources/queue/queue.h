#pragma once

#include "resource.h"
#include <queue>

// Declarations
namespace mtq
{

using gen::size_t;

template <class T, class Alloc>
class Queue;

template <class T, class Alloc>
class QueueIterator
    : public std::iterator<std::random_access_iterator_tag, T>
{
    friend class Queue<T, Alloc>;

 public:
    using iterator_category = std::random_access_iterator_tag;
    using value_t = T;

    QueueIterator(const QueueIterator& src);

    bool operator!=(const QueueIterator& rhs) const;
    bool operator==(const QueueIterator& rhs) const;

    typename QueueIterator::reference operator*() const;

    QueueIterator<T, Alloc>& operator+=(size_t offset);
    QueueIterator<T, Alloc>& operator-=(size_t offset);

    QueueIterator<T, Alloc>& operator++();
    const QueueIterator<T, Alloc> operator++(int);

    QueueIterator<T, Alloc>& operator--();
    const QueueIterator<T, Alloc> operator--(int);

 private:
    value_t* item_;
    value_t* base_;
    size_t size_;

    QueueIterator(value_t* p, value_t* ring, size_t size);
};

template <class T, class Alloc = std::allocator<T>>
class Queue
    : public gen::Resource<T>
{
 public:
    using alloc_traits = std::allocator_traits<Alloc>;
    using iterator = QueueIterator<T, Alloc>;
    using allocator_t = Alloc;
    using value_t = T;

    Queue();
    ~Queue();

    explicit Queue(size_t init_capacity);
    Queue(std::initializer_list<T> list);

    Queue(const Queue& src);
    Queue(Queue&& src) noexcept;

    Queue& operator=(const Queue& rhs);
    Queue& operator=(Queue&& rhs) noexcept;

    const value_t& front() const noexcept;
    const value_t& back() const noexcept;

    value_t& front() noexcept;
    value_t& back() noexcept;

    bool empty() const noexcept;

    size_t size() const noexcept;
    size_t max_size() const noexcept;

    void pop() noexcept;

    void clear() noexcept;

    value_t take_first() noexcept;

    void push(const value_t& x);
    void push(value_t&& x);

    template <class...Args>
    void emplace(Args&& ...args);

    void move_to(Queue<T>& other);

    void swap(Queue<T>& other) noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;
    T get_data() override;
    bool is_full() override;
    bool is_empty() override;

 private:
    value_t* ring_;

    size_t size_;
    size_t capacity_;

    size_t front_;
    size_t back_;

    static constexpr size_t MIN_CAP = 8;
    static constexpr size_t MAX_CAP = 4096;
    static allocator_t& Get_Allocator();

//    void resize_(size_t new_capacity) noexcept;

    template <class Q = Queue<T>>
    void emplace_(Q&& q) noexcept;
};
}

// Definitions
namespace mtq
{

template <class T, class Alloc>
QueueIterator<T, Alloc>::QueueIterator(const QueueIterator& src)
    : item_(src.item_), base_(src.base_), size_(src.size_)
{ }

template <class T, class Alloc>
bool QueueIterator<T, Alloc>::operator!=(const QueueIterator& rhs) const
{
    return item_ != rhs.item_;
}

template <class T, class Alloc>
bool QueueIterator<T, Alloc>::operator==(const QueueIterator& rhs) const
{
    return item_ == rhs.item_;
}

template <class T, class Alloc>
typename QueueIterator<T, Alloc>::reference
QueueIterator<T, Alloc>::operator*() const
try
{
    if (item_ < base_ || item_ - size_ > base_)
        throw std::out_of_range("Invalid iterator\n");
    return *item_;
}
catch (std::out_of_range& e) {
    e.what();
    throw;
}

template <class T, class Alloc>
QueueIterator<T, Alloc>&
QueueIterator<T, Alloc>::operator+=(size_t offset)
{
    item_ = ((item_ - base_) + offset) % size_ + base_;
    return *this;
}

template <class T, class Alloc>
QueueIterator<T, Alloc>&
QueueIterator<T, Alloc>::operator-=(size_t offset)
{
    item_ = (item_ >= offset) ? item_ - offset : size_ - offset + item_;
    return *this;
}

template <class T, class Alloc>
QueueIterator<T, Alloc>& QueueIterator<T, Alloc>::operator++()
{
    *this += 1;
    return *this;
}

template <class T, class Alloc>
const QueueIterator<T, Alloc> QueueIterator<T, Alloc>::operator++(int)
{
    QueueIterator<T, Alloc> copy = *this;
    ++(*this);
    return copy;
}

template <class T, class Alloc>
QueueIterator<T, Alloc>& QueueIterator<T, Alloc>::operator--()
{
    item_ = (item_) ? item_ - 1 : size_ - 1;
    return *this;
}

template <class T, class Alloc>
const QueueIterator<T, Alloc> QueueIterator<T, Alloc>::operator--(int)
{
    QueueIterator<T, Alloc> copy = *this;
    --(*this);
    return copy;
}

template <class T, class Alloc>
QueueIterator<T, Alloc>::QueueIterator(
    value_t* p,
    QueueIterator::value_t* ring,
    const size_t size
)
{
    item_ = p;
    base_ = ring;
    size_ = size;
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue()
    :
    ring_(nullptr),
    size_(0),
    capacity_(0),
    front_(0),
    back_(0)
{ }

template <class T, class Alloc>
Queue<T, Alloc>::~Queue()
{ clear(); }

template <class T, class Alloc>
Queue<T, Alloc>::Queue(size_t init_capacity)
    : Queue()
{
    if (init_capacity) {
        size_t cap = 1;
        while (cap < init_capacity)
            cap <<= 1;
        capacity_  = cap;
        ring_      = alloc_traits::allocate(Get_Allocator(), capacity_);
    }
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(const std::initializer_list<T> list)
    : Queue()
{
    capacity_ = MIN_CAP;
    while (capacity_ < list.size())
        capacity_ <<= 1;
    ring_     = alloc_traits::allocate(Get_Allocator(), capacity_);
    front_    = back_ = 0;
    for (auto& item : list) {
        alloc_traits::construct(Get_Allocator(), ring_ + back_, item);
        ++back_;
    }
    size_ = back_;
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(const Queue& src)
    : Queue()
{
    if (src.capacity_) {
        ring_     = alloc_traits::allocate(Get_Allocator(), src.capacity_);
        capacity_ = src.capacity_;
        size_     = src.size_;
        back_     = src.back_;
        front_    = src.front_;
    }

    for (size_t i = 0; i < src.size_; ++i)
        alloc_traits::construct(Get_Allocator(), ring_ + i, src.ring_[i]);
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(Queue&& src) noexcept
    : Queue()
{ this->swap(src); }

template <class T, class Alloc>
Queue<T, Alloc>&
Queue<T, Alloc>::operator=(const Queue& rhs)
{
    if (this != &rhs) {
        Queue<T> copy(rhs);
        this->swap(copy);
    }
    return *this;
}

template <class T, class Alloc>
Queue<T, Alloc>&
Queue<T, Alloc>::operator=(Queue&& rhs) noexcept
{
    if (this != &rhs) {
        Queue<T> moved(std::move(rhs));
        this->swap(moved);
    }
    return *this;
}

template <class T, class Alloc>
const T& Queue<T, Alloc>::front() const noexcept
{ return ring_[front_]; }

template <class T, class Alloc>
const T& Queue<T, Alloc>::back() const noexcept
{ return (back_ ? ring_[back_ - 1] : ring_[capacity_ - 1]); }

template <class T, class Alloc>
T& Queue<T, Alloc>::front() noexcept
{ return ring_[front_]; }

template <class T, class Alloc>
T& Queue<T, Alloc>::back() noexcept
{ return (back_ ? ring_[back_ - 1] : ring_[capacity_ - 1]); }

template <class T, class Alloc>
bool Queue<T, Alloc>::empty() const noexcept
{ return (size_ == 0); }

template <class T, class Alloc>
size_t Queue<T, Alloc>::size() const noexcept
{ return size_; }

template <class T, class Alloc>
size_t Queue<T, Alloc>::max_size() const noexcept
{ return capacity_; }

template <class T, class Alloc>
void Queue<T, Alloc>::pop() noexcept
{
    if (size_) {
        alloc_traits::destroy(Get_Allocator(), ring_ + front_);
        front_ = (front_ + 1) & (capacity_ - 1);

        if (--size_ == 0)
            front_ = back_ = 0;
    }
}

template <class T, class Alloc>
void Queue<T, Alloc>::clear() noexcept
{
    if (capacity_ == 0)
        return;

    while (size_)
        pop();

    alloc_traits::deallocate(Get_Allocator(), ring_, capacity_);
    capacity_ = size_ = 0;
    front_    = back_ = 0;
    ring_     = nullptr;
}

template <class T, class Alloc>
T Queue<T, Alloc>::take_first() noexcept
{
    value_t value = front();
    pop();
    return value;
}

template <class T, class Alloc>
void Queue<T, Alloc>::push(const value_t& x)
{ emplace(x); }

template <class T, class Alloc>
void Queue<T, Alloc>::push(value_t&& x)
{ emplace(std::move(x)); }

template <class T, class Alloc>
template <class...Args>
void Queue<T, Alloc>::emplace(Args&& ...args)
{
    if (size_ == capacity_) {
        size_t new_capacity = capacity_ ? (capacity_ * 2) : MIN_CAP;
        T* new_ring = alloc_traits::allocate(Get_Allocator(), new_capacity);
        alloc_traits::construct(
            Get_Allocator(), new_ring + size_, std::forward<Args>(args)...
        );

        for (size_t it = 0; it < size_; ++it) {
            alloc_traits::construct(
                Get_Allocator(),
                new_ring + it,
                std::move_if_noexcept(ring_[front_])
            );
            front_ = (front_ + 1) & (capacity_ - 1);
        }

        size_t t = size_ + 1;
        clear();
        size_ = t;
        back_ = t;
        capacity_ = new_capacity;
        ring_ = new_ring;

        return;
    }

    alloc_traits::construct(
        Get_Allocator(), ring_ + back_, std::forward<Args>(args)...
    );
    back_ = capacity_ ? (back_ + 1) & (capacity_ - 1) : 1;
    ++size_;
}

template <class T, class Alloc>
void Queue<T, Alloc>::move_to(Queue<T>& other)
try
{
    if (&other == this) {
        throw std::invalid_argument("Queue::emplace_: "
                                    "Moving to self detected\n");
    }
    other.emplace_(std::move(*this));
}
catch (std::invalid_argument& e) {
    e.what();
    throw;
}

template <class T, class Alloc>
void Queue<T, Alloc>::swap(Queue<T>& other) noexcept
{
    if (this != &other) {
        std::swap(ring_, other.ring_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(front_, other.front_);
        std::swap(back_, other.back_);
    }
}

template <class T, class Alloc>
typename Queue<T, Alloc>::iterator
Queue<T, Alloc>::begin() noexcept
{
    return QueueIterator<value_t, allocator_t>(
        ring_ + front_, ring_, capacity_);
}

template <class T, class Alloc>
typename Queue<T, Alloc>::iterator
Queue<T, Alloc>::end() noexcept
{
    return QueueIterator<value_t, allocator_t>(
        ring_ + back_, ring_, capacity_);
}

template <class T, class Alloc>
Alloc& Queue<T, Alloc>::Get_Allocator()
{
    static auto allocator = allocator_t();
    return allocator;
}

//template <class T, class Alloc>
//void
//Queue<T, Alloc>::resize_(size_t new_capacity) noexcept
//{
//    capacity_ = new_capacity;
//    T* new_ring = alloc_traits::allocate(Get_Allocator(), capacity_);
//    for (size_t it = 0; it < size_; ++it) {
//        alloc_traits::construct(
//            Get_Allocator(),
//            new_ring + it,
//            std::move_if_noexcept(ring_[front_])
//        );
//        front_ = (front_ + 1) % (capacity_ / 2);
//    }
//    if (ring_) {
//        size_t prev_sz = size_;
//        size_t prev_cp = capacity_;
//        clear();
//        size_          = prev_sz;
//        capacity_      = prev_cp;
//    }
//    alloc_traits::deallocate(Get_Allocator(), ring_, capacity_);
//    ring_  = new_ring;
//    front_ = 0;
//    back_  = size_;
//}

template <class T, class Alloc>
template <class Q>
void Queue<T, Alloc>::emplace_(Q&& q) noexcept
{
    size_t      it = q.front_;
    for (size_t i  = 0; i < q.size_; ++i) {
        emplace(std::move_if_noexcept(q.ring_[it]));
        it = (it + 1) & (q.capacity_ - 1);
    }
    q.clear();
}

template <class T, class Alloc>
T Queue<T, Alloc>::get_data()
{
    return take_first();
}

template <class T, class Alloc>
bool Queue<T, Alloc>::is_full()
{
    return size_ == capacity_;
}

template <class T, class Alloc>
bool Queue<T, Alloc>::is_empty()
{
    return empty();
}

}