vpath = include:src
vpath %.cpp src
vpath %.h include

cmd = g++
opt = -c -o

main: RecordManager.o
	$(cmd) -o main $^ 
%.o : %.cpp
	$(cmd) $(opt) $@ $<
clean:
	rm main
	rm -f *.o
