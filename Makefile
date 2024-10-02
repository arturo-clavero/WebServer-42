# Compiler and flags
CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98 #-fsanitize=address

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
	   $(SRC_DIR)/post_request_body_handling/PostRequestBodySnatcher.cpp \
	   $(SRC_DIR)/post_request_body_handling/PostRequestBodyPart.cpp \
	   $(SRC_DIR)/response_builder/RequestResponse.cpp \
	   $(SRC_DIR)/response_builder/ResponseUtils.cpp \
	  	$(SRC_DIR)/server/ServerCore.cpp \
	   $(SRC_DIR)/server/Multiplex.cpp \
	   $(SRC_DIR)/server/ServerSocket.cpp \
	   $(SRC_DIR)/server/ClientSocket.cpp \
	   $(SRC_DIR)/server/Utils.cpp \


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

run: all
	./$(NAME) conf/default.conf

file: all
	./$(NAME) conf/default.conf > output.txt

.PHONY: all clean fclean re