#line 20 "README.md"
#include <iostream>

void print_rec(unsigned val, std::ostream& out) {
	char dig = (val % 10) + '0';
	if (val /= 10) { print_rec(val, out); }
	out.put(dig);
}
#line 34
void print_str(unsigned val, std::ostream& out) {
	std::string res;
	do {
		char dig = (val % 10) + '0';
		res.insert(res.begin(), dig);
	} while (val /= 10);
	out << res;
}
#line 57
#include <limits>

void print_raw(unsigned val, std::ostream& out) {
	char buf[std::numeric_limits<decltype(val)>::digits10 + 2];
	char* cur { buf + sizeof(buf) };
	*--cur = '\0';
	do {
		*--cur = (val % 10) + '0';
	} while (val /= 10);
	out << cur;
}
#line 91
template<void (FN)(unsigned, std::ostream&)>
void print_int(int val, std::ostream& out) {
	if (val < 0) {
		out.put('-');
		if (val == std::numeric_limits<decltype(val)>::min()) {
			FN(static_cast<unsigned>(-(val / 10)), out);
			val %= 10;
		}
		val = -val;
	}
	FN(static_cast<unsigned>(val), out);
}
#line 112
#include <cassert>
#include <sstream>

template<typename T>
void assert_print(void (fn)(T, std::ostream&)) { }

template<typename T, typename... R>
void assert_print(void (fn)(T, std::ostream&), T val, R... rst) {
	std::ostringstream out;
	fn(val, out);
	assert(out.str() == std::to_string(val));
	assert_print<T>(fn, rst...);
}
	
template<typename T>
void assert_unsigned(void (fn)(T, std::ostream&)) {
	assert_print<T>(
		fn, 0, 1, 9, 10, 11, 9999, 10000,
		std::numeric_limits<T>::max()
	);
}

template<void (FN)(unsigned, std::ostream&)>
void assert_fn() {
	assert_unsigned(FN);
	auto fn { print_int<FN> };
	assert_unsigned(fn);
	assert_print(
		fn, -1, -9, -10, -11, -9999, -10000,
		std::numeric_limits<int>::min()
	);
}

int main() {
	assert_fn<print_rec>();
	assert_fn<print_str>();
	assert_fn<print_raw>();
	std::cout << "ok\n";
}
