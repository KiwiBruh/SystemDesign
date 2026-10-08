
-- тестовые данные

INSERT INTO users (login, password_hash, name) VALUES
('alice',    'fc07c43afc51cd1c3ba246cb14a5ba919bea4de6308e3f32822a33a0bf3ca864', 'Alice Ivanova'),
('alex',    '7f5286e8663d1530f02ef27153bf3f1776f2266302c471618b57c68965f86152', 'Alex Sidorov'),
('bob',    '6eb5c2c97445fda87492479ff3db69e2af567eb820fe7ebf00400508b6166426', 'Bobby Goodboy'),
('ben',    '571a2daf102e9fd0d156c1e7b264f1703097b8d649d3a39087d29014e4bafbfa', 'Benjamin Franklin'),
('candice',    'be027abb71721e32a8882c9dd83eb7ecaa51faa951431e694c3de369bc83fffc', 'Candice Wallace'),
('charlie',    'b78470df2863f08019a95fb64f09a8edc356fd4f15b43aeed98926905323b2db', 'Charlie Chaplin'),
('dan',    'f5860783d9508f9f7c0097ae59a6ae9d2aaf62c9ab5d753a1010361ecd09fcd3', 'Dan DeVito'),
('derek',    '8eef9188187021557806a2c4c381322bee9e24410c576a98392cbe2fbb5197db', 'Derek Crause'),
('ethan',    '69b7635b5078360f7a1b966b48f0e6eb021de815e7c40685e3a9a4374325b9eb', 'Ethan Chief'),
('emma',    'd054794b6741e7d714d5a3e942e013a5fe6cd915e370be21ae55c988e3c199b8', 'Emma Watson');


INSERT INTO books (title, author, is_available) VALUES
('War and Peace',                'Tolstoy',       TRUE),
('Anna Karenina',                'Tolstoy',       TRUE),
('Crime and Punishment',         'Dostoevsky', TRUE),
('The Idiot',                    'Dostoevsky', TRUE),
('The Master and Margarita',     'Bulgakov',  TRUE),
('Heart of a Dog',             	 'Bulgakov', TRUE),
('1984',                         'Orwell',     TRUE),
('Animal Farm',                  'Orwell',     TRUE),
('Brave New World',              'Huxley',     TRUE),
('Fahrenheit 451',               'Bradbury',      TRUE);


-- (часть активных, часть возвращённых)
INSERT INTO loans (book_id, user_id, issued_at, due_at, returned_at, status) VALUES
(1,  1,  NOW() - INTERVAL '15 days', NOW() + INTERVAL '15 days', NULL,                          'active'),
(2,  2,  NOW() - INTERVAL '12 days', NOW() + INTERVAL '18 days', NULL,                          'active'),
(3,  3,  NOW() - INTERVAL '20 days', NOW() + INTERVAL '10 days', NULL,                          'active'),
(4,  1,  NOW() - INTERVAL '40 days', NOW() - INTERVAL '10 days', NOW() - INTERVAL '5 days',     'returned'),
(5,  5,  NOW() - INTERVAL '35 days', NOW() - INTERVAL '5 days',  NOW() - INTERVAL '10 days',    'returned'),
(6,  6,  NOW() - INTERVAL '8 days',  NOW() + INTERVAL '22 days', NULL,                          'active'),
(7,  7,  NOW() - INTERVAL '25 days', NOW() + INTERVAL '5 days',  NOW() - INTERVAL '2 days',     'returned'),
(8,  8,  NOW() - INTERVAL '3 days',  NOW() + INTERVAL '27 days', NULL,                          'active'),
(9,  9,  NOW() - INTERVAL '18 days', NOW() + INTERVAL '12 days', NOW() - INTERVAL '1 day',      'returned'),
(10, 10, NOW() - INTERVAL '1 day',   NOW() + INTERVAL '29 days', NULL, 				'active');

-- is_available для выданных книг
UPDATE books SET is_available = FALSE WHERE id IN (SELECT book_id FROM loans WHERE status = 'active');