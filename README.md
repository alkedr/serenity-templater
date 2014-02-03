serenity-templater
==================

serenity-templater -- templating engine for C++11.

### Architecture

serenity-templater's preprocesor (htmltpp) translates one or more template files into c++11 code. This code is then included and compiled. Templates can use most of C++ and reference variables from C++ code.


### Simplest example

##### example.html
```html
<!DOCTYPE html>
<html>
  <body>
    <h1>Hello, user</h1>
  </body>
</html>
```

##### Command to generate example.hpp from example.html
```bash
htmltpp example.hpp example.html
```

##### example.cpp
```c++
#include "example.hpp"

int main() {
  std::cout << TEMPLATE(example) << std::endl;
  return 0;
}
```


### Using C++ from template

##### example.html
```html
<!DOCTYPE html>
<html>
  <body>
  <h1>Hello, $userName</h1>
  <h2>List of ints:</h2>
    <ul>
      $(foreach x : ints)
        <li>$x</li>
      $end
    </ul>
</body>
</html>
```

##### example.cpp
```c++
#include "example.hpp"

int main() {
  std::string userName = "[your name here]";
  std::vector<int> ints = { 1, 2, 4 };
  std::cout << TEMPLATE(example) << std::endl;
  return 0;
}
```
