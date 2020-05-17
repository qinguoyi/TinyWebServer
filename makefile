CXX ?= g++

DEBUG ?= 1
ifeq (DEBUG, 1)
    CFLAGS = -g
else
    CFLAGS = -O2
endif

server: main.cpp  ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	$(CXX) -o server  $^ $(CXXFLAGS) -lpthread -lmysqlclient

CGISQL.cgi:./CGImysql/sign.cpp
	g++ -o ./root/CGISQL.cgi $^ -lpthread -lmysqlclient 

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
