## Multiple Access Resource Management Interface

### ```namespace gen```

```c++
template <
        class T,
        class Alloc = std::allocator<T>
> class Queue
```

Thread-safety queue class template.

```c++
/// If init_capacity is not a power of two,  
/// capacity will be set to upperbound(init_capacity)
explicit Queue(size_t init_capacity);

template <class...Args>
void emplace(Args&&...args);

/// Returns popped element
value_t take_first() noexcept;

/// Moves *this to the back of other
/// Not thread-safety method!
void move_to(Queue<T>& other);
```

```c++
template <data_t>
class Resource
{
public:
    virtual ~Resource() = default;
    
    virtual data_t get_data() = 0;
    virtual bool is_empty() = 0;
};
```

Resource interface. Your resource class must implement this.

```c++
template <data_t>
class DataHandler
{
public:
    virtual void process(data_t&& data) = 0;
    virtual ~DataHandler() = default;
};
```

Handler interface. Your data-handling class must implement this.

```c++
template <data_t>
class ResourceManager
```
Resource manager interface - main class of this project.
```c++
ResourceManager
(
    resource_t & resource,      // reference to your resource
    handler_t  & handler,       // reference to your handler
    size_t     max_queue_size,  // maximum allowed data queue size
    size_t     n_of_threads     // number of threads limit
);

// creates 1 thread for pushing data into queue and
// (n_of_threads - 1) for handling data from queue 
void start();

// terminate all working threads
// after completing their current jobs
void stop();

// move waiting queue to the backup if
// manager is not running, otherwise
// throw IllegalAccess exception and
// call stop()
void save_session_data(Queue<T>& backup);

// move backup to the waiting queue if
// manager is not running and size of
// backup is less or equal to the capacity
// of waiting queue. In the case when there
// are running processes, throw IllegalAccess
// exception and call stop(); if size of backup
// is greater than capacity of the waiting queue,
// throw UnsavedDataLeak without stopping and moving
void restore_session_data(Queue<T>& backup);
```

