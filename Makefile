CXXFLAGS+=-pedantic -std=c++11 -Wall -Wextra -Werror

example: example.cpp multicast.hpp
	${CXX} ${CXXFLAGS} -o $@ $<
