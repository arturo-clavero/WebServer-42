# # Compiler and flags
# NAME     = webserv
# CXX 	 = c++
# CFLAGS   = -Wall -Wextra -Werror -std=c++98 -I./include -g
# RM       = rm -rf
# OUTPUT   = ./$(NAME)

# # Directories
# SRC_DIR = src
# OBJ_DIR = obj
# INC_DIR = src

# # Source files
# SRCS = $(SRC_DIR)/main.cpp \
#        $(SRC_DIR)/config/ConfigParser.cpp \
#        $(SRC_DIR)/config/ServerConfig.cpp \
# 	   $(SRC_DIR)/request_parser/HttpRequest.cpp \
# 	   $(SRC_DIR)/request_parser/RequestParser.cpp \
# 	   $(SRC_DIR)/response_builder/ResponseBuilder.cpp \
# 	   $(SRC_DIR)/response_builder/RequestResponse.cpp \
# 	   $(SRC_DIR)/response_builder/ResponseUtils.cpp \
# 	   $(SRC_DIR)/server/Server.cpp \
# 	   $(SRC_DIR)/server/ServerSocket.cpp

# # File directions
# SRCS = $(foreach dir, $(SRC_DIR), $(wildcard $(dir)/*.cpp))
# OBJS = $(subst $(SRC), $(OBJ), $(SRCS:.cpp=.o))
# # Object files
# OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# # Executable name
# NAME = webserv

# # Targets
# all: $(NAME)

# $(NAME): $(LIB_DIR) Makefile $(OBJS)
# 	@$(CXX) -o $(NAME) $(OBJS) -g $(CFLAGS) -lncurses

# $(OBJ)/%.o: $(SRC)/%.cpp $(LIB_DIR)
# 	@mkdir -p $(OBJ) $(OBJ_DIR)
# 	@$(CXX) $(CFLAGS) $(LIBS) -c $< -o $@

# clean:
# 	@$(RM) $(OBJ)

# fclean: clean
# 	@$(RM) $(NAME)

# .PHONY: all clean fclean re


# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98

# Directories
SRC_DIR = src
OBJ_DIR = obj
INC_DIR = src

# Source files
SRCS = $(SRC_DIR)/main.cpp \
       $(SRC_DIR)/config/ConfigParser.cpp \
       $(SRC_DIR)/config/ServerConfig.cpp \
	   $(SRC_DIR)/request_parser/HttpRequest.cpp \
	   $(SRC_DIR)/request_parser/RequestParser.cpp \
	   $(SRC_DIR)/response_builder/ResponseBuilder.cpp \
	   $(SRC_DIR)/response_builder/RequestResponse.cpp \
	   $(SRC_DIR)/response_builder/ResponseUtils.cpp \
	   $(SRC_DIR)/server/Server.cpp \

# Object files
OBJS = $(SRCS:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# Executable name
NAME = webserv

# Targets
all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -I$(INC_DIR) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re