# Multicast delegate for C++

C# has the nice concept of a *multicast delegate* which is the generalisation of a function pointer for multiple targets.

In other words, it’s kind of like an observer with nice syntax. (And I’m intentionally leaving out the details here.)

## Usage

The usage is pretty straight-forward and echoes C#’s as much as possible.

```c++
void f(int n) { cout << "f: " << n << "\n"; }

util::multifunction<void(int)> event;

auto cookie = event += f;
event += [](int n) { cout << "[]: " << n << "\n"; };

event(42);
// Output:
// f: 42
// []: 42

event -= cookie;

event(23);
// Output:
// []: 23
```

A multi-function behaves essentially like a function – it’s callable, and defines the appropriate typedefs such as `result_type`.
It also supports adding and removing listeners, via the functions `operator+=` and `operator-=`.

Notice that in C#, you’d use the listener delegate itself as an argument to `-=`. Unfortunately, that’s not possible in C++ because function-like objects are not generally identifiable, so `std::function` doesn’t define equality comparison. We wouldn’t know which object to remove.

Although this could be worked around for many (but not all) cases, multi-functions employ a slightly different approach:

Adding a listener to a multi-function returns a “cookie” of an opaque type identifying the listener that has been added (hat-tip to [Abyx](http://chat.stackoverflow.com/transcript/message/3715809#3715809) for coming up with the idea).

Removing a cookie more than once does nothing.
*Adding* a function more than once, on the other hand, causes the function to be invoked several times. (Does this make sense?)

`util::multifunction` supports all function-like objects that are supported by `std::function`.
It defines all the same member types and member functions, with the exception of `assign` and `operator bool` which don’t make sense in `util::multifunction`.

Calling a `util::multifunction` with a non-`void` return type returns the value of the last call only. The order in which the listeners are called is undefined, though, so the result might not always be usable. 
