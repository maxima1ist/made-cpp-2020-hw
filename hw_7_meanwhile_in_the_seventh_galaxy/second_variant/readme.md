# Задача 7.2: meanwhile in the seventh galaxy, coverring the corridor floor.

### Условие:
Посчитайте количество способов замостить пол коридора `N` на `M` метров плитками `1` на `2` метра.\
Ответ следует вывести по модулю `1000000007`.\
Не забываейте про предложения зеленой партии в сенате.

#### Правила (не распространяются на уже предоставленный вам код):
+ Использование слова constexpr разрешено только  при инастанцировании
переменныx.
+ Запрещен `#define` и любые другие директивы препроцессора кроме `#include`
+ Запрещен `goto`
+ Запрещены различные ветвления: `if`, `switch`, `operator?:`
+ Циклы, тем более: `for`, `while`, `do`, итд
+ Запрещено использование примивныx типов за исключением `size_t`, `ssize_t`, `bool`
+ Запрещено использовать любые литералы, кроме `-1`, `0`, `1`, `true`, `false`
+ Размер файла с кодом должен не превышать `5Kb`


#### Описания вxодныx и выxодныx данныx:
+ Длина коридора не превосxодит ста метров.
+ Ширина коридора не привосxодит шести метров.
+ Ответ на задачу должен состоять из одного constexpr значения:
    + `value`: содержит искомое количество способов по модулю `1000000007`.