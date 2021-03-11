## Multiple Access Resource Management Interface

### ```namespace gen```

```c++
template <class T>
class ResourceManager
```

```c++
ResourceManager
        (
            resource_t                  & resource,
            const func_t<void()>        & supply_f,
            const func_t<data_t(void)>  & access_f,
            const func_t<void(data_t)>  & handle_f,
            const func_t<bool()>        & is_full,
            const func_t<bool()>        & is_empty,
            size_t                      n_of_threads_limit
        );

void launch();
void terminate();
```

The main idea of this project is to encapsulate mechanisms for filling and
multithreading processing data from the one resource inside one class.
This class is ```ResourceManager<T>```, where ```T``` is the type of data
inside container (queue, for example). Use this in 5 steps:  
  1. Inherit your ```Container<T>``` class from ```Resource<T>```.
  2. Define the following functions (using lambda expressions or closures):  
     ```c++
     /// data_t is your data type (T)
     
     void   supply_data (void)  ; 
     data_t access_data (void)  ;
     void   handle_data (data_t);
     bool   is_full     (void)  ;
     bool   is_empty    (void)  ;
     ```  
  3. Create a ```Container<T>``` object  
  4. Create a ```ResourceManager<T>``` object
  5. Call ```manager.launch()```  

If you want to terminate processing, use ```manager.terminate()```  
If you want more control / features, the class is open to extensions.

**P.S. Function ```handle_data``` should be implemented considering that it will
be called by multiple threads, and it must protect the used resources.** 

### ```namespace mtq```

```c++
template <
     class T,
     class Alloc = std::allocator<T>
> class Queue : public gen::Resource<T>
```

Template multithreading queue class.

```c++
/// If init_capacity is not a power of two,  
/// capacity will be set to upperbound(init_capacity)
explicit Queue(size_t init_capacity);
Queue(std::initializer_list<T> list);

Queue(const Queue& src);
Queue(Queue&& src) noexcept;

Queue& operator=(const Queue& rhs);
Queue& operator=(Queue&& rhs) noexcept;

void push(const value_t& x);
void push(value_t&& x);

template <class...Args>
void emplace(Args&&...args);

/// Returns popped element
value_t take_first() noexcept;

/// Moves *this to the back of other
void move_to(Queue<T>& other);

/// Swaps *this and other
void swap(Queue<T>& other) noexcept;
```

