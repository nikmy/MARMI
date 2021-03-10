#pragma once

#include "resource.h"

// Declarations
namespace mtq
{

template <typename T, typename Alloc>
class Queue;

template <typename T, typename Alloc>
class QueueIterator
    : public std::iterator<std::random_access_iterator_tag, T>
{
    friend class Queue<T, Alloc>;

 public:
    using iterator_category = std::random_access_iterator_tag;
    using size_type = std::size_t;
    using value_type = T;

    QueueIterator(const QueueIterator& src);

    bool operator!=(const QueueIterator& rhs) const;
    bool operator==(const QueueIterator& rhs) const;

    typename QueueIterator::reference operator*() const;

    QueueIterator<T, Alloc>& operator+=(size_type offset);
    QueueIterator<T, Alloc>& operator-=(size_type offset);

    QueueIterator<T, Alloc>& operator++();
    const QueueIterator<T, Alloc> operator++(int);

    QueueIterator<T, Alloc>& operator--();
    const QueueIterator<T, Alloc> operator--(int);

 private:
    value_type* item_;
    value_type* base_;
    size_type size_;

    QueueIterator(value_type* p, value_type* ring, size_type size);
};

template <typename T, typename Alloc = std::allocator<T>>
class Queue : public gen::Resource<T>
{
 public:
    using alloc_traits = std::allocator_traits<Alloc>;
    using iterator = QueueIterator<T, Alloc>;
    using size_type = std::size_t;
    using allocator_type = Alloc;
    using value_type = T;

    Queue();
    ~Queue();

    explicit Queue(size_type init_capacity);
    Queue(std::initializer_list<T> list);

    Queue(const Queue& src);
    Queue(Queue&& src) noexcept;

    Queue& operator=(const Queue& rhs);
    Queue& operator=(Queue&& rhs) noexcept;

    const value_type& front() const noexcept;
    const value_type& back() const noexcept;

    value_type& front() noexcept;
    value_type& back() noexcept;

    bool empty() const noexcept;

    size_type size() const noexcept;

    void pop() noexcept;

    void clear() noexcept;

    value_type take_first() noexcept;

    void push(const value_type& x);
    void push(value_type&& x);

    template <typename...Args>
    void emplace(Args&&...args);

    void move_to(Queue<T>& q);

    void swap(Queue<T>& q) noexcept;

    iterator begin() noexcept;
    iterator end() noexcept;

 private:
    value_type* ring_;

    size_type size_;
    size_type capacity_;

    size_type front_;
    size_type back_;

    static constexpr size_type MIN_CAP = 256;
    static allocator_type& Get_Allocator();

    void resize_(size_type new_capacity) noexcept;

    template <typename Q = Queue<T>>
    void emplace_(Q&& q) noexcept;
};
}

