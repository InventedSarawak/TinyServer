# 🎉 TinyServer: A Simple Lightweight Message Server 🚀

Welcome to **TinyServer**, a straightforward and lightweight messaging server that allows multiple clients to communicate with each other, just like in a group chat! 💬✨

## 📜 Features
- **Multi-client Support**: Handle multiple clients simultaneously using threads. 👥
- **Instant Messaging**: Forward messages to all connected clients instantly! ⚡
- **Exit Commands**: Cleanly disconnect clients with specific commands like `exit`, `quit`, `bye`, etc. 👋

## 🛠️ Getting Started

### Prerequisites
- A C compiler (like `gcc`)
- Basic knowledge of C programming and networking concepts 🌐

### Installation
1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/TinyServer.git
   cd TinyServer
   ```

2. Compile the server and client:
   ```bash
   make
   ```

### Usage
1. **Start the Server**: Run the server and specify a port number:
   ```bash
   make run_server ARGS="<port_number>"
   ```
   Example:
   ```bash
   make run_server ARGS="9999"
   ```

2. **Connect the Client**: In another terminal, run the client:
   ```bash
   make run_client ARGS="<hostname> <port_number>"
   ```
   Example:
   ```bash
   make run_client ARGS="loopback 9999"
   ```

3. **Start Chatting**: Send messages between clients! 🎉

## 🧑‍💻 Code Overview
The project consists of two main files:
- `/messaging/tcp_server.c`: The main server code that handles client connections and message forwarding.
- `/messaging/tcp_client.c`: The client code that sends and receives messages.

## 📚 How It Works
1. The server listens for incoming connections on a specified port. 📡
2. When a client connects, it creates a new thread to handle that client. 🔄
3. Messages received from any client are instantly forwarded to all other connected clients. 🔊

## 🤝 Contributing
Contributions are welcome! If you have suggestions or improvements, feel free to open an issue or submit a pull request. 💡

## 📝 License
This project is licensed under the MIT License.
