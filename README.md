# echo_chat
Echo Chat create by C language

## multiprocess echo server
내부 흐름도
![multiprocess echo server](./multiprocess_echo_server.png)
동작중 스크린샷
![multiprocess echo server photo](./multiprocess_echo_server_photo.jpg)
### how to use
#### server
```
./server <port>
```
#### client
```
./client <ip> <port>
```
### compile
#### echo_server
```
cd multiprocess_server
cc echo_server.c -o ./bin/echo_server
```
#### echo_client
```
cd multiprocess_server
cc echo_client.c -o ./bin/echo_client
```