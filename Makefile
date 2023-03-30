NAME := ircserv

SRC := $(wildcard *.cpp) $(wildcard */*.cpp)
HEADS := $(wildcard *.hpp)

CPPFLAGS := -std=c++98 -Wall -Wextra # -ILeakSanitizer/include -LLeakSanitizer -llsan -lc++ #-Werror

all : $(NAME)

$(NAME) : $(SRC) $(HEADS) Makefile
	c++ $(CPPFLAGS) $(SRC) -o $(NAME)

run :
	$(MAKE) all
	./$(NAME) 42 password

clean :
	rm -f *.o

fclean : clean
	rm -f $(NAME)

add :
	git add $(SRC) $(HEADS) Makefile

test :
	$(MAKE) all
	./$(NAME)