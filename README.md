# TinyServer

**TinyServer** is a lightweight, customizable HTTP server built with simplicity and flexibility in mind. This project aims to provide the fundamental building blocks of an HTTP server, giving you the ability to serve static content and handle basic HTTP requests.

## Features

- **Lightweight**: A simple, minimal HTTP server that can be easily customized.
- **Static Content**: Serve HTML, CSS, JavaScript, and other static files.
- **Basic HTTP Methods**: Supports GET requests for serving files.
- **Configurable**: Easily tweak settings to customize the server's behavior.
- **Cross-Platform**: Built with cross-platform compatibility in mind (Linux, macOS, Windows).

## Installation

To get started with **TinyServer**, clone this repository to your local machine:

```bash
git clone https://github.com/InventedSarawak/TinyServer.git
cd TinyServer
```

Then, compile the project:

```bash
make
```

Or, if you’re using a different environment:

```bash
gcc -o TinyServer main.c
```

## Usage

Run the server on your local machine by specifying the port number:

```bash
./TinyServer 8080
```

By default, it will serve files from the current directory. You can customize this by editing the configuration in the code or passing a different directory as an argument.

To test the server, open your browser or use `curl`:

```bash
curl http://localhost:8080
```

## How It Works

1. **Socket Programming**: TinyServer uses sockets to accept incoming HTTP requests from clients.
2. **Request Parsing**: The server reads the request, extracts the necessary details (like method and resource path), and processes the request.
3. **Response Generation**: Depending on the request, TinyServer generates an appropriate HTTP response and sends it back to the client.
4. **Serving Static Content**: Files requested by the client (e.g., HTML, CSS, images) are read from the server's file system and returned in the response body.

## Contributing

Contributions are welcome! If you'd like to improve TinyServer, feel free to fork the repository and submit a pull request. Please ensure that your changes are well-documented and include tests if possible.

## License

This project is open-source and available under the [MIT License](LICENSE).

## Contact

- **Author**: [Vedant Kesarwani](https://github.com/inventedsarawak)
- **Email**: [relativityeismc2@gmail.com](mailto:relativityeismc2@gmail.com)
