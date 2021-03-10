## Multiple Access Resource Management Interface

### ```namespace gen```

The main idea of this project is to encapsulate mechanisms for filling and
multi-threaded processing data from the one resource inside one class.
This class is ```ResourceManager<T>```, where ```T``` is type of data
inside container (queue, for example). Use this in 5 steps:  
  1. Inherit your ```Container<T>``` class from ```Resource<T>```.
  2. Define the following functions (using lambda expressions or closures):
     - ```void supply_data()```  
     - ```T access_data()```  
     - ```void handle_data(T)```
     - ```bool is_full()```  
     - ```bool is_empty()```  
  3. Create a ```Container<T>``` object  
  4. Create a ```ResourceManager<T>``` object
  5. Call ```manager.launch()```  

If you want to terminate processing, use ```manager.terminate()```  
If you want more control / features, the class is open to extensions.