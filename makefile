# Grupo SO-018 integrado por:
#      Pedro Correia fc59791
#      Renan Silva fc59802
#      Sofia Reis fc59880

BIN_dir = bin
INC_dir = include
OBJ_dir = obj
SRC_dir = src

OBJETOS = main.o doctor.o memory.o patient.o process.o receptionist.o synchronization.o configuration.o log.o hosptime.o hospsignal.o stats.o
main.o = main.h
doctor.o = doctor.h
memory.o = memory.h
patient.o = patient.h
process.o = process.h
receptionist.o = receptionist.h

synchronization.o = synchronization.h
configuration.o = configuration.h
log.o = log.h
hosptime.o = hosptime.h
hospsignal.o = hospsignal.h
stats.o = stats.h

CC = gcc
CFLAGS = -Wall -I $(INC_dir)

vpath %.o $(OBJ_dir)

$(BIN_dir)/hOSpital: $(OBJETOS) | $(BIN_dir)
	$(CC) $(addprefix $(OBJ_dir)/,$(OBJETOS)) -o $@

$(BIN_dir):
	mkdir -p $(BIN_dir)

%.o: $(SRC_dir)/%.c $($@)
	$(CC) $(CFLAGS) -o $(OBJ_dir)/$@ -c $<

clean: 
	rm -f $(OBJ_dir)/*.o
	rm -f $(BIN_dir)/hOSpital
	@echo "A remover os ficheiros objeto e o ficheiro executavel"
