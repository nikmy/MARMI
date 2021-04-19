#pragma once

#include "gendef.h"

#include <iterator>
#include <mutex>

// Declarations
namespace gen
{

using std::size_t;

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

    typename QueueIterator::reference operator*() const noexcept;

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

    bool full() const noexcept;
    bool empty() const noexcept;

    size_t size() const noexcept;
    size_t max_size() const noexcept;

    void pop() noexcept;

    void clear() noexcept;

    value_t take_first() noexcept;

    void push(const value_t& x);
    void push(value_t&& x);

    template <class...Args>
    void emplace(Args&& ...args) noexcept;

    void move_to(Queue<T>& other) noexcept;

    void swap(Queue<T>& other) noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;

 private:
    value_t* data_;

    size_t size_;
    size_t capacity_;

    size_t front_;
    size_t back_;

    mutable std::mutex access_mutex_;

    static constexpr size_t MIN_CAP = 8;
    static allocator_t& Get_Allocator();

    template <class Q = Queue<T>>
    void emplace_tail_(Q&& q) noexcept;
};
}

// Definitions
namespace gen
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
QueueIterator<T, Alloc>::operator*() const noexcept
{ return *item_; }

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
    data_(nullptr),
    size_(0),
    capacity_(0),
    front_(0),
    back_(0)
{ }

template <class T, class Alloc>
Queue<T, Alloc>::~Queue()
{
    clear();
    alloc_traits::deallocate(Get_Allocator(), data_, capacity_);
    data_  = nullptr;
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(size_t init_capacity)
    : Queue()
{
    if (init_capacity) {
        size_t cap = 1;
        while (cap < init_capacity) {
            cap <<= 1;
        }
        capacity_  = cap;
        data_      = alloc_traits::allocate(Get_Allocator(), capacity_);
    }
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(const std::initializer_list<T> list)
    : Queue()
{
    capacity_ = MIN_CAP;
    while (capacity_ < list.size())
        capacity_ <<= 1;

    data_  = alloc_traits::allocate(Get_Allocator(), capacity_);
    front_ = back_ = 0;

    for (auto& item : list) {
        alloc_traits::construct(Get_Allocator(), data_ + back_, item);
        ++back_;
    }
    size_ = back_;
}

template <class T, class Alloc>
Queue<T, Alloc>::Queue(const Queue& src)
    : Queue()
{
    if (src.capacity_) {
        data_     = alloc_traits::allocate(Get_Allocator(), src.capacity_);
        capacity_ = src.capacity_;
        size_     = src.size_;
        back_     = src.back_;
        front_    = src.front_;
    }

    for (size_t i = 0; i < src.size_; ++i)
        alloc_traits::construct(Get_Allocator(), data_ + i, src.data_[i]);
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
{ return data_[front_]; }

template <class T, class Alloc>
const T& Queue<T, Alloc>::back() const noexcept
{ return (back_ ? data_[back_ - 1] : data_[capacity_ - 1]); }

template <class T, class Alloc>
T& Queue<T, Alloc>::front() noexcept
{ return data_[front_]; }

template <class T, class Alloc>
T& Queue<T, Alloc>::back() noexcept
{ return (back_ ? data_[back_ - 1] : data_[capacity_ - 1]); }

template <class T, class Alloc>
bool Queue<T, Alloc>::full() const noexcept
{
    std::lock_guard<std::mutex> guard(access_mutex_);
    return size_ == capacity_;
}

template <class T, class Alloc>
bool Queue<T, Alloc>::empty() const noexcept
{
    std::lock_guard<std::mutex> guard(access_mutex_);
    return (size_ == 0);
}

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
        alloc_traits::destroy(Get_Allocator(), data_ + front_);
        front_ = (front_ + 1) & (capacity_ - 1);

        if (--size_ == 0)
            front_ = back_ = 0;
    }
}

template <class T, class Alloc>
void Queue<T, Alloc>::clear() noexcept
{
    while (size_)
        pop();
    size_  = 0;
    front_ = back_ = 0;
}

template <class T, class Alloc>
T Queue<T, Alloc>::take_first() noexcept
{
#ifdef __INFO_DEBUG__
    if (empty()) {
        IllegalAccess("Nothing to take: queue is empty\n").what();
        return *data_;
    }
#endif  // __INFO_DEBUG__

    std::lock_guard<std::mutex> guard(access_mutex_);

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
void Queue<T, Alloc>::emplace(Args&& ...args) noexcept
{
    std::lock_guard<std::mutex> guard(access_mutex_);
    if (size_ == capacity_) {
        size_t new_capacity = capacity_ ? (capacity_ * 2) : MIN_CAP;
        try {
            T* new_ring = alloc_traits::allocate(Get_Allocator(), new_capacity);
            alloc_traits::construct(
                Get_Allocator(), new_ring + size_, std::forward<Args>(args)...
            );

            for (size_t it = 0; it < size_; ++it) {
                alloc_traits::construct(
                    Get_Allocator(),
                    new_ring + it,
                    std::move_if_noexcept(data_[front_])
                );
                front_ = (front_ + 1) & (capacity_ - 1);
            }
            alloc_traits::deallocate(Get_Allocator(), data_, capacity_);

            back_ = ++size_;
            capacity_ = new_capacity;
            data_ = new_ring;
        } catch (std::bad_alloc& e) {
#ifdef __INFO_DEBUG__
            e.what();
#endif  // __INFO_DEBUG__
        }
        return;
    }

    alloc_traits::construct(
        Get_Allocator(), data_ + back_, std::forward<Args>(args)...
    );
    back_ = capacity_ ? (back_ + 1) & (capacity_ - 1) : 1;
    ++size_;
}

template <class T, class Alloc>
void Queue<T, Alloc>::move_to(Queue<T>& other) noexcept
{
    if (&other != this)
        other.emplace_tail_(std::move(*this));
}

template <class T, class Alloc>
void Queue<T, Alloc>::swap(Queue<T>& other) noexcept
{
    if (this != &other) {
        std::swap(data_, other.data_);
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
        data_ + front_, data_, capacity_);
}

template <class T, class Alloc>
typename Queue<T, Alloc>::iterator
Queue<T, Alloc>::end() noexcept
{
    return QueueIterator<value_t, allocator_t>(
        data_ + back_, data_, capacity_);
}

template <class T, class Alloc>
Alloc& Queue<T, Alloc>::Get_Allocator()
{
    static auto allocator = allocator_t();
    return allocator;
}

template <class T, class Alloc>
template <class Q>
void Queue<T, Alloc>::emplace_tail_(Q&& q) noexcept
{
    size_t it = q.front_;
    while (q.size_ && size_ < capacity_) {
        emplace(std::move_if_noexcept(q.data_[it]));
        q.pop();
        it = (it + 1) & (q.capacity_ - 1);
    }
}

}
