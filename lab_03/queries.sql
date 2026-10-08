-- 1. Создание нового пользователя
INSERT INTO users (login, password_hash, name)
VALUES ('newuser', 'fcf730b6d95236ecd3c9fc2d92d7b6b2bb7d5e6c1f8d1a2a0e5c3b4d6e7f8a9', 'New User');

-- 2. Поиск пользователя по логину
SELECT * FROM users WHERE login = 'alice';

-- 3. Поиск пользователя по маске имя и фамилия
SELECT * FROM users WHERE name ILIKE '%alice%';

-- 4. Добавление книги в библиотеку
INSERT INTO books (title, author, is_available)
VALUES ('New Book', 'New Author', TRUE);

-- 5. Поиск книги по названию
SELECT * FROM books WHERE title ILIKE '%war%';

-- 6. Поиск книги по автору
SELECT * FROM books WHERE author ILIKE '%tolstoy%';

-- 7. Создание выдачи книги пользователю
BEGIN;
UPDATE books SET is_available = FALSE WHERE id = 1;
INSERT INTO loans (book_id, user_id, status, due_at)
VALUES (1, 1, 'active', NOW() + INTERVAL '30 days');
COMMIT;

-- 8. Получение списка выданных книг пользователя
SELECT l.id, b.title, b.author, l.issued_at, l.due_at, l.status
FROM loans l
JOIN books b ON l.book_id = b.id
WHERE l.user_id = 1;

-- 9. Возврат книги
BEGIN;
UPDATE loans SET status = 'returned', returned_at = NOW() WHERE id = 1;
UPDATE books SET is_available = TRUE WHERE id = (SELECT book_id FROM loans WHERE id = 1);
COMMIT;