# Rekursion ist nicht böse

Moderne Integrated Development Environments (IDEs) geben
eine Warnung aus, wenn eine Funktion oder Methode Rekursion verwendet.
Wie so viele andere sinnfreie Warnungen verbessern sie nicht
die Code-Qualität. Nein, sie reduzieren sie sogar.

Rekursionen sind ein probates Mittel, um bestimmte Probleme
leicht zu lösen. Zwar lassen sich alle rekursiven Probleme auch iterativ
lösen, aber oft wird der resultierende Code länger und unübersichtlicher.
Speziell wenn Container anfangen, einen Stapel zu simulieren.

## Dezimalzahlen ausgeben

Ich betrachte ein einfaches Beispiel: eine nicht-negative Zahl
in Dezimal-Notation ausgeben. In C++ schreibe ich die Funktion in
`rekursion.cpp` mit Rekursion so:

```c++
#include <iostream>

void print_rec(unsigned val, std::ostream& out) {
	char dig = (val % 10) + '0';
	if (val /= 10) { print_rec(val, out); }
	out.put(dig);
}
```

Das sind nur fünf Zeilen. Meine iterative Lösung mit einem `std::string`
sieht gar nicht so viel schlimmer aus:

```c++
// ...
void print_str(unsigned val, std::ostream& out) {
	std::string res;
	do {
		char dig = (val % 10) + '0';
		res.insert(res.begin(), dig);
	} while (val /= 10);
	out << res;
}
```

Immerhin schon acht Zeilen (ein Blowup von über 50%). Und schneller und
schöner ist diese Version auch nicht. Das Detail ist in der `insert`-Methode
versteckt. Diese muss Platz für die neuen Zeichen schaffen und bei einer
mittelmäßigen Implementierung in jedem Durchgang alle bisherigen Zeichen
kopieren.

## Raw Buffer

Wenn ich auf das Kopieren verzichte, wird der Code sogar etwas
schneller als die Rekursion. Aber hübscher ist er nicht geworden:

```c++
// ...
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
```

Auch wenn die Zeilenzahl nur auf neun wächst: der Code ist komplizierter,
da ich zusätzlich die Verwaltung des Arrays übernehmen muss.

Und für die IDE ist nicht leicht zu erkennen, dass kein Array-Unterlauf
stattfinden kann.

## Fazit

Schon mit diesem kleinen Beispiel wird klar, dass eine Rekursion
kein Grund für eine Warnung ist. Auf inherent rekursive Algorithmen wie
das Parsen von Grammatiken bin ich noch gar nicht eingegangen.
Verwendet Rekursion wo es sinnvoll ist!

## Negative Zahlen

Nur zur Vollständigkeit: negative Zahlen können wir recht einfach auf
nicht-negative Zahlen abbilden. Wir müssen nur aufpassen, da es für die
kleinste negative Zahl keinen positiven Wert gibt:

```c++
// ...
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
```

## Test Cases

Hier sind ein paar Test-Cases für die Funktionen, um sicherzustellen, dass
die ganzen Programme das machen, was ich von ihnen erwarte:

```c++
// ...
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
```
