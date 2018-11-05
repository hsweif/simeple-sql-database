vpath = include:src
vpath %.cpp src
vpath %.h include

cmd = g++
opt = -c -o

main: main.o RM_PageHead.o RM_Manager.o RM_data.o RM_FileHandle.o \
	RM_Record.o RM_data.o RID.o
	$(cmd) -o main $^ 
%.o : %.cpp
	$(cmd) $(opt) $@ $<
clean:
	rm -f *.o
	rm main
