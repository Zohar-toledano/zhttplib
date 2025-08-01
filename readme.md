
# ZServer

**ZServer** is a lightweight, proof-of-concept HTTP/1.1 server written in C++ using the Windows API. It demonstrates core web server concepts including:

- Process pooling and thread pooling for concurrent request handling
- Shared memory and pipes for inter-process communication
- Custom view routing (handler functions mapped to URL paths)

---

## 🚀 Features

- ✅ HTTP/1.1 support (basic request/response)
- ⚙️ Process pool + thread pool for scalable performance
- 🔄 Shared memory and pipe-based IPC
- 🛠️ Easily add custom views (handlers) in `main.cpp`
- 🧪 Built as a research or educational project

---

## 📦 Getting Started

### 1. Clone the Repository

```sh
git clone https://github.com/yourusername/zserver.git
cd zserver
````

### 2. Add Your Views

Edit `main.cpp` and use the routing API:

```cpp
s.at("/say_hello", [](HTTPRequest* req, HTTPResponse* res) {
    res->body = "HI!!!";
});
```

You can add any number of custom route handlers like the one above.

---

### 3. Build

Make sure you have CMake and Visual Studio (or any compatible compiler) installed.

```sh
mkdir build
cd build
cmake ..
cmake --build .
```

The compiled binary will be placed in the build output directory.

---

## 📁 Project Structure (Simplified)

```
zserver/
├── include/           # Header files (HTTP, server, routing, etc.)
├── src/               # Source files for server internals
├── main.cpp           # Entry point, define routes here
├── CMakeLists.txt     # Build script
└── README.md
```

---

## 🤓 Notes

* Designed for **Windows** (uses Windows-specific APIs for sockets, processes, and synchronization).
* This is not intended for production use. It’s a learning tool demonstrating how a basic HTTP server can be implemented from scratch using OS-level constructs.

---

## 📬 Example Response

```http
GET /say_hello HTTP/1.1
Host: localhost:8080

HTTP/1.1 200 OK
Content-Length: 6

HI!!!
```

---
## 🤝 Contributing

Contributions are welcome! If you want to fix bugs, improve the architecture, or add new features, feel free to open a pull request or issue.

