CXX ?= g++
CXXFLAGS = -Wall -Werror -Wextra

DEBUG ?= 1
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -Ddebug
else
    CXXFLAGS += -O2
endif

server: main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	$(CXX) -lmysqlclient -lpthread $^ $(CXXFLAGS) -o server

CGISQL.cgi:./CGImysql/sign.cpp
	$(CXX) -o ./root/CGISQL.cgi ./CGImysql/sign.cpp -lmysqlclient -lpthread

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
