server:
	g++ ./server.cpp ./src/ThreadPool.cpp ./src/Buffer.cpp ./src/Connection.cpp ./src/Acceptor.cpp ./src/Channel.cpp ./src/Epoll.cpp ./src/EventLoop.cpp ./src/InetAddress.cpp ./src/Server.cpp ./src/Socket.cpp ./src/util.cpp  -o server && \
	g++ ./client.cpp ./src/Buffer.cpp ./src/InetAddress.cpp  ./src/Socket.cpp ./src/util.cpp  -o client 

clean:
	rm server && rm client