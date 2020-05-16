CXX ?= g++
CXXFLAGS = -Wall -Werror -Wextra

DEBUG ?= 1
ifeq (DEBUG, 1)
    CFLAGS += -g -Ddebug
else
    CFLAGS += -O2
endif

server: main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	$(CXX) $^ -lpthread -lmysqlclient -g -o server

CGISQL.cgi:./CGImysql/sign.cpp
	$(CXX) -o ./root/CGISQL.cgi ./CGImysql/sign.cpp -lmysqlclient -lpthread

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
