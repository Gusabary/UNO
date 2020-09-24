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