\# Оптимизация запросов



Сравнение планов выполнения запросов до и после создания индексов.

Все планы получены через `EXPLAIN` в PostgreSQL 16.



\## 1. Поиск пользователя по логину



\*\*Запрос:\*\*



```sql

SELECT \* FROM users WHERE login = 'alice';

```



\*\*С индексом (`users\_login\_key`):\*\*



```

&#x20;                                 QUERY PLAN

\-------------------------------------------------------------------------------

&#x20;Index Scan using users\_login\_key on users  (cost=0.14..8.16 rows=1 width=952)

&#x20;  Index Cond: ((login)::text = 'alice'::text)

(2 rows)

```



\*\*Без индекса:\*\*



```

&#x20;                                 QUERY PLAN

\-------------------------------------------------------------------------------

&#x20;Index Scan using users\_login\_key on users  (cost=0.14..8.16 rows=1 width=952)

&#x20;  Index Cond: ((login)::text = 'alice'::text)

(2 rows)

```



\*\*Вывод:\*\* индекс `users\_login\_key` создаётся \*\*автоматически\*\* из-за ограничения `UNIQUE` на `login`. Удалить его нельзя. Поэтому план \*\*одинаковый\*\* — `Index Scan` в обоих случаях.



\---



\## 2. Поиск книги по названию



\*\*Запрос:\*\*



```sql

SELECT \* FROM books WHERE title = 'War and Peace';

```



\*\*С индексом (`books\_title`):\*\*



```

&#x20;                                QUERY PLAN

\----------------------------------------------------------------------------

&#x20;Index Scan using books\_title on books  (cost=0.14..8.16 rows=1 width=1049)

&#x20;  Index Cond: ((title)::text = 'War and Peace'::text)

(2 rows)

```



\*\*Без индекса:\*\*



```

&#x20;                      QUERY PLAN

\---------------------------------------------------------

&#x20;Seq Scan on books  (cost=0.00..10.88 rows=1 width=1049)

&#x20;  Filter: ((title)::text = 'War and Peace'::text)

(2 rows)

```



\*\*Вывод:\*\* индекс `books\_title` \*\*используется\*\* — план меняется с `Seq Scan` на `Index Scan`. Стоимость падает с `10.88` до `8.16`.



\---



\## 3. Поиск книги по автору



\*\*Запрос:\*\*



```sql

SELECT \* FROM books WHERE author = 'Tolstoy';

```



\*\*С индексом (`books\_author`):\*\*



```

&#x20;                                QUERY PLAN

\-----------------------------------------------------------------------------

&#x20;Index Scan using books\_author on books  (cost=0.14..8.16 rows=1 width=1049)

&#x20;  Index Cond: ((author)::text = 'Tolstoy'::text)

(2 rows)

```



\*\*Без индекса:\*\*



```

&#x20;                      QUERY PLAN

\---------------------------------------------------------

&#x20;Seq Scan on books  (cost=0.00..10.88 rows=1 width=1049)

&#x20;  Filter: ((author)::text = 'Tolstoy'::text)

(2 rows)

```



\*\*Вывод:\*\* индекс `books\_author` \*\*используется\*\* — план меняется с `Seq Scan` на `Index Scan`. Стоимость падает с `10.88` до `8.16`.



\---



\## 4. Выдачи пользователя с JOIN



\*\*Запрос:\*\*



```sql

SELECT l.id, b.title, l.status

FROM loans l

JOIN books b ON l.book\_id = b.id

WHERE l.user\_id = 1;

```



\*\*С индексом (`loans\_user\_id`):\*\*



```

&#x20;                                      QUERY PLAN

\----------------------------------------------------------------------------------------

&#x20;Hash Join  (cost=11.32..22.29 rows=3 width=574)

&#x20;  Hash Cond: (b.id = l.book\_id)

&#x20;  ->  Seq Scan on books b  (cost=0.00..10.70 rows=70 width=524)

&#x20;  ->  Hash  (cost=11.28..11.28 rows=3 width=66)

&#x20;        ->  Bitmap Heap Scan on loans l  (cost=4.17..11.28 rows=3 width=66)

&#x20;              Recheck Cond: (user\_id = 1)

&#x20;              ->  Bitmap Index Scan on loans\_user\_id  (cost=0.00..4.17 rows=3 width=0)

&#x20;                    Index Cond: (user\_id = 1)

(8 rows)

```



\*\*Без индекса:\*\*



```

&#x20;                            QUERY PLAN

\---------------------------------------------------------------------

&#x20;Hash Join  (cost=18.04..29.01 rows=3 width=574)

&#x20;  Hash Cond: (b.id = l.book\_id)

&#x20;  ->  Seq Scan on books b  (cost=0.00..10.70 rows=70 width=524)

&#x20;  ->  Hash  (cost=18.00..18.00 rows=3 width=66)

&#x20;        ->  Seq Scan on loans l  (cost=0.00..18.00 rows=3 width=66)

&#x20;              Filter: (user\_id = 1)

(6 rows)

```



\*\*Вывод:\*\* индекс `loans\_user\_id` \*\*используется\*\* через `Bitmap Index Scan` — это разновидность сканирования по индексу. Стоимость падает с `29.01` до `22.29`. `Seq Scan on books` остаётся — потому что JOIN нужны \*\*все книги\*\*, индекс тут не поможет.



\---



\## Итог



| Запрос | Без индекса | С индексом | Ускорение |

|---|---|---|---|

| Поиск по логину | `Index Scan` | `Index Scan` | — одинаково |

| Поиск по названию | `Seq Scan` | `Index Scan` | есть |

| Поиск по автору | `Seq Scan` | `Index Scan` | есть  |

| Выдачи с JOIN | `Seq Scan on loans` | `Bitmap Index Scan` | есть |



\*\*Вывод:\*\* индексы \*\*ускоряют\*\* поиск по названию, автору и фильтрацию выдач по пользователю. Для `UNIQUE`-поля индекс создаётся \*\*автоматически\*\*. Для JOIN с `books` индекс \*\*не помогает\*\*, потому что нужны \*\*все строки\*\* таблицы.