// Definitions
namespace mtq
{

template <typename T, typename Alloc>
QueueIterator<T, Alloc>::QueueIterator(const QueueIterator& src)
    : item_(src.item_), base_(src.base_), size_(src.size_)
{}

template <typename T, typename Alloc>
bool QueueIterator<T, Alloc>::operator!=(const QueueIterator& rhs) const
{
    return item_ != rhs.item_;
}

template <typename T, typename Alloc>
bool QueueIterator<T, Alloc>::operator==(const QueueIterator& rhs) const
{
    return item_ == rhs.item_;
}

template <typename T, typename Alloc>
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

template <typename T, typename Alloc>
QueueIterator<T, Alloc>&
QueueIterator<T, Alloc>::operator+=(QueueIterator::size_type offset)
{
    item_ = ((item_ - base_) + offset) % size_ + base_;
    return *this;
}

template <typename T, typename Alloc>
QueueIterator<T, Alloc>&
QueueIterator<T, Alloc>::operator-=(QueueIterator::size_type offset)
{
    item_ = (item_ >= offset) ? item_ - offset : size_ - offset + item_;
    return *this;
}

template <typename T, typename Alloc>
QueueIterator<T, Alloc>& QueueIterator<T, Alloc>::operator++()
{
    *this += 1;
    return *this;
}

template <typename T, typename Alloc>
const QueueIterator<T, Alloc> QueueIterator<T, Alloc>::operator++(int)
{
    QueueIterator<T, Alloc> copy = *this;
    ++(*this);
    return copy;
}

template <typename T, typename Alloc>
QueueIterator<T, Alloc>& QueueIterator<T, Alloc>::operator--()
{
    item_ = (item_) ? item_ - 1 : size_ - 1;
    return *this;
}

template <typename T, typename Alloc>
const QueueIterator<T, Alloc> QueueIterator<T, Alloc>::operator--(int)
{
    QueueIterator<T, Alloc> copy = *this;
    --(*this);
    return copy;
}

template <typename T, typename Alloc>
QueueIterator<T, Alloc>::QueueIterator(
    value_type* p,
    QueueIterator::value_type* ring,
    const QueueIterator::size_type size
)
{
    item_ = p;
    base_ = ring;
    size_ = size;
}

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue() :
    ring_(nullptr),
    size_(0),
    capacity_(0),
    front_(0),
    back_(0)
{}

template <typename T, typename Alloc>
Queue<T, Alloc>::~Queue()
{ clear(); }

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue(Queue::size_type init_capacity) : Queue()
{
    if (init_capacity) {
        size_type cap = 1;
        while (cap < init_capacity)
            cap <<= 1;
        capacity_ = cap;
        ring_ = alloc_traits::allocate(Get_Allocator(), capacity_);
    }
}

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue(const std::initializer_list<T> list) : Queue()
{
    size_type cap = MIN_CAP;
    while (cap < list.size())
        cap <<= 1;
    resize_(cap);
    back_ = 0;
    for (auto& item : list) {
        alloc_traits::construct(Get_Allocator(), ring_ + back_, item);
        ++back_;
    }
    size_ = back_;
}

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue(const Queue& src)
    : Queue()
{
    if (src.capacity_) {
        ring_ = alloc_traits::allocate(Get_Allocator(), src.capacity_);
        capacity_ = src.capacity_;
        size_ = src.size_;
        back_ = src.back_;
        front_ = src.front_;
    }

    for (size_type i = 0; i < src.size_; ++i)
        alloc_traits::construct(Get_Allocator(), ring_ + i, src.ring_[i]);
}

template <typename T, typename Alloc>
Queue<T, Alloc>::Queue(Queue&& src) noexcept : Queue()
{ this->swap(src); }

template <typename T, typename Alloc>
Queue<T, Alloc>&
Queue<T, Alloc>::operator=(const Queue& rhs)
{
    if (this != &rhs) {
        Queue<T> copy(rhs);
        this->swap(copy);
    }
    return *this;
}

template <typename T, typename Alloc>
Queue<T, Alloc>&
Queue<T, Alloc>::operator=(Queue&& rhs) noexcept
{
    if (this != &rhs) {
        Queue<T> moved(std::move(rhs));
        this->swap(moved);
    }
    return *this;
}

template <typename T, typename Alloc>
const T& Queue<T, Alloc>::front() const noexcept
{ return ring_[front_]; }

template <typename T, typename Alloc>
const T& Queue<T, Alloc>::back() const noexcept
{ return (back_ ? ring_[back_ - 1] : ring_[capacity_ - 1]); }

template <typename T, typename Alloc>
T& Queue<T, Alloc>::front() noexcept
{ return ring_[front_]; }

template <typename T, typename Alloc>
T& Queue<T, Alloc>::back() noexcept
{ return (back_ ? ring_[back_ - 1] : ring_[capacity_ - 1]); }

template <typename T, typename Alloc>
bool Queue<T, Alloc>::empty() const noexcept
{ return (size_ == 0); }

template <typename T, typename Alloc>
typename Queue<T, Alloc>::size_type Queue<T, Alloc>::size() const noexcept
{ return size_; }

template <typename T, typename Alloc>
void Queue<T, Alloc>::pop() noexcept
{
    if (size_) {
        alloc_traits::destroy(Get_Allocator(), ring_ + front_);
        front_ = (front_ + 1) & (capacity_ - 1);

        if (--size_ == 0)
            front_ = back_ = 0;
    }
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::clear() noexcept
{
    if (capacity_ == 0)
        return;

    while (size_)
        pop();

    alloc_traits::deallocate(Get_Allocator(), ring_, capacity_);
    capacity_ = size_ = 0;
    front_ = back_ = 0;
    ring_ = nullptr;
}

template <typename T, typename Alloc>
T Queue<T, Alloc>::take_first() noexcept
{
    value_type value = front();
    pop();
    return value;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::push(const value_type& x)
{ emplace(x); }

template <typename T, typename Alloc>
void Queue<T, Alloc>::push(value_type&& x)
{ emplace(std::move(x)); }

template <typename T, typename Alloc>
template <typename...Args>
void Queue<T, Alloc>::emplace(Args&&...args)
{
    if (size_ == capacity_) {
        size_type new_capacity = capacity_ ? (capacity_ * 2) : MIN_CAP;
        resize_(new_capacity);
    }

    alloc_traits::construct(
        Get_Allocator(), ring_ + back_, std::forward<Args>(args)...
    );
    back_ = capacity_ ? (back_ + 1) & (capacity_ - 1) : 1;
    ++size_;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::move_to(Queue<T>& q)
try
{
    if (&q == this) {
        throw std::invalid_argument("Queue::emplace_: "
                                    "Moving to self detected\n");
    }
    q.emplace_(std::move(*this));
}
catch (std::invalid_argument& e) {
    e.what();
    throw;
}

template <typename T, typename Alloc>
void Queue<T, Alloc>::swap(Queue<T>& q) noexcept
{
    if (this != &q) {
        std::swap(ring_, q.ring_);
        std::swap(size_, q.size_);
        std::swap(capacity_, q.capacity_);
        std::swap(front_, q.front_);
        std::swap(back_, q.back_);
    }
}

template <typename T, typename Alloc>
typename Queue<T, Alloc>::iterator
Queue<T, Alloc>::begin() noexcept
{
    return QueueIterator<value_type, allocator_type>(
        ring_ + front_, ring_, capacity_);
}

template <typename T, typename Alloc>
typename Queue<T, Alloc>::iterator
Queue<T, Alloc>::end() noexcept
{
    return QueueIterator<value_type, allocator_type>(
        ring_ + back_, ring_, capacity_);
}

template <typename T, typename Alloc>
Alloc& Queue<T, Alloc>::Get_Allocator()
{
    static auto allocator = allocator_type();
    return allocator;
}

template <typename T, typename Alloc>
void
Queue<T, Alloc>::resize_(Queue::size_type new_capacity) noexcept
{
    capacity_ = new_capacity;
    T* new_ring = alloc_traits::allocate(Get_Allocator(), capacity_);
    if (capacity_ > MIN_CAP) {
        for (size_type it = 0; it < size_; ++it) {
            alloc_traits::construct(
                Get_Allocator(),
                new_ring + it,
                std::move_if_noexcept(ring_[front_])
            );
            front_ = (front_ + 1) % (capacity_ / 2);
        }
    }
    if (ring_) {
        size_type prev_sz = size_;
        size_type prev_cp = capacity_;
        clear();
        size_ = prev_sz;
        capacity_ = prev_cp;
    }
    alloc_traits::deallocate(Get_Allocator(), ring_, capacity_);
    ring_ = new_ring;
    front_ = 0;
    back_ = size_;
}

template <typename T, typename Alloc>
template <typename Q>
void Queue<T, Alloc>::emplace_(Q&& q) noexcept
{
    for (size_type i = q.front_; i != q.back_; i = ((i + 1) % q.capacity_))
        emplace(std::move_if_noexcept(q.ring_[i]));
    q.clear();
}

}