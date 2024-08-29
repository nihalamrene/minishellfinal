NAME		=	minishell
CC			=	cc

LDFLAGS  = "-L/Users/$(USER)/.brew/opt/readline/lib"
CPPFLAGS = "-I/Users/$(USER)/.brew/opt/readline/include"

CFLAGS		=	 -Werror -Wextra -Wall -lreadline -fsanitize=address -g3
RM			=	rm -rf
SRCS		=	main.c signals.c src/lexer/tokenizer_line.c src/tools/lst_addback_m.c \
				src/tools/lst_env.c src/envir/environment.c src/expand/expansion.c \
				src/expand/expansion2.c src/expand/expansion3.c  \
				src/lexer/checks.c src/parse/parsing.c src/parse/parsing2.c src/tools/lst_redir.c \
				src/tools/lst_cmd.c \
				src/exec/exe_cmd.c src/exec/exec.c src/exec/handel_redi.c src/exec/build_in_exec.c src/exec/handel_heredoc.c src/exec/expand_helper.c \
				src/build_in/echo.c src/build_in/unset.c src/build_in/env.c src/build_in/export.c src/build_in/cd.c src/build_in/pwd.c src/build_in/exit.c

OBJS		= ${SRCS:.c=.o}

all			:	$(NAME)

$(NAME)		: $(SRCS)
			@printf "\rMinishell loading ...⏳\n"
			@make -C libft
			@$(CC) $(CFLAGS) $(SRCS) libft/libft.a -o $(NAME) -lreadline $(CPPFLAGS) $(LDFLAGS)
			@printf "\rMinishell Ready \n" 
			
clean		:
			@printf "$(RED)\rCleaning ... 🗑️\n"
			@$(RM) $(OBJS)
			@make clean -C libft

fclean		: clean
			@$(RM) $(NAME)
			@make fclean -C libft

re			: fclean all

.PHONY: all clean fclean re