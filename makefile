vpath = include:src
vpath %.cpp src
vpath %.h include

cmd = g++
opt = -c -o

main: PageHead.o RM_FileHandle.o RM_Manager.o
	$(cmd) -o main $^ 
%.o : %.cpp
	$(cmd) $(opt) $@ $<
clean:
	rm -f *.o
	rm main
