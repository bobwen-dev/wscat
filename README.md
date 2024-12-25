# wscat

`wscat` is a command-line tool that acts as a WebSocket client, similar to the `cat` command but for WebSocket connections. It allows you to send data from standard input to a WebSocket server and receive data from the server to standard output.

## Features

- Supports WebSocket (`ws`) and secure WebSocket (`wss`) connections.
- Supports HTTP and HTTPS proxies.
- Supports custom WebSocket protocol names.
- Supports buffer size configuration.
- Supports connection and DNS timeout configuration.
- Supports deflate compression.
- Supports ignoring invalid SSL certificates.

## Requirements

- `libcurl` with WebSocket support.

## Installation

### Debian/Ubuntu

1. Install the required dependencies:

    ```sh
    sudo apt-get update
    sudo apt-get install build-essential libcurl4-openssl-dev
    ```

2. Clone the repository:

    ```sh
    git clone https://github.com/yourusername/wscat.git
    cd wscat
    ```

3. Compile the `wscat` tool:

    ```sh
    gcc -o wscat wscat.c -lcurl
    ```

### macOS

1. Install the required dependencies using Homebrew:

    ```sh
    brew install curl
    ```

2. Clone the repository:

    ```sh
    git clone https://github.com/yourusername/wscat.git
    cd wscat
    ```

3. Compile the `wscat` tool:

    ```sh
    gcc -o wscat wscat.c -lcurl
    ```

## Usage

```sh
./wscat [options] <WebSocket URL>
```

### Options

- `--protocol <protocol name>`: Set the WebSocket protocol name (default: `wscat-protocol`).
- `--buffer-size <buffer size>`: Set the buffer size (default: `1024`).
- `--timeout <timeout>`: Set the connection timeout in seconds (default: `60`).
- `--dns-timeout <dns timeout>`: Set the DNS resolution timeout in seconds (default: `30`).
- `--deflate`: Enable deflate compression.
- `--insecure`: Ignore invalid SSL certificates.

### Examples

1. Connect to a WebSocket server with default settings:

    ```sh
    ./wscat ws://echo.example.site
    ```

2. Connect to a WebSocket server with a custom protocol name and buffer size:

    ```sh
    ./wscat --protocol my-protocol --buffer-size 2048 ws://echo.example.site
    ```

3. Connect to a WebSocket server with a connection timeout and DNS timeout:

    ```sh
    ./wscat --timeout 60 --dns-timeout 30 ws://echo.example.site
    ```

4. Connect to a WebSocket server with deflate compression enabled:

    ```sh
    ./wscat --deflate ws://echo.example.site
    ```

5. Connect to a WebSocket server and ignore invalid SSL certificates:

    ```sh
    ./wscat --insecure wss://echo.example.site
    ```

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

## Acknowledgments

- `libcurl` for providing the WebSocket support.