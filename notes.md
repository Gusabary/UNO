# Notes

+ after [upgrading cmake version](https://blog.csdn.net/fancyler/article/details/78009812), it may need to flush some cache under project root directory with the command below:

    ```bash
    hash -r
    ```

+ when passing a member function as a callable argument, remember that the first parameter of `std::function` should be the class containing that member function and the callable argument should be the *address* of the member function:

    ```c++
    explicit Session(tcp::socket socket, std::function<void(Server &, const std::string &)> deliverCallback);

    Session s(std::move(socket), &Server::Deliver);
    ```

    What's more, if the member function is declared as `const`, the first parameter of `std::function` should also be `const`.

+ when using `async_read` instead of `async_read_some`, it will block if the current read stream cannot fill the `asio::buffer`. that is to say, if the length of the `asio::buffer` is too large, the `async_read` may block until an EOF.

+ in the implementation of asio's `async_` functions, it seems that you cannot capture a char* in the lambda of callback. instead, a `self` got from `shared_from_this()` should be captured.

+ `bad_weak_ptr` could happen if using `shared_from_this()` on an object which has no `shared_ptr` pointing to it yet.

+ `std::*_pointer_cast` could only be applied to `shared_ptr` (because copy operation of `unique_ptr` is deleted). if you must cast the type of a `unique_ptr`, use below:

    ```c++
    dst.reset(dynamic_cast<A *>(src.release()));
    ```
    note that `release()` should be used here instead of `get()` because you need to release the ownership of managed object by `src`, otherwise `dst` would gain nothing

    sounds like **move semantic**, aha?

+ when copying data to vector (or something like this), you need to pay more attention:

    ```c++
    std::vector<int> v;    
    int a[] = {1, 2, 3};
    std::copy(a, a + 3, std::back_inserter(v));
    ```

    `std::back_inserter()` returns a `std::back_insert_iterator`, which is an output iterator pointing to the **end** of the container and `push_back()` method of the container will be called whenever the iterator is assigned to.

    But if you must copy to the begin of the container, it's better to resize the container in advance to ensure that the size of container is larger than the size of data copied:

    ```c++
    std::vector<int> v;    
    int a[] = {1, 2, 3};
    v.resize(3);
    std::copy(a, a + 3, v.begin());
    ```

+ note that `size()` of `std::vector` (maybe most stl containers are also applied) return a **unsigned** value:

    ```c++
    std::vector<int> v;
    std::cout << v.size() - 1 << std::endl;  // 18446744073709551615
    ```

+ `constexpr` does not always have an address. for example, the following code will complain `undefined reference to 'Card::NonWildColors'`. that is because `Card::NonWildColors` doesn't have an address, which cannot be traversed in a range-based loop

    ```c++
    struct Card {
        constexpr static std::initializer_list<CardColor> NonWildColors = {/*data*/};
    };

    for (auto color : Card::NonWildColors) {
        // do something
    }
    ```

    the right way is to declare as `const static` and define somewhere else:

    ```c++
    struct Card {
        const static std::initializer_list<CardColor> NonWildColors;
    };

    const std::initializer_list<CardText> Card::NonWildTexts = {/*data*/};
    ```

    *[reference](https://stackoverflow.com/questions/8452952/c-linker-error-with-class-static-constexpr)*

+ when passing a method of a class as callable, usually there are two ways:

    + using `std::bind`:

        ```c++
        class A {
            void f() {}
            void g() {
                some_func(std::bind(&A::f, this));
            }
        };
        ```
    
    + using lambda:

        ```c++
        class A {
            void f() {}
            void g() {
                some_func([this]() { return f(); }]);
            }
        };
        ```

+ when using asio, invocation to `io_context::run()` will block if there is still some async work to complete. *[reference](https://think-async.com/Asio/asio-1.18.0/doc/asio/tutorial/tuttimer2.html)*

+ to install MinGw on Windows, take [this](https://www.tutorialspoint.com/How-to-Install-Cplusplus-Compiler-on-Windows) for a reference. after installation, some necessary binarys should already have been added to *PATH*, run `g++` to have a check. and then, we can use it like `cmake -G "MinGW Makefiles" ..`

+ using Python to run an executable and provide input interactively, take [this](https://stackoverflow.com/questions/32570029/input-to-c-executable-python-subprocess) for a reference.

+ use `firewall-cmd` commands to query and manipulate configuration about firewall to expose service. *[reference](https://www.tecmint.com/fix-no-route-to-host-ssh-error-in-linux/)*