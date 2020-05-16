server: main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	g++ main.cpp ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp -lpthread -lmysqlclient -g -o server

CGISQL.cgi:./CGImysql/sign.cpp ./lock/locker.h
	g++ -o ./root/CGISQL.cgi ./CGImysql/sign.cpp ./lock/locker.h -lmysqlclient -lpthread

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
