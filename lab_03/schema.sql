
DROP TABLE IF EXISTS users CASCADE;
DROP TABLE IF EXISTS books CASCADE;
DROP TABLE IF EXISTS loans CASCADE;

CREATE TABLE IF NOT EXISTS  users (
    id            BIGSERIAL PRIMARY KEY,
    login         VARCHAR(64)  NOT NULL UNIQUE,
    password_hash VARCHAR(128) NOT NULL,
    name          VARCHAR(255) NOT NULL,
    created_at    TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),

    CONSTRAINT login_length   CHECK (LENGTH(login) >= 3),
    CONSTRAINT empty_name CHECK (LENGTH(TRIM(name)) > 0)
);


CREATE TABLE IF NOT EXISTS  books (
    id           BIGSERIAL PRIMARY KEY,
    title        VARCHAR(255) NOT NULL,
    author       VARCHAR(255) NOT NULL,
    is_available BOOLEAN NOT NULL DEFAULT TRUE,
    created_at   TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),

    CONSTRAINT empty_title  CHECK (LENGTH(TRIM(title)) > 0),
    CONSTRAINT empty_author CHECK (LENGTH(TRIM(author)) > 0)
);


CREATE TABLE IF NOT EXISTS  loans (
    id          BIGSERIAL PRIMARY KEY,
    book_id     BIGINT NOT NULL REFERENCES books(id) ON DELETE CASCADE,
    user_id     BIGINT NOT NULL REFERENCES users(id) ON DELETE CASCADE,
    issued_at   TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT NOW(),
    due_at      TIMESTAMP WITH TIME ZONE NOT NULL DEFAULT (NOW() + INTERVAL '30 days'),
    returned_at TIMESTAMP WITH TIME ZONE,
    status      VARCHAR(16) NOT NULL DEFAULT 'active',

    CONSTRAINT chk_status CHECK (status IN ('active', 'returned'))
);


CREATE INDEX loans_book_id ON loans(book_id);
CREATE INDEX loans_user_id ON loans(user_id);
CREATE INDEX loans_status  ON loans(status);
CREATE INDEX books_title   ON books(title);
CREATE INDEX books_author  ON books(author);