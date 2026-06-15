NAME = codexion
CURRENT_FILE = codexion.c

PTHREAD = -pthread
CFLAGS = cc -Wall -Wextra -Werror -fsanitize=thread -g
RM = rm -f
AR = ar rcs
INCLUDES = -I./includes

SRCS_DIR = ./srcs

SRCS =	$(SRCS_DIR)/heaps.c \
		$(SRCS_DIR)/ft_atoil.c \
		$(SRCS_DIR)/routines.c \
		$(SRCS_DIR)/verifier.c \
		$(SRCS_DIR)/get_time_ms.c \
		$(SRCS_DIR)/initializing.c \
		$(SRCS_DIR)/heap_helpers.c \
		$(SRCS_DIR)/$(CURRENT_FILE) \
		$(SRCS_DIR)/lenght_helpers.c \
		$(SRCS_DIR)/general_helpers.c \
		$(SRCS_DIR)/routine_helpers.c \

OBJS = $(SRCS:.c=.o)

all: $(NAME)

$(NAME): $(OBJS)
	$(CFLAGS) $(PTHREAD) $(INCLUDES) $(OBJS) -o $(NAME)

%.o: %.c
	$(CFLAGS) $(PTHREAD) $(INCLUDES) -c $< -o $@

clean:
	$(RM) $(OBJS)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re

.SILENT:

