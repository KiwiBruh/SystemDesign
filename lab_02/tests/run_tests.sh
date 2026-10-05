#!/bin/bash
# Запускать bash tests/run_tests.sh

BASE="http://localhost:8080"
PASSED=0
FAILED=0

# Функция проверки: check "название" "ожидаемый_код" "фактический_код"
check() {
    if [ "$2" = "$3" ]; then
        echo "  OK: $1 (HTTP $3)"
        PASSED=$((PASSED + 1))
    else
        echo "  FAIL: $1 — ожидалось $2, получено $3"
        FAILED=$((FAILED + 1))
    fi
}

# Функция запроса: возвращает HTTP-код
status() {
    curl -s -o /dev/null -w "%{http_code}" "$@"
}

echo "Проверка сервера на $BASE ..."
if [ "$(status $BASE/ping)" != "200" ]; then
    echo "Сервер не отвечает. Запусти: docker compose up -d"
    exit 1
fi

# --- PING ---
echo ""
echo "=== PING ==="
check "GET /ping" 200 "$(status $BASE/ping)"

# --- РЕГИСТРАЦИЯ ---
echo ""
echo "=== РЕГИСТРАЦИЯ ==="
LOGIN="user_$(date +%s)"

check "успешная регистрация" 201 "$(status -X POST $BASE/register \
    -H 'Content-Type: application/json' \
    -d "{\"login\":\"$LOGIN\",\"password\":\"secret123\",\"name\":\"Test\"}")"

check "дубликат логина" 409 "$(status -X POST $BASE/register \
    -H 'Content-Type: application/json' \
    -d "{\"login\":\"$LOGIN\",\"password\":\"secret123\",\"name\":\"Test\"}")"

check "короткий пароль" 400 "$(status -X POST $BASE/register \
    -H 'Content-Type: application/json' \
    -d '{"login":"shortuser","password":"123","name":"X"}')"

# --- ЛОГИН ---
echo ""
echo "=== ЛОГИН ==="
TOKEN=$(curl -s -X POST $BASE/login \
    -H 'Content-Type: application/json' \
    -d "{\"login\":\"$LOGIN\",\"password\":\"secret123\"}" \
    | grep -o '"token":"[^"]*' | cut -d'"' -f4)

if [ -n "$TOKEN" ]; then
    echo "  OK: получен токен"
    PASSED=$((PASSED + 1))
else
    echo "  FAIL: токен не получен"
    FAILED=$((FAILED + 1))
fi

check "неверный пароль" 401 "$(status -X POST $BASE/login \
    -H 'Content-Type: application/json' \
    -d "{\"login\":\"$LOGIN\",\"password\":\"wrong\"}")"

# --- КНИГИ ---
echo ""
echo "=== КНИГИ ==="
check "POST /books без токена" 401 "$(status -X POST $BASE/books \
    -H 'Content-Type: application/json' \
    -d '{"title":"X","author":"Y"}')"

check "POST /books с токеном" 201 "$(status -X POST $BASE/books \
    -H 'Content-Type: application/json' \
    -H "Authorization: Bearer $TOKEN" \
    -d '{"title":"War and Peace","author":"Tolstoy"}')"

check "GET /books" 200 "$(status $BASE/books)"

check "GET /books поиск" 200 "$(status "$BASE/books?title=War")"

# --- ВЫДАЧИ ---
echo ""
echo "=== ВЫДАЧИ ==="
check "POST /loans без токена" 401 "$(status -X POST $BASE/loans \
    -H 'Content-Type: application/json' \
    -d '{"book_id":1}')"

check "POST /loans несуществующая книга" 404 "$(status -X POST $BASE/loans \
    -H 'Content-Type: application/json' \
    -H "Authorization: Bearer $TOKEN" \
    -d '{"book_id":99999}')"

# --- ИТОГИ ---
echo ""
echo "================================"
echo "  PASSED: $PASSED"
echo "  FAILED: $FAILED"
echo "================================"

[ $FAILED -eq 0 ] && exit 0 || exit 1