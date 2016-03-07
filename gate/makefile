# This is a general makefile template.
													
                               BIN_DIR = bin/
                               TARGET = $(BIN_DIR)/gate
                               C_CFLAGS = -Wall -W -Wpointer-arith -pipe -fPIC
                              
                               MACROS = -D_REENTRANT -D__USE_POSIX -DUSE_SNAP -DDESC=\"${DESC}\" -DOS_LINUX -D__linux__ -DGAMESERVER
                       
                                #-DSERVER_DEPLOY_FULICHENG
                                    CC = gcc
                            CPP_CFLAGS = -Wall -W -Wpointer-arith -pipe -fPIC -Wno-ignored-qualifiers
                                  MAKE = make
                                LINKER = g++
                          INCLUDE_DIRS = -I../../libJMEngine/third_party -I../../libJMEngine/third_party/google -I../../libJMEngine/third_party/mongo \
                                         -I../../libJMEngine/JMEngine/include -I../../libJMEngine/JMEngine/include/json -I../../libJMEngine/JMEGame/include \
                                         -I../public -I../protocol \
                                         -I/usr/include -I/usr/include/mysql -I/boost_1_49_0 -I/usr/include/lua5.1 -I/usr/include/luabind \
					 -I./ -I./source 
                            
                            LIBS = -lJMEGame -lJMEngine -lboost_thread -lboost_system -lmongoclient -lboost_filesystem -lboost_date_time -lboost_signals -llua \
				   -lboost_program_options -lluabind -lprotobuf -ldl -lpthread -lrt -lmysqlclient_r -lz
                            
                            OPTIM_FLAG = #-O2
                                   CPP = g++ -std=c++11
                                LFLAGS = -W -fPIC
                              LIB_DIRS = -L/usr/local/lib/ -L/usr/lib -L../../libJMEngine/JMEGame -L../../libJMEngine/JMEngine 

                                 VPATH = ./source ../public ../protocol 
						  
                            OBJECT_DIR = ./.obj/
                              CPPFILES = \
					 main.cpp \
					 gate_server.cpp \
					 gate_handler.cpp 

 	 
                                CFILES = 
				
				CCFILES = \
					 server_rpc.pb.cc  
# To use 'make debug=0' build release edition.
ifdef debug
	ifeq ("$(origin debug)", "command line")
		IS_DEBUG = $(debug)
	endif
else
	IS_DEBUG = 1
endif
ifndef IS_DEBUG
	IS_DEBUG = 1
endif
ifeq ($(IS_DEBUG), 1)
	OPTIM_FLAG += -g3
else
	MACROS += -DNDEBUG
endif

# To use 'make publish=0' build release edition.
ifdef publish 
	ifeq ("$(origin publish)", "command line")
		IS_PUB = $(publish)
	endif
else
	IS_PUB = 0
endif
ifeq ($(IS_PUB), 1)
	MACROS += -DPUBLISH
endif

# To use 'make quiet=1' all the build command will be hidden.
# To use 'make quiet=0' all the build command will be displayed.
ifdef quiet
	ifeq ("$(origin quiet)", "command line")
		QUIET = $(quiet)
	endif
endif
ifeq ($(QUIET), 1)
	Q = @
else
	Q =
endif

OBJECTS := $(addprefix $(OBJECT_DIR), $(notdir $(CPPFILES:%.cpp=%.o)))
OBJECTS += $(addprefix $(OBJECT_DIR), $(notdir $(CFILES:%.c=%.o)))
OBJECTS += $(addprefix $(OBJECT_DIR), $(notdir $(CCFILES:%.cc=%.o)))

CALL_CFLAGS := $(C_CFLAGS) $(INCLUDE_DIRS) $(MACROS) $(OPTIM_FLAG)
CPPALL_CFLAGS := $(CPP_CFLAGS) $(INCLUDE_DIRS) $(MACROS) $(OPTIM_FLAG)
LFLAGS += $(OPTIM_FLAG) $(LIB_DIRS) $(LIBS)

all: checkdir $(TARGET)

$(TARGET): $(OBJECTS)
	$(Q)$(LINKER) -o $@ $(OBJECTS) $(strip $(LFLAGS))

$(OBJECT_DIR)%.o:%.cpp
	$(Q)$(CPP) $(strip $(CPPALL_CFLAGS)) -c $< -o $@

$(OBJECT_DIR)%.o:%.c
	$(Q)$(CC) $(strip $(CALL_CFLAGS)) -c $< -o $@

$(OBJECT_DIR)%.o:%.cc
	$(Q)$(CPP) $(strip $(CPPALL_CFLAGS)) -c $< -o $@

checkdir:
	@if ! [ -d "$(BIN_DIR)" ]; then \
		mkdir $(BIN_DIR) ; \
		fi
	@if ! [ -d "$(OBJECT_DIR)" ]; then \
		mkdir $(OBJECT_DIR); \
		fi

rebuild: clean all
clean:
	$(Q)rm -f $(OBJECTS)
cleanall: clean
	$(Q)rm -f $(TARGET)

.PHONY: all clean cleanall checkdir
