

FLAGS = -I ../jpeg/arm-jpeg/include -L ../jpeg/arm-jpeg/lib -ljpeg -lpthread -std=gnu99 -I/usr/local/arm_sql/include/ -L/usr/local/arm_sql/lib/ -lsqlite3

cap:main.c lcd.c v4l2.c convert.c myjpeg.c click.c MySqlite.c
	arm-linux-gnueabihf-gcc $^ -o $@ $(FLAGS)

clean:
	rm cap -rf

install:
	cp cap ~/tftpboot
