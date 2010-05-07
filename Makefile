all: rambo camper

rambo: roscoche/rambo.cpp
	g++ roscoche/rambo.cpp -o rambo

camper: exemplos/c++_example/camper.cpp
	g++ exemplos/c++_example/camper.cpp -o camper

clean:
	rm rambo camper
