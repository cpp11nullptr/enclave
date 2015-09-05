# Enclave: C++ network library

Enclave is a small C++ network library which is based on `Qt` framework. This library has next advantages:
* multithreading server
* optional encryption mode
* network streams with request/response model
* simple and intuitive API

### Requirements

Qt 5+ (core and network modules) and any modern C++ compiler with support C++11.

### Introduction

Project structure:

| Directory       | Description                              |
|-----------------|------------------------------------------|
| `benchmark/`    | Benchmark for testing library perfomance |
| `enclave/`      | Library                                  |
| `example-chat/` | Simple chat based on this library        |

### Architecture

This library internally use `QTcpServer` for server and `QSslSocket` (in both encryption and unecryption modes)
for server and client connections. Network sockets are lived in separate threads - one thread on client
and one or more threads on server (threads count depends on server thread policy). Interaction with user code
occurs via Qt signal/slot mechanism and special classes which make safe using of internal structures.
For transfer and receive data library provides abstraction of network socket with convenient interface -
network streams.

### How to use

The main two classes are `Enclave::Server` and `Enclave::Client`. Aggregation is prefer but you can also
use inheritance. For interaction with its classes you have to connect to following signals. Notice that
even if you set ignored SSL errors the signal `errorSignal` will be emitted (with flag `ignored` = true).

##### Server

| Signal                           | Description                                 |
|----------------------------------|---------------------------------------------|
| _connectionEstablishedSignal_    | Emit when client connection was established |
| _connectionClosedSignal_         | Emit when client connection was closed      |
| _dataReceivedSignal_             | Emit when network client data was received  |
| _errorSignal_                    | Emit when error happened                    |

##### Client

| Signal                           | Description                                 |
|----------------------------------|---------------------------------------------|
| _connectedSignal_                | Emit when client was connected              |
| _disconnectedSignal_             | Emit when client connection was closed      |
| _dataReceivedSignal_             | Emit when network server data was received  |
| _errorSignal_                    | Emit when error happened                    |

Before starting client or server you can set encryption options and set appropriate thread policy
(for server only).

### Essential classes

Working with this library is provided by next essential classes.

##### NetworkConnection

It's wrapper of server or client connection. You can safely use methods of this class even if connection
has already disappeared. The main method for network interaction is `createStream`. It create new network
stream for specified command which you can use for send network data. Overloaded function have callback as 
parameter which will be executed when request of another network node will be received:
* `node one` create stream with callback
* `node one` put data to stream and send request
* `node two` receive data as stream and create response stream
* `node two` put data to stream and send response
* `node one` callback is executed

##### NetworkStream

This class give abstraction for sending and receiving network data.

### Benchmark

With console benchmark you can comparsion performance of different server thread policies.

### Chat example

This simple chat example illustrate usage of Enclave library. If you want to run it you should set
working directory `enclave/example-chat/workdir`.

Commands for create necessary certificates of this example:
```bash
# private key for root CA certificate
openssl genrsa -out ca.key 4096

# self-signed root CA certificate
openssl req -new -x509 -days 730 -key ca.key -out ca.crt

# private key for server certificate
openssl genrsa -out server.key 4096

# request a server certificate
openssl req -new -key server.key -out server.csr

# server certificate by request
openssl x509 -req -days 730 -in server.csr -CA ca.crt -CAkey ca.key -set_serial 01 -out server.crt

# private key for client certificate
openssl genrsa -out client.key 4096

# request a client certificate
openssl req -new -key client.key -out client.csr

# client certificate by request
openssl x509 -req -days 365 -in client.csr -CA ca.crt -CAkey ca.key -set_serial 02 -out client.crt
```
