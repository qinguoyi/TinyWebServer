server: main.cpp  ./timer/lst_timer.cpp ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp  webserver.cpp config.cpp
	g++ -o server main.cpp ./timer/lst_timer.cpp  ./http/http_conn.cpp ./log/log.cpp ./CGImysql/sql_connection_pool.cpp webserver.cpp config.cpp -lpthread -lmysqlclient

CGISQL.cgi:./CGImysql/sign.cpp
	g++ -o ./root/CGISQL.cgi ./CGImysql/sign.cpp -lmysqlclient -lpthread

clean:
	rm  -r server
	rm  -r ./root/CGISQL.cgi
